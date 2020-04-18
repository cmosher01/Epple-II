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

static const std::uint8_t C_QTRACK(160);

/**
 * @brief WOZ file (floppy disk image)
 * Represents a floppy disk. We only handle 5.25" disks.
 * A standard disk has 141 quarter-track possible positions
 * (0.00, 0.25, 0.5, 0.75, 1.00, ..., 33.5, 33.75, 34.00),
 * but will typically have 35 tracks (0.00, 1.00, ..., 34.00).
 *
 * Non-standard disks can have more tracks. Especially, modified DOSes
 * exists that can easily handle one additional track. Also, the WOZ2
 * specification allows for more tracks.
 *
 * Here we handle what the WOZ2 specification can handle, that is,
 * 160 quarter-track positions, tracks 0.00 through 39.75.
 *
 * The floppy disk "knows" it's rotational position.
 *
 * Note, the floppy disk has no concept of a "current" track;
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

    // map of quarter-tracks from T00.00 through T39.75, values are indexes into trk
    std::uint8_t* tmap;
    // first actual quarter-track (e.g., 0 for normal 35-track disk), or 0xFF if no tracks
    std::uint8_t initialQtrack;
    // last actual quarter-track (e.g., 136 for normal 35-track disk), or 0xFF if no tracks
    std::uint8_t finalQtrack;

    // array of track buffers (null if no track data)
    std::uint8_t* trk[C_QTRACK];
    // count of bits in each track
    std::uint32_t trk_bits[C_QTRACK];
    // count of allocated bytes
    std::uint16_t trk_byts[C_QTRACK];

    // bit and byt together represent the rotational position
    // of the floppy disk.
    // bit is a mask indicating current bit within current byte (byt).
    // WOZ file bits are packed into bytes starting at bit 7
    // through bit 0 of the first byte, then bits 7-0 of the
    // next byte, etc., from bits 0 through trk_bits[qt]-1
    std::uint8_t bit;
    std::uint16_t byt;

    // We need to store which track were on, only so we can detect
    // a change in tracks, and if so adjust our current byt/bit
    // to be proportional with the new track. This is discussed
    // in the WOZ file spec.
    std::uint8_t lastQuarterTrack;

    // optimal timing, in "one-eighth microseconds per bit" units
    // standard is 32 ( = 4 microseconds per bit)
    std::uint8_t timing;

    bool sync;
    bool cleaned;
    std::string creator;

    void checkForWriteProtection();
    void expandTracks();
    void reduceTracks();
    bool trackIsZeroes(std::uint8_t qt);
    bool tracksAreIdentical(std::uint8_t qt1, std::uint8_t qt2);
    void removeTrack(const int trackIndex);
    std::uint32_t calcNewTrackLengthBits(std::uint8_t qt);
    void copyTrack(std::uint8_t qt_dest, std::uint8_t qt_src);
    void createNewTrack(const std::uint8_t qt);
    void dumpTmap();
    void dumpTracks();
    void rawSet(std::uint8_t currentQuarterTrack, bool on);

public:
    WozFile();
    ~WozFile();

    bool load(const std::string& filePath);

    std::string getFileName() const {
        return this->fileName;
    }

    bool isLoaded() const {
        return this->loaded;
    }

    void save();
    void unload();
    bool isWriteProtected() const {
        return !this->writable;
    }

    bool isModified() const {
        return this->modified;
    }

    void rotateOneBit(std::uint8_t currentQuarterTrack);
    bool exists(std::uint8_t currentQuarterTrack);
    bool getBit(std::uint8_t currentQuarterTrack);
    void setBit(std::uint8_t currentQuarterTrack, bool on);
    void rawSet();

    int optimal_timing() const {
        if (this->loaded) {
            return this->timing;
        }
        return 32;
    }
};

#endif // WOZFILE_H
