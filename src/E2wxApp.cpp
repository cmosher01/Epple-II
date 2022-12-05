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
#include "configep2.h"
#include <wx/app.h>
#include <wx/xrc/xmlres.h>
#include <wx/fileconf.h>
#include <wx/stdpaths.h>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/expressions/formatters/stream.hpp>
#include <boost/log/expressions.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <wx/debugrpt.h>
#include <iostream>
#include <thread>
#include <string>
#include <memory>
#include <algorithm>



wxIMPLEMENT_APP_NO_MAIN(E2wxApp);



#ifndef PROJECT_VERSION
#define PROJECT_VERSION 0.0.1
#endif

E2wxApp::E2wxApp() : id("nu.mine.mosher.epple2"), version(wxSTRINGIZE_T(PROJECT_VERSION)) {
}

E2wxApp::~E2wxApp() {
}










static std::filesystem::path dirCache() {
    return std::filesystem::path(wxStandardPaths::Get().GetUserDir(wxStandardPaths::Dir_Cache).t_str());
}

static std::filesystem::path dirConfig() {
    return std::filesystem::path(wxStandardPaths::Get().GetUserConfigDir().t_str());
}

static std::filesystem::path dirDocuments() {
    return std::filesystem::path(wxStandardPaths::Get().GetAppDocumentsDir().t_str());
}

static std::filesystem::path dirResources() {
    return std::filesystem::path(wxStandardPaths::Get().GetResourcesDir().t_str());
}







bool E2wxApp::OnInit() {
    if (!wxApp::OnInit()) {
        return false;
    }

#ifdef wxUSE_ON_FATAL_EXCEPTION
    wxHandleFatalExceptions();
#endif

    wxStandardPaths& stdpaths = wxStandardPaths::Get();
    //stdpaths.SetInstallPrefix(".");
    stdpaths.SetFileLayout(wxStandardPaths::FileLayout_XDG);



    InitBoostLog();




    this->confdir = dirConfig() / std::filesystem::path(GetID()+".d");
    std::filesystem::create_directories(this->confdir);
    BOOST_LOG_TRIVIAL(info) << "Configuration directory path: " << this->confdir;

    this->conffile = dirConfig() / std::filesystem::path(GetID());
    BOOST_LOG_TRIVIAL(info) << "Configuration      file path: " << this->conffile;
    wxConfigBase::Set(new wxFileConfig("", "", GetID()));

    this->docsdir = dirDocuments() / std::filesystem::path(GetID());
    BOOST_LOG_TRIVIAL(info) << "User document directory path: " << this->docsdir;

    const std::filesystem::path exe = std::filesystem::path(stdpaths.GetExecutablePath().t_str());
    std::cout << "Executable         file path: " << exe << std::endl;
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
    std::cout << "Resource      directory path: " << this->resdir << std::endl;

    wxXmlResource::Get()->InitAllHandlers();
    if (!wxXmlResource::Get()->LoadAllFiles(this->resdir.c_str())) {
        return false;
    }



    StartSdlEpple2();



    E2wxFrame *frame = new E2wxFrame();
    frame->DoInit();
    frame->Show();



    return true;
}

static int run(const std::string config_file) {
    std::unique_ptr<Emulator> emu(new Emulator());

    Config cfg(config_file);
    emu->config(cfg);

    emu->init();

    return emu->run();
}

void E2wxApp::StartSdlEpple2() {
    std::cout << "starting sdl thread..." << std::endl;
    this->thread_sdl = new std::thread(run, this->arg_configfile);
    std::cout << "started sdl thread." << std::endl;
}

int E2wxApp::OnExit() {
    std::cout << "stopping sdl thread..." << std::endl;
    GUI::queueQuit();
    this->thread_sdl->join();
    std::cout << "exiting wx application..." << std::endl;
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

static const wxCmdLineEntryDesc cmdLineDesc[] =
{
    { wxCMD_LINE_PARAM,  NULL, NULL, "config file", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
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

    const int n = parser.GetParamCount();

    if (n <= 0) {
        std::cout << "no config file specified on the command line; will use config file specified in user-preferences" << std::endl;
    } else {
        this->arg_configfile = parser.GetParam(0);
        std::cout << "using config file specified on the command line: " << this->arg_configfile << std::endl;
    }

    return true;
}


const std::filesystem::path E2wxApp::GetLogFile() const {
    return this->logfile;
}

const std::filesystem::path E2wxApp::GetResDir() const {
    return this->resdir;
}

const std::string E2wxApp::GetID() const {
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
        std::filesystem::path(GetID()) /
        std::filesystem::path("log");

    std::filesystem::create_directories(logfile);
    logfile = std::filesystem::canonical(logfile);

    const std::string ts = to_iso_string(boost::posix_time::second_clock::universal_time());
    logfile /= ts + ".log";

    return logfile;
}

void E2wxApp::InitBoostLog() {
    this->logfile = BuildLogFilePath();

    std::cout << "log file: " << this->logfile << std::endl;



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
