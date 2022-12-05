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

#ifndef E2WXAPP_H
#define E2WXAPP_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/app.h>
#include <wx/cmdline.h>
#include <wx/string.h>
#include <thread>
#include <filesystem>
#include <string>

class E2wxApp : public wxApp {
    const std::string id;
    const wxString version;
    std::filesystem::path logfile;
    std::filesystem::path resdir;
    std::filesystem::path conffile;
    std::filesystem::path confdir;
    std::filesystem::path docsdir;
    std::string arg_configfile;
    std::thread *thread_sdl;

    const std::filesystem::path BuildLogFilePath() const;
    void InitBoostLog();
    void StartSdlEpple2();

public:
    E2wxApp();
    virtual ~E2wxApp();

    const std::string GetID() const;
    const wxString GetVersion() const;
    const std::filesystem::path GetLogFile() const;
    const std::filesystem::path GetResDir() const;
    const std::filesystem::path GetConfigFile() const;
    const std::filesystem::path GetConfigDir() const;
    const std::filesystem::path GetDocumentsDir() const;

    virtual bool OnInit() override;
    virtual int OnExit() override;
    virtual void OnFatalException() override;
    virtual void OnInitCmdLine(wxCmdLineParser& parser) override;
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser) override;
};

wxDECLARE_APP(E2wxApp);

#endif /* E2WXAPP_H */
