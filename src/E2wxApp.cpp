/*
    epple2
    Copyright (C) 2022 by Christopher A. Mosher <cmosher01@gmail.com>

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

    Created on December 3, 2022, 3:02 PM
 */

#include "config.h"
#include "E2wxApp.h"
#include "E2wxFrame.h"
#include "emulator.h"
#include "gui.h"
#include "e2config.h"
#include "e2filesystem.h"
#include "e2const.h"

#include <wx/app.h>
#include <wx/textdlg.h>
#include <wx/window.h>
#include <wx/xrc/xmlres.h>
#include <wx/fileconf.h>
#include <wx/log.h>
#include <wx/stdpaths.h>
#include <wx/debugrpt.h>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/expressions/formatters/stream.hpp>
#include <boost/log/expressions.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <iostream>
#include <string>
#include <memory>
#include <algorithm>



wxIMPLEMENT_APP_NO_MAIN(E2wxApp);



#ifndef PROJECT_VERSION
#define PROJECT_VERSION 0.0.1
#endif
#ifndef PROJECT_VENDOR
#define PROJECT_VENDOR nu.mine.mosher
#endif
#ifndef PROJECT_NAME
#define PROJECT_NAME Epple-II
#endif





EmuTimer::EmuTimer(Emulator *e) : wxTimer(), emu(e) {
}

EmuTimer::~EmuTimer() {
}

#define EXPECTED_MS 50 // see CHECK_EVERY_CYCLE in Emulator.cpp
void EmuTimer::begin() {
    this->Start(EXPECTED_MS);
}

void EmuTimer::Notify() {
    this->emu->tick50ms();
}




E2wxApp::E2wxApp() :
    id(wxSTRINGIZE(PROJECT_VENDOR) "." wxSTRINGIZE(PROJECT_NAME)),
    version(wxSTRINGIZE(PROJECT_VERSION)),
    frame(nullptr),
    emu(nullptr),
    emu_timer(nullptr) {
}

E2wxApp::~E2wxApp() {
}





static std::filesystem::path dirCache() {
    return path_from_string(wxStandardPaths::Get().GetUserDir(wxStandardPaths::Dir_Cache));
}

static std::filesystem::path dirConfig() {
    return path_from_string(wxStandardPaths::Get().GetUserConfigDir());
}

static std::filesystem::path dirDocuments() {
    return path_from_string(wxStandardPaths::Get().GetAppDocumentsDir());
}







bool E2wxApp::OnInit() {
    if (!wxApp::OnInit()) {
        return false;
    }

#ifdef wxUSE_ON_FATAL_EXCEPTION
#ifndef __WINDOWS__
    wxHandleFatalExceptions();
#endif
#endif



    wxStandardPaths& stdpaths = wxStandardPaths::Get();
    //stdpaths.SetInstallPrefix(".");
    stdpaths.SetFileLayout(wxStandardPaths::FileLayout_XDG);



    InitBoostLog();

//    TODO investigate redirecting wxLogs to boost
//    wxLog* logger = new wxLogStream(&std::cerr);
//    wxLog::SetActiveTarget(logger);
//    wxLogWarning("%s", "a warning has occurred");
//    wxLogInfo("%s", "informational");
//    wxLogVerbose("%s", "verbose");
//    wxFile().Open("foobar.txt");

//    TODO wx log file window?
//    TODO remove all logging from stdout/err to log file
//    TODO define components to turn on/off for trace level logging (disk, cassette, woz, keyboard, ram, lss, audio, etc.)

    BOOST_LOG_TRIVIAL(info) << "Application ID: " << this->GetID();
    BOOST_LOG_TRIVIAL(info) << "Application version: " << this->GetVersion();


    this->confdir = dirConfig() / path_from_string(GetID()+".d");
    std::filesystem::create_directories(this->confdir);
    BOOST_LOG_TRIVIAL(info) << "Configuration directory path: " << this->confdir;

    this->conffile = dirConfig() / path_from_string(GetID());
    BOOST_LOG_TRIVIAL(info) << "Configuration      file path: " << this->conffile;
    wxConfigBase::Set(new wxFileConfig("", "", GetID()));

    this->docsdir = dirDocuments() / path_from_string(GetID());
    BOOST_LOG_TRIVIAL(info) << "User document directory path: " << this->docsdir;

    const std::filesystem::path exe = path_from_string(stdpaths.GetExecutablePath());
    BOOST_LOG_TRIVIAL(info) << "Executable         file path: " << exe;
    std::filesystem::path res = exe.parent_path();
    if (res.filename() == "bin" || res.filename() == "MacOS") {
        res = res.parent_path();
    }
    if (std::filesystem::is_directory(res / "share")) {
        res /= "share";
    }
    if (std::filesystem::is_directory(res / "Resources")) {
        res /= "Resources";
    }
    this->resdir = res;
    BOOST_LOG_TRIVIAL(info) << "Resource      directory path: " << this->resdir;

    wxXmlResource::Get()->InitAllHandlers();
    if (!wxXmlResource::Get()->LoadAllFiles(this->resdir.c_str())) {
        return false;
    }



    frame = new E2wxFrame();
    frame->DoInit();
    frame->Show();



    StartEmulator();



    return true;
}



