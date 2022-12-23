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
 * File:   KeyEventHandler.cpp
 * Author: chris.mosher
 * 
 * Created on December 19, 2022, 7:36 AM
 */



#include "KeyEventHandler.h"



/*
 5 POKE 49168,0
10 K =  PEEK (49152)
15 IF K < 128 GOTO 10
20 HI =  INT (K / 16)
30 LO = K - (HI * 16)
40 IF LO > 9 THEN LO = LO + 7
50 LO = LO + 48
60 IF HI > 9 THEN HI = HI + 7
70 HI = HI + 48
80 PRINT "$"; CHR$ (HI); CHR$ (LO)
90 GOTO 5
*/



static bool is_key_down(const wxKeyEvent& keyEvent) {
    const int sym = keyEvent.GetKeyCode();
    return (
        (sym < 0x7F || sym == WXK_LEFT || sym == WXK_RIGHT) &&
        !(sym == WXK_TAB || sym == '`' || sym == '[' || sym == '\\' || sym == WXK_DELETE) &&
        !(sym == ']' && keyEvent.ShiftDown())
    );
}

// Take real-world keystrokes from SDL and filter them to emulate the Apple ][ keyboard
static bool translate_key(const wxKeyEvent& keyEvent, unsigned char* key) {
    const int keycode = keyEvent.GetKeyCode();

    if (keycode == WXK_LEFT) {
        *key = 8;
        return true;
    }

    if (keycode == WXK_RIGHT) {
        *key = 21;
        return true;
    }

    if (keycode < 0 || 0x100 <= keycode) {
        return false;
    }

    *key = (unsigned char)(keycode & 0x7F);

    // The Apple ][ hardware keyboard only generates upper-case
    if ('a' <= *key && *key <= 'z') {
        *key -= 32;
    }

    // from SDL 1.2 to 2.0, we can't use UNICODE so we need to
    // apply shift and control modifiers ourselves
    if (keyEvent.ShiftDown()) {
        if (keycode == '`') *key = '~';
        else if (keycode == '1') *key = '!';
        else if (keycode == '2') *key = '@';
        else if (keycode == '3') *key = '#';
        else if (keycode == '4') *key = '$';
        else if (keycode == '5') *key = '%';
        else if (keycode == '6') *key = '^';
        else if (keycode == '7') *key = '&';
        else if (keycode == '8') *key = '*';
        else if (keycode == '9') *key = '(';
        else if (keycode == '0') *key = ')';
        else if (keycode == '-') *key = '_';
        else if (keycode == '=') *key = '+';

        else if (keycode == ';') *key = ':';
        else if (keycode == '\'') *key = '\"';

        else if (keycode == ',') *key = '<';
        else if (keycode == '.') *key = '>';
        else if (keycode == '/') *key = '?';

        else if (keycode == 'M') *key = ']';
        else if (keycode == 'N') *key = '^';
        else if (keycode == 'P') *key = '@';
    }

    if (keyEvent.RawControlDown()) {
        if (('A' <= *key && *key <= 'Z') || (*key == ']') || (*key == '^') || (*key == '@')) {
            *key -= 64;
        }
    }

    if (keyEvent.ShiftDown() && keyEvent.RawControlDown() && keycode == ' ') {
        // Ctrl-Shift-Space is the same as Space
        *key = ' ';
    } else if (keyEvent.RawControlDown() && !keyEvent.ShiftDown() && (('0' <= keycode && keycode <= '9') || keycode == '/' || keycode == ' ')) {
        // Control-only upon 0-9, / and space leaves them unchanged, the same as unmodified
        *key = keycode;
    } else if (keycode == ']') {
        if (keyEvent.ShiftDown()) {
            // ignore '}' (shift ']')
            return false;
        }
        if (keyEvent.RawControlDown()) {
            // Ctrl-] == ASCII: $1D
            *key = 29;
        }
    } // else if this is one of the *keys that can't be typed on an Apple ][ keyboard
    else if (*key == 0 || keycode == WXK_TAB || keycode == '`' || keycode == '[' || keycode == '\\' || keycode == WXK_DELETE) {
        return false;
    }

    return true;
}



KeyEventHandler::KeyEventHandler(KeypressQueue &q, KeyRepeatHandler &r) : keysDown(0), keypresses(q), repeater(r) {
}

KeyEventHandler::~KeyEventHandler() {
}



void KeyEventHandler::dispatchKeyDown(const wxKeyEvent& keyEvent) {
    if (keyEvent.IsAutoRepeat()) {
        // To repeat on the real Apple ][, you need to use the REPT key (emulated by F10)
        return;
    }

    const int sym = keyEvent.GetKeyCode();

    //printf("keydown:   mod: %04X   sym: %08X   scan:%04X   name:%s\n", mod, sym, scan, SDL_GetKeyName(sym));

    if (is_key_down(keyEvent)) {
        ++this->keysDown;
    }

    if (sym == WXK_F10) {
        this->repeater.press();
    } else {
        unsigned char key;
        const bool sendKey = translate_key(keyEvent, &key);
        if (sendKey) {
            //printf("    sending to apple as ASCII ------------------------------> %02X (%02X) (%d)\n", key, key | 0x80, key | 0x80);
            this->keypresses.push(key);
            this->repeater.setKey(key);
        }
    }
}

void KeyEventHandler::dispatchKeyUp(const wxKeyEvent& keyEvent) {
    const int sym = keyEvent.GetKeyCode();

    if (is_key_down(keyEvent)) {
        --this->keysDown;
        if (this->keysDown <= 0) {
            this->repeater.clearKey();
        }
    }

    if (sym == WXK_F10) {
        this->repeater.release();
    }
}
