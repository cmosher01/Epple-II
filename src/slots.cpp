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
#include "slots.h"
#include "screenimage.h"
#include <algorithm>

Slots::Slots(ScreenImage& gui):
	gui(gui),
	empty(),
	cards(8,&this->empty)
{
		forceGuiUpdate();
}

Slots::~Slots()
{
}

unsigned char Slots::io(const int islot, const int iswch, const unsigned char b, const bool writing)
{
	return this->cards[islot]->io(iswch,b,writing);
}

struct Slots_Card_reset
{
	void operator() (Card* p) { p->reset(); }
};

void Slots::reset()
{
	std::for_each(this->cards.begin(),this->cards.end(),Slots_Card_reset());
}

unsigned char Slots::readRom(const int islot, const unsigned short addr, const unsigned char data)
{
	return this->cards[islot]->readRom(addr,data);
}

struct Slots_Card_readSeventhRom
{
	const unsigned short addr;
	unsigned char* b;
	Slots_Card_readSeventhRom(const unsigned short addr, unsigned char* b):addr(addr),b(b){}
	void operator() (Card* p) { p->readSeventhRom(this->addr,this->b); }
};

unsigned char Slots::readSeventhRom(const unsigned short addr, const unsigned char data)
{
	unsigned char b(data);
	std::for_each(this->cards.begin(),this->cards.end(),Slots_Card_readSeventhRom(addr,&b));
	return b;
}

struct Slots_Card_ioBankRom
{
	const unsigned short addr;
	unsigned char* b;
	const bool write;
	Slots_Card_ioBankRom(const unsigned short addr, unsigned char* b, const bool write):addr(addr),b(b),write(write){}
	void operator() (Card* p) { p->ioBankRom(this->addr,this->b,this->write); }
};

unsigned char Slots::ioBankRom(const unsigned short addr, const unsigned char data, const bool write)
{
	unsigned char b(data);
	std::for_each(this->cards.begin(),this->cards.end(),Slots_Card_ioBankRom(addr,&b,write));
	return b;
}

struct Slots_Card_inhibitMotherboardRom
{
	bool inhibit;
	Slots_Card_inhibitMotherboardRom():inhibit(false) { }
	void operator() (Card* p) { if (p->inhibitMotherboardRom()) { inhibit = true; }}
};

bool Slots::inhibitMotherboardRom()
{
	return std::for_each(this->cards.begin(),this->cards.end(),Slots_Card_inhibitMotherboardRom()).inhibit;
}

void Slots::set(const int slot, Card* card)
{
	remove(slot);
	this->cards[slot] = card;
	this->gui.updateSlotName(slot,this->cards[slot]);
}

void Slots::remove(const int slot)
{
	if (this->cards[slot] != &this->empty)
	{
		delete this->cards[slot];
		this->cards[slot] = &this->empty;
		this->gui.removeCard(slot,this->cards[slot]);
	}
}

Card* Slots::get(const int slot)
{
	return this->cards[slot];
}


void Slots::forceGuiUpdate()
{
	for (int slot(0); slot < 8; ++slot)
		this->gui.updateSlotName(slot,this->cards[slot]);
}

/*
struct isAnyDiskDriveMotorOnCard
{
	bool on;
	isAnyDiskDriveMotorOnCard():on(false) {}
	void operator() (Card* p) { if (p->isMotorOn()) on = true; }
};

bool isAnyDiskDriveMotorOn()
{
	isAnyDiskDriveMotorOnCard on = isAnyDiskDriveMotorOnCard();
	std::for_each(this->cards.begin(),this->cards.end(),inh);
	return on.inhibit;
}
*/
struct Slots_Card_isDirty
{
	bool dirty;
	Slots_Card_isDirty():dirty(false) {}
	void operator() (Card* p) { if (p->isDirty()) dirty = true; }
};

bool Slots::isDirty()
{
	return std::for_each(this->cards.begin(),this->cards.end(),Slots_Card_isDirty()).dirty;
}
