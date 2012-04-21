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
#include "diskbytes.h"

#include <vector>
#include <algorithm>
#include <istream>
#include <ostream>
#include <fstream>

DiskBytes::DiskBytes()
{
	unload();
}

DiskBytes::~DiskBytes()
{
}

bool DiskBytes::load(const std::string& filePath)
{


// TODO better I/O error handling during disk loading and saving
	std::ifstream in(filePath.c_str(),std::ios::binary|std::ios::in);
	if (!in.is_open())
	{
		return false;
	}
	if (isLoaded())
	{
		unload();
	}
	for (int t(0); t < TRACKS_PER_DISK; ++t)
	{
		this->bytes[t].resize(BYTES_PER_TRACK);
		in.read((char*)&this->bytes[t][0],BYTES_PER_TRACK);
	}
	in.close();

	this->filePath = filePath;

	checkForWriteProtection();

	this->loaded = true;
	this->modified = false;

	return true;
}

void DiskBytes::checkForWriteProtection()
{
	std::ofstream outf(filePath.c_str(),std::ios::binary|std::ios::app);
	this->writable = outf.is_open();
	outf.close();
}

void DiskBytes::save()
{
	if (isWriteProtected() || !isLoaded())
	{
		return;
	}
	std::ofstream out(filePath.c_str(),std::ios::binary);
	for (int t(0); t < TRACKS_PER_DISK; ++t)
		out.write((char*)&this->bytes[t][0],BYTES_PER_TRACK);
	out.flush();
	out.close();

	this->modified = false;
}

void DiskBytes::unload()
{
	this->byt = 0;
	this->writable = true;
	this->loaded = false;
	this->filePath = "";
	this->modified = false;
}

unsigned char DiskBytes::get(const int track)
{
	if (!isLoaded())
	{
		return 0xFF;
	}
	const unsigned char ret = this->bytes[track][this->byt];
	nextByte();
	return ret;
}

void DiskBytes::put(const unsigned char track, const unsigned char value)
{
	if (TRACKS_PER_DISK <= track)
	{
		throw 0;
	}
	if (isWriteProtected() || !isLoaded())
	{
		return;
	}
	this->bytes[track][this->byt] = value;
	this->modified = true;
	nextByte();
}

void inline DiskBytes::nextByte()
{
	// emulates circular disk track
	++this->byt;
	if (this->byt >= BYTES_PER_TRACK)
	{
		this->byt = 0;
	}
}
