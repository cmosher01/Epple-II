#ifndef MEMORYROW_H
#define MEMORYROW_H

#include "memorychip.h"
#include <map>
#include <vector>
#include <array>
#include <cstdint>

class MemoryRow {
    private:
        /* C, D, E */
        const char label;

        bool power = false;

        /* 8 sockets for memory chips (empty socket represented by MemoryChipEmptySocket) */
        std::array<MemoryChip,8> chips = {MemoryChipEmptySocket(),MemoryChipEmptySocket(),MemoryChipEmptySocket(),MemoryChipEmptySocket(),MemoryChipEmptySocket(),MemoryChipEmptySocket(),MemoryChipEmptySocket(),MemoryChipEmptySocket()};

        /* bit mask of empty chip sockets */
        std::uint8_t missing_bits = 0xFFu;

        /*
         * Instead of storing each bit in a MemoryChip (too slow), we store the data here as bytes,
         * representing one bit for each chip.
         */
        std::vector<std::uint8_t> values_stored;

        std::uint16_t calculate_size() const;

    public:
        MemoryRow(const char label);
        virtual ~MemoryRow();

        void insert_chip(MemoryChip chip, const std::uint_fast8_t socket);
        void remove_chip(const std::uint_fast8_t socket);
        void powerOff();
        void powerOn();

        /* 4K or 16K, size of each chip (or minimum in corner case of mixed sizes) */
        std::uint16_t size() const;

        std::uint8_t read(const std::uint16_t address) const;
        void write(const std::uint16_t address, const std::uint8_t data);

        static std::uint8_t missing_memory_byte_value();
};

#endif // MEMORYROW_H
