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
#include "screenimage.h"
#include "e2const.h"
#include "applentsc.h"
#include "card.h"
#include "util.h"
#include <SDL/SDL.h>
#include <ctime>
#include <sstream>

static const char* power =
" @@@@    @@@   @    @    @  @@@@@  @@@@ "
" @   @  @   @  @   @ @   @  @      @   @"
" @   @  @   @  @   @ @   @  @      @   @"
" @@@@   @   @   @ @   @ @   @@@@@  @@@@ "
" @      @   @   @ @   @ @   @      @ @  "
" @      @   @   @ @   @ @   @      @  @ "
" @       @@@     @     @    @@@@@  @   @";

#define POWERD 56
#define LABEL_Y 24
#define ON_CLR 0xF0D050
#define OFF_CLR 0x807870
#define SCRW 640
#define SCRH 480

static const int HEIGHT = E2Const::VISIBLE_ROWS_PER_FIELD*2;
static const int WIDTH = AppleNTSC::H-AppleNTSC::PIC_START-2;

#include "font3x5.h"

class ScreenException { };

ScreenImage::ScreenImage():
	fullscreen(false),
	hyper(false),
	buffer(true),
	fillLines(false),
	display(AnalogTV::MONITOR_COLOR),
	slotnames(8),
	cassettename(32,' ')
{
	createScreen();
}

void ScreenImage::toggleFullScreen()
{
	this->fullscreen = !this->fullscreen;
	createScreen();
}

void ScreenImage::createScreen()
{
	this->screen = SDL_SetVideoMode(SCRW,SCRH,32,SDL_HWSURFACE|SDL_HWPALETTE|(this->fullscreen?SDL_FULLSCREEN:0));
	if (this->screen == NULL)
	{
		printf("Unable to set video mode: %s\n",SDL_GetError());
		throw ScreenException();
	};
	if (this->screen->pitch/4 != SCRW)
	{
		printf("Cannot set video screen pitch to 640*4 pixels (gets set to %d)\n",this->screen->pitch);
		throw ScreenException();
	};
	drawLabels();
	notifyObservers();
}

void ScreenImage::drawLabels()
{
	drawText("EPPLE ][",0,141);
	drawSlots();
	drawCassette();
	drawFnKeys();
}

void ScreenImage::drawSlots()
{
	int r(65);
	int c(17);
	drawText("SLOTS:",r++,c);
	for (int slot(0); slot < 8; ++slot)
	{
		drawSlot(slot,r++,c);
	}
}

void ScreenImage::drawSlot(int slot, int r, int c)
{
	drawChar('0'+slot,r,c++);
	drawChar(':',r,c++);
	drawChar(' ',r,c++);
	drawText(this->slotnames[slot],r,c);
	const int len = this->slotnames[slot].length();
	if (len < 100)
	{
		drawText(std::string(100-len,' '),r,c+len);
	}
}

void ScreenImage::drawCassette()
{
	int r(65);
	int c(85);
	drawText("CASSETTE:",r,c);
	c += 9;
	drawText(this->cassettename,r,c);
	const int len = this->cassettename.length();
	if (len < 40)
	{
		drawText(std::string(40-len,' '),r,c+len);
	}
}

static const char* displays[] =
{
	"COLOR MONITOR ",
	"WHITE MONITOR ",
	"GREEN MONITOR ",
	"ORANGE MONITOR",
	"OLD COLOR TV  ",
	"OLD B & W TV  ",
	"NEW COLOR TV  ",
	"NEW B & W TV  ",
};

void ScreenImage::drawFnKeys()
{
	int r(76);
	int c(1);
	drawText(
"                                FULLSCRN   SCAN-LINES                     KEYBOARD",r++,c);
	drawText(
"                XXXXXXXXXXXXXX  WINDOW     FILL-LINES  CMD  REPT   HYPER   BUFFER   RESET    PASTE     SAVE BMP     QUIT!",r++,c);
	drawText(
"      F1              F2          F3          F4       F5   F10     F11     F12     Break    Insert    PrintScreen   End",r++,c);

	if (this->fullscreen)
		invertText(76,32,42); // FULLSCRN
	else
		invertText(77,32,40); // WINDOW

	if (this->fillLines)
		invertText(77,43,55); // FILL-LINES
	else
		invertText(76,43,55); // SCAN-LINES

	if (this->hyper)
		invertText(77,67,74); // HYPER

	if (this->buffer)
		invertText(77,75,83); // BUFFER

	drawDisplayLabel();
}

