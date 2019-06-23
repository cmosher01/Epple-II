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

#ifndef DISK2STEPPERMOTOR_H
#define DISK2STEPPERMOTOR_H

#include "disk2steppermotorcan.h"
#include "disk2steppermotorrotor.h"
#include "movable.h"
#include "magneticfield.h"

class Disk2StepperMotor {
public:
    Disk2StepperMotor(Movable& movable);

    void tick();
    void set_phase(int i_phase_0_to_3, bool on);

    static bool test();

private:
    void build_magnetic_field();
    bool magnetized() const;
    int magnetic_position() const;

    Disk2StepperMotorCan can_0;
    Disk2StepperMotorCan can_1;

    MagneticField field;

    Disk2StepperMotorRotor rotor;
};

#endif // DISK2STEPPERMOTOR_H
