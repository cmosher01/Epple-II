/*
    epple2

    Copyright © 2018, Christopher Alan Mosher, Shelton, CT, USA. <cmosher01@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY, without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "wozfile.h"

#include <istream>
#include <ostream>
#include <fstream>
#include <cmath>
#include <cstring>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

struct trk_t {
    std::uint16_t blockFirst;
    std::uint16_t blockCount;
    std::uint32_t bitCount;
};

WozFile::WozFile() : tmap(0) {
    for (int i(0); i < C_QTRACK; ++i) {
        this->trk[i] = 0;
    }
    unload();
}

WozFile::~WozFile() {
}

static void print_compat(std::uint16_t compat, std::uint16_t mask, const char *name) {
    if (compat & mask) {
        printf("    Apple %s\n", name);
    }
}

bool WozFile::load(const std::string& filePath) {
    std::ifstream in(filePath.c_str(),std::ios::binary|std::ios::in);
    if (!in.is_open()) {
        printf("Error opening file: %d\n", errno);
        return false;
    }
    if (isLoaded()) {
        unload();
    }

    std::uint32_t woz2;
    in.read((char*)&woz2, sizeof(woz2));
    if (woz2 != 0x325A4F57u) {
        printf("WOZ2 magic bytes missing.");
        throw "WOZ2 magic bytes missing";
    }
    printf("WOZ2 magic bytes present\n");

    std::uint32_t sanity;
    in.read((char*)&sanity, sizeof(sanity));
    if (sanity != 0x0A0D0AFFu) {
        printf("FF 0A 0D 0A bytes corrupt.\n");
        throw "FF 0A 0D 0A bytes corrupt";
    }

    std::uint32_t crc_given;
    in.read((char*)&crc_given, sizeof(crc_given));
    printf("Read given CRC: %08x\n", crc_given);
    // TODO verify CRC

    std::uint32_t chunk_id;
    std::uint32_t chunk_size;
    bool five_25(false);
    while (in.read((char*)&chunk_id, sizeof(chunk_id))) {
        in.read((char*)&chunk_size, sizeof(chunk_size));
        printf("Chunk %.4s of size 0x%08x\n", (char*)&chunk_id, chunk_size);
        switch (chunk_id) {
            case 0x4F464E49: { // INFO
                std::uint8_t* buf = new std::uint8_t[chunk_size];
                in.read((char*)buf, chunk_size);
                printf("INFO version %d\n", *buf);
                if (*buf != 2) {
                    printf("File is not WOZ2 version.\n");
                    throw "File is not WOZ2 version";
                }
                five_25 = (buf[1]==1);
                printf("Disk type: %s\n", five_25 ? "5.25" : buf[1]==2 ? "3.5" : "?");
                if (!five_25) {
                    printf("Only 5 1/4\" disk images are supported.\n");
                    throw "Only 5 1/4\" disk images are supported";
                }
                this->writable = !(buf[2]==1);
                printf("Write protected?: %s\n", this->writable ? "No" : "Yes");
                this->sync = buf[3]==1;
                printf("Imaged with cross-track sync?: %s\n", this->sync ? "Yes" : "No");
                this->cleaned = buf[4]==1;
                printf("MC3470 fake bits removed?: %s\n", this->cleaned ? "Yes" : "No");
                this->creator = std::string((char*)buf+5, 32);
                printf("Creator: \"%.32s\"\n", buf+5);
                this->timing = buf[39];
                printf("Timing: %d/8 microseconds per bit\n", this->timing);
                std::uint16_t compat = *((std::uint16_t*)buf+40);
                printf("Campatible hardware: ");
                if (!compat) {
                    printf("unknown\n");
                } else {
                    printf("\n");
                    print_compat(compat, 0x0001, "][");
                    print_compat(compat, 0x0002, "][ plus");
                    print_compat(compat, 0x0004, "//e");
                    print_compat(compat, 0x0008, "//c");
                    print_compat(compat, 0x0010, "//e (enhanced)");
                    print_compat(compat, 0x0020, "IIGS");
                    print_compat(compat, 0x0040, "IIc Plus");
                    print_compat(compat, 0x0080, "///");
                    print_compat(compat, 0x0100, "/// plus");
                }
                delete[] buf;
            }
            break;
            case 0x50414D54: { // TMAP
                this->tmap = new std::uint8_t[chunk_size];
                in.read((char*)this->tmap, chunk_size);

                this->initialQtrack = 0;
                while (this->initialQtrack < chunk_size && this->tmap[this->initialQtrack] == 0xFFu) {
                    ++this->initialQtrack;
                }
                if (this->initialQtrack == chunk_size) {
                    this->initialQtrack = 0xFFu;
                    printf("Could not find any initial track (%02X).\n", this->initialQtrack);
                }

                this->finalQtrack = chunk_size-1;
                while (this->finalQtrack != 0xFFu && this->tmap[this->finalQtrack] == 0xFFu) {
                    --this->finalQtrack;
                }
                if (this->finalQtrack == 0xFFu) {
                    printf("Could not find any final track (%02X).\n", this->finalQtrack);
                }

                printf("\x1b[31;47m-------------------------------------------\x1b[0m\n");
                for (std::uint8_t qt(0); qt < chunk_size; ++qt) {
                    const std::uint16_t t(qt*25);
                    if (tmap[qt] == 0xFFu) {
                        printf("\x1b[31;47m");
                    }
                    if (t % 100) {
                        printf("TMAP track 0x%02X +.%02d: TRKS track index 0x%02X", t/100, t%100, tmap[qt]);
                    } else {
                        printf("TMAP track 0x%02X     : TRKS track index 0x%02X", t/100, tmap[qt]);
                    }
                    printf("\x1b[0m");
                    if (qt == this->initialQtrack && qt == this->finalQtrack) {
                        printf(" <-- lone track");
                    } else if (qt == this->initialQtrack) {
                        printf(" <-- initial track");
                    } else if (qt == this->finalQtrack) {
                        printf(" <-- final track");
                    }
                    printf("\n");
                }
                printf("\x1b[31;47m-------------------------------------------\x1b[0m\n");
            }
            break;
            case 0x534B5254: { // TRKS
                if (chunk_size < C_QTRACK*8) {
                    throw "TRKS chunk doesn't have 160 track entries";
                }
                std::uint8_t* buf = new std::uint8_t[chunk_size];
                in.read((char*)buf, chunk_size);
                std::uint8_t* te = buf;
                for (std::uint8_t qt(0); qt < C_QTRACK; ++qt) {
                    struct trk_t ts;
                    ts.blockFirst = *((std::uint16_t*)te)-3;
                    te += 2;
                    ts.blockCount = *((std::uint16_t*)te);
                    te += 2;
                    ts.bitCount = *((std::uint32_t*)te);
                    te += 4;
                    if (ts.blockCount) {
                        printf("TRK index %02X: start byte in BITS %08x; %08x bytes; %08x bits ", qt, ts.blockFirst<<9, ts.blockCount<<9, ts.bitCount);
                        this->trk_bits[qt] = ts.bitCount;
                        this->trk_byts[qt] = ts.blockCount<<9;
                        this->trk[qt] = new std::uint8_t[this->trk_byts[qt]];
                        memcpy(this->trk[qt], buf+C_QTRACK*8+(ts.blockFirst<<9), this->trk_byts[qt]);
                        printf("("
                               BYTE_TO_BINARY_PATTERN
                               BYTE_TO_BINARY_PATTERN
                               BYTE_TO_BINARY_PATTERN
                               BYTE_TO_BINARY_PATTERN
                               BYTE_TO_BINARY_PATTERN
                               BYTE_TO_BINARY_PATTERN
                               BYTE_TO_BINARY_PATTERN
                               BYTE_TO_BINARY_PATTERN
                               "...)\n",
                            BYTE_TO_BINARY(this->trk[qt][0]),
                            BYTE_TO_BINARY(this->trk[qt][1]),
                            BYTE_TO_BINARY(this->trk[qt][2]),
                            BYTE_TO_BINARY(this->trk[qt][3]),
                            BYTE_TO_BINARY(this->trk[qt][4]),
                            BYTE_TO_BINARY(this->trk[qt][5]),
                            BYTE_TO_BINARY(this->trk[qt][6]),
                            BYTE_TO_BINARY(this->trk[qt][7]));
                    }
                }
                delete[] buf;
            }
            break;
            case 0x4154454D: { // META
                std::uint8_t* buf = new std::uint8_t[chunk_size];
                in.read(reinterpret_cast<char*>(buf), chunk_size);
                std::uint32_t i(0);
                char* pc(reinterpret_cast<char*>(buf));
                while (i++ < chunk_size) {
                    if (*pc == '\t') {
                        printf(": ");
                    } else {
                        printf("%c", *pc);
                    }
                    pc++;
                }
                delete[] buf;
            }
            break;
            default: { // unknown type of chunk; safely skip past it and ignore it
                // TODO save all unknown chunks and write out during save (at end of file)
                in.seekg(chunk_size, in.cur);
            }
            break;
        }
    }





    in.close();

    this->filePath = filePath;

    checkForWriteProtection();

    this->loaded = true;
    this->modified = false;

    // TODO if the file is not write protected, then "fill out" our tracks:
    // 1. create new TRKS entries and full copies of duplicate TMAP pointers
    // 2. create new TRKS entries (and fill with zeroes) for FF entries in TMAP. (with length of avg of adjacent tracks?)
    //
    // ...
    // 6.50: FF x            08 - 00 00 00
    // 6.75: 02 \            02 - d5 aa 96
    // 7.00: 02 - d5 aa 96   03 - d5 aa 96
    // 7.25: 02 /            04 - d5 aa 96
    // 7.50: FF x            09 - 00 00 00
    // 7.75: 05 \            05 - d5 aa 96
    // 8.00: 05 - d5 aa 96   06 - d5 aa 96
    // 8.25: 05 /            07 - d5 aa 96
    // 8.50: FF x            0A - 00 00 00
    // ...

    return true;
}

void WozFile::checkForWriteProtection() {
    if (!this->writable) {
        return;
    }

    std::ofstream outf(filePath.c_str(),std::ios::binary|std::ios::app);
    this->writable = outf.is_open();
    outf.close();
}

void WozFile::save() {
    if (isWriteProtected() || !isLoaded()) {
        return;
    }
    std::ofstream out(filePath.c_str(), std::ios::binary);

    std::uint32_t woz2(0x325A4F57u);
    out.write((char*)&woz2, sizeof(woz2));
    std::uint32_t sanity(0x0A0D0AFFu);
    out.write((char*)&sanity, sizeof(sanity));
    std::uint32_t crc(0u); // TODO calc CRC
    out.write((char*)&crc, sizeof(crc));



    std::uint32_t chunk_id;
    std::uint32_t chunk_size;

    // INFO
    chunk_id = 0x4F464E49u;
    out.write((char*)&chunk_id, sizeof(chunk_id));
    chunk_size = 60;
    out.write((char*)&chunk_size, sizeof(chunk_size));

    std::uint8_t vers(2);
    out.write((char*)&vers, sizeof(vers));
    std::uint8_t floppy_size(1);
    out.write((char*)&floppy_size, sizeof(floppy_size));
    std::uint8_t write_protected(!this->writable);
    out.write((char*)&write_protected, sizeof(write_protected));
    std::uint8_t sync(0);
    out.write((char*)&sync, sizeof(sync));
    std::uint8_t cleaned(0);
    out.write((char*)&cleaned, sizeof(cleaned));
    const char* creator = "epple2                          ";
    out.write(creator, 32);
    std::uint8_t sided(1);
    out.write((char*)&sided, sizeof(sided));
    std::uint8_t sector_format(0);
    out.write((char*)&sector_format, sizeof(sector_format));
    std::uint8_t us_per_bit(32);
    out.write((char*)&us_per_bit, sizeof(us_per_bit));
    std::uint16_t compat_hw(0);
    out.write((char*)&compat_hw, sizeof(compat_hw));
    std::uint16_t ram(0);
    out.write((char*)&ram, sizeof(ram));

    std::uint16_t largest_track(0);
    for (std::uint8_t qt(0); qt < C_QTRACK; ++qt) {
        if (largest_track < (this->trk_byts[qt]>>9)) {
            largest_track = (this->trk_byts[qt]>>9);
        }
    }
    out.write((char*)&largest_track, sizeof(largest_track));
    //fill
    for (int i(0); i < 14; ++i) {
        std::uint8_t fill(0);
        out.write((char*)&fill, sizeof(fill));
    }


    // TMAP
    chunk_id = 0x50414D54u;
    out.write((char*)&chunk_id, sizeof(chunk_id));
    chunk_size = 160;
    out.write((char*)&chunk_size, sizeof(chunk_size));
    for (std::uint8_t qt(0); qt < C_QTRACK; ++qt) {
        out.write((char*)&this->tmap[qt], 1);
    }



    // TRKS
    chunk_id = 0x534B5254u;
    out.write((char*)&chunk_id, sizeof(chunk_id));
    chunk_size = 0;
    for (std::uint8_t qt(0); qt < C_QTRACK; ++qt) {
        chunk_size += 8+this->trk_byts[qt];
    }
    out.write((char*)&chunk_size, sizeof(chunk_size));
    uint16_t block(3);
    for (std::uint8_t qt(0); qt < C_QTRACK; ++qt) {
        struct trk_t ts;
        ts.blockFirst = block;
        ts.blockCount = (this->trk_byts[qt]>>9);
        block += ts.blockCount;
        ts.bitCount = this->trk_bits[qt];
        out.write((char*)&ts, sizeof(ts));
    }

    // (BITS)
    for (std::uint8_t qt(0); qt < C_QTRACK; ++qt) {
        if (this->trk[qt]) {
            out.write(reinterpret_cast<char*>(this->trk[qt]), this->trk_byts[qt]);
        }
    }
    out.flush();
    out.close();

    this->modified = false;
}

void WozFile::unload() {
    this->bit = 0x80u;
    this->byt = 0x00u;
    this->writable = true;
    this->loaded = false;
    this->filePath = "";
    this->modified = false;
    if (this->tmap) {
        delete [] this->tmap;
        this->tmap = 0;
    }
    for (int i(0); i < C_QTRACK; ++i) {
        if (this->trk[i]) {
            delete [] this->trk[i];
            this->trk[i] = 0;
        }
    }
}

static std::uint8_t bc(std::uint8_t bit) {
    switch (bit) {
    case 0x80u: return 0u;
    case 0x40u: return 1u;
    case 0x20u: return 2u;
    case 0x10u: return 3u;
    case 0x08u: return 4u;
    case 0x04u: return 5u;
    case 0x02u: return 6u;
    case 0x01u: return 7u;
    }
    return 255u; // should never happen
}

static std::uint8_t cb(std::uint8_t bit) {
    switch (bit) {
    case 0u: return 0x80u;
    case 1u: return 0x40u;
    case 2u: return 0x20u;
    case 3u: return 0x10u;
    case 4u: return 0x08u;
    case 5u: return 0x04u;
    case 6u: return 0x02u;
    case 7u: return 0x01u;
    }
    return 255u; // should never happen
}

/*
 * Rotate the floppy disk by one bit.
 * In real life we don't care what track we're on, but for the
 * emulator we need to know. This is because the tracks within the
 * WOZ file could be different lengths. So in order to know when
 * we need to loop back to the beginning of the track (circular
 * track on the floppy), we need to know the actual bit length
 * of that track in our WOZ file.
 */
