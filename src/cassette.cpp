/*
    epple2

    Copyright © 2008, 2019, Christopher Alan Mosher, Shelton, CT, USA. <cmosher01@gmail.com>

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
#include <fstream>
#include <iostream>
#include <cstdlib>
#include "tinyfiledialogs.h"
#include "cassette.h"

Cassette::Cassette(ScreenImage& gui):
    gui(gui),
    t(0),
    t_active(0),
    playing(false),
    modified(false) {
}

Cassette::~Cassette() {
}

void Cassette::note(const char *n) {
    std::cout << "cassette (" << port() << "): " << n << std::endl;
}

void Cassette::tick() {
    if (this->playing) {
        ++this->t;

        /*
         * Automatically stop the tape if the Apple doesn't use
         * it within the given number of cycles.
         */
        if (this->t_active+100000 <= this->t) {
            note("STOP");
            std::cout << "cassette: t=" << this->t << std::endl;
            this->playing = false;
        }
    }
}



// 0=cancel(abort), 1=yes(save), 2=no(discard)
static int askSave() {
    return tinyfd_messageBox(
        "Save changes",
        "You have unsaved changes to your tape image.\nDo you want to SAVE them?",
        "yesnocancel",
        "warning",
        0);
}

bool Cassette::eject() {
    if (isLoaded()) {
        if (isModified()) {
            const int resp = askSave();
            if (resp == 0) { // cancel
                return false;
            }

            if (resp == 1) { // yes (save)
                if (!write()) {
                    return false;
                }
            }

            this->modified = false;
            this->gui.setCassetteDirty(false);
        }
        this->file.clear();
        this->playing = false;
        this->t = 0;
        this->t_active = 0;
    }
    return true;
}

void Cassette::save() {
    if (isLoaded()) {
        if (isModified()) {
            if (write()) {
                this->modified = false;
                this->gui.setCassetteDirty(false);
            }
        }
    }
}



bool Cassette::isLoaded() {
    return !this->file.empty();
}

bool Cassette::isModified() {
    return this->modified;
}
