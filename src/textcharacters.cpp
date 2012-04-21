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
#include "textcharacters.h"

TextCharacters::TextCharacters():
	rows(0x40*8)
{
	int r(0);

	const char *pi =
#include "textcharacterimages.h"
	;

	for (int ch(0); ch < 0x40; ++ch)
	{

		rows[r] = 0;
		++r;
		for (int ln(1); ln < 8; ++ln)
		{
			for (int bt(0); bt < 5; ++bt)
			{
				rows[r] >>= 1;
				if (*pi++=='@')
					rows[r] |= 0x20;
			}
			++r;
		}
	}
}
