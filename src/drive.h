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
#ifndef DRIVE_H
#define DRIVE_H

#include <string>

#include "diskbytes.h"
#include "steppermotor.h"

class Drive
{
private:
	enum { TRACKS_PER_DISK = 0x23 };

	DiskBytes& disk;
	StepperMotor& arm;

public:
	Drive(DiskBytes& disk, StepperMotor& arm):
		disk(disk),
		arm(arm)
	{
	}

	~Drive() {}

	bool loadDisk(const std::string& fnib)
	{
		return this->disk.load(fnib);
	}

	void unloadDisk()
	{
		this->disk.unload();
	}
	bool isLoaded()
	{
		return this->disk.isLoaded();
	}

	void saveDisk()
	{
		this->disk.save();
	}

	bool isWriteProtected() const
	{
		return this->disk.isWriteProtected();
	}

	bool isModified() const
	{
		return this->disk.isModified();
	}



	void setMagnet(unsigned char q, bool on)
	{
		this->arm.setMagnet(q,on);
	}

	int getTrack() const
	{
		return this->arm.getTrack();
	}



	unsigned char get() const
	{
		return this->disk.get(this->arm.getTrack());
	}

	void set(unsigned char value)
	{
		this->disk.put(this->arm.getTrack(),value);
	}



	const DiskBytes& getDiskBytes()
	{
		return this->disk;
	}
};

#endif
