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
#ifndef EMULATOR_H
#define EMULATOR_H

#include "keyboard.h"
#include "paddlebuttonstates.h"
#include "apple2.h"
#include "videostaticgenerator.h"
#include "screenimage.h"
#include "analogtv.h"
#include "keyboardbuffermode.h"
#include "hypermode.h"
#include "clipboardhandler.h"
#include <SDL.h>

class Timable;
class E2Config;

class Emulator {
    PaddleButtonStates paddleButtonStates;
    KeypressQueue keypresses;

    HyperMode fhyper;
    KeyboardBufferMode buffered;
    ScreenImage screenImage;
    AnalogTV display;
    VideoStaticGenerator videoStatic;
    Apple2 apple2;
    ClipboardHandler clip;

    Timable* timable;

    bool repeat;
    int keysDown;
    int rept;
    unsigned char lastKeyDown;
    Uint32 prev_ms;
    bool command;
    bool pendingCommandExit;
    std::string cmdline;

    void dispatchKeyDown(const SDL_KeyboardEvent& keyEvent);
    void dispatchKeyUp(const SDL_KeyboardEvent& keyEvent);
    void cmdKey(const SDL_KeyboardEvent& keyEvent);
    void processCommand();
    void powerOnComputer();
    void powerOffComputer();

    void handleRepeatKey();
    void handleAnyPendingEvents();

public:
    Emulator();
    virtual ~Emulator();

    void config(E2Config& cfg);
    void tick50ms();
    bool isSafeToQuit();

    void toggleComputerPower();
    void monitorCycle();
    void paste();
    void reset();
    void toggleBuffered();
    void toggleFullScreen();
    void screenshot();
};

#endif
