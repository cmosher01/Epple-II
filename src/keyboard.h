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
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <queue>
#include <cstdint>

typedef std::queue<unsigned char> KeypressQueue;

class KeyboardBufferMode;

class Keyboard {
    KeypressQueue& keys;
    KeyboardBufferMode& buffered;

    unsigned char latch;
    unsigned char cGet;
    uint32_t lastGet;

public:
    Keyboard(KeypressQueue& q, KeyboardBufferMode& buffered);
    void powerOn();
    void clear();
    unsigned char get();
};

#endif
