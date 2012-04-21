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
#ifndef STANDARDIN_H
#define STANDARDIN_H

#include "card.h"
#include "standardinproducer.h"

class StandardIn : public Card
{
private:
	StandardInProducer producer;
	unsigned char latch;
	bool gotEOF;

public:
	StandardIn();
	~StandardIn();

	virtual unsigned char io(const unsigned short address, const unsigned char data, const bool writing);
	virtual std::string getName() { return "standard input"; }
};

#endif
