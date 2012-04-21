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
#ifndef DISKBYTES_H
#define DISKBYTES_H

#include <string>
#include <vector>

class DiskBytes
{
private:
	enum { TRACKS_PER_DISK = 0x23 };
	enum { BYTES_PER_TRACK = 0x1A00 };

	std::vector<unsigned char> bytes[TRACKS_PER_DISK];

	std::string fileName;
	std::string filePath;
	bool writable;
	bool loaded;
	unsigned int byt; // represents rotational position of disk
	bool modified;

	void nextByte();
	void checkForWriteProtection();

public:
	DiskBytes();
	~DiskBytes();

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
	unsigned char get(const int track);
	void put(const unsigned char track, const unsigned char value);
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
