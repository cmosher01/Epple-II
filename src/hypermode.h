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
#ifndef HYPERMODE_H
#define HYPERMODE_H

class HyperMode
{
private:
    bool fhyper;

public:
    HyperMode(): fhyper(false) { }
    ~HyperMode() { }
    bool isHyper()
    {
        return this->fhyper;
    }

    void setHyper(bool isHyper)
    {
        this->fhyper = isHyper;
    }

    void toggleHyper()
    {
        this->fhyper = !this->fhyper;
    }
};

#endif
