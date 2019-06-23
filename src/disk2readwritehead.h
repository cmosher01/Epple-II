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

#ifndef DISK2READWRITEHEAD_H
#define DISK2READWRITEHEAD_H

#include "movable.h"

/**
 * @brief Disk ][ read/write head assembly
 *
 * Represents the read/write head assembly of a Disk ][ floppy drive.
 * The assembly includes a "sled" that can move back and forth, with a
 * read/write head attached to it. The sled is moved by the rotor from
 * the stepper motor.
 *
 * Rotation of the rotor will cause the sled to move back or forth, along
 * with the read/write head, positioning it over a "track" on the floppy disk.
 *
 * If the rotor attempts to move the sled too far, the sled will resist and
 * prevent the rotor from rotating.
 */
class Disk2ReadWriteHead : public Movable {
public:
    virtual ~Disk2ReadWriteHead();

    int position() const;
    virtual bool move_by(int delta_position);

private:
    /**
     * Total count of possible positions for this read/write head.
     *
     * A position will represent a track on a floppy disk.
     * The number of tracks really depends on the floppy disk.
     * The Disk ][ has a "stop" at track zero, but not at the high end,
     * so this is where it can vary from disk to disk.
     * Typical disks have 35 tracks, and allow quarter-track precision.
     *
     * In this emulator, we adhere to the WOZ v2 specification for floppy disks.
     * It allows for up to 40 tracks (with quarter-track precision):
     *
     * 0=t0, 1=t0.25, 2=t0.5, 3=t0.75, 4=t1, ... 140=t35.00 ... 159=t39.75
     */
    const static int C_POSITION = 40*4;

    int current_position = C_POSITION/2; // random start
};

#endif // DISK2READWRITEHEAD_H
