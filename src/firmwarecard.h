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
#ifndef FIRMWARECARD_H
#define FIRMWARECARD_H

#include "card.h"
#include "memory.h"
#include "screenimage.h"

class FirmwareCard : public Card
{
private:
	ScreenImage& gui;
	int slot;
	bool inhibitBankRom;
	bool inhibitF8Rom;
	bool inhibit;
	Memory bankRom;

public:
	FirmwareCard(ScreenImage& gui, int slot);
	~FirmwareCard();

	virtual void ioBankRom(const unsigned short addr, unsigned char* const pb, const bool write);

	virtual void reset()
	{
		this->inhibitBankRom = false;
		this->inhibitF8Rom = false;
		this->gui.setFirmCard(this->slot,this->inhibitBankRom,this->inhibitF8Rom);
	}
	
	
	
	virtual unsigned char io(const unsigned short address, const unsigned char data, const bool writing)
	{
		this->inhibitBankRom = !(address & 1);
		this->inhibitF8Rom = (address & 2);
		this->gui.setFirmCard(this->slot,this->inhibitBankRom,this->inhibitF8Rom);
		return data;
	}
	
	virtual void loadBankRom(const unsigned short base, std::istream& in)
	{
		this->bankRom.load(base,in);
	}
	
	
	
	virtual bool inhibitMotherboardRom()
	{
		return this->inhibit;
	}

	virtual std::string getName() { return "firmware     "; }
};

#endif
