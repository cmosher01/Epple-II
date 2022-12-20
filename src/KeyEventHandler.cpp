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

static bool isKeyDown(const SDL_Keycode sym, const SDL_Keymod mod) {
    return (
        (sym < 0x7F || sym == SDLK_LEFT || sym == SDLK_RIGHT) &&
        !(sym == SDLK_TAB || sym == SDLK_BACKQUOTE || sym == '[' || sym == '\\' || sym == SDLK_DELETE) &&
        !(sym == ']' && mod & KMOD_SHIFT)
    );
}

static bool translateKeysToAppleModernized(SDL_Keycode keycode, SDL_Keymod modifiers, unsigned char* key) {
    if (keycode == SDLK_LEFT) {
        *key = 8;
        return true;
    }

    if (keycode == SDLK_RIGHT) {
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



KeyEventHandler::KeyEventHandler(KeypressQueue &q, KeyRepeatHandler &r) : keysDown(0), keypresses(q), repeater(r) {
}

KeyEventHandler::~KeyEventHandler() {
}


// Take real-world keystrokes from SDL and filter them to emulate the Apple ][ keyboard
void KeyEventHandler::dispatchKeyDown(const SDL_KeyboardEvent& keyEvent) {
    if (keyEvent.repeat) {
        // To repeat on the real Apple ][, you need to use the REPT key (emulated by F10)
        return;
    }

    const SDL_Keycode sym = keyEvent.keysym.sym;
    const SDL_Keymod mod = (SDL_Keymod)keyEvent.keysym.mod;

    //printf("keydown:   mod: %04X   sym: %08X   scan:%04X   name:%s\n", mod, sym, scan, SDL_GetKeyName(sym));

    if (isKeyDown(sym, mod)) {
        ++this->keysDown;
    }

    if (sym == SDLK_F10) {
        this->repeater.press();
//    } else if (SDLK_F1 <= sym && sym <= SDLK_F12) {
//        wxGetApp().OnFnKeyPressed(sym);
    } else {
        unsigned char key;
        const bool sendKey = translateKeysToAppleModernized(sym, mod, &key);
        if (sendKey) {
            //printf("    sending to apple as ASCII ------------------------------> %02X (%02X) (%d)\n", key, key | 0x80, key | 0x80);
            this->keypresses.push(key);
            this->repeater.setKey(key);
        }
    }
}

void KeyEventHandler::dispatchKeyUp(const SDL_KeyboardEvent& keyEvent) {
    const SDL_Keycode sym = keyEvent.keysym.sym;
    const SDL_Keymod mod = (SDL_Keymod)keyEvent.keysym.mod;

    if (isKeyDown(sym, mod)) {
        --this->keysDown;
        if (this->keysDown <= 0) {
            this->repeater.clearKey();
        }
    }

    if (sym == SDLK_F10) {
        this->repeater.release();
    }
}