void ScreenImage::toggleFillLinesLabel()
{
	this->fillLines = !this->fillLines;
	invertText(76,43,55); // SCAN-LINES
	invertText(77,43,55); // FILL-LINES
}

void ScreenImage::drawDisplayLabel()
{
	const char* label = displays[(int)(this->display)];
	drawText(label,77,17);
}

void ScreenImage::cycleDisplayLabel()
{
	this->display = (AnalogTV::DisplayType)((((int)this->display)+1)%AnalogTV::NUM_DISPLAY_TYPES);
	drawDisplayLabel();
}

void ScreenImage::toggleHyperLabel()
{
	this->hyper = !this->hyper;
	invertText(77,67,74); // HYPER
}

void ScreenImage::toggleKdbBufferLabel()
{
	this->buffer = !this->buffer;
	invertText(77,75,83); // BUFFER
}

void ScreenImage::invertText(int row, int begincol, int endcol)
{
	unsigned int* pn = (unsigned int*)this->screen->pixels;
	pn += row*FONTH*SCRW+begincol*FONTW;
	const int dc = (endcol-begincol)*FONTW;
	for (int ir = 0; ir < FONTH; ++ir)
	{
		for (int ic = 0; ic < dc; ++ic)
		{
			*pn = ~*pn;
			++pn;
		}
		pn -= dc;
		pn += SCRW;
	}
}

void ScreenImage::drawText(const std::string& text, int row, int col, int color, int bgcolor)
{
	for (std::string::const_iterator i = text.begin(); i != text.end(); ++i)
	{
		char c = (*i) & 0x7F;
		if (0 <= c && c < 0x20)
			c += 0x40;
		drawChar(c,row,col++,color,bgcolor);
	}
}

void ScreenImage::drawChar(const char ch, int row, int col, int color, int bgcolor)
{
	if (!(0 <= row  && row < SCRH/6 && 0 <= col && col < SCRW/4))
	{
		printf("bad text plotting (r %d, c %d): \"%c\"\n",row,col,ch);
	}
	unsigned int* pn = (unsigned int*)this->screen->pixels;
	pn += row*FONTH*SCRW+col*FONTW;

	const char* pt = font3x5+FONTH*FONTW*(ch-0x20);
	for (int r = 0; r < FONTH; ++r)
	{
		for (int c = 0; c < FONTW; ++c)
		{
			*pn++ = *pt++ == '@' ? color : bgcolor;
		}
		pn -= FONTW;
		pn += SCRW;
	}
	SDL_UpdateRect(this->screen,col*FONTW,row*FONTH,(col+1)*FONTW,(row+1)*FONTH);
}

void ScreenImage::displayHz(int hz)
{
	char s[20];
	sprintf(s,"%4.2f MHz   ",hz/1e6);
	drawText(s,3,141);
}

void ScreenImage::drawPower(bool on)
{
	unsigned int* pn = (unsigned int*)this->screen->pixels;
	pn += (HEIGHT+5)*(this->screen->pitch/4)+5;
	for (int r = 0; r < POWERD; ++r)
	{
		if (r < LABEL_Y || LABEL_Y+7 <= r)
		{
			for (int c = 0; c < POWERD; ++c)
			{
				*pn++ = on ? ON_CLR : OFF_CLR;
			}
		}
		else
		{
			{
				for (int c = 0; c < 8; ++c)
				{
					*pn++ = on ? ON_CLR : OFF_CLR;
				}
			}
			for (const char* ppow = power+(r-(LABEL_Y))*40; ppow < power+(r-(LABEL_Y-1))*40; ++ppow)
			{
				if (*ppow == '@')
					*pn++ = 0;
				else
					*pn++ = on ? ON_CLR : OFF_CLR;
			}
			{
				for (int c = 0; c < 8; ++c)
				{
					*pn++ = on ? ON_CLR : OFF_CLR;
				}
			}
		}
		pn -= POWERD;
		pn += this->screen->pitch/4;
	}
	SDL_UpdateRect(this->screen,0,HEIGHT,POWERD,HEIGHT+POWERD);
}

