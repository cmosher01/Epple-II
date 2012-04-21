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
#ifndef SCREENIMAGE_H
#define SCREENIMAGE_H

#include "analogtv.h"
#include <vector>
#include <string>

class Card;
struct SDL_Surface;

class ScreenImage
{
private:
	SDL_Surface* screen;
	bool fullscreen;
	bool hyper;
	bool buffer;
	bool fillLines;
	AnalogTV::DisplayType display;
	unsigned int cmdpos;
	void createScreen();
	std::vector<std::string> slotnames;
	std::string cassettename;

	static std::string truncateFilePath(const std::string& filepath);

// TODO some of these methods should be private
public:
	ScreenImage();
	~ScreenImage();

	void toggleFullScreen();
	void drawPower(bool on);
	void notifyObservers();
	void setElem(const unsigned int i, const unsigned int val);
	void blank();
	void drawText(const std::string& text, int row, int col, int color = 0xFFFFFF, int bgcolor = 0);
	void drawChar(const char ch, int row, int col, int color = 0xFFFFFF, int bgcolor = 0);
	void drawLabels();
	void drawSlots();
	void drawSlot(int slot, int r, int c);
	void drawCassette();
	void drawFnKeys();
	void toggleHyperLabel();
	void toggleKdbBufferLabel();
	void cycleDisplayLabel();
	void displayHz(int hz);
	void toggleFillLinesLabel();
	void invertText(int row, int begincol, int endcol);
	void drawDisplayLabel();
	void updateSlotName(const int slot, Card* card);
	void removeCard(const int slot, Card* card /* empty */);

	void enterCommandMode();
	void exitCommandMode();
	void addkeyCommand(unsigned char key);
	void backspaceCommand();

	void setDiskFile(int slot, int drive, const std::string& filename);

	void clearCurrentDrive(int slt, int drv);
	void setCurrentDrive(int slt, int drv, int track, bool on);
	void setTrack(int slot, int drive, int track);
	void setIO(int slot, int drive, bool on);
	void setDirty(int slot, int drive, bool dirty);

	void setCassetteFile(const std::string& filepath);
	void setCassetteDirty(bool dirty);
	void setCassettePos(int pos, int siz);

	void setLangCard(int slot, bool readEnable, bool writeEnable, int bank);
	void setFirmCard(int slot, bool bank, bool F8);

	void saveBMP();
};

#endif
