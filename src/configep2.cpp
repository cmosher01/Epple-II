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
#include "configep2.h"

#include "memory.h"
#include "slots.h"
#include "diskcontroller.h"
#include "languagecard.h"
#include "firmwarecard.h"
#include "standardout.h"
#include "standardin.h"
#include "clockcard.h"
#include "cassette.h"

#include <iostream>
#include <istream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

unsigned char Config::disk_mask(0);

Config::Config(const std::string& file_path):
	file_path(file_path)
{
}


Config::~Config()
{
}

static void strip_comment(std::string& str)
{
	const size_t comment = str.find('#');
	if (comment < std::string::npos)
	{
		str.erase(comment);
	}
}

static void trim(std::string& str)
{
	{
		const size_t p = str.find_first_not_of(" \t");
		if (p < std::string::npos)
		{
			str.erase(0,p);
		}
	}
	{
		const size_t p = str.find_last_not_of(" \t");
		if (p+1 < std::string::npos)
		{
			str.erase(p+1);
		}
	}
}

void Config::parse(Memory& ram, Memory& rom, Slots& slts, int& revision, ScreenImage& gui, Cassette& cassette)
{
	std::ifstream* pConfig;

	std::string path(this->file_path);

	if (!path.empty())
	{
		pConfig = new std::ifstream(path.c_str());
		if (!pConfig->is_open())
		{
			std::stringstream ss;
			ss << "Cannot open config file " << this->file_path.c_str();
			throw std::runtime_error(ss.str());
		}
	}
	if (path.empty())
	{
		/*
			On Windows, the default directory will be
			C:\Program Files\Epple2 if they start the
			program from the Start Menu; therefore
			etc/epple2/epple2.conf would be
			C:\Program Files\epple2\etc\epple2\epple2.conf
			On Linux... the current directory could be
			anything, so this probably won't find it (unless
			the current directory is /).
		*/
		path = "etc/epple2/epple2.conf";
		pConfig = new std::ifstream(path.c_str());
		if (!pConfig->is_open())
			path.clear();
	}
	if (path.empty())
	{
		/*
			This is primarily for Linux. If configured for
			a PREFIX of "/usr/local", then this would be
			/usr/local/etc/epple2/epple2.conf
		*/
		path = ETCDIR "/epple2/epple2.conf";
		pConfig = new std::ifstream(path.c_str());
		if (!pConfig->is_open())
			path.clear();
	}
	if (path.empty())
	{
		/*
			Last effort to find it (most likely will
			only work on Linux).
		*/
		path = "/etc/epple2/epple2.conf";
		pConfig = new std::ifstream(path.c_str());
		if (!pConfig->is_open())
			path.clear();
	}
	if (path.empty())
	{
		std::cerr << "Cannot open config file /etc/epple2/epple2.conf" << std::endl;
		return;
	}

	std::string line;
	std::getline(*pConfig,line);
	while (!pConfig->eof())
	{
		strip_comment(line);
		trim(line);
		if (!line.empty())
		{
			parseLine(line,ram,rom,slts,revision,gui,cassette);
		}
		std::getline(*pConfig,line);
	}
	pConfig->close();
	delete pConfig;

	// TODO: make sure there is no more than ONE stdin and/or ONE stdout card
}
void Config::parseLine(const std::string& line, Memory& ram, Memory& rom, Slots& slts, int& revision, ScreenImage& gui, Cassette& cassette)
{
	try
	{
		tryParseLine(line,ram,rom,slts,revision,gui,cassette);
	}
	catch (const ConfigException& err)
	{
		std::cerr << err.msg.c_str() << std::endl;
	}
}

