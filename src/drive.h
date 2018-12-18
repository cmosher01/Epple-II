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
#include "wozfile.h"
#include "steppermotor.h"

class Drive {
private:
    WozFile& disk;
    StepperMotor& arm;

    bool pulse;
    std::uint8_t cContiguousZeroBits;



    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution;

    bool randomBit() {
        return !distribution(generator);
    }

public:
    Drive(WozFile& disk, StepperMotor& arm):
        disk(disk),
        arm(arm),
        pulse(false),
        cContiguousZeroBits(0),
        generator(std::chrono::system_clock::now().time_since_epoch().count()),
        distribution(0,1) {
    }

    ~Drive() {
    }

    bool loadDisk(const std::string& fnib) {
        return this->disk.load(fnib);
    }

    void unloadDisk() {
        this->disk.unload();
    }
    bool isLoaded() {
        return this->disk.isLoaded();
    }

    void saveDisk() {
        this->disk.save();
    }

    bool isWriteProtected() const {
        return this->disk.isWriteProtected();
    }

    bool isModified() const {
        return this->disk.isModified();
    }



    void setMagnet(unsigned char q, bool on) {
        this->arm.setMagnet(q,on);
    }

    int getTrack() const {
        return this->arm.getTrack();
    }


    void rotateDiskOneBit() {
        this->disk.rotateOneBit(this->arm.getQuarterTrack());

        if (this->disk.getBit(this->arm.getQuarterTrack())) {
            this->pulse = true;
            cContiguousZeroBits = 0;
        } else {
            // keep a count of contiguous zero-bits and generate random bits when
            // we see more than three (emulating the MC3470, see UA2, 9-11)
            ++cContiguousZeroBits;
            if (3 < cContiguousZeroBits) {
//                if (cContiguousZeroBits == 4) printf("\n<GENERATING RANDOM BIT(S).....>\n");
                if (randomBit()) {
                    this->pulse = true;
                }
            }
        }
    }

    bool readPulse() {
        return this->pulse;
    }
    void clearPulse() {
        this->pulse = false;
    }

    void writeBit(bool on) {
        this->disk.setBit(this->arm.getQuarterTrack());
    }
    //	unsigned char get() const
    //	{
    //		return this->disk.get(this->arm.getTrack());
    //	}

    //	void set(unsigned char value)
    //	{
    //		this->disk.put(this->arm.getTrack(),value);
    //	}



//    const WozFile& getDiskBytes() {
//        return this->disk;
//    }
};

#endif
