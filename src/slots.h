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
#ifndef SLOTS_H
#define SLOTS_H

#include <vector>
#include "card.h"
#include "emptyslot.h"

class ScreenImage;

class Slots
{
private:
	ScreenImage& gui;
	EmptySlot empty;
	std::vector<Card*> cards;

public:
	Slots(ScreenImage& gui);
	~Slots();

	unsigned char io(const int islot, const int iswch, const unsigned char b, const bool writing);
	void reset();
	unsigned char readRom(const int islot, const unsigned short addr, const unsigned char data);
	unsigned char readSeventhRom(const unsigned short addr, const unsigned char data);
	unsigned char ioBankRom(const unsigned short addr, const unsigned char data, const bool write);
	bool inhibitMotherboardRom();
	void set(const int slot, Card* card);
	Card* get(const int slot);
	void remove(const int slot);
	bool isDirty();
	void forceGuiUpdate();
};

#endif
