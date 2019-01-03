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
#ifndef STEPPERMOTOR_H
#define STEPPERMOTOR_H

#include <cstdint>

class StepperMotor {
private:
    enum { QTRACKS = 160 };
    // quarter track: 0=t0, 1=t0.25, 2=t0.5, 3=t0.75, 4=t1, ... 140=t35.00 ... 159=t39.75
    // (see TMAP in WOZ2 file format spec)
    std::uint8_t quarterTrack;

    std::int8_t pos;
    std::uint8_t mags;

    std::int8_t pendingPos;
    std::uint32_t pendingTicks;

    static std::int8_t mapMagPos[];

    void moveCog();
    void calculateTrack(const std::int8_t delta);

    static std::int8_t calcDeltaPos(const std::int8_t cur, const std::int8_t next) {
        std::int8_t d = next-cur; // -7 to +7

        if (d == 4 || d == -4) {
            d = 0; // <--- TODO pick random direction?
        } else if (4 < d) {
            d -= 8;
        } else if (d < -4) {
            d += 8;
        }

        return d;
    }

public:
    StepperMotor();
    ~StepperMotor();

    void setMagnet(const std::uint8_t magnet, const bool on);
    std::uint8_t getTrack() {
        return this->quarterTrack >> 2;
    }
    std::uint8_t getQuarterTrack() {
        return this->quarterTrack;
    }
    void tick();
};

#endif
