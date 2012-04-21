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
#include "card.h"
#include "drive.h"
#include "screenimage.h"
#include <string>
#include <iostream>

class DiskController : public Card
{
private:
	ScreenImage& gui;
	int slot;
	DiskBytes diskBytes1;
	StepperMotor arm1;
	Drive drive1;

	DiskBytes diskBytes2;
	StepperMotor arm2;
	Drive drive2;

	Drive* currentDrive;

	bool write;
	bool motorOn;


	// TODO for a rev. 0 motherboard, the disk controller will auto reset the CPU

	void set(unsigned char data)
	{
		if (!this->motorOn)
		{
			return;
		}
		this->currentDrive->set(data);
	}

	unsigned char get() const
	{
		if (!this->motorOn)
		{
			return 0xFF;
		}
		return this->currentDrive->get();
	}

	Drive& getDrive(const unsigned char drive)
	{
		return (drive == 0) ? this->drive1 : this->drive2;
	}

	Drive& getOtherDrive()
	{
		return (this->currentDrive == &this->drive1) ? this->drive2 : this->drive1;
	}



public:
	DiskController(ScreenImage& gui, int slot);
	~DiskController();

	virtual unsigned char io(const unsigned short address, const unsigned char data, const bool writing);

	void reset()
	{
		this->gui.setIO(this->slot,getCurrentDriveNumber(),false);
		this->gui.clearCurrentDrive(this->slot,getCurrentDriveNumber());

		this->currentDrive = &this->drive1;
		this->motorOn = false;

		this->gui.setCurrentDrive(this->slot,getCurrentDriveNumber(),getTrack(),false);
	}

	void loadDisk(unsigned char drive, const std::string& fnib)
	{
		if (!this->getDrive(drive).loadDisk(fnib))
		{
			return;
		}
		this->gui.setDiskFile(this->slot,drive,fnib);
		this->gui.setDirty(this->slot,getCurrentDriveNumber(),false);
	}

	void unloadDisk(unsigned char drive)
	{
		this->getDrive(drive).unloadDisk();
		this->gui.setDiskFile(this->slot,drive,"");
		this->gui.setDirty(this->slot,getCurrentDriveNumber(),false);
	}

	void saveDisk(unsigned char drive)
	{
		this->getDrive(drive).saveDisk();
		this->gui.setDirty(this->slot,getCurrentDriveNumber(),false);
	}

	bool isMotorOn()
	{
		return this->motorOn;
	}

	const DiskBytes& getDiskBytes(unsigned char disk)
	{
		return this->getDrive(disk).getDiskBytes();
	}

	unsigned char getTrack()
	{
		return this->currentDrive->getTrack();
	}

	bool isWriting()
	{
		return this->write;
	}

	bool isModified()
	{
		return this->currentDrive->isModified();
	}

	bool isModifiedOther()
	{
		return getOtherDrive().isModified();
	}

	bool isWriteProtected()
	{
		return this->currentDrive->isWriteProtected();
	}

	bool isDirty()
	{
		return isModified() || isModifiedOther();
	}

	unsigned char getCurrentDriveNumber()
	{
		return this->currentDrive == &this->drive1 ? 0 : 1;
	}

	unsigned char getOtherDriveNumber()
	{
		return 1-getCurrentDriveNumber();
	}

	virtual std::string getName() { return "disk][  drive 1                         drive 2                      "; }
};
