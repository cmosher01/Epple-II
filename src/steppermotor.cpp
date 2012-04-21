/*
    epple2
    Copyright (C) 2008 by Chris Mosher <chris@mosher.mine.nu>

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
 * This implementation differs from the actual Disk ][ in
 * that it rounds half- and quarter-tracks down to the
 * next whole track. Also, this emulator moves the arm
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
0001 0  1
0011 1  3
0010 2  2
0110 3  6
0100 4  4
1100 5  C
1000 6  8
1001 7  9

(strange, but defined)
1011 0  B
0111 2  7
1110 4  E
1101 6  D

(undefined, i.e., no movement)
0000 ?  0
0101 ?  5
1010 ?  A
1111 ?  F
*/
#include "steppermotor.h"
#include "util.h"

StepperMotor::StepperMotor():
	quarterTrack(QTRACKS >> 1), // start in the middle of the disk... just for fun
// TODO if we want to be extremely accurate, we should save each arm's position on shutdown and restore on startup
// (because in the real-life Apple ][, the arm stays in the same position when powered off).
	pos(0),
	mags(0)
{
}

StepperMotor::~StepperMotor()
{
}

signed char StepperMotor::mapMagPos[] = {-1,0,2,1,4,-1,3,2,6,7,-1,0,5,6,4,-1};

void StepperMotor::setMagnet(const unsigned char magnet, const bool on)
{
	const unsigned char mask = 1 << magnet;
	if (on)
	{
		this->mags |= mask;
	}
	else
	{
		this->mags &= ~mask;
	}

	const char newPos = mapMagPos[this->mags];
	char d;
	if (newPos >= 0)
	{
		d = calcDeltaPos(this->pos,newPos);
		this->pos = newPos;

		this->quarterTrack += d;
		if (this->quarterTrack < 0)
			this->quarterTrack = 0;
		else if (this->quarterTrack > QTRACKS)
			this->quarterTrack = QTRACKS;
	}
/*
	std::cout << " ARM: magnet " << (unsigned int)magnet << " " << (on ? "on " : "off" );
	std::cout << " [" <<
		((mags&1)?"*":".") <<
		((mags&2)?"*":".") <<
		((mags&4)?"*":".") <<
		((mags&8)?"*":".") <<
		"]";
	if (d != 0)
	{
		std::cout << " track " << std::hex << (unsigned int)(this->quarterTrack >> 2);
		int fract = this->quarterTrack & 3;
		if (fract != 0)
		{
			std::cout << (fract == 1 ? " +.25" : fract == 2 ? " +.5" : " +.75");
		}
	}
	std::cout << std::endl;
*/
}
