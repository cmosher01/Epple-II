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
#ifndef APPLENTSC_H
#define APPLENTSC_H

class AppleNTSC
{
private:
	AppleNTSC() {}

public:
	enum { V = 262, H =  (25+40)*14+2 };
	enum { SIGNAL_LEN = V*H };

	enum
	{
		FP_START = 0,
		SYNC_START = FP_START+126,
		BP_START = SYNC_START+112,
		CB_START = BP_START+0,
		CB_END = CB_START+56,
		SPIKE = CB_END+34,
		PIC_START = CB_END+56
	};

	enum
	{
		WHITE_LEVEL = 100,
		BLANK_LEVEL = 0,
		SYNC_LEVEL = -40,
		CB_LEVEL = 20,
		LEVEL_RANGE = WHITE_LEVEL-SYNC_LEVEL
	};
};

#endif
