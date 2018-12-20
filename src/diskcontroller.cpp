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
#include "diskcontroller.h"

DiskController::DiskController(ScreenImage& gui, int slot, bool lss13):
    gui(gui),
    slot(slot),
    drive1(diskBytes1,arm1),
    drive2(diskBytes2,arm2),
    currentDrive(&this->drive1),
    load(false),
    write(false),
    lssp6rom(lss13),
    seq(0x20), // gotta start somewhere
    t(0) {
}

DiskController::~DiskController() {
}

unsigned char DiskController::io(const unsigned short addr, const unsigned char d, const bool writing) {
    this->dataBusReadOnlyCopy = d;

    const unsigned char q = (addr & 0x000E) >> 1;
    const bool on = (addr & 0x0001);

    switch (q) {
    case 0:
    case 1: // TODO if phase-1 is on, it also acts as write-protect (UA2, 9-8)
    case 2:
    case 3:
        this->currentDrive->setMagnet(q,on);
        this->gui.setTrack(this->slot, getCurrentDriveNumber(), getTrack());
        break;
    case 4:
        this->motor.power(on);
        break;
    case 5:
        this->gui.clearCurrentDrive(this->slot, getCurrentDriveNumber());
        this->currentDrive = (on ? &this->drive2 : &this->drive1);
        this->gui.setCurrentDrive(this->slot, getCurrentDriveNumber(),getTrack(), this->motor.isOn());
        break;
    case 6:
        this->load = on;
        // TODO when to do these GUI updates?
//        this->gui.setDirty(this->slot,getCurrentDriveNumber(),true);
        break;
    case 7:
        this->write = on;
        break;
    }
//    if (this->dataRegister == 0xD5u) {
//        printf("\n");
//    }
//    if (this->dataRegister & 0x80u) {
//        printf("%02X ", this->dataRegister);
//    }
    return on ? d : this->dataRegister;
}

/*
 * Get a timing cycle here, based on the MPU clock (1 MHz).
 * In the real Apple we don't get really get this here. But...
 * We need a 2MHz clock for the LSS; and
 * we need to rotate the floppy @ 1 bit per 4 microseconds.
 * (When the motor is on, that is.)
 */
void DiskController::tick() {
    this->gui.setIO(this->slot, getCurrentDriveNumber(), this->motor.isOn());
    if (!this->motor.isOn()) {
        return;
    }
    this->motor.tick(); // only need to send tick when motor is powered on

    rotateCurrentDisk();

    // run two LSS cycles = 2MHz
    stepLss();
    this->currentDrive->clearPulse();
    stepLss();
}

void DiskController::rotateCurrentDisk() {
    ++this->t;
    if (4 <= this->t) { // 4us interval between bits
        this->currentDrive->rotateDiskOneBit(); // (will also generate a read-pulse when it reads a 1-bit)
        this->t = 0;
    }
}

void DiskController::stepLss() {
    std::uint8_t adr = this->write<<3 | this->load<<2 | (this->dataRegister>>7)<<1 | this->currentDrive->readPulse();
    std::uint8_t cmd = this->lssp6rom.read(this->seq|adr);
    this->seq = cmd & 0xF0u;

    // LSS command functions (UA2, 9-15, Table 9.3)
//    printf("%02x:", cmd);
    if (cmd & 8u) {
        switch (cmd & 3u) {
        case 3:
            this->dataRegister = this->dataBusReadOnlyCopy;
            break;
        case 2:
            this->dataRegister >>= 1;
            this->dataRegister |= (isWriteProtected() << 7);
            // TODO how to handle writing?
            break;
        case 1:
            this->dataRegister <<= 1;
            this->dataRegister |= ((cmd & 4u) >> 2);
//            printf(this->dataRegister & 0x80u ? "\x1b[30;42m" : "\x1b[30;43m");
//            printf("%02X\x1b[0m ", this->dataRegister);
//            if (this->dataRegister & 0x80u) printf("\n");
            // TODO how to handle writing?
            break;
        }
    } else {
        this->dataRegister = 0;
    }
}