ScreenImage::~ScreenImage()
{
}

void ScreenImage::notifyObservers()
{
	SDL_UpdateRect(this->screen,0,0,SCRW,SCRH);
}

void ScreenImage::setElem(unsigned int i, const unsigned int val)
{
	unsigned int* pn = (unsigned int*)this->screen->pixels;
	i += (i/WIDTH)*(SCRW-WIDTH);
	pn += i;
	*pn = val;
}

void ScreenImage::blank()
{
	for (int r = 0; r < HEIGHT; ++r)
	{
		memset((char*)(this->screen->pixels)+r*SCRW*4,0,WIDTH*4);
	}
}

void ScreenImage::enterCommandMode()
{
	int r(76);
	int c(1);
	unsigned int* pn = (unsigned int*)this->screen->pixels;
	pn += r*FONTH*SCRW+c*FONTW;

	memset((char*)pn,0,SCRW*4*FONTH*3);

	drawText("command: ",78,1);
	this->cmdpos = 9;

	notifyObservers();
}

void ScreenImage::exitCommandMode()
{
	drawFnKeys();
	notifyObservers();
}

void ScreenImage::addkeyCommand(unsigned char key)
{
	++this->cmdpos;
	drawChar((char)key,78,this->cmdpos);
}

void ScreenImage::backspaceCommand()
{
	drawChar(' ',78,this->cmdpos);
	--this->cmdpos;
}

void ScreenImage::updateSlotName(const int slot, Card* card)
{
	int r(66+slot);
	int c(20);
	const std::string& name = card->getName();
	this->slotnames[slot] = name;
	drawText(std::string(80,' '),r,c);
	drawText(name,r,c);
}

void ScreenImage::removeCard(const int slot, Card* card /* empty */)
{
	updateSlotName(slot,card);
}

/*
1  2         3         4         5         6         7         8
789012345678901234567890123456789012345678901234567890123456789012345
6: disk][  drive 1M*filename.nib T$FF      drive 2M*filename.nib T$FF
*/
void ScreenImage::setDiskFile(int slot, int drive, const std::string& filepath)
{
	std::string f = truncateFilePath(filepath);
	int r(66+slot);
	int c(37+32*drive);
	drawText(f,r,c);

	const int dlen = 12 - f.length();
	if (dlen > 0)
	{
		std::string d(dlen,' ');
		drawText(d,r,c+f.length());
	}

	this->slotnames[slot].replace(c-20,12,12,' ');
	this->slotnames[slot].replace(c-20,f.length(),f);
}

std::string ScreenImage::truncateFilePath(const std::string& filepath)
{
	std::string f(filepath);
	size_t slash = f.find_last_of("/\\");
	if (slash != std::string::npos)
	{
		f = f.substr(slash+1);
	}
	if (f.length() > 12)
	{
		f = f.substr(0,12);
	}
	return f;
}

void ScreenImage::clearCurrentDrive(int slot, int drive)
{
	int r(66+slot);
	int c(35+32*drive);
	drawChar(' ',r,c);
	this->slotnames[slot][c-20] = ' ';
	c += 15;
	drawText("    ",r,c);
	this->slotnames[slot].replace(c-20,4,4,' ');
}

void ScreenImage::setCurrentDrive(int slot, int drive, int track, bool on)
{
	int r(66+slot);
	int c(35+32*drive);
	drawChar(' ',r,c,0xFFFFFF,on?0xFF0000:0);
	c += 15;
	drawChar('T',r,c);
	this->slotnames[slot][c-20] = 'T';
	++c;
	drawChar('$',r,c);
	this->slotnames[slot][c-20] = '$';
	++c;
	char nibh = Util::hexDigit((((unsigned char)track) >> 4) & 0xF);
	drawChar(nibh,r,c);
	this->slotnames[slot][c-20] = nibh;
	++c;
	char nibl = Util::hexDigit((unsigned char)track & 0xF);
	drawChar(nibl,r,c);
	this->slotnames[slot][c-20] = nibl;
}

