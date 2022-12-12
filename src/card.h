/*
    epple2
    Copyright (C) 2008 by Christopher A. Mosher <cmosher01@gmail.com>

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
#ifndef CARD_H
#define CARD_H

#include "memory.h"

#include <filesystem>
#include <istream>
#include <string>

class Card {
    bool activeSeventhRom;

protected:
    Memory rom;
    Memory seventhRom;

public:
    Card();
    virtual ~Card();

    virtual std::string getName();

    virtual void tick();
    virtual void reset();
    virtual unsigned char io(const unsigned short address, const unsigned char data, const bool writing);
    virtual unsigned char readRom(const unsigned short address, const unsigned char data);

    virtual bool hasSeventhRom();
    virtual void readSeventhRom(const unsigned short address, unsigned char* const pb);
    virtual void loadRom(const unsigned short base, std::istream& in);
    virtual void loadSeventhRom(const unsigned short base, std::istream& in);
    virtual bool inhibitMotherboardRom();
    virtual void ioBankRom(const unsigned short addr, unsigned char* const pb, const bool write);
    virtual void loadBankRom(const unsigned short base, std::istream& in);
    virtual bool isMediaDirty();

    virtual bool hasMedia();
    virtual void loadMedia(int unit, const std::filesystem::path &media);
    virtual void unloadMedia(int unit);
    virtual void saveMedia(int unit);
};

#endif
