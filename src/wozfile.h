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

class WozFile {
    std::string fileName;
    std::string filePath;
    bool writable;
    bool loaded;
    // TODO add bit position:
    unsigned int byt; // represents rotational position of disk
    bool modified;

    std::uint8_t tmap[141]; // quarter-tracks from 0 through 35, values are indexes into trks
    std::uint8_t c_trks; // count of actual tracks:
    std::uint8_t trks[141][6646]; // 141 is theoretical max; will always be less
    std::uint16_t trk_bits[141]; // count of bits in each track

    void nextByte();
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
};

#endif // WOZFILE_H
