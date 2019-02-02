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
#ifndef MEMORY_H
#define MEMORY_H

#include <vector>
#include <istream>
#include <cstdint>

class Memory {
    private:
        std::vector<std::uint8_t> bytes;
        const std::uint8_t clear_value;
        const std::uint8_t missing_bits;

    public:
        Memory(const size_t n);
        virtual ~Memory() { }

        size_t size() const;
        std::uint8_t read(const std::uint16_t address) const;
        void write(const std::uint16_t address, const std::uint8_t data);
        void powerOn();
        void powerOff();

        void clear();
        void init();
        void load(const std::uint16_t base, std::istream& in);
};

#endif
