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
#include "PreferencesDialog.h"
#include "gui.h"

#include <wx/menu.h>
#include <wx/accel.h>
#include <wx/msgdlg.h>
#include <wx/event.h>
#include <wx/persist/toplevel.h>

#include <boost/log/trivial.hpp>

#include <iostream>



wxBEGIN_EVENT_TABLE(E2wxFrame, wxFrame)
    EVT_CLOSE(E2wxFrame::HandleUserQuitRequest)
    EVT_MENU(wxID_EXIT, E2wxFrame::OnFileExitCommand)
    EVT_MENU(wxID_PREFERENCES, E2wxFrame::OnPreferences)
    EVT_MENU(wxID_ABOUT, E2wxFrame::OnAbout)
    EVT_MENU(ID_MENUITEM_POWER, E2wxFrame::OnTogglePower)
    EVT_MENU(ID_MENUITEM_CYCLE_MONITOR, E2wxFrame::OnCycleMonitor)
    EVT_MENU(ID_MENUITEM_TOGGLE_FULL_SCREEN, E2wxFrame::OnToggleFullScreen)
    EVT_MENU(ID_MENUITEM_EMULATOR_COMMAND, E2wxFrame::OnEmulatorCommand)
    EVT_MENU(ID_MENUITEM_RESET, E2wxFrame::OnReset)
    EVT_MENU(wxID_PASTE, E2wxFrame::OnPaste)
    EVT_MENU(ID_MENUITEM_SCREEN_SHOT, E2wxFrame::OnScreenShot)
    EVT_MENU(ID_MENUITEM_TOGGLE_BUFFERED, E2wxFrame::OnToggleBuffered)
    EVT_MENU(ID_MENUITEM_START_EMULATOR, E2wxFrame::OnStartEmulator)
    EVT_MENU(ID_MENUITEM_STOP_EMULATOR, E2wxFrame::OnStopEmulator)
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
    // TODO move menu def to xrc
    wxMenuBar *menuBar = new wxMenuBar();
    SetMenuBar(menuBar);

    wxMenu *menuFile = new wxMenu();
    menuBar->Append(menuFile, "&File");
    wxMenuItem *miStart = menuFile->Append(ID_MENUITEM_START_EMULATOR, "Open Emulator");
    miStart->SetAccel(new wxAcceleratorEntry(wxACCEL_CTRL, 'O'));
    wxMenuItem *miStop = menuFile->Append(ID_MENUITEM_STOP_EMULATOR, "Close Emulator");
    miStop->SetAccel(new wxAcceleratorEntry(wxACCEL_CTRL, 'W'));
    menuFile->AppendSeparator();
    wxMenuItem *miExit = menuFile->Append(wxID_EXIT);
    miExit->AddExtraAccel(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_F9));

    wxMenu *menuEdit = new wxMenu();
    menuBar->Append(menuEdit, "&Edit");
    wxMenuItem *miPaste = menuEdit->Append(wxID_PASTE);
    miPaste->AddExtraAccel(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_F7));
    menuEdit->AppendSeparator();
    wxMenuItem *miPrefs = menuEdit->Append(wxID_PREFERENCES);
    miPrefs->SetAccel(new wxAcceleratorEntry(wxACCEL_CTRL, ','));


    wxMenu *menuMachine = new wxMenu();
    menuBar->Append(menuMachine, "&Machine");
    wxMenuItem *miPower = menuMachine->Append(ID_MENUITEM_POWER, "Toggle Power");
    miPower->SetAccel(new wxAcceleratorEntry(wxACCEL_NORMAL, WXK_F1));
    wxMenuItem *miReset = menuMachine->Append(ID_MENUITEM_RESET, "Reset");
    miReset->SetAccel(new wxAcceleratorEntry(wxACCEL_NORMAL, WXK_F6));
    menuMachine->AppendSeparator();
    wxMenuItem *miCmd = menuMachine->Append(ID_MENUITEM_EMULATOR_COMMAND, "Command...");
    miCmd->SetAccel(new wxAcceleratorEntry(wxACCEL_NORMAL, WXK_F5));
    menuMachine->AppendSeparator();
    wxMenuItem *miBuffered = menuMachine->Append(ID_MENUITEM_TOGGLE_BUFFERED, "Toggle Keyboard Buffer");
    miBuffered->SetAccel(new wxAcceleratorEntry(wxACCEL_NORMAL, WXK_F12));

    wxMenu *menuMonitor = new wxMenu();
    menuBar->Append(menuMonitor, "Mo&nitor");
    wxMenuItem *miCycleMonitor = menuMonitor->Append(ID_MENUITEM_CYCLE_MONITOR, "Cycle Type");
    miCycleMonitor->SetAccel(new wxAcceleratorEntry(wxACCEL_NORMAL, WXK_F2));
    wxMenuItem *miToggleFullScreen = menuMonitor->Append(ID_MENUITEM_TOGGLE_FULL_SCREEN, "Toggle Full Screen");
    miToggleFullScreen->SetAccel(new wxAcceleratorEntry(wxACCEL_NORMAL, WXK_F3));
    menuMonitor->AppendSeparator();
    wxMenuItem *miScreenShot = menuMonitor->Append(ID_MENUITEM_SCREEN_SHOT, "Screen Shot!");
    miScreenShot->SetAccel(new wxAcceleratorEntry(wxACCEL_NORMAL, WXK_F8));

    wxMenu *menuHelp = new wxMenu();
    menuBar->Append(menuHelp, "&Help");
    menuHelp->Append(wxID_ABOUT);
}

