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
 * File:   KeyRepeatHandler.cpp
 * Author: chris.mosher
 * 
 * Created on December 19, 2022, 7:35 AM
 */

#include "KeyRepeatHandler.h"
#include "e2const.h"

// U.A.2 p. 7-13: REPT key repeats at 10Hz.
static const int CYCLES_PER_REPT(E2Const::AVG_CPU_HZ / 10);

KeyRepeatHandler::KeyRepeatHandler(KeypressQueue &q) :
    repeat(false),
    is_key_down(false),
    keypresses(q) {
}

KeyRepeatHandler::~KeyRepeatHandler() {
}

void KeyRepeatHandler::tick() {
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
            if (this->is_key_down) {
                // ...REPEAT the most recent one that was pressed
                this->keypresses.push(this->key);
            }
        }
    }
}

void KeyRepeatHandler::setKey(unsigned char lastKeyDown) {
    this->is_key_down = true;
    this->key = lastKeyDown;
}

void KeyRepeatHandler::clearKey() {
    this->is_key_down = false;
}

void KeyRepeatHandler::press() {
    this->repeat = true;
    this->rept = CYCLES_PER_REPT;
}

void KeyRepeatHandler::release() {
    this->repeat = false;
    this->rept = 0;
}
