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

WozFile::WozFile() : lastQuarterTrack(0) {
    unload();
}

WozFile::~WozFile() {
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

    std::uint32_t woz1;
    in.read((char*)&woz1, sizeof(woz1));
    if (woz1 != 0x315A4F57u) {
        printf("WOZ1 magic bytes missing.");
        throw "WOZ1 magic bytes missing";
    }
    printf("WOZ1 magic bytes present\n");

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
                five_25 = (buf[1]==1);
                printf("Disk type: %s\n", five_25 ? "5.25" : buf[1]==2 ? "3.5" : "?");
                this->writable = !(buf[2]==1);
                printf("Write protected?: %s\n", this->writable ? "No" : "Yes");
                printf("Imaged with cross-track sync?: %s\n", buf[3]==1 ? "Yes" : "No");
                printf("MC3470 fake bits removed?: %s\n", buf[4]==1 ? "Yes" : "No");
                printf("Creator: \"%.32s\"\n", buf+5);
                delete[] buf;
            }
            break;
            case 0x50414D54: { // TMAP
                std::uint8_t* buf = new std::uint8_t[chunk_size];
                in.read((char*)buf, chunk_size);
                if (!five_25) {
                    printf("Can only handle 5.25 floppy disk images.\n");
                    throw "Can only handle 5.25 floppy disk images.";
                } else {
                    printf("\x1b[31;47m-------------------------------------------\x1b[0m\n");
                    std::uint8_t i(0);
                    for (std::uint16_t t(0); t <= 3500; t += 25) {
                        if (buf[i] == 0xFFu) {
                            printf("\x1b[31;47m");
                        }
                        if (t % 100) {
                            printf("TMAP track 0x%02X +.%02d: TRKS track index 0x%02X", t/100, t%100, buf[i++]);
                        } else {
                            printf("TMAP track 0x%02X     : TRKS track index 0x%02X", t/100, buf[i++]);
                        }
                        printf("\x1b[0m\n");
                    }
                    printf("\x1b[31;47m-------------------------------------------\x1b[0m\n");
                    for (std::uint8_t qt(0); qt <= 140; ++qt) {
                        this->tmap[qt] = buf[qt];
                    }
                }
                delete[] buf;
            }
            break;
            case 0x534B5254: { // TRKS
                std::uint8_t* buf = new std::uint8_t[chunk_size];
                in.read((char*)buf, chunk_size);
                if (chunk_size % 6656) {
                    printf("chunk size is not an even multiple of 6656.");
                }
                this->c_trks = chunk_size / 6656;
                printf("Count of tracks: 0x%02X\n", this->c_trks);
                if (this->c_trks > 141) {
                    printf("Error: cannot handle more than 141 tracks.");
                    throw "Error: cannot handle more than 141 tracks";
                }
                for (std::uint8_t t(0); t < this->c_trks; ++t) {
                    printf("track 0x%02X:\n", t);
                    std::uint16_t usedBytes = *(std::uint16_t*)&buf[t*6656+6646+0];
                    printf("    used bytes: 0x%0X\n", usedBytes);
                    this->trk_bits[t] = *(std::uint16_t*)&buf[t*6656+6646+2];
                    printf("    count of bits: 0x%0X\n", this->trk_bits[t]);
                    std::uint16_t spliceBit = *(std::uint16_t*)&buf[t*6656+6646+4];
                    if (spliceBit == 0xFFFFu) {
                        printf("    no splice information exists\n");
                    } else {
                        printf("    bit after splice point: 0x%0X\n", spliceBit);
                    }
                    std::uint8_t spliceNib = *(std::uint8_t*)&buf[t*6656+6646+6];
                    printf("    Nibble value to use for splice: 0x%0X\n", spliceNib);
                    std::uint8_t cSpliceBit = *(std::uint8_t*)&buf[t*6656+6646+7];
                    printf("    Bit count of splice nibble: 0x%0X\n", cSpliceBit);

                    std::uint8_t *base = (std::uint8_t*)&buf[t*6656+0];
                    std::uint8_t *pd = base;
                    printf("    beginning of data: 0x: ");
                    for (int cd(0); cd < 32; ++cd) {
                        printf("%02X", *pd++);
                    }
                    printf("\n");
                    for (int i(0); i < 6646; ++i) {
                        this->trks[t][i] = *base++;
                    }
                }
                delete[] buf;
            }
            break;
            case 0x4154454D: { // META
                std::uint8_t* buf = new std::uint8_t[chunk_size];
                in.read((char*)buf, chunk_size);
                std::uint32_t i(0);
                char* pc((char*)buf);
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
//    std::ofstream out(filePath.c_str(),std::ios::binary);
    // TODO SAVE FILE!
//    out.flush();
//    out.close();

//    this->modified = false;
}

void WozFile::unload() {
    this->bit = 0x80u;
    this->byt = 0x00u;
    this->writable = true;
    this->loaded = false;
    this->filePath = "";
    this->modified = false;
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
 * In real life we don't care what track we're one, but for the
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
        double oldLen = this->trk_bits[this->tmap[this->lastQuarterTrack]];
        double newLen = this->trk_bits[this->tmap[currentQuarterTrack]];
        double ratio = newLen/oldLen;
        if (!(fabs(1-ratio) < 0.0001f)) {
            std::uint16_t newBit = (this->byt*8+bc(this->bit)) * ratio;
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
        printf("No disk to read from; will generate random data.\n");
        return false; // there's no disk, so no pulse
    }

    if (this->tmap[currentQuarterTrack] == 0xFFu) {
        return false; // empty track
    }

    if (this->c_trks <= this->tmap[currentQuarterTrack]) { // shouldn't happen
        printf("INVALID quarterTrack %d mapped to TRKS index %d (count of tracks: %d)\n", currentQuarterTrack, this->tmap[currentQuarterTrack], this->c_trks);
        return false;
    }

    return this->trks[this->tmap[currentQuarterTrack]][this->byt] & this->bit;
}

void WozFile::setBit(std::uint8_t currentQuarterTrack, bool on) {
    if (!isLoaded()) {
        return; // there's no disk to write data to
    }

    if (!this->writable) {
        return; // write-protected
    }

    if (this->c_trks <= this->tmap[currentQuarterTrack]) {
        return; // TODO track doesn't exist: create a new one
    }

    if (this->tmap[currentQuarterTrack] == 0xFFu) {
        // track does not exist, create new one
    }

    // TODO extend track length if needed????

    if (on) {
        this->trks[this->tmap[currentQuarterTrack]][this->byt] |= this->bit;
    } else {
        this->trks[this->tmap[currentQuarterTrack]][this->byt] &= ~this->bit;
    }
}
