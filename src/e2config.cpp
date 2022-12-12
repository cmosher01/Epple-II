/*
    epple2
    Copyright (C) 2008 by Christopher A. Mosher <cmosher01@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY, without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "e2config.h"

#include "e2command.h"
#include "E2wxApp.h"
#include "e2filesystem.h"
#include "apple2.h"
#include "memory.h"
#include "memoryrandomaccess.h"
#include "slots.h"
#include "diskcontroller.h"
#include "languagecard.h"
#include "firmwarecard.h"
#include "standardout.h"
#include "standardin.h"
#include "clockcard.h"
#include "cassettein.h"
#include "cassetteout.h"

#include <wx/filedlg.h>
#include <wx/config.h>
#include <wx/string.h>

#include <boost/log/trivial.hpp>

#include <filesystem>
#include <iostream>
#include <istream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <cctype>



static const wxString DEFAULT_CONFIG_NAME{"epple2"};







E2Config::E2Config(const std::filesystem::path& f, bool p): file_path {f}, prefs_only {p} {
}

E2Config::~E2Config() {
}

static void strip_comment(std::string& str) {
    const size_t comment = str.find('#');
    if (comment < std::string::npos) {
        str.erase(comment);
    }
}

static void trim(std::string& s) {
    wxString w{s};
    s = w.Trim(false).Trim(true);
}



/*
 * Searches for config file with the given name in the preferences areas.
 * The name must not be a filesystem path.
 * The actual config files themselves have ".conf" extension.
 * The preferences areas are searched in this order:
 * user area
 * built-in area
 *
 * If successful, returns an open stream, caller is responsible for deleting it
 * Otherwise, returns null
 */
std::ifstream *E2Config::openFilePref(const wxString& s_name) {
    std::ifstream *ret = nullptr;



    BOOST_LOG_TRIVIAL(info) << "config file name was specified as: " << s_name;
    std::filesystem::path path_name = path_from_string(s_name);
    if (path_name.has_parent_path()) {
        BOOST_LOG_TRIVIAL(warning) << "invalid name for config file (paths are not allowed): " << path_name.c_str();
        return ret;
    }

    if (path_name.empty()) {
        BOOST_LOG_TRIVIAL(warning) << "invalid: empty name for config file";
        return ret;
    }

    // required file extension for any file to be recognized as a config file
    path_name = path_from_string(s_name+".conf");



    std::filesystem::path path;

    path = valid_input_file(path_name, wxGetApp().GetConfigDir());
    if (!path.empty()) {
        ret = new std::ifstream(path);
        if (ret->is_open()) {
            return ret; // Found specified config file in user area of preferences
        }
        delete ret;
        ret = nullptr;
    }

    path = valid_input_file(path_name, wxGetApp().GetResDir());
    if (!path.empty()) {
        ret = new std::ifstream(path);
        if (ret->is_open()) {
            return ret; // Found specified config file in built-in area of preferences
        }
        delete ret;
        ret = nullptr;
    }



    return ret;
}

std::ifstream *E2Config::openFileExternal(const std::filesystem::path& path) {
    std::ifstream *ret = nullptr;

    const std::filesystem::path p = valid_input_file(path);
    if (!p.empty()) {
        ret = new std::ifstream(p);
        if (ret->is_open()) {
            return ret;
        }
        delete ret;
        ret = nullptr;
    }

    return ret;
}

static const std::array rs_path_legacy{
    "./etc/epple2/epple2.conf",
    ETCDIR "/epple2/epple2.conf",
    "/etc/epple2/epple2.conf",
    "/etc/epple2.conf",
    "./epple2.conf",
};

std::ifstream *E2Config::openFileLegacy() {
    std::ifstream *ret = nullptr;

    BOOST_LOG_TRIVIAL(warning) << "Searching for config file in legacy locations...";
    for (const auto &s_path_legacy : rs_path_legacy) {
        if ((ret = openFileExternal(std::filesystem::path{s_path_legacy})) != nullptr) {
            return ret;
        }
    }
    BOOST_LOG_TRIVIAL(warning) << "Could not file config file in any legacy locations.";

    return ret;
}

/*
 * Algorithm to locate and open the configuration file, as specified by
 * the user, either on command line, or via preferences, allowing for
 * backward compatibility with legacy file locations.
 */
std::ifstream *E2Config::openFile() {
    std::ifstream *ret = nullptr;

    if (this->file_path.empty()) {
        BOOST_LOG_TRIVIAL(info) << "No config file specified on command line.";

        wxString cname{};
        const bool stored_prefs_found = wxConfigBase::Get()->Read("/ActivePreferences/name", &cname, DEFAULT_CONFIG_NAME);

        if (stored_prefs_found) {
            BOOST_LOG_TRIVIAL(info) << "Will use config file name currently marked as active by the user: " << cname.wc_str();
            ret = openFilePref(cname);
            if (ret != nullptr) {
                return ret;
            }
            BOOST_LOG_TRIVIAL(warning) << "Could not find the config file currently marked as active by the user: " << cname.wc_str();

            if (!this->prefs_only) {
                ret = openFileLegacy();
                if (ret != nullptr) {
                    return ret;
                }
            }
        } else {
            if (!this->prefs_only) {
                BOOST_LOG_TRIVIAL(warning) << "User has not specified any active config file.";
                ret = openFileLegacy();
                if (ret != nullptr) {
                    return ret;
                }
            }
            ret = openFilePref(cname);
            if (ret != nullptr) {
                return ret;
            }
        }
    } else {
        BOOST_LOG_TRIVIAL(info) << "Config file specified on command line as: " << this->file_path.c_str();

        if (!this->prefs_only) {
            BOOST_LOG_TRIVIAL(info) << "Searching for external configuration file.";
            ret = openFileExternal(this->file_path);
            if (ret != nullptr) {
                return ret;
            }
        }

        ret = openFilePref(this->file_path.c_str());
        if (ret != nullptr) {
            return ret;
        }
    }

    BOOST_LOG_TRIVIAL(warning) << "Could not find any config file.";

    return ret;
}



void E2Config::parse(MemoryRandomAccess& ram, Memory& rom, Slots& slts, int& revision, ScreenImage& gui, CassetteIn& cassetteIn, CassetteOut& cassetteOut, Apple2* apple2) {
    std::ifstream *p_ifstream_config = openFile();

    if (p_ifstream_config == nullptr) {
        std::cerr << "Cannot find config file. Running without any RAM, ROM, or cards." << std::endl;
        return;
    }

    std::string line;
    std::getline(*p_ifstream_config, line);
    while (!p_ifstream_config->eof()) {
        strip_comment(line);
        trim(line);
        if (!line.empty()) {
            E2Command{}.parseLine(line, ram, rom, slts, revision, gui, cassetteIn, cassetteOut, apple2);
        }
        std::getline(*p_ifstream_config, line);
    }

    delete p_ifstream_config;

    // TODO: make sure there is no more than ONE stdin and/or ONE stdout card
}
