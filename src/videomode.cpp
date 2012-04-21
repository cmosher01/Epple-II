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
#include "videomode.h"

VideoMode::VideoMode()
{
}

unsigned char VideoMode::io(const unsigned short addr, const unsigned char b)
{
	const int sw = (addr & 0xE) >> 1;
	const bool on = addr & 0x1;
	switch (sw)
	{
		case 0:
			this->swText = on; break;
		case 1:
			this->swMixed = on; break;
		case 2:
			this->swPage2 = on ? 1 : 0; break;
		case 3:
			this->swHiRes = on; break;
	}
	return b;
}

void VideoMode::powerOn()
{
	this->swText = false;
	this->swMixed = false;
	this->swPage2 = 0;
	this->swHiRes = false;
}
