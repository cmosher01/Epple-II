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
#include "picturegenerator.h"
#include "analogtv.h"
#include "videomode.h"
#include "applentsc.h"
#include "e2const.h"

PictureGenerator::PictureGenerator(AnalogTV& display, VideoMode& mode, const int& revision):
	display(display), mode(mode), itestsig(testsig), itestsiglim(testsig+AppleNTSC::SIGNAL_LEN),
	VISIBLE_X_OFFSET(E2Const::BYTES_PER_ROW-E2Const::VISIBLE_BYTES_PER_ROW),
	revision(revision)
{
}


PictureGenerator::~PictureGenerator()
{
}


void PictureGenerator::powerOn()
{
	this->hpos = 0;
	this->line = 0;
	this->display.signal = this->testsig;
	this->itestsig = this->testsig;
}

void inline PictureGenerator::shiftLoRes()
{
	/*
	* For byte ABCDEFGH in register, perform
	* the following 4-bit end-around shifts:
	* 
	* +---<----+   +---<----+
	* |        |   |        |
	* +->ABCD->+   +->EFGH->+
	* 
	* Therefore:
	* 
	* ABCDEFGH --> DABCHEFG
	*/

	unsigned char rot_bits = this->latchGraphics & 0x11;
	// 000D000H
	rot_bits <<= 3;
	// D000H000

	this->latchGraphics &= 0xEE;
	// ABC0EFG0
	this->latchGraphics >>= 1;
	// 0ABC0EFG
	this->latchGraphics |= rot_bits;
	// DABCHEFG
}

void inline PictureGenerator::shiftHiRes()
{
	/*
	* For byte ABCDEFGH in register, perform
	* the following shift:
	* 
	* +---<----+
	* |        |
	* +->ABCD->+--->EFGH->
	* 
	* Therefore:
	* 
	* ABCDEFGH --> DABCDEFG
	*/

	unsigned char rot_bits = this->latchGraphics & 0x10;
	// 000D0000
	rot_bits <<= 3;
	// D0000000

	this->latchGraphics >>= 1;
	// 0ABCDEFG
	this->latchGraphics |= rot_bits;
	// DABCDEFG
}

void inline PictureGenerator::shiftText()
{
	this->latchText >>= 1;
}

bool inline PictureGenerator::getTextBit()
{
	return this->latchText & 1;
}

bool inline PictureGenerator::getHiResBit()
{
	return this->latchGraphics & 1;
}

bool inline PictureGenerator::getLoResBit(const bool odd, const bool vc)
{
	const int nibble = (this->latchGraphics >> (vc ? 4 : 0)) & 0x0F;
	return (nibble >> (odd ? 2 : 0)) & 1;
}

void inline PictureGenerator::loadGraphics(const unsigned char value)
{
	this->latchGraphics = value;
	this->d7 = this->latchGraphics & 0x80;
}

void inline PictureGenerator::loadText(const int value)
{
	this->latchText = value;
}

// TODO can we hand-optimize the main picture generator algorithm any more?
// Note that the innermost loop (which calls writeVideoSignal) has to execute
// at 14MHz, in order to maintain authentic Apple ][ speed.
void PictureGenerator::tick(const int t, const unsigned char rowToPlot)
{
	const bool isText(this->mode.isDisplayingText(t));
	const bool isHiRes(this->mode.isHiRes());

	signed char* is = this->itestsig;

	if (isText)
		loadText(rowToPlot);
	else
		loadGraphics(rowToPlot);

	if (t==0)
	{
		this->line = 0;
	}

	int cycles = E2Const::CRYSTAL_CYCLES_PER_CPU_CYCLE;
	if (this->hpos == E2Const::HORIZ_CYCLES-1)
	{
		cycles += E2Const::EXTRA_CRYSTAL_CYCLES_PER_CPU_LONG_CYCLE;
	}

	//		 hi-res half-pixel shift:
	const bool shift = !isText && isHiRes && this->d7 && this->line < E2Const::VISIBLE_ROWS_PER_FIELD && !(this->hpos < VISIBLE_X_OFFSET) && this->revision > 0;
	const bool showLastHiRes = shift && this->lasthires;

	int xtra(0);
	if (shift)
	{
		--cycles;
		++xtra;
	}
	const int firstBlankedCycle(E2Const::CRYSTAL_CYCLES_PER_CPU_CYCLE-xtra);

	int hcycle(this->hpos*E2Const::CRYSTAL_CYCLES_PER_CPU_CYCLE);
	const bool lineVis(this->line < E2Const::VISIBLE_ROWS_PER_FIELD);
	const bool hVis(this->hpos >= VISIBLE_X_OFFSET);
	for (int cycle(0); cycle < cycles-1; ++cycle)
	{
		const bool bit = shiftLatch(t,cycle,isText,isHiRes);
		is = writeVideoSignal(shift,showLastHiRes,firstBlankedCycle,cycle,hcycle,bit,lineVis,hVis,is);
		++hcycle;
	}
	// optimization: pull the last iteration of the loop out, so we don't getHiResBit every time
	{
		this->lasthires = getHiResBit(); // save it for the next plotted byte, just in case we need it
		const int cycle = cycles-1;
		const bool bit = shiftLatch(t,cycle,isText,isHiRes);
		is = writeVideoSignal(shift,showLastHiRes,firstBlankedCycle,cycle,hcycle,bit,lineVis,hVis,is);
	}

	this->itestsig = is;

	++this->hpos;
	if (this->hpos >= E2Const::HORIZ_CYCLES)
	{
		this->hpos = 0;
		++this->line;
		if (this->itestsig >= this->itestsiglim)
		{
			this->itestsig = this->testsig;
			this->display.drawCurrent();
		}
	}
}