void E2wxApp::OnFnKeyPressed(const SDL_Keycode k) {
    if (k == SDLK_F1) {
        this->TogglePower();
    } else if (k == SDLK_F2) {
        this->CycleMonitor();
    } else if (k == SDLK_F3) {
        this->ToggleFullScreen();
    } else if (k == SDLK_F4) {
        //
    } else if (k == SDLK_F5) {
        this->EmulatorCommand();
    } else if (k == SDLK_F6) {
        this->Reset();
    } else if (k == SDLK_F7) {
        this->Paste();
    } else if (k == SDLK_F8) {
        this->ScreenShot();
    } else if (k == SDLK_F9) {
        this->CloseMainFrame();
    } else if (k == SDLK_F10) {
        //
    } else if (k == SDLK_F11) {
        //
    } else if (k == SDLK_F12) {
        this->ToggleBuffered();
    }
}

bool E2wxApp::CloseMainFrame() {
    bool r = false;
    if (this->frame) {
        if (this->frame->Close(false)) {
            delete this->frame;
            this->frame = nullptr;
            r = true;
        }
    }
    return r;
}

int E2wxApp::OnExit() {
    if (this->emu_timer) {
        delete this->emu_timer;
        this->emu_timer = nullptr;
    }
    if (this->emu) {
        delete this->emu;
        this->emu = nullptr;
    }
    return 0;
}



void E2wxApp::OnFatalException() {
    wxDebugReport report;
    report.AddAll();

    wxDebugReportPreviewStd preview;
    if (preview.Show(report)) {
        report.Process();
    }
}



