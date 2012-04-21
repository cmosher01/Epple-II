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
#include "clockcard.h"
#include <ctime>

ClockCard::ClockCard():
	latch(0),
	pos(0)
{
}


ClockCard::~ClockCard()
{
}



unsigned char ClockCard::io(const unsigned short address, const unsigned char data, const bool writing)
{
	const int sw = address & 0x0F;
	if (sw == 0)
	{
		if (!(this->latch & 0x80))
		{
			if (this->pos == 0)
			{
				getTime();
			}
			char c = this->time[this->pos];
			this->latch = (unsigned char)(c | 0x80);
			++this->pos;
			if (this->pos >= this->timelen)
			{
				this->pos = 0;
			}
		}
	}
	else if (sw == 1)
	{
		this->latch &= 0x7F;
	}
	return this->latch;
}

#define TIMEFORMAT "%m,0%w,%d,%H,%M,%S,000,%Y,%Z,D\r"

void ClockCard::getTime()
{
	time_t now;
	::time(&now);
	struct tm* nowtm = ::localtime(&now);
	this->timelen = ::strftime(this->time,sizeof(this->time),TIMEFORMAT,nowtm);
	this->time[this->timelen-2] = nowtm->tm_isdst>0 ? '1' : '0';
}
