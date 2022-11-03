/*
    epple2
    Copyright (C) 2008 by Christopher A. Mosher <cmosher01@gmail.com>

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
#ifndef A2COLORSOBSERVED_H
#define A2COLORSOBSERVED_H

#include <vector>

class A2ColorsObserved
{
private:
    std::vector<unsigned int> COLOR;

public:
    A2ColorsObserved();
    ~A2ColorsObserved();

    const std::vector<unsigned int>& c() { return this->COLOR; }

    enum
    {
        BLACK,
        DARK_MAGENTA,
        DARK_BLUE,
        HIRES_VIOLET,
        DARK_BLUE_GREEN,
        GREY,
        HIRES_BLUE,
        LIGHT_BLUE,
        DARK_BROWN,
        HIRES_ORANGE,
        GREY_ALTERNATE,
        LIGHT_MAGENTA,
        HIRES_GREEN,
        LIGHT_BROWN,
        LIGHT_BLUE_GREEN,
        WHITE,
    };
};

#endif