bool inline PictureGenerator::shiftLatch(const int t, const int cycle, const bool isText, const bool isHiRes)
{
	bool bit;
	if (isText)
	{
		bit = getTextBit();
		if (cycle & 1) // @ 7MHz
		{
			shiftText();
		}
	}
	else if (isHiRes)
	{
		bit = getHiResBit();
		if (cycle & 1) // @ 7MHz
		{
			shiftHiRes();
		}
	}
	else // LO-RES
	{
		const int y = t / E2Const::BYTES_PER_ROW;
		bit = getLoResBit((t & 1) == (this->line & 1), y & 4);
		shiftLoRes();
	}
	return bit;
}

inline signed char* PictureGenerator::writeVideoSignal(const bool shift, const bool showLastHiRes, const int firstBlankedCycle, const int cycle, const int hcycle, const bool bit, const bool lineVis, const bool hVis, signed char* is)
{
	if (shift && !cycle)
	{
		*is++ = showLastHiRes ? AppleNTSC::WHITE_LEVEL : AppleNTSC::BLANK_LEVEL;
	}

	signed char sig;
	if (lineVis)
	{
		if (hVis)
		{
			if (bit && cycle < firstBlankedCycle)
			{
				sig = AppleNTSC::WHITE_LEVEL;
			}
			else
			{
				sig = AppleNTSC::BLANK_LEVEL;
			}
		}
		else
		{
			sig = hbl(hcycle);
		}
	}
	else
	{
		sig = vbl(hcycle);
	}
	*is++ = sig;
	return is;
}

// TODO Just to be extremely accurate, fix picture signal values during HBL and VBL
// (note that they vary by motherboard revision... there is a whole section in U.A.2)
signed char inline PictureGenerator::vbl(const int hcycle)
{
	signed char sig;
	if (224 <= this->line && this->line < 240) // VSYNC // TODO symbolize constants
	{
		sig = AppleNTSC::SYNC_LEVEL;
	}
	else
	{
		if (AppleNTSC::SYNC_START <= hcycle && hcycle < AppleNTSC::BP_START)
		{
			sig = AppleNTSC::SYNC_LEVEL;
		}
		else
		{
			sig = AppleNTSC::BLANK_LEVEL;
		}
	}
	return sig;
}


// Set up the color burst signal.
// TODO confirm that color burst signal phase is correct
// Note, I believe this is the correct phase. The only other
// possible configuration would be +10,-10,-10,+10. Can anyone confirm
// that 0,-20,0,+20 gives the correct phase? By eye, it seems
// +10,-10,-10,+10 makes brown look too green, but 0,-20,0,+20
// makes hi-res blue look too light.
// Note that this color burst signal only affects "old TV" mode;
// the other color modes use A2ColorsObserved.
const signed char PictureGenerator::lutCB[] =
{
	0,
	-AppleNTSC::CB_LEVEL,
	0,
	+AppleNTSC::CB_LEVEL,
};

signed char inline PictureGenerator::hbl(const int hcycle)
{
	signed char cb;
	if (AppleNTSC::CB_START <= hcycle && hcycle < AppleNTSC::CB_END)
	{
		if (this->mode.isText() && this->revision > 0)
		{
			cb = AppleNTSC::BLANK_LEVEL;
		}
		else
		{
			cb = lutCB[(hcycle-AppleNTSC::CB_START)%4];
		}
	}
	else if (AppleNTSC::SYNC_START <= hcycle && hcycle < AppleNTSC::BP_START)
	{
		cb = AppleNTSC::SYNC_LEVEL;
	}
	else
	{
		cb = AppleNTSC::BLANK_LEVEL;
	}
	return cb;
}
