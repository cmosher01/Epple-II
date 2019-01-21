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
#include "configep2.h"
#include "e2const.h"
#include "tinyfiledialogs.h"

#include <SDL2/SDL.h>

#include <ctime>

Emulator::Emulator() :
display(screenImage),
videoStatic(display),
apple2(keypresses, paddleButtonStates, display, fhyper, buffered, screenImage),
timable(0), // No ticked object (NULL pointer)
quit(false),
repeat(false),
keysDown(0),
command(false),
pendingCommandExit(false) {
}

Emulator::~Emulator() {
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
    // TODO Need to ask user if OK to lose any unsaved changes to disks
    this->apple2.powerOff();
    this->screenImage.drawPower(false);
    this->display.setNoise(true);
    this->videoStatic.powerOn();

    // The video static becomes the ticked object
    this->timable = &this->videoStatic;
}

void Emulator::config(Config& cfg) {
    cfg.parse(this->apple2.ram, this->apple2.rom, this->apple2.slts, this->apple2.revision, this->screenImage, this->apple2.cassetteIn, this->apple2.cassetteOut);
}

void Emulator::init() {
    powerOffComputer();
    this->display.setType(AnalogTV::MONITOR_COLOR);
    this->display.powerOn(true);
}

// How many emulation ticks between asking SDL if there is any new input
// from the user or other GUI events.
// This is also how often we shall update the estimate of the emulator's
// actual speed performance
// When the CPU is the object being ticked (each tick is a CPU cycle), then
// this is 20.04378892 Hz in emulated seconds time
#define CHECK_EVERY_CYCLE 51024
#define CHECK_CYCLES_K 51024000
#define EXPECTED_MS 50

// U.A.2 p. 7-13: REPT key repeats at 10Hz.
static const int CYCLES_PER_REPT(E2Const::AVG_CPU_HZ / 10);


// The core of this Apple

int Emulator::run() {
    int skip = CHECK_EVERY_CYCLE;
    Uint32 prev_ms = SDL_GetTicks();
    // While the user still wants to run this emulation...
    while (!this->quit) {
        // (Obligatory protection against NULL object pointer)
        if (this->timable) {
            this->timable->tick();
            // If the Apple ][ keyboard repeat is on (the REPT key is
            // down)...
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

        // People who have too many press releases should be referred to as
        // keyboards

        --skip;
        // If skip has been decremented to zero...
        if (!skip) {
            // ...then it's time to drain away any piled-up user interaction
            // events that SDL has stored up for us
            // Reload the skip quantity
            skip = CHECK_EVERY_CYCLE;
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                        // If SDL is going away...
                    case SDL_QUIT:
                        if (isSafeToQuit()) {
                            this->quit = true;
                        }
                        break;
                    case SDL_KEYDOWN:
                        // If we're collecting a command line for changing any
                        // of the configurables of the emulator...
                        if (this->command)
                            cmdKey(event.key);
                        else
                            // ...else we're collecting keypresses for the keyboard
                            // emulation (and thus the Apple ][ emulation itself)
                            dispatchKeypress(event.key);
                        break;
                    case SDL_KEYUP:
                        // If we're collecting a command line for changing any
                        // of the configurables of the emulator...
                        if (this->command) {
                            if (this->pendingCommandExit) {
                                this->command = false;
                                this->pendingCommandExit = false;
                            }
                        } else {
                            // ...else we're collecting keypresses for the keyboard
                            // emulation (and thus the Apple ][ emulation itself)
                            dispatchKeyUp(event.key);
                        }
                        break;
                }
            }
            // If we're trying to run as slow as a real Apple ][...
            if (!this->fhyper.isHyper()) {
                const int delta_ms = EXPECTED_MS - (SDL_GetTicks() - prev_ms);
                if (0 < delta_ms && delta_ms <= EXPECTED_MS) {
                    SDL_Delay(delta_ms);
                }

            }
            // Display the current estimate of the emulator's actual speed
            // performance
            this->screenImage.displayHz(CHECK_CYCLES_K / (SDL_GetTicks() - prev_ms));
            prev_ms = SDL_GetTicks();
        }
    }
    return 0;
}

