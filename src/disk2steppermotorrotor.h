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

#ifndef DISK2STEPPERMOTORROTOR_H
#define DISK2STEPPERMOTORROTOR_H

#include "movable.h"
#include "magneticfield.h"

class Disk2StepperMotorRotor {
public:
    Disk2StepperMotorRotor(const int total_can_stack, Movable& movable);

    void pend_rotation();
    void tick(const MagneticField &field);

private:
    void rotate_to(int new_position);

    const int total_can_stack;
    Movable& movable;
    int position = 0;
    int ticks_pending = 0;
};

#endif // DISK2STEPPERMOTORROTOR_H
