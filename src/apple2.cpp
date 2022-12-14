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
#include "apple2.h"
#include "slots.h"
#include "videomode.h"
#include "keyboard.h"
#include "addressbus.h"
#include "memory.h"
#include "picturegenerator.h"
#include "textcharacters.h"
#include "video.h"
#include "cpu.h"
#include "paddles.h"
#include "paddlebuttonstates.h"
#include "speakerclicker.h"
#include "analogtv.h"
#include "powerupreset.h"
#include "diskcontroller.h"
#include "languagecard.h"
#include "screenimage.h"

#include <iostream>
#include <istream>
#include <fstream>

Apple2::Apple2(KeypressQueue& keypresses, PaddleButtonStates& paddleButtonStates, AnalogTV& tv, KeyboardBufferMode& buffered, ScreenImage& gui) :
slts(gui),
kbd(keypresses, buffered),
rom(AddressBus::MOTHERBOARD_ROM_SIZ),
ram(revision),
cassetteIn(gui),
cassetteOut(gui),
addressBus(gui, revision, ram, rom, kbd, videoMode, paddles, paddleButtonStates, speaker, cassetteIn, cassetteOut, slts),
picgen(tv, videoMode, revision),
video(videoMode, addressBus, picgen, textRows),
transistors("transistors"), // TODO load file from resources
cpu(nullptr),
powerUpReset(*this),
revision(1) {
}

Apple2::~Apple2() {
    if (this->cpu) {
        delete this->cpu;
    }
}

void Apple2::useEpple2Cpu() {
    if (this->cpu == nullptr) {
        std::cout << "Using fast Epple2 CPU emulator" << std::endl;
        this->cpu = new CPU(this->addressBus);
    }
}

void Apple2::useVisual6502Cpu() {
    if (this->cpu == nullptr) {
        std::cout << "Using http://www.visual6502.org/ CPU emulation (which will be slow)." << std::endl;
        this->cpu = new Emu6502(this->transistors, this->addressBus);
    }
}

void Apple2::tick() {
    useEpple2Cpu(); // default, if not already set
    this->cpu->tick();
    this->slts.tick();
    this->video.tick();
    this->paddles.tick();
    this->speaker.tick();
    this->cassetteIn.tick();
    this->cassetteOut.tick();

    if (this->revision > 0) {
        this->powerUpReset.tick();
    }
}

void Apple2::powerOn() {
    useEpple2Cpu(); // default, if not already set
    this->ram.powerOn();
    this->cpu->powerOn();
    this->videoMode.powerOn();
    this->video.powerOn();
    this->picgen.powerOn();
    this->powerUpReset.powerOn();
    this->kbd.powerOn();
}

void Apple2::powerOff() {
    this->ram.powerOff();
}

void Apple2::reset() {
    useEpple2Cpu(); // default, if not already set
    this->cpu->reset();
    this->slts.reset();
}
