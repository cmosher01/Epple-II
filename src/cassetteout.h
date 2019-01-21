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
#ifndef CASSETTEOUT_H
#define CASSETTEOUT_H

#include <vector>
#include <string>
#include <cstdint>
#include "cassette.h"

class CassetteOut : public Cassette {
    private:
        std::vector<std::uint32_t> samp_out;

        virtual std::string port();

public:
        CassetteOut(ScreenImage& gui);
        virtual ~CassetteOut();

        virtual void tick();
        void output();

        bool blank(const std::string& filePath);
        virtual bool write();
        virtual bool eject();
};

#endif