void ScreenImage::setTrack(int slot, int drive, int track)
{
	int r(66+slot);
	int c(52+32*drive);
	char nibh = Util::hexDigit((((unsigned char)track) >> 4) & 0xF);
	drawChar(nibh,r,c);
	this->slotnames[slot][c-20] = nibh;
	++c;
	char nibl = Util::hexDigit((unsigned char)track & 0xF);
	drawChar(nibl,r,c);
	this->slotnames[slot][c-20] = nibl;
}

void ScreenImage::setIO(int slot, int drive, bool on)
{
	int r(66+slot);
	int c(35+32*drive);
	drawChar(' ',r,c,0xFFFFFF,on?0xFF0000:0);
}

void ScreenImage::setDirty(int slot, int drive, bool dirty)
{
	int r(66+slot);
	int c(36+32*drive);
	drawChar(dirty?'*':' ',r,c);
	this->slotnames[slot][c-20] = dirty?'*':' ';
}

void ScreenImage::setCassetteFile(const std::string& filepath)
{
	std::string f = truncateFilePath(filepath);
	int r(65);
	int c(85+11);
	drawText(f,r,c);

	const int dlen = 12 - f.length();
	if (dlen > 0)
	{
		std::string d(dlen,' ');
		drawText(d,r,c+f.length());
	}

	this->cassettename.replace(c-94,12,12,' ');
	this->cassettename.replace(c-94,f.length(),f);
}

void ScreenImage::setCassetteDirty(bool dirty)
{
	int r(65);
	int c(85+10);
	drawChar(dirty?'*':' ',r,c);
	this->cassettename[c-94] = dirty?'*':' ';
}

void ScreenImage::setCassettePos(int pos, int siz)
{
	int r(65);
	int c(110);
	std::ostringstream os;
	os << pos << '/' << siz << "                             ";
	drawText(os.str(),r,c);
}

/*
1  2         3         4         5         6         7         8
789012345678901234567890123456789012345678901234567890123456789012345678
0: language RW B2
*/
void ScreenImage::setLangCard(int slot, bool readEnable, bool writeEnable, int bank)
{
	int r(66+slot);
	int c(29);
	drawChar(readEnable?'R':' ',r,c);
	this->slotnames[slot][c-20] = readEnable?'R':' ';
	++c;
	drawChar(writeEnable?'W':' ',r,c);
	this->slotnames[slot][c-20] = writeEnable?'W':' ';
	++c;
	++c;
	drawChar('B',r,c);
	this->slotnames[slot][c-20] = 'B';
	++c;
	drawChar(bank==0?'1':'2',r,c);
	this->slotnames[slot][c-20] = bank==0?'1':'2';
}

/*
1  2         3         4         5         6         7         8
789012345678901234567890123456789012345678901234567890123456789012345678
0: firmware D F8
*/
void ScreenImage::setFirmCard(int slot, bool bank, bool F8)
{
	int r(66+slot);
	int c(29);
	drawChar(bank?'D':' ',r,c);
	this->slotnames[slot][c-20] = bank?'D':' ';
	++c;
	++c;
	drawChar(F8?'F':' ',r,c);
	this->slotnames[slot][c-20] = F8?'F':' ';
	++c;
	drawChar(F8?'8':' ',r,c);
	this->slotnames[slot][c-20] = F8?'8':' ';
}

#define TIMEFORMAT "ep2_%Y%m%d%H%M%S.bmp"

void ScreenImage::saveBMP()
{
	time_t now;
	::time(&now);
	struct tm* nowtm = ::localtime(&now);
	char time[64];
	::strftime(time,sizeof(time),TIMEFORMAT,nowtm);
	SDL_SaveBMP(this->screen,time);
}
