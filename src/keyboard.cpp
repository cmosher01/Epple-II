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
#include "keyboard.h"
#include "hypermode.h"
#include "keyboardbuffermode.h"

Keyboard::Keyboard(KeypressQueue& q, HyperMode& fhyper, KeyboardBufferMode& buffered):
	keys(q),
	fhyper(fhyper),
	buffered(buffered),
	cGet(0)
{
}

void Keyboard::clear()
{
	this->latch &= 0x7F;
}

unsigned char Keyboard::get()
{
	waitIfTooFast();
	if (!this->buffered.isBuffered() || !(this->latch & 0x80))
	{
		if (!this->keys.empty())
		{
			this->latch = this->keys.front() | 0x80;
			this->keys.pop();
		}
	}
	return this->latch;
}

void Keyboard::waitIfTooFast()
{
	if (this->fhyper.isHyper())
	{
		return;
	}

	++this->cGet;
	if (!this->cGet)
	{
		if (SDL_GetTicks() - this->lastGet <= 1000)
		{
			/*
			* Check every 256 gets to see if they are
			* happening too fast (within one second).
			* If so, it means we are probably just
			* looping waiting for a keypress, so
			* wait a millisecond (or so) just to
			* prevent us from using 100% of CPU time.
			*/
			SDL_Delay(1);
		}
	}
	this->lastGet = SDL_GetTicks();
}