void E2wxFrame::InitStatusBar() {
    CreateStatusBar();
    SetStatusText("Welcome to "+wxGetApp().GetAppDisplayName());
}



void E2wxFrame::HandleUserQuitRequest(wxCloseEvent& event) {
    BOOST_LOG_TRIVIAL(info) << "Main frame will handle request to close application...";
    if (!event.CanVeto()) {
        BOOST_LOG_TRIVIAL(warning) << "Application is being forced to quit; any unsaved changes will be discarded.";
        // TODO how to handle !event.CanVeto() ?
        // I'd like to auto-save everything
        Destroy();
    } else if (wxGetApp().EnsureCanQuit()) {
        BOOST_LOG_TRIVIAL(info) << "Destroying main frame now...";
        Destroy();
    } else {
        BOOST_LOG_TRIVIAL(info) << "User cancelled closing main frame.";
        event.Veto();
    }
}



void E2wxFrame::OnFileExitCommand(wxCommandEvent& event) {
    Close(false);
}

void E2wxFrame::OnPreferences(wxCommandEvent& event) {
    PreferencesDialog *dlg = new PreferencesDialog(this);
    if (dlg->OnInit()) {
        dlg->ShowModal();
        dlg->Destroy();
    }
    wxGetApp().StartEmulator();
}

void E2wxFrame::OnAbout(wxCommandEvent& event) {
    wxString msg = "";

    msg += wxGetApp().GetVendorName();
    msg += ".";
    msg += wxGetApp().GetAppName();
    msg += "\n";

    msg += "version: "+wxGetApp().GetVersion()+"\n";

    msg += "Current log file:\n";
    msg += wxGetApp().GetLogFile().c_str();

    wxMessageBox(msg, "About "+wxGetApp().GetAppDisplayName(), wxOK | wxICON_INFORMATION);
}



void E2wxFrame::OnTogglePower(wxCommandEvent& event) {
    wxGetApp().TogglePower();
}

void E2wxFrame::OnCycleMonitor(wxCommandEvent& event) {
    wxGetApp().CycleMonitor();
}

void E2wxFrame::OnToggleFullScreen(wxCommandEvent& event) {
//    wxGetApp().ToggleFullScreen();
}

void E2wxFrame::OnEmulatorCommand(wxCommandEvent& event) {
    wxGetApp().EmulatorCommand();
}

void E2wxFrame::OnReset(wxCommandEvent& event) {
    wxGetApp().Reset();
}

void E2wxFrame::OnPaste(wxCommandEvent& event) {
    wxGetApp().Paste();
}

void E2wxFrame::OnScreenShot(wxCommandEvent& event) {
    wxGetApp().ScreenShot();
}

void E2wxFrame::OnToggleBuffered(wxCommandEvent& event) {
    wxGetApp().ToggleBuffered();
}

void E2wxFrame::OnStartEmulator(wxCommandEvent& event) {
    wxGetApp().StartEmulator();
}

void E2wxFrame::OnStopEmulator(wxCommandEvent& event){
    wxGetApp().StopEmulator();
}
