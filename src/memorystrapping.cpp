#include "memorystrapping.h"
#include <algorithm>

MemoryStrapping::MemoryStrapping(MemoryRow &row): row{row}, strapped{false} {
}

void MemoryStrapping::strap_to(std::uint16_t addr_base, std::uint16_t addr_size) {
    this->addr_base = addr_base;
    this->addr_size = addr_size;
    this->strapped = true;
}

bool MemoryStrapping::contains(std::uint16_t address) const {
    if (!this->strapped) {
        return false;
    }
    return this->addr_base <= address && address < this->addr_base + size();
}

std::uint8_t MemoryStrapping::read(const std::uint16_t address, const std::uint8_t data) const {
    if (!this->strapped) {
        return 0xFFu;
    }
    return this->row.read(address - this->addr_base, data);
}

void MemoryStrapping::write(const std::uint16_t address, const std::uint8_t data) {
    if (!this->strapped) {
        return;
    }
    this->row.write(address - this->addr_base, data);
}

std::uint16_t MemoryStrapping::size() const {
    if (!this->strapped) {
        return 0;
    }
    return std::min(this->row.size(), this->addr_size);
}
