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
#ifndef MEMORY_H
#define MEMORY_H

#include <vector>
#include <istream>

class Memory
{
private:
	std::vector<unsigned char> bytes;
	static const int CLEAR_VALUE;

public:
	Memory(const size_t n);
	size_t size() const
	{
		return this->bytes.size();
	}
	
	unsigned char read(const unsigned short address) const
	{
		return this->bytes[address];
	}
	
	void write(const unsigned short address, const unsigned char data)
	{
		this->bytes[address] = data;
	}
	
	void clear();
	void powerOn();
	void powerOff();
	void load(const unsigned short base, std::istream& in);
};

#endif
