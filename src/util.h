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
#ifndef UTIL_H
#define UTIL_H

class Util
{
public:
	static int divideRoundUp(const int num, const int denom)
	{
		return (num+denom-1)/denom;
	}

	static int divideRound(const int dividend, const int divisor)
	{
		return (dividend+divisor/2)/divisor;
	}

	template<typename T> static T mod(T x, const T m)
	{
		x %= m;
		if (x < 0)
		{
			x += m;
		}
		return x;
	}

	template<typename T> static void constrain(const T& min, T& x, const T& lim)
	{
		if (x < min)
		{
			x = min;
		}
		else if (lim <= x)
		{
			x = lim-1;
		}
	}

	// 0x0 <= nib <= 0xF
	// '0' <= ret <= 'F'
	static char hexDigit(unsigned char nib)
	{
		return nib < 10 ? '0'+nib : 'A'+nib-10;
	}
};

#endif
