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
#include "standardinproducer.h"
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <iostream>

#define CR '\r'
#define LF '\n'

enum state_t { START, GOT_CR, GOT_LF, GOT_EOF };


static int readInput(void *voidkeys)
{
	KeypressQueue* keys = (KeypressQueue*)voidkeys;

	/*
		* Continuously read characters from standard in
		* and put them onto the queue.
		* Stop when we hit EOF (placing EOF onto the queue).
		* Translate LF to CR.
		* If we hit a CR immediately after a LF, drop it.
		* If we hit a LF immediately after a CR, drop it.
		*/
	enum state_t state = START;

	while (state != GOT_EOF)
	{
		char c = std::cin.get();
		c &= 0x7F;
		if (!std::cin.good())
		{
			state = GOT_EOF;
			keys->push(0xFF);
		}
		else
		{
			if (state == START)
			{
				if (c == CR)
				{
					state = GOT_CR;
				}
				else if (c == LF)
				{
					state = GOT_LF;
					c = CR;
				}
				keys->push(c);
			}
			else if (state == GOT_CR)
			{
				if (c != LF)
				{
					keys->push(c);
				}
				state = START;
			}
			else if (state == GOT_LF)
			{
				if (c != CR)
				{
					if (c == LF)
					{
						c = CR;
					}
					keys->push(c);
				}
				state = START;
			}
		}
	}

	return 0;
}

StandardInProducer::StandardInProducer()
{
	SDL_CreateThread(readInput,&this->keys);
}

StandardInProducer::~StandardInProducer()
{
}
