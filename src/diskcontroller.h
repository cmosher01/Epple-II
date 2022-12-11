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
#include "drivemotor.h"
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

    Disk2Drive drive1;
    Disk2Drive drive2;
    Disk2Drive* currentDrive;

    bool load; // Q6
    bool write; // Q7
    bool ioStepped;

    /*
     * Only one drive's motor can be on at a time,
     * so we only need one instance.
     */
    DriveMotor motor;

    // Maintain a copy of the last thing on the data bus, so it can
    // be read by the LSS algorithm when needed.
    std::uint8_t dataBusReadOnlyCopy;
    LSS lssp6rom; // the LSS PROM P6 chip (one command per sequence/state combination)
    std::uint8_t dataRegister; // C3 the controller's LS323 data register
    std::uint8_t seq; // A3 sequence control LS174 (current sequence number, 0-F)
    // For ease of use, we store the 4-bit seq number in the _high order_ nibble here.
    // On the real Apple the read pulse goes thru this LS174 too, but we don't emulate that here.

    std::uint8_t prev_seq; // remember previous seq, to determine if A7 changes (indicating write a 1 bit)
    double t; // used to keep track of optimal bit timing interval

    // TODO for a rev. 0 motherboard, the disk controller will auto reset the CPU (see UA2, 9-13)

    void writeBit(bool on) {
        if (!this->motor.isOn()) {
            return;
        }
        this->currentDrive->writeBit(on);
    }

    Disk2Drive& getDrive(const unsigned char drive) {
        return (drive == 0) ? this->drive1 : this->drive2;
    }

    Disk2Drive& getOtherDrive() {
        return (this->currentDrive == &this->drive1) ? this->drive2 : this->drive1;
    }

    void rotateCurrentDisk();
    void stepLss();

public:
    DiskController(ScreenImage& gui, int slot, bool lss13, double random_ones_rate);
    ~DiskController();

    void tick();
    virtual unsigned char io(const unsigned short address, const unsigned char data, const bool writing);

    void reset() {
        this->gui.setIO(this->slot,getCurrentDriveNumber(),false);
        this->gui.clearCurrentDrive(this->slot,getCurrentDriveNumber());

        this->currentDrive = &this->drive1;
        this->motor.reset();

        this->gui.setCurrentDrive(this->slot,getCurrentDriveNumber(),getTrack(),false);
    }

    void loadDisk(unsigned char drive, const std::string& fnib) {
        if (!this->getDrive(drive).loadDisk(fnib)) {
            return;
        }
        this->gui.setDiskFile(this->slot,drive,fnib);
        this->gui.setDirty(this->slot,getCurrentDriveNumber(),false);
    }

    void unloadDisk(unsigned char drive) {
        this->getDrive(drive).unloadDisk();
        this->gui.setDiskFile(this->slot,drive,"");
        this->gui.setDirty(this->slot,getCurrentDriveNumber(),false);
    }

    void save(int drive) {
        this->getDrive(drive).saveDisk();
        this->gui.setDirty(this->slot,getCurrentDriveNumber(),false);
    }

    bool isMotorOn() {
        return this->motor.isOn();
    }

    unsigned char getTrack() {
        return this->currentDrive->getTrack();
    }

    bool isWriting() {
        return this->write;
    }

    bool isModified() {
        return this->currentDrive->isModified();
    }

    bool isModifiedOther() {
        return getOtherDrive().isModified();
    }

    bool isWriteProtected() {
        return this->currentDrive->isWriteProtected();
    }

    bool isDirty() {
        return isModified() || isModifiedOther();
    }

    unsigned char getCurrentDriveNumber() {
        return this->currentDrive == &this->drive1 ? 0 : 1;
    }

    unsigned char getOtherDriveNumber() {
        return 1-getCurrentDriveNumber();
    }

    virtual std::string getName() {
        return "disk][  drive 1                         drive 2                      ";
    }

    void dumpLss() {
        this->lssp6rom.dump();
    }
};