static const wxCmdLineEntryDesc cmdLineDesc[] = {
    { wxCMD_LINE_SWITCH, "p", "prefs", "Read config only from preferences, never an external file.", wxCMD_LINE_VAL_NONE },
    { wxCMD_LINE_SWITCH, "t", "test", "Run internal tests.", wxCMD_LINE_VAL_NONE },
    { wxCMD_LINE_PARAM,  NULL, NULL, "config-file", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    wxCMD_LINE_DESC_END
};

void E2wxApp::OnInitCmdLine(wxCmdLineParser& parser) {
    wxApp::OnInitCmdLine(parser);
    parser.SetDesc(cmdLineDesc);
}

bool E2wxApp::OnCmdLineParsed(wxCmdLineParser& parser) {
    if (!wxApp::OnCmdLineParsed(parser)) {
        return false;
    }

    if (parser.Found("t")) {
        const int x = E2Const::test();
        if (x != -1) {
            std::cerr << x << std::endl;
            throw std::runtime_error("bad constant in e2const.h" );
        }
        std::cerr << "pass" << std::endl;
        return false;
    }

    this->opt_config_from_prefs_only = parser.Found("p");

    const int n = parser.GetParamCount();

    if (n == 1) {
        this->arg_configfile = path_from_string(parser.GetParam(0));
    } else if (1 < n) {
        // should never happen, anyway
        return false;
    }

    return true;
}



const std::filesystem::path E2wxApp::GetLogFile() const {
    return this->logfile;
}

const std::filesystem::path E2wxApp::GetResDir() const {
    return this->resdir;
}

const wxString E2wxApp::GetID() const {
    return this->id;
}

const wxString E2wxApp::GetVersion() const {
    return this->version;
}

const std::filesystem::path E2wxApp::GetConfigFile() const {
    return this->conffile;
}

const std::filesystem::path E2wxApp::GetConfigDir() const {
    return this->confdir;
}

const std::filesystem::path E2wxApp::GetDocumentsDir() const {
    return this->docsdir;
}



const std::filesystem::path E2wxApp::BuildLogFilePath() const {
    std::filesystem::path logfile =
        dirCache() /
        path_from_string(GetID()) /
        std::filesystem::path("log");

    std::filesystem::create_directories(logfile);
    logfile = std::filesystem::canonical(logfile);

    const std::string ts = to_iso_string(boost::posix_time::second_clock::universal_time());
    logfile /= ts + ".log";

    return logfile;
}

void E2wxApp::InitBoostLog() {
    this->logfile = BuildLogFilePath();

    std::cerr << "log file: " << this->logfile << std::endl;



    boost::log::add_file_log(
            boost::log::keywords::file_name = this->logfile,
            boost::log::keywords::auto_flush = true,
            boost::log::keywords::format = (
                boost::log::expressions::stream <<
                    to_iso_extended_string(boost::posix_time::microsec_clock::universal_time()) << "Z " <<
                    boost::log::trivial::severity << " " <<
                    boost::log::expressions::message
            ));

    boost::log::add_common_attributes();
}



void E2wxApp::StartEmulator() {
    if (this->emu_timer) {
        delete this->emu_timer;
    }
    if (this->emu) {
        delete this->emu;
    }

    this->emu = new Emulator();
    E2Config cfg{this->arg_configfile, this->opt_config_from_prefs_only};
    this->emu->config(cfg);

    this->emu_timer = new EmuTimer{this->emu};
    this->emu_timer->begin();
}



bool E2wxApp::EnsureCanQuit() {
    bool ok = true;
    if (this->emu) {
        ok = this->emu->isSafeToQuit();
    }
    return ok;
}

void E2wxApp::Paste() {
    if (this->emu) {
        this->emu->paste();
    }
}

void E2wxApp::CycleMonitor() {
    if (this->emu) {
        this->emu->monitorCycle();
    }
}

void E2wxApp::ScreenShot() {
    if (this->emu) {
        this->emu->screenshot();
    }
}

void E2wxApp::Reset() {
    if (this->emu) {
        this->emu->reset();
    }
}

void E2wxApp::TogglePower() {
    if (this->emu) {
        this->emu->toggleComputerPower();
    }
}

void E2wxApp::ToggleBuffered() {
    if (this->emu) {
        this->emu->toggleBuffered();
    }
}

void E2wxApp::ToggleFullScreen() {
    if (this->emu) {
        this->emu->toggleFullScreen();
    }
}

static const wxString message = "Enter a command for the emulator. See https://cmosher01.github.io/Epple-II/usermanual.html";
static const wxString title = "Emulator command";
void E2wxApp::EmulatorCommand() {
    if (this->emu) {
        wxTextEntryDialog dlg(this->frame, message, title);
        if (dlg.ShowModal() == wxID_OK) {
            this->emu->cmd(dlg.GetValue());
        }
    }
}
