#include "memoryrandomaccess.h"
#include <cstdint>
#include <cstdio>
#include <exception>

#define K 1024u

MemoryRandomAccess::MemoryRandomAccess(int &revision):
    revision(revision),
    rowE('E'),
    strapE(rowE),
    rowD('D'),
    strapD(rowD),
    rowC('C'),
    strapC(rowC) {
}

MemoryRow &MemoryRandomAccess::row_of(const std::string &row) {
    if (row == "E") {
        return this->rowE;
    }
    if (row == "D") {
        return this->rowD;
    }
    if (row == "C") {
        return this->rowC;
    }
    throw std::logic_error("expected C/D/E");
}

MemoryStrapping &MemoryRandomAccess::strapping_of(const std::string &row) {
    if (row == "E") {
        return this->strapE;
    }
    if (row == "D") {
        return this->strapD;
    }
    if (row == "C") {
        return this->strapC;
    }
    throw std::logic_error("expected C/D/E");
}

void MemoryRandomAccess::insert_chip(const std::string &row, MemoryChip *chip, const std::uint_fast8_t socket) {
    row_of(row).insert_chip(chip, socket);
}

void MemoryRandomAccess::remove_chip(const std::string &row, const std::uint_fast8_t socket) {
    row_of(row).remove_chip(socket);
}

void MemoryRandomAccess::strap_to(const std::string &row, std::uint16_t addr_base, std::uint16_t addr_size) {
    strapping_of(row).strap_to(addr_base, addr_size);
}

bool MemoryRandomAccess::k20or24() const {
    const std::uint32_t k = this->rowC.size() + this->rowD.size() + this->rowD.size();
    return (k==20*K) || (k==24*K);
}

/* for 20K or 24K on rev. 0: pages 40-5F are dup w/ 60-7F */
std::uint8_t MemoryRandomAccess::buggyRamRead(const std::uint16_t address) const {
    std::uint16_t ax = address & ~0x2000u;
    if (this->strapE.contains(ax)) {
        return this->strapE.read(ax);
    }
    if (this->strapD.contains(ax)) {
        return this->strapD.read(ax);
    }
    if (this->strapC.contains(ax)) {
        return this->strapC.read(ax);
    }
    ax = address | 0x2000u;
    if (this->strapE.contains(ax)) {
        return this->strapE.read(ax);
    }
    if (this->strapD.contains(ax)) {
        return this->strapD.read(ax);
    }
    if (this->strapC.contains(ax)) {
        return this->strapC.read(ax);
    }
    return MemoryRow::missing_memory_byte_value();
}

std::uint8_t MemoryRandomAccess::read(const std::uint16_t address) const {
    if (this->revision == 0 && k20or24() && ((address & 0xC000u) == 0x4000u)) {
        return buggyRamRead(address);
    }
    if (this->strapE.contains(address)) {
        return this->strapE.read(address);
    }
    if (this->strapD.contains(address)) {
        return this->strapD.read(address);
    }
    if (this->strapC.contains(address)) {
        return this->strapC.read(address);
    }
    return MemoryRow::missing_memory_byte_value();
}

void MemoryRandomAccess::buggyRamWrite(std::uint16_t address, const std::uint8_t data) {
    std::uint16_t ax = address & ~0x2000u;
    if (this->strapE.contains(ax)) {
        this->strapE.write(ax, data);
    }
    if (this->strapD.contains(ax)) {
        this->strapD.write(ax, data);
    }
    if (this->strapC.contains(ax)) {
        this->strapC.write(ax, data);
    }
    ax = address | 0x2000u;
    if (this->strapE.contains(ax)) {
        this->strapE.write(ax, data);
    }
    if (this->strapD.contains(ax)) {
        this->strapD.write(ax, data);
    }
    if (this->strapC.contains(ax)) {
        this->strapC.write(ax, data);
    }
}

void MemoryRandomAccess::write(const std::uint16_t address, const std::uint8_t data) {
    if (this->revision == 0 && k20or24() && ((address & 0xC000u) == 0x4000u)) {
        buggyRamWrite(address, data);
    }
    if (this->strapE.contains(address)) {
        this->strapE.write(address, data);
    }
    if (this->strapD.contains(address)) {
        this->strapD.write(address, data);
    }
    if (this->strapC.contains(address)) {
        this->strapC.write(address, data);
    }
}

void MemoryRandomAccess::powerOn() {
    this->rowE.powerOn();
    this->rowD.powerOn();
    this->rowC.powerOn();
}

void MemoryRandomAccess::powerOff() {
    this->rowE.powerOff();
    this->rowD.powerOff();
    this->rowC.powerOff();
}

static void dump_row(const std::string &name, const MemoryRow &row) {
    std::printf("RAM %s ", name.c_str());
    for (std::uint_fast8_t i = 0u; i < 8u; ++i) {
        std::printf("%s ", row.chip_id(i).c_str());
    }
    std::printf("\n");
}

void MemoryRandomAccess::dump_config() const {
    std::printf("motherboard RAM rows contain chips:\n");
    std::printf("    "); dump_row("E", this->rowE);
    std::printf("    "); dump_row("D", this->rowD);
    std::printf("    "); dump_row("C", this->rowC);
    std::printf("address $1000 blocks strapped to RAM row:\n");
    std::uint_fast8_t i = 0xCu;
    while (i--) {
        std::string r;
        const uint16_t addr = i*0x1000u;
        if (this->strapE.contains(addr)) {
            r = "E";
        } else if (this->strapD.contains(addr)) {
            r = "D";
        } else if (this->strapC.contains(addr)) {
            r = "C";
        } else {
            r = "[not strapped]";
        }
        std::printf("    $%X %s\n", i, r.c_str());
    }
    std::printf("total RAM: %dK\n", (this->strapE.size()+this->strapD.size()+this->strapC.size())/1024u);
}
