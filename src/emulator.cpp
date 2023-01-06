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
#include "emulator.h"
#include "E2wxApp.h"
#include "e2config.h"
#include "e2command.h"
#include "e2const.h"
#include "KeyRepeatHandler.h"

#include <wx/msgdlg.h>
#include <wx/string.h>

#include <SDL.h>

#include <boost/log/trivial.hpp>

#include <ctime>







Emulator::Emulator() :
    screenImage(*this, keyEventHandler),
    display(screenImage),
    videoStatic(display),
    apple2(keypresses, paddleButtonStates, display, buffered, screenImage),
    keyEventHandler(keypresses, apple2.keyrepeater),
    timable(nullptr), // No ticked object (NULL pointer)
    prev_ms(SDL_GetTicks()) {
}

Emulator::~Emulator() {
}



void Emulator::config(E2Config& cfg) {
    cfg.parse(this->apple2.ram, this->apple2.rom, this->apple2.slts, this->apple2.revision, this->screenImage, this->apple2.cassetteIn, this->apple2.cassetteOut, &this->apple2);
    this->apple2.ram.dump_config();
    powerOffComputer();
    this->display.powerOn(true);
}



// How many emulation ticks between asking SDL if there is any new input
// from the user or other GUI events.
// This is also how often we shall update the estimate of the emulator's
// actual speed performance
// When the CPU is the object being ticked (each tick is a CPU cycle), then
// this is 20.04378892 Hz in emulated seconds time
#define CHECK_EVERY_CYCLE 51024

// The core of this Apple
void Emulator::tick50ms() {
    if (this->timable) {
        for (int i = 0; i < CHECK_EVERY_CYCLE; ++i) {
            this->timable->tick(); // this runs the emulator!
        }
    }

    this->screenImage.displayHz((1000*CHECK_EVERY_CYCLE)/(SDL_GetTicks() - this->prev_ms));
    this->prev_ms = SDL_GetTicks();
}





void Emulator::cmd(const wxString& c) {
    if (c.empty()) {
        return;
    }
    E2Command{}.parseLine(
        c.c_str().AsChar(),
        this->apple2.ram, this->apple2.rom, this->apple2.slts, this->apple2.revision, this->screenImage, this->apple2.cassetteIn, this->apple2.cassetteOut, nullptr);
}





static int askSave() {
    wxMessageDialog *dlg = new wxMessageDialog{
        nullptr,
        "You have unsaved changes to your floppy disk images.\nDo you want to SAVE them?",
        "Save changes",
        (long)(wxYES_NO|wxCANCEL|wxCANCEL_DEFAULT)};

    return dlg->ShowModal();
}

bool Emulator::isSafeToQuit() {
    BOOST_LOG_TRIVIAL(info) << "Checking for any unsaved changes...";

    this->screenImage.exitFullScreen();

    if (!this->apple2.cassetteOut.eject()) {
        // TODO does this handle the case where we fail to save?
        return false;
    }

    if (!this->apple2.slts.isDirty()) {
        return true;
    }

    const int resp = askSave();

    if (resp == wxID_CANCEL) {
        return false;
    }

    if (resp == wxID_YES) {
        this->apple2.slts.save(0);
        this->apple2.slts.save(1);
        // TODO handle case where we fail to save,
        // in which case we should alert the user,
        // and return false
    }

    return true;
}










void Emulator::toggleComputerPower() {
    if (this->timable == &this->videoStatic) {
        powerOnComputer();
    } else {
        powerOffComputer();
    }
}

void Emulator::powerOnComputer() {
    this->apple2.powerOn();
    this->screenImage.drawPower(true);
    this->display.setNoise(false);

    // The apple2 becomes the ticked object
    this->timable = &this->apple2;
}

void Emulator::powerOffComputer() {
    this->apple2.powerOff();
    this->screenImage.drawPower(false);
    this->display.setNoise(true);
    this->videoStatic.powerOn();

    // The video static becomes the ticked object
    this->timable = &this->videoStatic;
}

void Emulator::paste() {
    // Feed input from the clipboard to the Apple keyboard
    wxString s = this->clip.getText();
    s.Replace("\n\r", "\r\n");
    s.Replace("\r\n", "\r");
    s.Replace("\n", "\r");
    s.MakeUpper();
    for (auto c : s) {
        this->keypresses.push(c);
    }
}

void Emulator::monitorCycle() {
    this->display.cycleType();
    this->screenImage.cycleDisplayLabel();
}

void Emulator::reset() {
    this->apple2.reset();
}

void Emulator::toggleBuffered() {
    this->buffered.toggleBuffered();
    this->screenImage.toggleKdbBufferLabel();
}

void Emulator::toggleFullScreen() {
//    this->screenImage.toggleFullScreen();
//    this->screenImage.drawPower(this->timable == &this->apple2);
}

void Emulator::screenshot() {
    this->screenImage.saveBMP();
}
