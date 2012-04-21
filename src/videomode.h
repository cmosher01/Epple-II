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
#ifndef VIDEOMODE_H
#define VIDEOMODE_H

#include "videoaddressing.h"
#include "e2const.h"

class VideoMode
{
private:
	bool swText;
	bool swMixed;
	int swPage2;
	bool swHiRes;

public:
	VideoMode();
	unsigned char io(const unsigned short addr, const unsigned char b);
	void powerOn();

	bool isText() const { return this->swText; }
	bool isHiRes() const { return this->swHiRes; }
	bool isMixed() const { return this->swMixed; }
	int getPage() const { return this->swPage2; }
	bool isDisplayingText(const int atTickInField) const { return this->swText || (this->swMixed && atTickInField >= E2Const::MIXED_TEXT_CYCLE); }
};

#endif
