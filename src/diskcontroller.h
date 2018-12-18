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
#include "card.h"
#include "drive.h"
#include "wozfile.h"
#include "lss.h"
#include "screenimage.h"
#include <string>
#include <iostream>
#include <cstdint>

class DiskController : public Card
{
private:
	ScreenImage& gui;
	int slot;
        WozFile diskBytes1;
	StepperMotor arm1;
	Drive drive1;

        WozFile diskBytes2;
	StepperMotor arm2;
	Drive drive2;

	Drive* currentDrive;

        bool load; // Q6
        bool write; // Q7
        bool motorOn; // TODO WOZ make it delay power-off by about 1 second.

        // Maintain a copy of the last thing on the data bus, so it can
        // be read by the LSS algorithm when needed.
        std::uint8_t dataBusReadOnlyCopy;
        LSS lssp6rom; // the LSS PROM P6 chip (one command per sequence/state combination)
        std::uint8_t dataRegister; // C3 the controller's LS323 data register
        std::uint8_t seq; // A3 sequence control LS174 (current sequence number, 0-F)
        // For ease of use, we store the 4-bit seq number in the _high order_ nibble here.
        // On the real Apple the read pulse goes thru this LS174 too, but we don't emulate that here.

        std::uint8_t t; // used to keep track of 4 MPU cycles

        // TODO for a rev. 0 motherboard, the disk controller will auto reset the CPU (see UA2, 9-13)

        void writeBit(bool on) {
                if (!this->motorOn) {
                        return;
                }
                this->currentDrive->writeBit(on);
        }

//	unsigned char get() const
//	{
//		if (!this->motorOn)
//		{
//			return 0xFF;
//		}
//		return this->currentDrive->get();
//	}

	Drive& getDrive(const unsigned char drive)
	{
		return (drive == 0) ? this->drive1 : this->drive2;
	}

	Drive& getOtherDrive()
	{
		return (this->currentDrive == &this->drive1) ? this->drive2 : this->drive1;
	}

        void rotateCurrentDisk();
        void stepLss();

public:
        DiskController(ScreenImage& gui, int slot, bool lss13);
	~DiskController();

        void tick();
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

//        const WozFile& getDiskBytes(unsigned char disk)
//	{
//		return this->getDrive(disk).getDiskBytes();
//	}

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
