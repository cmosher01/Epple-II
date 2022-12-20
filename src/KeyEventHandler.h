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
 */

/* 
 * File:   KeyEventHandler.h
 * Author: chris.mosher
 *
 * Created on December 19, 2022, 7:36 AM
 */

#ifndef KEYEVENTHANDLER_H
#define KEYEVENTHANDLER_H

#include "KeyRepeatHandler.h"
#include "keyboard.h"
#include <SDL.h>

class KeyEventHandler {
    int keysDown;

    KeypressQueue &keypresses;
    KeyRepeatHandler &repeater;

public:
    KeyEventHandler(KeypressQueue &keypresses, KeyRepeatHandler &repeater);
    virtual ~KeyEventHandler();

    void dispatchKeyDown(const SDL_KeyboardEvent& keyEvent);
    void dispatchKeyUp(const SDL_KeyboardEvent& keyEvent);
};

#endif /* KEYEVENTHANDLER_H */
