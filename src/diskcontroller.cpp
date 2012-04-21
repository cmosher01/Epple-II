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
#include "diskcontroller.h"

DiskController::DiskController(ScreenImage& gui, int slot):
	gui(gui),
	slot(slot),
	drive1(diskBytes1,arm1),
	drive2(diskBytes2,arm2),
	currentDrive(&this->drive1)
{
}

DiskController::~DiskController()
{
}

unsigned char DiskController::io(const unsigned short addr, const unsigned char d, const bool writing)
{
	unsigned char data(d);
	const unsigned char q = (addr & 0x000E) >> 1;
	const bool on = (addr & 0x0001);

	switch (q)
	{
		case 0:
		case 1:
		case 2:
		case 3:
			this->currentDrive->setMagnet(q,on);
			this->gui.setTrack(this->slot,getCurrentDriveNumber(),getTrack());
		break;
		case 4:
			this->motorOn = on;
			this->gui.setIO(this->slot,getCurrentDriveNumber(),on);
		break;
		case 5:
			this->gui.clearCurrentDrive(this->slot,getCurrentDriveNumber());
			this->currentDrive = (on ? &this->drive2 : &this->drive1);
			this->gui.setCurrentDrive(this->slot,getCurrentDriveNumber(),getTrack(),this->motorOn);
		break;
		case 6:
			if (on && this->write && writing)
			{
				set(data);
				this->gui.setIO(this->slot,getCurrentDriveNumber(),this->motorOn);
				this->gui.setDirty(this->slot,getCurrentDriveNumber(),true);
			}
			else if (!(on || this->write))
			{
				data = get();
			}
		break;
		case 7:
			this->write = on;
			if (this->currentDrive->isWriteProtected())
			{
				data |= 0x80;
			}
			else
			{
				data &= 0x7F;
			}
		break;
	}
	return data;
}
