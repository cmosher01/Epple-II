/*
    epple2

    Copyright © 2008, 2019 by Christopher A. Mosher <cmosher01@gmail.com>

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
#ifndef SPEAKERCLICKER_H
#define SPEAKERCLICKER_H

#include <cstdint>
#include <SDL_audio.h>

class SpeakerClicker {
    private:
        SDL_AudioDeviceID devid;
        std::uint8_t silence;
        bool clicked;
        std::uint8_t t;
    bool positive;
    public:
    SpeakerClicker();
    ~SpeakerClicker();
    void tick();
    void click();
};

#endif
