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
#ifndef STEPPERMOTOR_H
#define STEPPERMOTOR_H

class StepperMotor
{
private:
	enum { TRACKS_PER_DISK = 0x23 };
	enum { QTRACKS = TRACKS_PER_DISK << 2 };

	signed short quarterTrack;
	signed char pos; // 0 - 7
	unsigned char mags;

	static signed char mapMagPos[];

	static signed char calcDeltaPos(const unsigned char cur, const signed char next)
	{
		signed char d = next-cur; // -7 to +7

		if (d==4 || d==-4)
		{
			d = 0;
		}
		else if (d>4)
		{
			d -= 8;
		}
		else if (d<-4)
		{
			d += 8;
		}

		return d;
	}

public:
	StepperMotor();
	~StepperMotor();

	void setMagnet(const unsigned char magnet, const bool on);
	unsigned char getTrack()
	{
		return ((unsigned short)(this->quarterTrack)) >> 2;
	}
};

#endif
