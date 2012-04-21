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
#include "standardout.h"
#include <iostream>

StandardOut::StandardOut()
{
}


StandardOut::~StandardOut()
{
}


unsigned char StandardOut::io(const unsigned short address, const unsigned char data, const bool writing)
{
	if (!writing)
	{
		return data;
	}

	const char c = (char)(data&0x7F);
	if (c == '\r')
	{
		std::cout << std::endl;
	}
	else
	{
		std::cout << c;
	}
	std::cout << std::flush;

	return data;
}
