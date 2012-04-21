/*
    epple2
    Copyright (C) 2008 by Chris Mosher <chris@mosher.mine.nu>

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
#ifndef VIDEO_H_
#define VIDEO_H_

#include <vector>

class VideoMode;
class AddressBus;
class PictureGenerator;
class TextCharacters;

class Video
{
private:
	enum { TEXT_BASE_1 = 0x0400 };
	enum { TEXT_BASE_2 = 0x0800 };
	enum { TEXT_LEN = 0x0400 };
	
	enum { HRES_BASE_1 = 0x2000 };
	enum { HRES_BASE_2 = 0x4000 };
	enum { HRES_LEN = 0x2000 };
	
		std::vector<unsigned short> lutTEXT[2]; // [0] is page 1, [1] is page 2
	std::vector<unsigned short> lutHRES[2]; // [0] is page 1, [1] is page 2

	VideoMode& mode;
	AddressBus& addressBus;
	PictureGenerator& picgen;

	TextCharacters& textRows;

	unsigned int t;

	bool flash;
	int cflash;

	void updateFlash();
	unsigned char getDataByte();
	unsigned char getRowToPlot(unsigned char d);
	bool inverseChar(const unsigned char d);

public:
	Video(VideoMode& mode, AddressBus& addressBus, PictureGenerator& picgen, TextCharacters& textRows);
	~Video();
	void powerOn();
	void tick();
};

#endif /*VIDEO_H_*/
