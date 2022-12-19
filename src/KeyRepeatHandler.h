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
 * File:   KeyRepeatHandler.h
 * Author: chris.mosher
 *
 * Created on December 19, 2022, 7:35 AM
 */

#ifndef KEYREPEATHANDLER_H
#define KEYREPEATHANDLER_H

#include "keyboard.h"

class KeyRepeatHandler {
    bool repeat;
    int rept;
    bool is_key_down;
    unsigned char key;

    KeypressQueue &keypresses;

public:
    KeyRepeatHandler(KeypressQueue &keypresses);
    virtual ~KeyRepeatHandler();

    virtual void tick();

    void setKey(unsigned char lastKeyDown);
    void clearKey();

    void press();
    void release();
};

#endif /* KEYREPEATHANDLER_H */
