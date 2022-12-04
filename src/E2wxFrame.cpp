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

#include "E2wxFrame.h"
#include "E2wxApp.h"
//#include "PreferencesDialog.h"
#include <wx/persist/toplevel.h>



wxBEGIN_EVENT_TABLE(E2wxFrame, wxFrame)
    EVT_MENU(wxID_EXIT, E2wxFrame::OnExit)
    EVT_MENU(wxID_PREFERENCES, E2wxFrame::OnPreferences)
    EVT_MENU(wxID_ABOUT, E2wxFrame::OnAbout)
wxEND_EVENT_TABLE()



E2wxFrame::E2wxFrame() : wxFrame(nullptr, wxID_ANY, "epple2") {
}

E2wxFrame::~E2wxFrame() {
}

void E2wxFrame::DoInit() {
    InitMenuBar();
    InitStatusBar();

    if (!wxPersistentRegisterAndRestore(this, "main")) {
        Center();
    }
}

void E2wxFrame::InitMenuBar() {
    wxMenuBar *menuBar = new wxMenuBar();
    SetMenuBar(menuBar);

    wxMenu *menuFile = new wxMenu();
    menuBar->Append(menuFile, "&File");
    menuFile->Append(wxID_EXIT);

    wxMenu *menuEdit = new wxMenu();
    menuBar->Append(menuEdit, "&Edit");
    menuEdit->Append(wxID_PREFERENCES);

    wxMenu *menuHelp = new wxMenu();
    menuBar->Append(menuHelp, "&Help");
    menuHelp->Append(wxID_ABOUT);
}

void E2wxFrame::InitStatusBar() {
    CreateStatusBar();
    SetStatusText(wxT("Welcome to ")+wxGetApp().GetID());
}



void E2wxFrame::OnExit(wxCommandEvent& event) {
    Close(true);
}

void E2wxFrame::OnAbout(wxCommandEvent& event) {
    wxString msg = "";

    msg += wxGetApp().GetID()+wxT("\n");

    msg += wxT("version: ")+wxGetApp().GetVersion()+wxT("\n");

    msg += wxT("Current log file:\n");
    msg += wxGetApp().GetLogFile().c_str();

    wxMessageBox(msg, wxT("About ")+wxGetApp().GetID(), wxOK | wxICON_INFORMATION);
}

void E2wxFrame::OnPreferences(wxCommandEvent& event) {
//    PreferencesDialog *dlg = new PreferencesDialog(this);
//    dlg->OnInit();
//    dlg->ShowModal();
}
