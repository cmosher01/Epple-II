/*
    epple2
    Copyright (C) 2008 by Chris Mosher <chris@mosher.mine.nu>

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
#include "paddlebuttonstates.h"

#include <SDL/SDL.h>

const int PaddleButtonStates::PADDLE_COUNT(3);

PaddleButtonStates::PaddleButtonStates()
{
}

PaddleButtonStates::~PaddleButtonStates()
{
}

bool PaddleButtonStates::isDown(const int paddle)
{
	if (paddle < 0 || PADDLE_COUNT <= paddle)
	{
		return false;
	}
	unsigned char btn = SDL_GetMouseState(0,0);
	if (paddle==0)
		return btn&SDL_BUTTON_LMASK;
	if (paddle==1)
		return btn&SDL_BUTTON_RMASK;
	return btn&SDL_BUTTON_MMASK;
}
