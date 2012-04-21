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
#ifndef CASSETTE_H
#define CASSETTE_H

#include <vector>
#include <string>

#include "screenimage.h"

class Cassette
{
private:
	unsigned int t;
	unsigned int prevT;

	unsigned int markT;
	bool positive;

	std::vector<unsigned char> half_cycles;

	std::string fileName;
	std::string filePath;
	bool writable;
	bool loaded;
	unsigned int pos;
	bool modified;

	ScreenImage& gui;

	void checkForWriteProtection();

	unsigned char getHalfCycleTime(); // in 10-microsecond units

public:
	Cassette(ScreenImage& gui);
	~Cassette();

	void tick();
	void output();
	bool input();
	void rewind();
	bool newFile(const std::string& filePath);
	bool load(const std::string& filePath);
	std::string getFileName()
	{
		return this->fileName;
	}

	bool isLoaded()
	{
		return this->loaded;
	}

	void save();
	void unload();
	bool isWriteProtected()
	{
		return !this->writable;
	}

	bool isModified()
	{
		return this->modified;
	}
};

#endif
