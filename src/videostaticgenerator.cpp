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
#include "videostaticgenerator.h"
#include "analogtv.h"
#include "e2const.h"
#include <ctime>
#include <cstdlib>

VideoStaticGenerator::VideoStaticGenerator(AnalogTV& display):
	display(display),
	isig(sig),
	isiglim(sig+AppleNTSC::SIGNAL_LEN),
	hpos(0)
{
	this->display.signal = sig;
	srand(time(0));
}


VideoStaticGenerator::~VideoStaticGenerator()
{
}


void VideoStaticGenerator::tick()
{
	signed char* is = this->isig;
	unsigned int cycles = E2Const::CRYSTAL_CYCLES_PER_CPU_CYCLE;
	if (this->hpos == E2Const::HORIZ_CYCLES-1)
	{
		cycles += E2Const::EXTRA_CRYSTAL_CYCLES_PER_CPU_LONG_CYCLE;
	}
	for (unsigned int i = 0; i < cycles; ++i)
	{
		*is++ = (rand()>>7&0x7F)-27;
	}
	this->isig = is;
	++this->hpos;
	if (this->hpos >= E2Const::HORIZ_CYCLES)
	{
		this->hpos = 0;
		if (isig >= isiglim)
		{
			isig = sig;
			this->display.drawCurrent();
		}
	}
}

void VideoStaticGenerator::powerOn()
{
	this->hpos = 0;
	this->display.signal = sig;
	isig = sig;
}
