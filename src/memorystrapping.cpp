#include "memorystrapping.h"
#include <algorithm>

MemoryStrapping::MemoryStrapping(MemoryRow &row):
    row(row) {
}

void MemoryStrapping::strap_to(std::uint16_t addr_base, std::uint16_t addr_size) {
    this->addr_base = addr_base;
    this->addr_size = addr_size;
}

bool MemoryStrapping::contains(std::uint16_t address) const {
    return this->addr_base <= address && address < this->addr_base + size();
}

std::uint8_t MemoryStrapping::read(const std::uint16_t address, const std::uint8_t data) const {
    return this->row.read(address - this->addr_base, data);
}

void MemoryStrapping::write(const std::uint16_t address, const std::uint8_t data) {
    this->row.write(address - this->addr_base, data);
}

std::uint16_t MemoryStrapping::size() const {
    return std::min(this->row.size(), this->addr_size);
}
