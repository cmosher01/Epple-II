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
#include "e2const.h"
#include "paddles.h"
#include <SDL/SDL.h>
#include <iostream>
#include <ostream>


Paddles::Paddles():
	rTick(PADDLE_COUNT)
{
}


Paddles::~Paddles()
{
}


void Paddles::tick()
{
	for (int paddle = 0; paddle < PADDLE_COUNT; ++paddle)
	{
		if (this->rTick[paddle] > 0)
			--this->rTick[paddle];
	}
}

void Paddles::startTimers()
{
	try
	{
		tryStartPaddleTimers();
	}
	catch (...)
	{
		std::cerr << "Warning: cannot start paddle timers; mouse will not function as paddles." << std::endl;
	}
}

void Paddles::tryStartPaddleTimers()
{
	int x, y;
	SDL_GetMouseState(&x,&y);

	double pMin = 0;
	double pMax = 500;
	x = (int)((x-pMin)/(pMax-pMin)*PADDLE_CYCLES+.5);
	y = (int)((y-pMin)/(pMax-pMin)*PADDLE_CYCLES+.5);

	if (isTimedOut(0))
		this->rTick[0] = x;
	if (isTimedOut(1))
		this->rTick[1] = y;

	/*
		Here we emulate having 4700 ohm across pins 7 and 1
		of the game controller, and a 47Kohm resistor acros
		pins 11 and 1, to give cheap real-time clocks at
		paddles 2 and 3. Paddle 2 is the 100 microsecond reference,
		and paddle 3 is the 1 millisecond reference. This is
		described in U.A.2, p. 7-33.
	*/
	if (isTimedOut(2))
		this->rTick[2] = E2Const::AVG_CPU_HZ/10000; // was 90, but why?
	if (isTimedOut(3))
		this->rTick[3] = E2Const::AVG_CPU_HZ/1000;
}

bool Paddles::isTimedOut(const int paddle)
{
	if (paddle < 0 || PADDLE_COUNT <= paddle)
	{
		return false;
	}
	return this->rTick[paddle] <= 0;
}
