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
#include "drive.h"
Disk2Drive::Disk2Drive():
    stepper(head),
    pulse(false),
    bitBufferRead(0),
    generator(std::chrono::system_clock::now().time_since_epoch().count()),
    distribution(0,1) {
}

bool Disk2Drive::loadDisk(const std::string& fnib) {
    return this->disk.load(fnib);
}

void Disk2Drive::unloadDisk() {
    this->disk.unload();
}
bool Disk2Drive::isLoaded() const {
    return this->disk.isLoaded();
}

void Disk2Drive::saveDisk() {
    this->disk.save();
}

bool Disk2Drive::isWriteProtected() const {
    return this->disk.isWriteProtected();
}

bool Disk2Drive::isModified() const {
    return this->disk.isModified();
}

int Disk2Drive::optimal_timing() const {
    return this->disk.optimal_timing();
}

int Disk2Drive::position() const {
    return this->head.position();
}

void Disk2Drive::tick() {
    this->stepper.tick();
}

void Disk2Drive::set_phase(int i_phase_0_to_3, bool on) {
    this->stepper.set_phase(i_phase_0_to_3, on);
}

int Disk2Drive::getTrack() const {
    return this->head.position() >> 2;
}

void Disk2Drive::rotateDiskOneBit() {
    this->disk.rotateOneBit(this->head.position());

    bitBufferRead <<= 1;
    bitBufferRead |= this->disk.getBit(this->head.position());
    if (bitBufferRead & 0x0Fu) {
        this->pulse = (bitBufferRead & 0x02u) >> 1;
    } else {
        this->pulse = randomBit();
    }
}

bool Disk2Drive::readPulse() const {
    return this->pulse;
}
void Disk2Drive::clearPulse() {
    this->pulse = false;
}

void Disk2Drive::writeBit(bool on) {
    this->disk.setBit(this->head.position(), on);
}
