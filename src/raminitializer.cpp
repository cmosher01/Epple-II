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
#include "raminitializer.h"
#include "memory.h"
#include <ctime>
#include <cstdlib>

RAMInitializer::RAMInitializer(Memory& mem):
	ram(mem)
{
	srand(time(0));
}

void RAMInitializer::init()
{
	this->ram.clear();

	int b(0);
	// TODO make the types of RAM chips configurable
	putBytesUntilFull(b++,1);
	putBytesUntilFull(b++,2);
	putBytesUntilFull(b++,1);
	putBytesUntilFull(b++,2);
	putBytesUntilFull(b++,1);
	putBytesUntilFull(b++,2);
	putBytesUntilFull(b++,2);
	putBytesUntilFull(b++,1);
};


void RAMInitializer::putBytesUntilFull(int bit, int pat)
{
	this->nextinit = 0;
	try
	{
		while (true)
		{
			if (pat==1)
				ramPattern1(bit);
			else
				ramPattern2(bit);
		}
	}
	catch (const done&)
	{
		// done filling this bit in RAM
	}
}

void RAMInitializer::ramPattern1(const int bit) throw (done)
{
	for (int k = 0; k < 2; ++k)
	{
		for (int j = 0; j < 8; ++j)
		{
			int i;
			for (i = 0; i < 0x10; ++i)
			{
				putn(4,false,bit);
				putn(2,true,bit);
				putn(2,false,bit);
			}
			for (i = 0; i < 0x40; ++i)
			{
				putn(2,true,bit);
				putn(2,false,bit);
			}
			for (i = 0; i < 0x08; ++i)
			{
				putn(2,true,bit);
				putn(1,false,bit);
				putn(3,true,bit);
				putn(2,false,bit);
				putn(2,true,bit);
				putn(2,false,bit);
				putn(2,true,bit);
				putn(2,false,bit);
			}
		}
		for (int i = 0; i < 0x400; ++i)
		{
			putn(2,true,bit);
			putn(2,false,bit);
		}
	}
}

void RAMInitializer::ramPattern2(const int bit) throw (done)
{
	for (int i = 0; i < 0x40; ++i)
	{
		putn(0x80,true,bit);
		putn(0x80,false,bit);
	}
}



void RAMInitializer::putn(const int c, bool on, const int bit) throw (done)
{
	if (((rand() >> 9) & 0x1F) == 5)
		on = !on;
	const unsigned char mask(1 << bit);
	for (int i = 0; i < c; ++i)
	{
		if (this->nextinit >= this->ram.size())
		{
			throw done();
		}
		unsigned char b = this->ram.read(this->nextinit);
		if (on)
		{
			b |= mask;
		}
		else
		{
			b &= ~mask;
		}
		this->ram.write(this->nextinit,b);
		++this->nextinit;
	}
}
