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
#include "memory.h"
#include <vector>
#include <algorithm>
#include <istream>
#include "raminitializer.h"

const int Memory::CLEAR_VALUE(0);

Memory::Memory(const size_t n):
	bytes(n)
{
}

void Memory::clear()
{
	std::fill(this->bytes.begin(),this->bytes.end(),CLEAR_VALUE);
}

void Memory::powerOn()
{
	RAMInitializer initRam(*this);
	initRam.init();
}

void Memory::powerOff()
{
	clear();
}

void Memory::load(const unsigned short base, std::istream& in)
{
	in.read((char*)&this->bytes[base],this->bytes.size()-base);
}
