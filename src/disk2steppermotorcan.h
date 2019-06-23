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

#ifndef DISK2STEPPERMOTORCAN_H
#define DISK2STEPPERMOTORCAN_H

class Disk2StepperMotorCan {
public:
    Disk2StepperMotorCan(int index, int total_stack);

    bool magnetized() const;
    int magnetic_position() const;

    void set_lead(int index, bool on);

private:
    int charge() const;

    const int index;
    const int total_stack;

    bool lead_north = false;
    bool lead_south = false;
};

#endif // DISK2STEPPERMOTORCAN_H
