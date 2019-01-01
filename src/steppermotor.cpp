/*
    epple2
    Copyright (C) 2008 by Christopher A. Mosher <cmosher01@gmail.com>

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
 * This emulator moves the arm
 * instantaneously, whereas the Disk ][ arm would actually
 * take some time to reach its new position (this would
 * cause a difference if the state of the magnets changed
 * during this interval).
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
    mags(0) {
}

StepperMotor::~StepperMotor() {
}

signed char StepperMotor::mapMagPos[] = {-1,0,2,1,4,-1,3,2,6,7,-1,0,5,6,4,-1};

void StepperMotor::setMagnet(const unsigned char magnet, const bool on) {
    const unsigned char mask = 1 << magnet;
    if (on) {
        this->mags |= mask;
    } else {
        this->mags &= ~mask;
    }

    const std::uint8_t oldQT = this->quarterTrack;

    const char newPos = mapMagPos[this->mags];
    char d = 0;
    if (newPos >= 0) {
        d = calcDeltaPos(this->pos,newPos);
        this->pos = newPos;

        // TODO: delay moving arm by a small amount
        // For example, Locksmith, in order to write "quarter tracks" (i.e., T+.25 or T+.75), it positions
        // to the correct track (by turning two adjacent magnets on), then turns them both off in rapid
        // succession. In real life the arm doesn't move in such a case. In order to emulate that, we need
        // to delay the arm move for a bit, to see if the magnets change in the meantime.
        /*
         * ARM: ph2 + [..*.] T$0D.00     +0.00
         * ARM: ph3 + [..**] T$0D.25 --> +0.25
         * switching from tmap[34] --> [35]
         *
         * ARM: ph3 - [..*.] T$0D.00 <-- -0.25   <-\
         * switching from tmap[35] --> [34]      <--\-- this needs to get delayed
         * ARM: ph2 - [....] T$0D.00     +0.00
         */
        this->quarterTrack += d;
        if (this->quarterTrack < 0)
            this->quarterTrack = 0;
        else if (QTRACKS <= this->quarterTrack)
            this->quarterTrack = QTRACKS-1;
    }

    const std::uint8_t newQT = this->quarterTrack;
    const std::int8_t deltaQT = newQT - oldQT;

    printf("ARM: ph%d %s [%c%c%c%c] T$%02X.%02d %s %+0.2f\n",
       (std::uint8_t)magnet,
       on ? "+" : "-",
       (mags&1)?'*':'.',
       (mags&2)?'*':'.',
       (mags&4)?'*':'.',
       (mags&8)?'*':'.',
       this->quarterTrack / 4,
       (this->quarterTrack % 4) * 25,
       deltaQT>0 ? "-->" : deltaQT<0 ? "<--" : "   ",
       (deltaQT % 4) / 4.0);
}
