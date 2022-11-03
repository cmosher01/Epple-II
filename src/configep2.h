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
#ifndef CONFIG_H
#define CONFIG_H

#include <string>
class Memory;
class MemoryRandomAccess;
class Slots;
class ScreenImage;
class CassetteIn;
class CassetteOut;
class Apple2;

class ConfigException {
public:
    const std::string msg;
    ConfigException(const std::string& msg) : msg(msg) {}
};

class Config {
private:
    const std::string& file_path;
    static unsigned char disk_mask;

    static void loadDisk(Slots& slts, int slot, int drive, const std::string& fnib);
    static void unloadDisk(Slots& slts, int slot, int drive);
    static void saveDisk(Slots& slts, int slot, int drive);
    static void insertCard(const std::string& cardType, int slot, Slots& slts, ScreenImage& gui, std::istringstream& tok);
    static void tryParseLine(const std::string& line, MemoryRandomAccess& ram, Memory& rom, Slots& slts, int& revision, ScreenImage& gui, CassetteIn& cassetteIn, CassetteOut& cassetteOut, Apple2* apple2);

public:
    Config(const std::string& file_path);
    ~Config();

    void parse(MemoryRandomAccess& ram, Memory& rom, Slots& slts, int& revision, ScreenImage& gui, CassetteIn& cassetteIn, CassetteOut& cassetteOut, Apple2* apple2);
    static void parseLine(const std::string& line, MemoryRandomAccess& ram, Memory& rom, Slots& slts, int& revision, ScreenImage& gui, CassetteIn& cassetteIn, CassetteOut& cassetteOut, Apple2* apple2);
};

#endif
