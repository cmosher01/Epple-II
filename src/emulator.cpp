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

#include <wx/msgdlg.h>

#include <SDL.h>

#include <ctime>






static bool isKeyDown(const SDL_Keycode sym, const SDL_Keymod mod) {
    return (
        (sym < 0x7F || sym == SDLK_LEFT || sym == SDLK_RIGHT) &&
        !(sym == SDLK_TAB || sym == SDLK_BACKQUOTE || sym == '[' || sym == '\\' || sym == SDLK_DELETE) &&
        !(sym == ']' && mod & KMOD_SHIFT)
    );
}





Emulator::Emulator() :
    display(screenImage),
    videoStatic(display),
    apple2(keypresses, paddleButtonStates, display, fhyper, buffered, screenImage),
    timable(nullptr), // No ticked object (NULL pointer)
    repeat(false),
    keysDown(0),
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


// U.A.2 p. 7-13: REPT key repeats at 10Hz.
static const int CYCLES_PER_REPT(E2Const::AVG_CPU_HZ / 10);

// If the Apple ][ keyboard repeat is on (the REPT key is
// down)...
void Emulator::handleRepeatKey() {
    if (this->repeat) {
        // Count our way down to when the timer for the REPT key
        // fires off: 10Hz in terms of how many CPU cycles have gone
        // by
        --this->rept;
        // If it's time for the REPT key timer to fire (at long
        // last)...
        if (this->rept <= 0) {
            // ...reload the timer for the next firing 1/10 second from
            // now ( *reset* the timer )
            this->rept = CYCLES_PER_REPT;
            // If any other keys are actually being held down...
            if (this->keysDown > 0) {
                // ...REPEAT the most recent one that was pressed
                this->keypresses.push(this->lastKeyDown);
            }
        }
    }
}

void Emulator::handleAnyPendingEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                // If SDL is going away...
                // could be due to user closing the SDL window, pressing cmd-Q on Mac,
                // ctrl-C from the command line on Linux, process being killed, etc.
                wxGetApp().CloseMainFrame();
            break;
            case SDL_KEYDOWN:
                // we're collecting keypresses for the keyboard
                // emulation (and thus the Apple ][ emulation itself)
                dispatchKeyDown(event.key);
                // People who have too many press-releases should be referred to as "keyboards"
            break;
            case SDL_KEYUP:
                dispatchKeyUp(event.key);
            break;
        }
    }
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
            handleRepeatKey();
        }
    }
    handleAnyPendingEvents();
    this->screenImage.displayHz((1000*CHECK_EVERY_CYCLE)/(SDL_GetTicks() - this->prev_ms));
    this->prev_ms = SDL_GetTicks();
}



