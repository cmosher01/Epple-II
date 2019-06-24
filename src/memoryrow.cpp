#include "memoryrow.h"
#include <exception>
#include <cstdlib>



MemoryRow::MemoryRow(const char label):
    label(label) {
    this->chips.fill(MemoryChip::instance("-"));
}

MemoryRow::~MemoryRow() {
}

void MemoryRow::insert_chip(MemoryChip *chip, const std::uint_fast8_t socket) {
    if (socket < 8u) {
        remove_chip(socket);
        if (chip->exists()) {
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
        this->chips[socket] = MemoryChip::instance("-");
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
        const MemoryChip *chip = this->chips[i_chip];
        if (chip->exists()) {
            const std::uint16_t s = chip->size();
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
            const MemoryChip *chip = this->chips[i_bit];
            if (chip->exists()) {
                chip->init(mask_bit, this->values_stored, size());
            }
            mask_bit <<= 1;
        }
    }
}

std::uint16_t MemoryRow::size() const {
    return static_cast<std::uint16_t>(this->values_stored.size());
}

std::uint8_t MemoryRow::read(const std::uint16_t address_offset, std::uint8_t data) const {
    if (this->power) {
        if (address_offset < this->values_stored.size()) {
            /* We need to float the data bits corresponding to missing chips:
             * 01010101 data bits (previously places on data bus from elsewhere)
             * 00110011 mask of memory chips (1=missing, 0=present)
             * 00001111 bits stored in chips
             * --------
             * 00010001 data & missing
             * 11001100 ~missing
             * 00001100 stored & ~missing
             * --------
             * 00011101 (stored & ~missing) | (data & missing)
             */
             data = (this->values_stored[address_offset] & ~this->missing_bits) | (data & this->missing_bits);
        }
        return data;
    } else {
        throw std::logic_error("cannot read memory when power is off");
    }
}

void MemoryRow::write(const std::uint16_t address, const std::uint8_t data) {
    if (this->power) {
        // if there are missing bits, they do get stored, so we need to
        // be careful to mask them out when giving them back (in read method)
        this->values_stored[address] = data;
    } else {
        throw std::logic_error("cannot write memory when power is off");
    }
}

std::string MemoryRow::chip_id(std::uint_fast8_t socket) const {
    return this->chips[socket]->id();
}
