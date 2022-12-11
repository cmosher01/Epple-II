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
#ifndef ADDRESSBUS_H
#define ADDRESSBUS_H

class ScreenImage;
class Memory;
class MemoryRandomAccess;
class Keyboard;
class VideoMode;
class Paddles;
class PaddleButtonStates;
class SpeakerClicker;
class CassetteIn;
class CassetteOut;
class Slots;

class AddressBus {
    private:
        ScreenImage& gui;
        const int& revision;
        MemoryRandomAccess& ram;
        Memory& rom; // TODO fix ROM so it doesn't use deprecated Memory class
        Keyboard& kbd;
        VideoMode& vid;
        Paddles& paddles;
        PaddleButtonStates& paddleButtonStates;
        SpeakerClicker& speaker;
        CassetteIn& cassetteIn;
        CassetteOut& cassetteOut;
        Slots& slts;

        unsigned char data; // this emulates the (floating) data bus

        int debugoutpos;
        bool debugfirst;

        void setD7(const bool set);
        void readSwitch(unsigned short address);
        void writeSwitch(unsigned short address);

public:
        AddressBus(ScreenImage& gui, int& revision, MemoryRandomAccess& ram, Memory& rom, Keyboard& kbd, VideoMode& vid, Paddles& paddles, PaddleButtonStates& paddleButtonStates, SpeakerClicker& speaker, CassetteIn& cassetteIn, CassetteOut& cassetteOut, Slots& slts);
        ~AddressBus();

        unsigned char read(const unsigned short address);
        void write(const unsigned short address, const unsigned char d);

        static const int MOTHERBOARD_RAM_BAS = 0x00000;
        static const int MOTHERBOARD_RAM_LIM = 0x0C000;
        static const int MOTHERBOARD_RAM_SIZ = MOTHERBOARD_RAM_LIM-MOTHERBOARD_RAM_BAS;

        static const int MOTHERBOARD_ROM_BAS = 0x0D000;
        static const int MOTHERBOARD_ROM_LIM = 0x10000;
        static const int MOTHERBOARD_ROM_SIZ = MOTHERBOARD_ROM_LIM-MOTHERBOARD_ROM_BAS;
};

#endif
