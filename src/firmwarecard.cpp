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
#include "firmwarecard.h"
#include "memory.h"

FirmwareCard::FirmwareCard(ScreenImage& gui, int slot):
	gui(gui),
	slot(slot),
	inhibitBankRom(false),
	inhibitF8Rom(false),
	inhibit(false),
	bankRom(0x10000-0xD000)
{
}


FirmwareCard::~FirmwareCard()
{
}






void FirmwareCard::ioBankRom(const unsigned short addr, unsigned char* const pb, const bool)
{
	this->inhibit = false;
	if (addr < 0x2800)
	{
		if (this->inhibitBankRom)
		{
			*pb = this->bankRom.read(addr);
			this->inhibit = true;
		}
	}
	else if (0x2800 <= addr && addr < 0x3000)
	{
		if (this->inhibitF8Rom)
		{
			*pb = this->bankRom.read(addr);
			this->inhibit = true;
		}
	}
}
