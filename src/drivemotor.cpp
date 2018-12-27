/*
    epple2
    Copyright (C) 2018 by Christopher A. Mosher <cmosher01@gmail.com>

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

#include "drivemotor.h"
#include "e2const.h"
//#include <cstdio>

DriveMotor::DriveMotor():
    on(false),
    pendingTicks(0) {
}

DriveMotor::~DriveMotor() {
}

bool DriveMotor::isOn() const {
    return this->on;
}

void DriveMotor::power(bool on) {
    if (on) {
        this->on = true;
        if (this->pendingTicks > 0) {
            this->pendingTicks = 0;
//            printf("MOTOR: cancel pending power off\n");
        }
//        printf("MOTOR: power on\n");
    } else {
        // delay power-off by about one second (a little longer, for momentum)
        // U.A.II, p. 9-13 (NE556 timer)
        this->pendingTicks = (int)(E2Const::AVG_CPU_HZ*1.2);
//        printf("MOTOR: power off pending...\n");
    }
}

void DriveMotor::reset() {
    // immediate power off
    // U.A.II, p. 9-13
    this->on = false;
//    printf("MOTOR: power off (immediate)\n");
}

void DriveMotor::tick() {
    if (this->pendingTicks > 0) {
        --this->pendingTicks;
        if (this->pendingTicks == 0){
            this->on = false;
//            printf("MOTOR: power off\n");
        }
    }
}
