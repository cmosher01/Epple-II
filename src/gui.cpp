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
#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#include <iostream>
#include "gui.h"
#include <SDL2/SDL.h>

// Create, initialize, and cable together the UI objects to serve this
// program

GUI::GUI() {
    const int result = SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    if (result != 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        throw GUI::NotInitException();
    }

    const SDL_bool ok = SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengles2");
    if (ok != SDL_TRUE) {
        std::cerr << "could not set opengles2 rendering" << std::endl;
// maybe it's OK, so don't throw GUI::NotInitException();
    }

    SDL_ShowCursor(0);
}

GUI::~GUI() {
    SDL_Quit();
}

GUI::NotInitException::NotInitException() :
runtime_error("Unable to initialize SDL") {
    SDL_GetError();
}
