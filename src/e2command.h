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

/* 
 * File:   e2command.h
 * Author: user
 *
 * Created on December 11, 2022, 8:21 PM
 */

#ifndef E2COMMAND_H
#define E2COMMAND_H



#include <filesystem>
#include <sstream>
#include <string>

class MemoryRandomAccess;
class Memory;
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



class E2Command {
    void tryParseLine(const std::string& line, MemoryRandomAccess& ram, Memory& rom, Slots& slts, int& revision, ScreenImage& gui, CassetteIn& cassetteIn, CassetteOut& cassetteOut, Apple2* apple2);

    void insertCard(const std::string& cardType, const int slot, Slots& slts, ScreenImage& gui, const double random_ones_rate);

    void loadDisk(Slots& slts, int slot, int drive, const std::filesystem::path &media);
    void unloadDisk(Slots& slts, int slot, int drive);
    void saveDisk(Slots& slts, int slot, int drive);

public:
    E2Command();
    virtual ~E2Command();

    void parseLine(const std::string& line, MemoryRandomAccess& ram, Memory& rom, Slots& slts, int& revision, ScreenImage& gui, CassetteIn& cassetteIn, CassetteOut& cassetteOut, Apple2* apple2);
};



#endif /* E2COMMAND_H */
