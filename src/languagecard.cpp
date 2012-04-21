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
#include "languagecard.h"
#include "screenimage.h"

LanguageCard::LanguageCard(ScreenImage& gui, int slot):
	gui(gui),
	slot(slot),
	inhibit(false),
	ramTop(0x10000-0xE000),
	bank(1),
	readEnable(false),
	writeEnable(true),
	writeCount(0)
{
	this->ramBank.push_back(new Memory(0xE000-0xD000));
	this->ramBank.push_back(new Memory(0xE000-0xD000));
}

LanguageCard::~LanguageCard()
{
}


unsigned char LanguageCard::io(const unsigned short address, const unsigned char data, const bool writing)
{
	if ((address & 1) && !writing)
	{
		++this->writeCount;
	}
	else
	{
		this->writeCount = 0;
	}
	if (this->writeCount > 1)
	{
		this->writeEnable = true;
	}
	if (!(address & 1))
	{
		this->writeEnable = false;
	}

	const int r = address & 3;
	this->readEnable = (r==0 || r==3);

	this->bank = !(address & 8);

	this->gui.setLangCard(this->slot,this->readEnable,this->writeEnable,this->bank);

	return data;
}

void LanguageCard::ioBankRom(const unsigned short addr, unsigned char* const pb, const bool write)
{
	this->inhibit = false;
	if (this->readEnable && !write)
	{
		if (addr < 0x1000)
		{
			*pb = this->ramBank[this->bank]->read(addr);
		}
		else
		{
			*pb = this->ramTop.read(addr-0x1000);
		}
		this->inhibit = true;
	}
	else if (this->writeEnable && write)
	{
		if (addr < 0x1000)
		{
			this->ramBank[this->bank]->write(addr,*pb);
		}
		else
		{
			this->ramTop.write(addr-0x1000,*pb);
		}
	}
}
