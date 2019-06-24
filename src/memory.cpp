/*
    epple2

    Copyright © 2008, 2019, Christopher Alan Mosher, Shelton, CT, USA. <cmosher01@gmail.com>

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
#include <cstdlib>
#include <cstddef>



Memory::Memory(const size_t n):
    bytes(n),
    clear_value(0u),
    missing_bits(0u) {
}

void Memory::clear() {
    std::fill(this->bytes.begin(), this->bytes.end(), this->clear_value);
}

void Memory::load(const std::uint16_t base, std::istream& in) {
    in.read(reinterpret_cast<char*>(&this->bytes[base]), static_cast<ptrdiff_t>(this->bytes.size()-base));
}

void Memory::powerOn() {
    clear();
}

void Memory::powerOff() {
    clear();
}

size_t Memory::size() const {
    return this->bytes.size();
}

std::uint8_t Memory::read(const std::uint16_t address, const std::uint8_t data) const {
    return (this->bytes[address] & ~this->missing_bits) | (data & this->missing_bits);
}

void Memory::write(const std::uint16_t address, const std::uint8_t data) {
    this->bytes[address] = data;
}
