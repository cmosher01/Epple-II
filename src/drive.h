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
#ifndef DRIVE_H
#define DRIVE_H

#include <random>
#include <chrono>
#include <string>
#include <cstdint>
#include <iostream>
#include "disk2steppermotor.h"
#include "disk2readwritehead.h"
#include "wozfile.h"

class Disk2Drive {
private:
    Disk2StepperMotor stepper;
    Disk2ReadWriteHead head;
    WozFile disk;

    bool pulse;
    std::uint8_t bitBufferRead;



    const double random_ones_rate;
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution;

    bool randomBit() {
        return distribution(generator) < random_ones_rate;
    }

public:
    Disk2Drive(double p_random_ones_rate);
    bool loadDisk(const std::string& fnib);
    void unloadDisk();
    bool isLoaded() const;
    void saveDisk();
    bool isWriteProtected() const;
    bool isModified() const;
    int optimal_timing() const;
    int position() const;
    void tick();
    void set_phase(int i_phase_0_to_3, bool on);
    int getTrack() const;
    void rotateDiskOneBit();
    bool readPulse() const;
    void clearPulse();
    void writeBit(bool on);
};

#endif
