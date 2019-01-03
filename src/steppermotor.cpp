/*
    epple2

    Copyright © 2008–2018, Christopher Alan Mosher, Shelton, CT, USA. <cmosher01@gmail.com>

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
/**
 * Emulates the arm stepper motor in the Disk ][.
 *
 * @author Chris Mosher
 */
/*


mags ps magval
3210
---- -- ------

Each postition is a quarter track.
One complete cycle through the 4 phases will
move the arm 2 tracks. (UA2, 9-7.)
0001 0  1
0011 1  3
0010 2  2
0110 3  6
0100 4  4
1100 5  C
1000 6  8
1001 7  9

strange, but still defined
1011 0  B
0111 2  7
1110 4  E
1101 6  D

all off (no movement)
0000 ?  0

undefined
0101 ?  5 // <-- TODO pick one at random?
1010 ?  A // <-- TODO pick one at random?
1111 ?  F // TODO what to do here?
*/
#include "steppermotor.h"
#include "util.h"
#include <iostream>

StepperMotor::StepperMotor():
    quarterTrack(QTRACKS >> 1), // start in the middle of the disk... just for fun
// TODO if we want to be extremely accurate, we should save each arm's position on shutdown and restore on startup
// (because in the real-life Apple ][, the arm stays in the same position when powered off).
    pos(0),
    mags(0),
    pendingPos(0),
    pendingTicks(0) {
}

StepperMotor::~StepperMotor() {
}

std::int8_t StepperMotor::mapMagPos[] = {-1,0,2,1,4,-1,3,2,6,7,-1,0,5,6,4,-1};

void StepperMotor::calculateTrack(const std::int8_t delta) {
    std::int16_t q = this->quarterTrack;
    q += delta;
    if (q < 0) {
        q = 0;
    } else if (QTRACKS <= q) {
        q = QTRACKS-1;
    }
    this->quarterTrack = static_cast<std::uint8_t>(q);
}

void StepperMotor::moveCog() {
    if (this->pendingPos >= 0) {
        calculateTrack(calcDeltaPos(this->pos,this->pendingPos));
        this->pos = this->pendingPos;
    }
}

void StepperMotor::setMagnet(const std::uint8_t magnet, const bool on) {
    const std::uint8_t mask = static_cast<std::uint8_t>(1u << magnet);
    if (on) {
        this->mags |= mask;
    } else {
        this->mags &= ~mask;
    }

    // set magnets (above), but delay actual movement of the stepper
    // motor cog, to emulate force of inertia while trying to move it
    // This allows Locksmith to write on the quarter-track, for example.

    this->pendingPos = mapMagPos[this->mags];
    this->pendingTicks = 1000; // about 1 millisecond
}

void StepperMotor::tick() {
    if (this->pendingTicks) {
        --this->pendingTicks;
        if (!this->pendingTicks) {
            moveCog();
        }
    }
}



// TODO fix logging (due to new delayed movement algorithm)
//    const std::uint8_t oldQT = this->quarterTrack;
//...
//    const std::uint8_t newQT = this->quarterTrack;
//    const std::int8_t deltaQT = newQT - oldQT;
//    printf("ARM: ph%d %s [%c%c%c%c] T$%02X.%02d %s %+0.2f\n",
//       (std::uint8_t)magnet,
//       on ? "+" : "-",
//       (mags&1)?'*':'.',
//       (mags&2)?'*':'.',
//       (mags&4)?'*':'.',
//       (mags&8)?'*':'.',
//       this->quarterTrack / 4,
//       (this->quarterTrack % 4) * 25,
//       deltaQT>0 ? "-->" : deltaQT<0 ? "<--" : "   ",
//       (deltaQT % 4) / 4.0);
