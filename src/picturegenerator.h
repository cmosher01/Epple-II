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
#ifndef PICTUREGENERATOR_H
#define PICTUREGENERATOR_H
#include "applentsc.h"
class AnalogTV;
class VideoMode;

class PictureGenerator
{
private:
	AnalogTV& display;
	VideoMode& mode;

	unsigned char latchGraphics;
	bool d7;
	unsigned char latchText;
	unsigned int hpos;
	unsigned int line;
	bool lasthires;
	static const signed char lutCB[];

	signed char testsig[AppleNTSC::SIGNAL_LEN];
	signed char* itestsig;
	signed char* itestsiglim;

	void shiftLoRes();
	void shiftHiRes();
	void shiftText();
	bool getTextBit();
	bool getHiResBit();
	bool getLoResBit(const bool odd, const bool vc);
	void loadGraphics(const unsigned char value);
	void loadText(const int value);
	bool shiftLatch(const int t, const int cycle, const bool isText, const bool isHiRes);
	signed char* writeVideoSignal(const bool shift, const bool showLastHiRes, const int firstBlankedCycle, const int cycle, const int hcycle, const bool bit, const bool lineVis, const bool hVis, signed char* is);
	signed char vbl(const int hcycle);
	signed char hbl(const int hcycle);

	const unsigned int VISIBLE_X_OFFSET;

	const int& revision;

public:

	PictureGenerator(AnalogTV& display, VideoMode& mode, const int& revision);
	~PictureGenerator();

	void powerOn();
	void tick(const int t, const unsigned char c);
};

#endif
