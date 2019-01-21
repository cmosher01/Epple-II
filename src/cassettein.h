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
#ifndef CASSETTEIN_H
#define CASSETTEIN_H

#include <string>
#include <cstdint>
#include "cassette.h"

class CassetteIn : public Cassette {
    private:
        float *samp;
        std::uint32_t samp_siz;

        bool positive;
        std::int_fast8_t slope_was;

        virtual std::string port();
        void note_pos();

public:
        CassetteIn(ScreenImage& gui);
        virtual ~CassetteIn();

        virtual void tick();
        bool input();

        bool load(const std::string& filePath);
        void rewind();
        virtual bool eject();
};

#endif
