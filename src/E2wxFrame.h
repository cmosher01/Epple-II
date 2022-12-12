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

#ifndef E2WXFRAME_H
#define E2WXFRAME_H

#include <wx/frame.h>
#include <wx/event.h>
#include <string>

class E2wxFrame : public wxFrame {
public:
    E2wxFrame();
    virtual ~E2wxFrame();

    void DoInit();

private:
    void OnExit(wxCommandEvent& event);
    void OnPreferences(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnTogglePower(wxCommandEvent& event);
    void HandleUserQuitRequest(wxCloseEvent& event);

    void InitMenuBar();
    void InitStatusBar();

    wxDECLARE_EVENT_TABLE();
};

#endif /* E2WXFRAME_H */
