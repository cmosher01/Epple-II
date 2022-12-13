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
#include "keyboard.h"
#include "keyboardbuffermode.h"
#include <cstdlib>



Keyboard::Keyboard(KeypressQueue& q, KeyboardBufferMode& buffered) :
    keys(q),
    buffered(buffered),
    latch(0),
    cGet(0) {
}

void Keyboard::powerOn() {
    this->latch = 0xA1u + std::rand() % (0xFEu-0xA1+1u);
}

void Keyboard::clear() {
    this->latch &= 0x7F;
}

unsigned char Keyboard::get() {
    if (!this->buffered.isBuffered() || !(this->latch & 0x80)) {
        if (!this->keys.empty()) {
            this->latch = this->keys.front() | 0x80;
            this->keys.pop();
        }
    }
    return this->latch;
}
