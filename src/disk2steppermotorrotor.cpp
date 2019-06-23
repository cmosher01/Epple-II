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

#include "disk2steppermotorrotor.h"



/**
 * @brief positive_modulo
 * @param x value to constrain
 * @param n modulus
 * @return constrained value
 */
static int positive_modulo(const int x, const int n) {
    return (x%n + n) % n;
}

/**
 * @brief
 *   Constrains x to within range (-n,n)
 *   representing movements around a circle
 * @param x value to constrain
 * @param n range
 * @return constrained value
 */
static int cyclic_motion(int x, const int n) {
    while (x <= -n) {
        x += 2*n;
    }
    while (+n <= x) {
        x -= 2*n;
    }
    return x%n;
}



Disk2StepperMotorRotor::Disk2StepperMotorRotor(const int total_can_stack, Movable& movable):
    total_can_stack(total_can_stack),
    movable(movable) {
}

void Disk2StepperMotorRotor::pend_rotation() {
    this->ticks_pending = 1000; // about 1 millisecond delay due to inertia
}

void Disk2StepperMotorRotor::tick(const MagneticField &magnetic_field) {
    if (this->ticks_pending) {
        --this->ticks_pending;
        if (!this->ticks_pending) {
            if (magnetic_field.is_on()) {
                rotate_to(magnetic_field.position());
            }
        }
    }
}

void Disk2StepperMotorRotor::rotate_to(const int new_position) {
    const int potential_movement = cyclic_motion(new_position-this->position, this->total_can_stack*2);
    if (potential_movement && this->movable.move_by(potential_movement)) {
        this->position = positive_modulo(this->position+potential_movement, this->total_can_stack*4);
    }
}
