/*
    epple2

    Copyright © 2018, Christopher Alan Mosher, Shelton, CT, USA. <cmosher01@gmail.com>

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

#ifndef LSS_H
#define LSS_H

#include <cstdint>

class LSS
{
private:
    bool use13Sector;
    std::uint8_t lssrom[0x100];
    std::uint8_t lss13rom[0x100];

public:
    LSS(bool use13SectorDos32LSS);
    ~LSS();

    std::uint8_t read(const std::uint8_t addr) {
        return use13Sector ? lss13rom[addr] : lssrom[addr];
    }
};

#endif
