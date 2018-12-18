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

#ifndef WOZFILE_H
#define WOZFILE_H

#include <string>
#include <cstdint>

/**
 * @brief WOZ file (floppy disk image)
 * Represents a floppy disk. We only handle 5.25" disks.
 * The disk has 141 quarter-track possible positions, but
 * typically will have 35 tracks.
 * The floppy disk "knows" it's rotational position.
 *
 * Note, the floppy has no notion of the current track;
 * rather, that information is known by the stepper motor and arm.
 */
class WozFile {
    std::string fileName;
    std::string filePath;
    bool loaded;
    bool modified;

    // represents (negation of) write-protect tab
    // We consider the disk to be write-protected if the user does
    // not have write access to the file OR the WOZ file INFO chuck
    // indicates that the floppy was write-protected.
    bool writable;

    std::uint8_t tmap[141]; // quarter-tracks from T0 through T35, values are indexes into trks
    std::uint8_t c_trks; // count of actual tracks:
    std::uint8_t trks[141][6646]; // 141 is theoretical max; will always be less
    std::uint16_t trk_bits[141]; // count of bits in each track

    // bit and byt together represent the rotational position
    // of the floppy disk.
    // bit is a mask indicating current bit within current byte (byt).
    // WOZ file bits are packed into bytes starting at bit 7
    // through bit 0 of the first byte, then bits 7-0 of the
    // next byte, etc., from bits 0 through trk_bits-1
    std::uint8_t bit;
    std::uint16_t byt;

    std::uint8_t lastQuarterTrack;

    void checkForWriteProtection();

public:
    WozFile();
    ~WozFile();

    bool load(const std::string& filePath);

    std::string getFileName() {
        return this->fileName;
    }

    bool isLoaded() {
        return this->loaded;
    }

    void save();
    void unload();
    bool isWriteProtected() {
        return !this->writable;
    }

    bool isModified() {
        return this->modified;
    }

    void rotateOneBit(std::uint8_t currentQuarterTrack);
    bool getBit(std::uint8_t currentQuarterTrack);
    void setBit(std::uint8_t currentQuarterTrack);
};

#endif // WOZFILE_H
