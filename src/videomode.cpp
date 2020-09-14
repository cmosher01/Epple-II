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
#include "videomode.h"

VideoMode::VideoMode():
    pendText(0),
    pendMixed(0),
    pendHiRes(0) {
}

unsigned char VideoMode::io(const unsigned short addr, const unsigned char b) {
    const int sw = (addr & 0x6) >> 1;
    const bool on = addr & 0x1;

    // TODO verify all soft-switch change timings
    switch (sw) {
    case 0:
        if (this->swText != on) {
            this->pendText = 1;
        }
        break;
    case 1:
        if (this->swMixed != on) {
            this->pendMixed = 1;
        }
        break;
    case 2:
        this->pendPage2 = on;
//        if (this->swPage2 != on) {
//            this->pendPage2 = 1;
//        }
        break;
    case 3:
        this->swHiRes = on;
//        if (this->swHiRes != on) {
//            if (on) {
//                this->pendHiRes = 1;
//            } else {
//                this->pendHiRes = 2;
//            }
//        }
        break;
    }
    return b;
}

void VideoMode::tick() {
    if (this->pendText) {
        if (--this->pendText <= 0) {
            this->swText = !this->swText;
        }
    }
    if (this->pendMixed) {
        if (--this->pendMixed <= 0) {
            this->swMixed = !this->swMixed;
        }
    }
    if (this->pendPage2) {
        if (--this->pendPage2 <= 0) {
            this->swPage2 = !this->swPage2;
        }
    }
    if (this->pendHiRes) {
        if (--this->pendHiRes <= 0) {
            this->swHiRes = !this->swHiRes;
        }
    }
}

void VideoMode::powerOn() {
    this->swText = false;
    this->swMixed = false;
    this->swPage2 = 0;
    this->swHiRes = false;
}
