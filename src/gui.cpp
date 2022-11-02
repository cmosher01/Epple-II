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
#include <iostream>
#include "gui.h"
#include <SDL.h>

/*
 * Initialize SDL 2
 */

GUI::GUI() {
    const int result = SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    if (result != 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        throw GUI::NotInitException();
    }
}

GUI::~GUI() {
    SDL_Quit();
}

GUI::NotInitException::NotInitException() :
    runtime_error("Unable to initialize SDL") {
    SDL_GetError();
}
