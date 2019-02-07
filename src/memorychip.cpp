#include "memorychip.h"
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstdlib>

#define K 1024

static MemoryChip *chip_empty = new MemoryChipEmptySocket();
static MemoryChip *chip_4096 = new MemoryChip(4u*K, "4096");
static MemoryChip *chip_4116 = new MemoryChip(16u*K, "4116");
static MemoryChip *chip_mm5290 = new MemoryChipMM5290();
static MemoryChip *chip_mk4116 = new MemoryChipMK4116();
static MemoryChip *chip_mcm4116 = new MemoryChipMCM4116();

MemoryChip *MemoryChip::instance(const std::string &id_model) {
    if (id_model == "-") {
        return chip_empty;
    }
    if (id_model == "4K" || id_model == "4096") {
        return chip_4096;
    }
    if (id_model == "16K" || id_model == "4116") {
        return chip_4116;
    }
    if (id_model == "MM5290") {
        return chip_mm5290;
    }
    if (id_model == "MK4116") {
        return chip_mk4116;
    }
    if (id_model == "MCM4116") {
        return chip_mcm4116;
    }
    throw std::logic_error("unrecognized RAM chip model: "+id_model);
}

MemoryChip::MemoryChip(const std::uint16_t size, const std::string &id_model):
    size_bits(size),
    id_model(id_model) {
    if (size_bits != 4*K && size_bits != 16*K) {
        throw std::out_of_range("MemoryChip must be 4K or 16K");
    }
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

void MemoryChip::init(const std::uint8_t mask, std::vector<std::uint8_t> &bytes, const std::uint16_t size) const {
    rand_init(mask, bytes, size, 128u, 0.0, 0.0);
}

void MemoryChip::rand_init(const std::uint8_t mask, std::vector<std::uint8_t> &bytes, const std::uint16_t size, const std::uint_fast32_t cycle, const double glitch, const double chaos) const {
    const double GLITCH = glitch*0.001;
    const double CHAOS = chaos*0.05;

    std::uint_fast32_t c_cycle = 0u;
    std::uint_fast32_t c_chaos16 = 65u;
    std::uint_fast32_t c_chaos16sub = 15u;
    bool on = false;
    for (std::uint_fast16_t i = 0u; i < 16*K; ++i) {
        double r = static_cast<double>(std::rand())/RAND_MAX;
        bool is_rand = false;
        if (r < GLITCH && c_chaos16 == 65u) {
            --c_chaos16;
        }
        if (c_chaos16 < 65u) {
            if (c_chaos16sub-- == 0u) {
                is_rand = true;
                c_chaos16sub = 15u;
                if (c_chaos16-- == 0u) {
                    c_chaos16 = 65u;
                }
            }
        }
        is_rand |= (r < CHAOS);
        bitflag(is_rand?!on:on, mask, bytes[i]);
        if (c_cycle++ == cycle-1) {
            on = !on;
            c_cycle = 0u;
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



MemoryChipMM5290::MemoryChipMM5290(): // National Semiconductor
    MemoryChip(16*K,"MM5290") {
}

MemoryChipMM5290::~MemoryChipMM5290() {
}

void MemoryChipMM5290::init(const std::uint8_t mask, std::vector<std::uint8_t> &bytes, const std::uint16_t size) const {
    rand_init(mask, bytes, size, 128u, 0.1, 0.05);
}


MemoryChipMK4116::MemoryChipMK4116(): // Mostek
    MemoryChip(16*K,"MK4116") {
}

MemoryChipMK4116::~MemoryChipMK4116() {
}

void MemoryChipMK4116::init(const std::uint8_t mask, std::vector<std::uint8_t> &bytes, const std::uint16_t size) const {
    rand_init(mask, bytes, size, 2u, 0.1, 0.05);
}



MemoryChipMCM4116::MemoryChipMCM4116(): // Motorola
    MemoryChip(16*K,"MCM4116") {
}

MemoryChipMCM4116::~MemoryChipMCM4116() {
}

void MemoryChipMCM4116::init(const std::uint8_t mask, std::vector<std::uint8_t> &bytes, const std::uint16_t size) const {
    rand_init(mask, bytes, size, 4096u, 0.1, 0.05);
}