void WozFile::rotateOneBit(std::uint8_t currentQuarterTrack) {
    if (!isLoaded()) {
        return; // there's no disk to rotate
    }

    if (C_QTRACK <= currentQuarterTrack) {
        printf("attempt to move to illegal track.\n");
        return;
    }

    // Move to next bit
    this->bit >>= 1;

    // If we hit end of this byte, move on to beginning of next byte
    if (this->bit == 0) {
        ++this->byt;
        this->bit = 0x80u;
    }

    // this is an empty track, so any of the following byte/bit
    // adjustments don't apply now (they will be handled the
    // next time we hit a non-empty track)
    if (this->tmap[currentQuarterTrack] == 0xFFu) {
        return;
    }

    // If we changed tracks since the last time we were called,
    // we may need to adjust the rotational position. The new
    // position will be at the same relative position as the
    // previous, based on each track's length (tracks can be of
    // different lengths in the WOZ image).
    if (currentQuarterTrack != this->lastQuarterTrack) {
        const double oldLen = this->trk_bits[this->tmap[this->lastQuarterTrack]];
        const double newLen = this->trk_bits[this->tmap[currentQuarterTrack]];
        const double ratio = newLen/oldLen;
        if (!(fabs(1-ratio) < 0.0001)) {
            const std::uint16_t newBit = static_cast<std::uint16_t>(round((this->byt*8+bc(this->bit)) * ratio));
            this->byt = newBit / 8;
            this->bit = cb(newBit % 8);
        }
        this->lastQuarterTrack = currentQuarterTrack;
    }

    // Check for hitting the end of our track,
    // and if so, move back to the beginning.
    // This is how we emulate a circular track on the floppy.
    if (this->trk_bits[this->tmap[currentQuarterTrack]] <= this->byt*8+bc(this->bit)) {
        this->byt = 0;
        this->bit = 0x80u;
    }
}



bool WozFile::getBit(std::uint8_t currentQuarterTrack) {
    if (!isLoaded()) {
//        printf("No disk to read from; will generate random data.\n");
        return false; // there's no disk, so no pulse
    }

    if (this->tmap[currentQuarterTrack] == 0xFFu) {
//        printf("Reading from uninitialized track; will generate random data.\n");
        return false; // empty track
    }

    return this->trk[this->tmap[currentQuarterTrack]][this->byt] & this->bit;
}

void WozFile::setBit(std::uint8_t currentQuarterTrack, bool on) {
    if (!isLoaded()) {
        return; // there's no disk to write data to
    }

    if (!this->writable) {
        return; // write-protected
    }

//    printf("%c",(on?'1':'0')); fflush(stdout);
    if (on) {
        this->trk[this->tmap[currentQuarterTrack]][this->byt] |= this->bit;
    } else {
        this->trk[this->tmap[currentQuarterTrack]][this->byt] &= ~this->bit;
    }

    // TODO: also write preceding and following quarter tracks (at relative position, and if they exist)
}
