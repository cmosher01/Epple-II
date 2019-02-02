#include "memorychip.h"
#include <stdexcept>
#include <algorithm>
#include <vector>

#define K 1024

MemoryChip::MemoryChip(const std::uint16_t size, const std::string &id_model):
    size_bits(size),
    id_model(id_model) {
    if (size_bits != 4*K && size_bits != 16*K) {
        throw std::out_of_range("MemoryChip must be 4K or 16K");
    }
}

MemoryChip::MemoryChip(const MemoryChip &that):
    size_bits(that.size_bits),
    id_model(that.id_model) {
}

MemoryChip &MemoryChip::operator=(const MemoryChip &that) {
    this->size_bits = that.size_bits;
    this->id_model = that.id_model;
    return *this;
}

MemoryChip::~MemoryChip() {
}

std::uint16_t MemoryChip::size() const {
    return this->size_bits;
}

std::string MemoryChip::id() const {
    return this->id_model;
}

bool MemoryChip::exists() const {
    return true;
}

static void bitflag(const bool on, const std::uint8_t mask, std::uint8_t &byte) {
    if (on) {
        byte |= mask;
    } else {
        byte &= ~mask;
    }
}

//#define CYCLE 128u
#define CYCLE 2u

void MemoryChip::init(const std::uint8_t mask, std::vector<std::uint8_t> &bytes, const std::uint16_t size) const {
    bool on = false;
    std::uint8_t c = 0u;
    for (std::uint16_t i = 0u; i < std::min(size, this->size_bits); ++i) {
        bitflag(on, mask, bytes[i]);
        if (CYCLE <= ++c) {
            c = 0u;
            on = !on;
        }
    }
}

MemoryChipEmptySocket::MemoryChipEmptySocket():
    MemoryChip(4*K,"[empty]") {
}

MemoryChipEmptySocket::~MemoryChipEmptySocket() {
}

bool MemoryChipEmptySocket::exists() const {
    return false;
}
