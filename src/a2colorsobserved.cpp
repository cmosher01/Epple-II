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
#include "a2colorsobserved.h"

static unsigned char tobyt(float x)
{
	x *= 256.0f;
	x += 0.0001f;
	int xi = (int)x;

	if (xi > 255)
		xi = 255;

	return (unsigned char)xi;
}

// 0 <= h < 360 degrees;   0 <= s,v <= 1
static int HSVtoRGB(const int h, const float s, const float v)
{
	const float f = (h % 60) / 60.0;

	float r, g, b;
	switch (h / 60)
	{
		case 0:
			r = v;
			g = v * (1 - s * (1 - f));
			b = v * (1 - s);
		break;
		case 1:
			r = v * (1 - s * f);
			g = v;
			b = v * (1 - s);
		break;
		case 2:
			r = v * (1 - s);
			g = v;
			b = v * (1 - s * (1 - f));
		break;
		case 3:
			r = v * (1 - s);
			g = v * (1 - s * f);
			b = v;
		break;
		case 4:
			r = v * (1 - s * (1 - f));
			g = v * (1 - s);
			b = v;
		break;
		case 5:
			r = v;
			g = v * (1 - s);
			b = v * (1 - s * f);
		break;
	}

	return (tobyt(r) << 16) | (tobyt(g) << 8) | (tobyt(b));
}

A2ColorsObserved::A2ColorsObserved():
	COLOR(0x10)
{
//	const unsigned int clr[] = { 0x1, 0xB, 0x3, 0x2, 0x7, 0x6, 0x4, 0xE, 0xC, 0x8, 0xD, 0x9, 0x5, 0xA, 0xF, 0x0 };
	const unsigned int map[] = { 0xF, 0x0, 0x3, 0x2, 0x6, 0xC, 0x5, 0x4, 0x9, 0xB, 0xD, 0x1, 0x8, 0xA, 0x7, 0xE };
	const unsigned int hue[] = { 342, 342, 277, 233, 233, 213, 160, 160,  75,  33,  52,  24,   0,   0,   0,   0 };
	const unsigned int sat[] = { 100,  50,  75, 100,  50, 100, 100, 100, 100, 100, 100, 100,   0,   0,   0,   0 };
	const unsigned int val[] = {  67, 100, 100,  75, 100, 100,  33, 100,  75,  50, 100, 100,  50,  50, 100,   0 };

	for (unsigned int i(0); i < COLOR.size(); ++i)
	{
		COLOR[i] = HSVtoRGB(hue[map[i]],sat[map[i]]/100.0f,val[map[i]]/100.0f);
	}

}

A2ColorsObserved::~A2ColorsObserved()
{
}
