#include "memoryrow.h"
#include <exception>
#include <cstdlib>

/*
 * If any RAM IC sockets are empty, set the corresponding bits to 1 most of the time.
 * For some addresses it seems they are always 1, but for other addresses they can return
 * 0 sometimes, empirically I've seen anywhere from 8% to 15% of the time.
 */
static std::uint8_t randomize_missing_bits(std::uint8_t v, const std::uint8_t bits) {
    std::uint8_t bit = 1u;
    for (std::uint_fast8_t i = 0; i < 8; ++i) {
        if (bits & bit) {
            double r = static_cast<double>(std::rand())/RAND_MAX;
            if (r < 0.11) {
                v &= ~bit;
            } else {
                v |= bit;
            }
        }
        bit <<= 1;
    }
    return v;
}



MemoryRow::MemoryRow(const char label):
    label(label) {
}

MemoryRow::~MemoryRow() {
}

void MemoryRow::insert_chip(MemoryChip chip, const std::uint_fast8_t socket) {
    if (socket < 8u) {
        remove_chip(socket);
        if (chip.exists()) {
            this->chips[socket] = chip;
            this->missing_bits &= ~(1u << socket);
            this->values_stored.resize(calculate_size());
        }
    } else {
        throw std::out_of_range("socket must be < 8");
    }
}

void MemoryRow::remove_chip(const std::uint_fast8_t socket) {
    if (socket < 8u) {
        this->chips[socket] = MemoryChipEmptySocket();
        this->missing_bits |= (1u << socket);
        this->values_stored.resize(calculate_size());
    } else {
        throw std::out_of_range("socket must be < 8");
    }
}

/*
 * If no chips, return 0.
 * Otherwise, return minimum of existing chips (4K or 16K)
 */
std::uint16_t MemoryRow::calculate_size() const {
    std::uint16_t size_new = 0u;

    for (std::uint_fast8_t i_chip = 0; i_chip < 8; ++i_chip) {
        const MemoryChip &chip = this->chips[i_chip];
        if (chip.exists()) {
            const std::uint16_t s = chip.size();
            if (size_new == 0u || s < size_new) {
                size_new = s;
            }
        }
    }

    return size_new;
}

void MemoryRow::powerOff() {
    this->power = false;
}

/*
 * Calls init of each chip, and uses a bit mask to assemble into bytes.
 */
void MemoryRow::powerOn() {
    if (!this->power) {
        this->power = true;

        std::uint8_t mask_bit = 1u;
        for (std::uint_fast8_t i_bit = 0; i_bit < 8; ++i_bit) {
            const MemoryChip &chip = this->chips[i_bit];
            if (chip.exists()) {
                chip.init(mask_bit, this->values_stored, size());
            }
            mask_bit <<= 1;
        }
    }
}

std::uint16_t MemoryRow::size() const {
    return static_cast<std::uint16_t>(this->values_stored.size());
}

std::uint8_t MemoryRow::missing_memory_byte_value() {
    return randomize_missing_bits(0xFFu, 0xFFu);
}

std::uint8_t MemoryRow::read(const std::uint16_t address_offset) const {
    if (this->power) {
        std::uint8_t v;
        if (address_offset < this->values_stored.size()) {
            v = this->values_stored[address_offset];
            if (this->missing_bits) {
                v = randomize_missing_bits(v, this->missing_bits);
            }
        } else {
            v = missing_memory_byte_value();
        }
        return v;
    } else {
        throw std::logic_error("cannot read memory when power is off");
    }
}

void MemoryRow::write(const std::uint16_t address, const std::uint8_t data) {
    if (this->power) {
        this->values_stored[address] = data;
    } else {
        throw std::logic_error("cannot write memory when power is off");
    }
}