void Config::tryParseLine(const std::string& line, Memory& ram, Memory& rom, Slots& slts, int& revision, ScreenImage& gui, Cassette& cassette)
{
	std::istringstream tok(line);

	std::string cmd;
	tok >> cmd;
	if (cmd == "slot")
	{
		int slot;
		std::string sCardType;
		tok >> slot >> sCardType;

		insertCard(sCardType,slot,slts,gui);
	}
	else if (cmd == "import")
	{
		std::string sm;
		tok >> sm;

		int slot(-1);
		if (sm == "slot")
		{
			tok >> slot;
		}
		else if (sm != "motherboard")
		{
			throw ConfigException("error at \""+sm+"\"; expected \"slot #\" or \"motherboard\"");
		}

		std::string romtype;
		tok >> romtype;

		unsigned short base(0);
		tok >> std::hex >> base;

		std::string file;
		std::getline(tok,file);
		trim(file);
		std::ifstream memfile(file.c_str(),std::ios::binary);
		if (!memfile.is_open())
		{
			throw ConfigException("cannot open file "+file);
		}

		if (slot < 0) // motherboard
		{
			if (romtype == "rom")
			{
				rom.load(base,memfile);
			}
			else if (romtype == "ram")
			{
				ram.load(base,memfile);
			}
			else
			{
				throw ConfigException("error at \""+romtype+"\"; expected rom or ram");
			}
		}
		else
		{
			if (8 <= slot)
			{
				throw ConfigException("invalid slot number");
			}
			Card* card = slts.get(slot);
			if (romtype == "rom")
				card->loadRom(base,memfile);
			else if (romtype == "rom7")
				card->loadSeventhRom(base,memfile);
			else if (romtype == "rombank")
				card->loadBankRom(base,memfile);
			else
				throw ConfigException("error at \""+romtype+"\"; expected rom, rom7, or rombank");
		}
		memfile.close();
	}
	else if (cmd == "load" || cmd == "save" || cmd == "unload")
	{
		std::string slotk;
		tok >> slotk;
		if (slotk != "slot")
		{
			throw ConfigException("error at \""+slotk+"\"; expected \"slot\"");
		}

		int slot(-1);
		tok >> slot;

		std::string drivek;
		tok >> drivek;
		if (drivek != "drive")
		{
			throw ConfigException("error at \""+drivek+"\"; expected \"drive\"");
		}

		int drive(-1);
		tok >> drive;

		if (cmd == "load")
		{
			std::string fnib;
			std::getline(tok,fnib);
			trim(fnib);
			loadDisk(slts,slot,drive,fnib);
		}
		else if (cmd == "unload")
		{
			unloadDisk(slts,slot,drive);
		}
		else if (cmd == "save")
		{
			saveDisk(slts,slot,drive);
		}
	}
	else if (cmd == "revision")
	{
		tok >> std::hex >> revision;
	}
	else if (cmd == "cassette")
	{
		std::string cas;
		tok >> cas;

		if (cas == "rewind")
		{
			cassette.rewind();
		}
		else if (cas == "new")
		{
			std::string fcas;
			std::getline(tok,fcas);
			trim(fcas);
			cassette.newFile(fcas);
		}
		else if (cas == "load")
		{
			std::string fcas;
			std::getline(tok,fcas);
			trim(fcas);
			cassette.load(fcas);
		}
		else if (cas == "unload")
		{
			cassette.unload();
		}
		else if (cas == "save")
		{
			cassette.save();
		}
		else
		{
			throw ConfigException("error: unknown cassette command: "+cas);
		}
	}
	else
	{
		throw ConfigException("Invalid command: "+cmd);
	}
}

void Config::loadDisk(Slots& slts, int slot, int drive, const std::string& fnib)
{
	if (drive < 1 || 2 < drive)
	{
		throw ConfigException("Invalid drive; must be 1 or 2");
	}

	// TODO if file doesn't exist, name still gets displayed, and there's no error message
	Card* card = slts.get(slot);
	if (!(disk_mask & (1 << slot)))
	{
		std::cerr << "Slot " << slot << " doesn't have a disk controller card" << std::endl;
		return;
	}

	DiskController* controller = (DiskController*)card;
	controller->loadDisk(drive-1,fnib);
}

void Config::unloadDisk(Slots& slts, int slot, int drive)
{
	if (drive < 1 || 2 < drive)
	{
		throw ConfigException("Invalid drive; must be 1 or 2");
	}

	Card* card = slts.get(slot);
	if (!(disk_mask & (1 << slot)))
	{
		std::cerr << "Slot " << slot << " doesn't have a disk controller card" << std::endl;
		return;
	}

	DiskController* controller = (DiskController*)card;
	controller->unloadDisk(drive-1);
}

void Config::saveDisk(Slots& slts, int slot, int drive)
{
	if (drive < 1 || 2 < drive)
	{
		throw ConfigException("Invalid drive; must be 1 or 2");
	}
	Card* card = slts.get(slot);
	DiskController* controller = (DiskController*)card;
	controller->saveDisk(drive-1);
}

void Config::insertCard(const std::string& cardType, int slot, Slots& slts, ScreenImage& gui)
{
	if (slot < 0 || 8 <= slot)
	{
		throw ConfigException("Invalid slot number");
	}

	Card* card;

	disk_mask &= ~(1 << slot);

	if (cardType == "language")
	{
		card = new LanguageCard(gui,slot);
	}
	else if (cardType == "firmware")
	{
		card = new FirmwareCard(gui,slot);
	}
	else if (cardType == "disk")
	{
		card = new DiskController(gui,slot);
		disk_mask |= (1 << slot);
	}
	else if (cardType == "clock")
	{
		card = new ClockCard();
	}
	else if (cardType == "stdout")
	{
		card = new StandardOut();
	}
	else if (cardType == "stdin")
	{
		card = new StandardIn();
	}
	else if (cardType == "empty")
	{
		card = 0;
	}
	else
	{
		throw ConfigException("Invalid card type: "+cardType);
	}

	if (card)
		slts.set(slot,card);
	else
		slts.remove(slot);
}
