#ifndef MEMORYSTRAPPING_H
#define MEMORYSTRAPPING_H

#include "memoryrow.h"
#include <cstdint>

class MemoryStrapping {
    private:
        MemoryRow &row;
        std::uint16_t addr_base;
        std::uint16_t addr_size;
    public:
        MemoryStrapping(MemoryRow &row);
        void strap_to(std::uint16_t addr_base, std::uint16_t addr_size);
        bool contains(std::uint16_t address) const;
        std::uint8_t read(const std::uint16_t address, const std::uint8_t data) const;
        void write(const std::uint16_t address, const std::uint8_t data);
        std::uint16_t size() const;
};

#endif