static bool translateKeysToAppleModernized(SDL_Keycode keycode, SDL_Keymod modifiers, unsigned char* key) {
    if (keycode == SDLK_LEFT) {
        *key = 8;
        return true;
    } else if (keycode == SDLK_RIGHT) {
        *key = 21;
        return true;
    }

    if (keycode >= 0x100) {
        return false;
    }

    *key = (unsigned char) (keycode & 0x7F);

    // The Apple ][ hardware keyboard only generates upper-case
    if ('a' <= *key && *key <= 'z') {
        *key -= 32;
    }

    // from SDL 1.2 to 2.0, we can't use UNICODE so we need to
    // apply shift and control modifiers ourselves
    if (modifiers & KMOD_SHIFT) {
        if (keycode == SDLK_BACKQUOTE) *key = '~';
        else if (keycode == SDLK_1) *key = '!';
        else if (keycode == SDLK_2) *key = '@';
        else if (keycode == SDLK_3) *key = '#';
        else if (keycode == SDLK_4) *key = '$';
        else if (keycode == SDLK_5) *key = '%';
        else if (keycode == SDLK_6) *key = '^';
        else if (keycode == SDLK_7) *key = '&';
        else if (keycode == SDLK_8) *key = '*';
        else if (keycode == SDLK_9) *key = '(';
        else if (keycode == SDLK_0) *key = ')';
        else if (keycode == SDLK_MINUS) *key = '_';
        else if (keycode == SDLK_EQUALS) *key = '+';

        else if (keycode == SDLK_SEMICOLON) *key = ':';
        else if (keycode == SDLK_QUOTE) *key = '\"';

        else if (keycode == SDLK_COMMA) *key = '<';
        else if (keycode == SDLK_PERIOD) *key = '>';
        else if (keycode == SDLK_SLASH) *key = '?';

        else if (keycode == SDLK_m) *key = ']';
        else if (keycode == SDLK_n) *key = '^';
        else if (keycode == SDLK_p) *key = '@';
    }

    if (modifiers & KMOD_CTRL) {
        if (('A' <= *key && *key <= 'Z') || (*key == ']') || (*key == '^') || (*key == '@')) {
            *key -= 64;
        }
    }

    if ((modifiers & KMOD_SHIFT) && (modifiers & KMOD_CTRL) && keycode == ' ') {
        // Ctrl-Shift-Space is the same as Space
        *key = ' ';
    } else if ((modifiers & KMOD_CTRL) && !(modifiers & KMOD_SHIFT) && (('0' <= keycode && keycode <= '9') || keycode == '/' || keycode == ' ')) {
        // Control-only upon 0-9, / and space leaves them unchanged, the same as unmodified
        *key = keycode;
    } else if (keycode == ']') {
        if (modifiers & KMOD_SHIFT) {
            // ignore '}' (shift ']')
            return false;
        }
        if (modifiers & KMOD_CTRL) {
            // Ctrl-] == ASCII: $1D
            *key = 29;
        }
    } // else if this is one of the *keys that can't be typed on an Apple ][ keyboard
    else if (*key == 0 || keycode == SDLK_TAB || keycode == SDLK_BACKQUOTE || keycode == '[' || keycode == '\\' || keycode == SDLK_DELETE) {
        return false;
    }

    return true;
}

// Take real-world keystrokes from SDL and filter them to emulate the Apple ][ keyboard
void Emulator::dispatchKeyDown(const SDL_KeyboardEvent& keyEvent) {
    if (keyEvent.repeat) {
        return;
    }

    const SDL_Keycode sym = keyEvent.keysym.sym;
    const SDL_Keymod mod = (SDL_Keymod) keyEvent.keysym.mod;

    //printf("keydown:   mod: %04X   sym: %08X   scan:%04X   name:%s\n", mod, sym, scan, SDL_GetKeyName(sym));

    if (isKeyDown(sym, mod)) {
        ++this->keysDown;
    }

    if (sym == SDLK_F10) {
        // ...start auto-repeat
        this->repeat = true;
        this->rept = CYCLES_PER_REPT;
        return;
    } else if (SDLK_F1 <= sym && sym <= SDLK_F12) {
        wxGetApp().OnFnKeyPressed(sym);
        return;
    }

    unsigned char key;
    const bool sendKey = translateKeysToAppleModernized(sym, mod, &key);

    if (sendKey) {
        //printf("    sending to apple as ASCII ------------------------------> %02X (%02X) (%d)\n", key, key | 0x80, key | 0x80);
        this->keypresses.push(key);
        this->lastKeyDown = key;
    }
}

void Emulator::dispatchKeyUp(const SDL_KeyboardEvent& keyEvent) {
    const SDL_Keycode sym = keyEvent.keysym.sym;
    const SDL_Keymod mod = (SDL_Keymod) keyEvent.keysym.mod;

    if (isKeyDown(sym, mod)) {
        --this->keysDown;
    } else if (sym == SDLK_F10) {
        // ...else if this is the emulated REPT key on the Apple keyboard...
        // ...stop repeating. The key has been released
        this->repeat = false;
        this->rept = 0;
    }
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
    if (this->timable == &this->videoStatic)
        powerOnComputer();
    else
        powerOffComputer();
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
    std::string s = this->clip.getText();
    for (unsigned int i = 0; i < s.length(); ++i) {
        unsigned char key = s[i];
        // TODO fix pasting line-endings
        if (key == '\n')
            key = '\r';
        if ('a' <= key && key <= 'z') {
            key -= 32;
        }
        this->keypresses.push(key);
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
    this->screenImage.toggleFullScreen();
    this->screenImage.drawPower(this->timable == &this->apple2);
}

void Emulator::screenshot() {
    this->screenImage.saveBMP();
}
