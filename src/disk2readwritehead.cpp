/*
    epple2

    Copyright © 2019, Christopher Alan Mosher, Shelton, CT, USA. <cmosher01@gmail.com>

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

#include "disk2readwritehead.h"

Disk2ReadWriteHead::~Disk2ReadWriteHead() = default;

int Disk2ReadWriteHead::position() const {
    return this->current_position;
}

bool Disk2ReadWriteHead::move_by(const int delta_position) {
    const int candidate_position = this->current_position + delta_position;

    const bool ok_position = (0 <= candidate_position && candidate_position < C_POSITION);
    if (ok_position) {
        this->current_position = candidate_position;
    }

    return ok_position;
}
