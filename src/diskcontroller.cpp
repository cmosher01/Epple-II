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

DiskController::DiskController(ScreenImage& gui, int slot, bool lss13, double random_ones_rate):
    gui(gui),
    slot(slot),
    drive1(random_ones_rate),
    drive2(random_ones_rate),
    currentDrive(&this->drive1),
    load(false),
    write(false),
    ioStepped(false),
    dataBusReadOnlyCopy(0),
    lssp6rom(lss13),
    dataRegister(0),
    seq(0x20), // gotta start somewhere
    prev_seq(seq),
    t(0) {
}

DiskController::~DiskController() {
}

unsigned char DiskController::io(const unsigned short addr, const unsigned char d, const bool writing) {
    this->dataBusReadOnlyCopy = d;

    const unsigned char q = (addr & 0x000E) >> 1;
    const bool on = (addr & 0x0001);

//    printf("Q%d<--%s\n", q, on?"ON":"OFF");

    switch (q) {
    case 0:
    case 1: // TODO if phase-1 is on, it also acts as write-protect (UA2, 9-8)
    case 2:
    case 3:
        this->currentDrive->set_phase(q,on);
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
        break;
    case 7:
        this->write = on;
        break;
    }
    if (this->write && !this->load) {
        this->gui.setDirty(this->slot,getCurrentDriveNumber(),true);
    }

    // UA2, 9-23, Figure 9.12
    // 2 LSS cycles need to happen AFTER setting the Qx switch, and
    // BEFORE reading LSS's update of the data register
    this->ioStepped = false;
    tick();
    this->ioStepped = true; // flag that we ran it already

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
    this->currentDrive->tick();

    if (this->ioStepped) { // if we already ran it, above in io(), skip here
        this->ioStepped = false;
        return;
    }
    this->gui.setIO(this->slot, getCurrentDriveNumber(), this->motor.isOn());
    if (!this->motor.isOn()) {
        this->ioStepped = false;
        return;
    }



    this->motor.tick(); // only need to send tick when motor is powered on



    // run two LSS cycles = 2MHz

    rotateCurrentDisk();
    stepLss();

    // pulse lasts only 500 nanoseconds (1 LSS clock cycle), so clear it now:
    this->currentDrive->clearPulse();

    rotateCurrentDisk();
    stepLss();
}

void DiskController::rotateCurrentDisk() {
    this->t += 1.0;
    if (this->currentDrive->optimal_timing()/4.0 <= this->t) { // wait for optimal interval between bits
        this->currentDrive->rotateDiskOneBit(); // (will also generate a read-pulse when it reads a 1-bit)
        this->t -= this->currentDrive->optimal_timing()/4.0;
    }
}

enum lss_cmd { NOP, SL, SR, LD };

void DiskController::stepLss() {
    std::uint8_t adr = this->write<<3 | this->load<<2 | (this->dataRegister>>7)<<1 | this->currentDrive->readPulse();
    std::uint8_t cmd = this->lssp6rom.read(this->seq|adr);
    this->seq = cmd & 0xF0u;

    // LSS command functions (UA2, 9-15, Table 9.3)
    if (cmd & 8u) {
        if (cmd & 3u) {
            if (this->write) {
                const bool one = (this->seq&0x80u) != (this->prev_seq&0x80u);
                this->prev_seq = this->seq;
                this->currentDrive->writeBit(one);
            }
        }
        switch (cmd & 3u) {
        case LD:
            this->dataRegister = this->dataBusReadOnlyCopy;
            break;
        case SR:
            this->dataRegister >>= 1;
            this->dataRegister |= (isWriteProtected() << 7);
            break;
        case SL:
            this->dataRegister <<= 1;
            this->dataRegister |= ((cmd & 4u) >> 2);
            break;
        }
    } else {
        this->dataRegister = 0;
    }
}
