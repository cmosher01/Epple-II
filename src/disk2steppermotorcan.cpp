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

#include "disk2steppermotorcan.h"
#include <cassert>

Disk2StepperMotorCan::Disk2StepperMotorCan(int index, int total_stack):
    index(index),
    total_stack(total_stack) {
}

void Disk2StepperMotorCan::set_lead(int i_lead, bool on) {
    assert(0 <= i_lead && i_lead < 2);
    if (i_lead == 0) {
        this->lead_north = on;
    } else {
        this->lead_south = on;
    }
}

int Disk2StepperMotorCan::charge() const {
    int coil_charge = 0;
    if (this->lead_north) {
        ++coil_charge;
    }
    if (this->lead_south) {
        --coil_charge;
    }
    return coil_charge;
}

bool Disk2StepperMotorCan::magnetized() const {
    return this->charge() != 0;
}

int Disk2StepperMotorCan::magnetic_position() const {
    assert(magnetized());
    return this->total_stack * (this->index + 2*(this->charge() < 0));
}
