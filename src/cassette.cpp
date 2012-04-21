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



/*
cassette tape image file format
-------------------------------
Each byte represents one half cycle, in 10-microsoecond units.
For example, consider the following values in the file (decimal):

65 65 65 65 65 20 25 50 50 25 25 25 25 50 50

This represents the following half-cycles (in microseconds)
650 650 650 650 650 200 250 500 500 250 250 250 250 500 500
which has the following meaning:

|--------HEADER-----|--sync-|-1-bit-|-0-bit-|-0-bit-|-1-bit-|
|                   |       |       |       |       |       |
|650 650 650 650 650|200 250|500 500|250 250|250 250|500 500|
*/

#include <fstream>
#include <iostream>
#include "cassette.h"

Cassette::Cassette(ScreenImage& gui):
	gui(gui), t(0), prevT(0), markT(0), positive(false)
{
	unload();
}


Cassette::~Cassette()
{
}

void Cassette::tick()
{
	++this->t;
	// TODO: check for roll-over of tick-count in Cassette???
	// if (this->t == 0)
}

void Cassette::output()
{
	if (isWriteProtected() || !isLoaded())
	{
		return;
	}
	if (this->half_cycles.size() <= this->pos)
	{
		this->half_cycles.push_back(getHalfCycleTime());
		this->pos = this->half_cycles.size();
		this->gui.setCassettePos(this->pos,this->half_cycles.size());
		this->modified = true;
		this->gui.setCassetteDirty(true);
	}
	else
	{
		// TODO should we allow overwriting of cassestte tape data?
		// If so, need to watch out because while reading Apple will
		// be calling Cassette::output, too.
	}

	this->prevT = this->t;
}

unsigned char Cassette::getHalfCycleTime() // in 10-microsecond units
{
	const unsigned int delta_cycles(this->t-this->prevT);
	if (delta_cycles < 225)
		return 20;
	if (delta_cycles < 375)
		return 25;
	if (delta_cycles < 575)
		return 50;
	return 65;
}

bool Cassette::input()
{
	if (!isLoaded())
	{
		// no tape loaded
		return true;
	}

	if (this->markT <= this->t) // we've hit our mark
	{
		this->positive = !this->positive;
		if (this->pos < this->half_cycles.size())
		{
			// set our next mark to be at the end of next half-cycle read from tape
			this->markT = this->t+this->half_cycles[this->pos++]*10;
			this->gui.setCassettePos(this->pos,this->half_cycles.size());
		}
	}

	return this->positive;
}

void Cassette::rewind()
{
	this->pos = 0;
	this->gui.setCassettePos(this->pos,this->half_cycles.size());
}

bool Cassette::newFile(const std::string& filePath)
{
	std::ifstream in(filePath.c_str(),std::ios::binary|std::ios::in);
	if (in.is_open())
	{
		in.close();
		std::cerr << "Error creating file; file already exists: " << filePath << std::endl;
		return false;
	}
	std::ofstream out(filePath.c_str(),std::ios::binary|std::ios::out);
	out.close();
	return load(filePath);
}

bool Cassette::load(const std::string& filePath)
{
//	TODO better I/O error handling during cassette loading and saving
	std::ifstream in(filePath.c_str(),std::ios::binary|std::ios::in|std::ios::ate);
	if (!in.is_open())
	{
		std::cerr << "Error loading " << filePath << std::endl;
		return false;
	}
	if (isLoaded())
	{
		unload();
	}

	const std::ifstream::pos_type size = in.tellg();
	if (size > 0)
	{
		this->half_cycles.resize(size);
		in.seekg(0,std::ios::beg);
		in.read((char*)&this->half_cycles[0],size);
//		std::cerr << "Read " << size << " bytes from " << filePath << std::endl;
	}
	in.close();

	this->filePath = filePath;

	checkForWriteProtection();

	this->loaded = true;
	this->modified = false;

	this->gui.setCassetteFile(filePath);
	this->gui.setCassetteDirty(false);
	this->gui.setCassettePos(this->pos,size);

	return true;
}

void Cassette::checkForWriteProtection()
{
	std::ofstream outf(filePath.c_str(),std::ios::binary|std::ios::app);
	this->writable = outf.is_open();
	outf.close();
}

void Cassette::save()
{
	if (isWriteProtected() || !isLoaded())
	{
		return;
	}
	std::ofstream out(filePath.c_str(),std::ios::binary);
	out.write((char*)&this->half_cycles[0],this->half_cycles.size());
	out.flush();
	out.close();

	this->modified = false;
	this->gui.setCassetteDirty(false);
}

void Cassette::unload()
{
	this->pos = 0;
	this->writable = true;
	this->loaded = false;
	this->filePath = "";
	this->modified = false;
	this->half_cycles.clear();
	this->gui.setCassetteFile("");
	this->gui.setCassetteDirty(false);
	this->gui.setCassettePos(this->pos,this->half_cycles.size());
}