void Emulator::dispatchKeyUp(const SDL_KeyboardEvent& keyEvent) {
    SDL_Keycode sym = keyEvent.keysym.sym;
    SDL_Keymod mod = (SDL_Keymod) keyEvent.keysym.mod;

    if ((sym < 0x7F || sym == SDLK_LEFT || sym == SDLK_RIGHT) &&
            !(sym == SDLK_TAB || sym == SDLK_BACKQUOTE || sym == '[' || sym == '\\' || sym == SDLK_DELETE) &&
            !(sym == ']' && mod & KMOD_SHIFT)) {
        --this->keysDown;
    }// ...else if this is the emulated REPT key on the Apple keyboard...
    else if (sym == SDLK_F10) {
        // ...stop repeating. The key has been released
        this->repeat = false;
        this->rept = 0;
    }
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



// Take real-world keystrokes from SDL and filter them to emulate the
// Apple ][ or Apple ][ plus keyboard

void Emulator::dispatchKeypress(const SDL_KeyboardEvent& keyEvent) {
    if (keyEvent.repeat) {
        return;
    }

    SDL_Keycode sym = keyEvent.keysym.sym;
    SDL_Keymod mod = (SDL_Keymod) keyEvent.keysym.mod;

    //printf("keydown:   mod: %04X   sym: %08X   scan:%04X   name:%s\n", mod, sym, scan, SDL_GetKeyName(sym));

    if ((sym < 0x7F || sym == SDLK_LEFT || sym == SDLK_RIGHT) &&
            !(sym == SDLK_TAB || sym == SDLK_BACKQUOTE || sym == '[' || sym == '\\' || sym == SDLK_DELETE) &&
            !(sym == ']' && mod & KMOD_SHIFT)) {
        ++this->keysDown;
    }



    /*
     * First handle the key presses that are for commands, as opposed to
     * simple key-presses that we send to the apple.
     */

    if (sym == SDLK_F6) {
        this->apple2.reset();
        return;
    } else if (sym == SDLK_F7) {
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
        return;
    }// ...else if this is the emulated REPT key on the Apple keyboard...
    else if (sym == SDLK_F10) {
        // ...start auto-repeat
        this->repeat = true;
        this->rept = CYCLES_PER_REPT;
        return;
    }// ...else if the user wants to run at full speed instead of emulating
        // the Apple's speed...
    else if (sym == SDLK_F11) {
        this->fhyper.toggleHyper();
        this->screenImage.toggleHyperLabel();
        return;
    } else if (sym == SDLK_F12) {
        this->buffered.toggleBuffered();
        this->screenImage.toggleKdbBufferLabel();
        return;
    }// ...else if the user has hit the rocker switch on the back of the Apple...
    else if (sym == SDLK_F1) {
        toggleComputerPower();
        return;
    }// ...else if the user wants to look at a different video display medium...
    else if (sym == SDLK_F2) {
        this->display.cycleType();
        this->screenImage.cycleDisplayLabel();
        return;
    }// ...else if the user wants to switch to/from full screen and an
        // individual application window...
    else if (sym == SDLK_F3) {
        this->screenImage.toggleFullScreen();
        this->screenImage.drawPower(this->timable == &this->apple2);
        return;
    }// ...else if the user wants to switch between the interlaced extension
        // of the display and the non-interlaced historically correct display...
    else if (sym == SDLK_F4) {
        this->display.toggleBleedDown();
        this->screenImage.toggleFillLinesLabel();
        return;
    }// ...else initiate command line entry at the bottom of the emulator window
    else if (sym == SDLK_F5) {
        this->command = true;
        this->screenImage.enterCommandMode();
        return;
    }// ...else exit the entire emulation
    else if (sym == SDLK_F9) {
        this->screenImage.exitFullScreen();
        if (isSafeToQuit()) {
            this->quit = true;
        }
        return;
    }// ...else save a screen shot
    else if (sym == SDLK_F8) {
        this->screenImage.saveBMP();
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


// Collect and edit a command line typed at the bottom of the emulator
// window

void Emulator::cmdKey(const SDL_KeyboardEvent& keyEvent) {
    SDL_Keycode sym = keyEvent.keysym.sym;
    unsigned char key = (unsigned char) (sym & 0x7F);

    if (sym == SDLK_RETURN) {
        processCommand();
    } else if (sym == SDLK_ESCAPE) {
        cmdline.erase(cmdline.begin(), cmdline.end());
        processCommand();
    } else if (sym == SDLK_BACKSPACE) {
        if (cmdline.length()) {
            cmdline.erase(cmdline.end() - 1);
            this->screenImage.backspaceCommand();
        }
    } else if (sym == SDLK_INSERT) {
        std::string s = this->clip.getText();
        for (unsigned int i = 0; i < s.length(); ++i) {
            key = s[i];
            if (key == '\n' || key == '\r') {
                processCommand();
                break;
            } else {
                cmdline += key;
                this->screenImage.addkeyCommand(key);
            }
        }
    } else if (key) {
        cmdline += key;
        this->screenImage.addkeyCommand(key);
    }
}

// Process a command line typed at the bottom of the emulator window

void Emulator::processCommand() {
    this->screenImage.exitCommandMode();
    this->screenImage.drawPower(this->timable == &this->apple2);
    this->pendingCommandExit = true;

    if (cmdline.empty()) {
        return;
    }

    Config::parseLine(cmdline, this->apple2.ram, this->apple2.rom, this->apple2.slts, this->apple2.revision, this->screenImage, this->apple2.cassetteIn, this->apple2.cassetteOut);
    cmdline.erase(cmdline.begin(), cmdline.end());
}

bool Emulator::isSafeToQuit() {
    if (!this->apple2.slts.isDirty()) {
        return true;
    }

    const int resp = tinyfd_messageBox(
        "Save changes",
        "You have unsaved changes to your floppy disk images.\nDo you want to SAVE them?",
        "yesnocancel",
        "warning",
        0);

    if (resp == 0) { // cancel
        return false;
    }

    if (resp == 1) { // yes (save)
        this->apple2.slts.save(0);
        this->apple2.slts.save(1);
    }

    return true;
}
