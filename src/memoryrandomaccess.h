#ifndef MEMORYRANDOMACCESS_H
#define MEMORYRANDOMACCESS_H

#include "memorystrapping.h"
#include "memoryrow.h"
#include <cstdint>
#include <string>

class MemoryRandomAccess {
    private:
        int &revision;
        MemoryRow rowE;
        MemoryStrapping strapE;
        MemoryRow rowD;
        MemoryStrapping strapD;
        MemoryRow rowC;
        MemoryStrapping strapC;

        MemoryRow &row_of(const std::string &row);
        MemoryStrapping &strapping_of(const std::string &row);
        std::uint8_t buggyRamRead(std::uint16_t address) const;
        void buggyRamWrite(std::uint16_t address, const std::uint8_t data);
        bool k20or24() const;

public:
        MemoryRandomAccess(int &revision);

        void insert_chip(const std::string &row, MemoryChip chip, const std::uint_fast8_t socket);
        void remove_chip(const std::string &row, const std::uint_fast8_t socket);
        void strap_to(const std::string &row, std::uint16_t addr_base, std::uint16_t addr_size);

        std::uint8_t read(std::uint16_t address) const;
        void write(std::uint16_t address, const std::uint8_t data);
        void powerOn();
        void powerOff();
};

#endif
