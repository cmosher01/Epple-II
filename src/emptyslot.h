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
#ifndef EMPTYSLOT_H
#define EMPTYSLOT_H

#include "card.h"

class EmptySlot : public Card
{
public:
	EmptySlot() {}
	virtual ~EmptySlot() {}

	virtual std::string getName() { return "empty"; }

	// empty slots have no ROMs, so just return data (for floating bus emulation)
	virtual unsigned char readRom(const unsigned short address, const unsigned char data) { return data; }
	virtual void readSeventhRom(const unsigned short address, unsigned char* const pb) { }
};

#endif
