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
#include "standardin.h"

StandardIn::StandardIn():
	latch(0),
	gotEOF(false)
{
}


StandardIn::~StandardIn()
{
}




unsigned char StandardIn::io(const unsigned short address, const unsigned char data, const bool writing)
{
	int sw = address & 0x0F;
	if (sw == 0)
	{
		if (!(this->latch & 0x80))
		{
			if (this->gotEOF)
			{
				this->latch = 0xFF;
			}
			else
			{
				if (!this->producer.getKeys().empty())
				{
					this->latch = this->producer.getKeys().front();
					this->producer.getKeys().pop();
					if (this->latch == 0xFF)
					{
						this->gotEOF = true;
					}
					this->latch |= 0x80;
				}
			}
		}
	}
	else if (sw == 1)
	{
		this->latch &= 0x7F;
	}
	return this->latch;
}
