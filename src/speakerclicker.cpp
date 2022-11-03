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
#include "speakerclicker.h"
#include "e2const.h"
#include <SDL_audio.h>
#include <iostream>
#include <ostream>

#define TICKS_PER_SAMPLE 50

SpeakerClicker::SpeakerClicker():
    devid(0),
    silence(128),
    clicked(false),
    t(TICKS_PER_SAMPLE),
    positive(false) {
    SDL_AudioSpec audio;
    audio.freq = E2Const::AVG_CPU_HZ/TICKS_PER_SAMPLE; // samples per second
    audio.format = AUDIO_U8; // 8 bits (1 byte) per sample
    audio.channels = 1; // mono
    audio.silence = 128;
    audio.samples = 512;
    audio.callback = 0;
    audio.userdata = 0;

    SDL_AudioSpec obtained;
    obtained.callback = 0;
    obtained.userdata = 0;

    this->devid = SDL_OpenAudioDevice(0,0,&audio,&obtained,0);
    if (!this->devid) {
        std::cerr << "Unable to initialize audio: " << SDL_GetError() << std::endl;
    } else {
        this->silence = obtained.silence;

        std::cout << "initialized audio: " << std::endl;
        std::cout << "    device ID: " << this->devid << std::endl;
        std::cout << "    freq: " << obtained.freq << std::endl;
        std::cout << "    format: " << obtained.format << std::endl;
        std::cout << "    channels: " << (int)obtained.channels << std::endl;
        std::cout << "    silence: " << (int)obtained.silence << std::endl;
        std::cout << "    samples: " << obtained.samples << std::endl;
        std::cout << "    size: " << obtained.size << std::endl;
        SDL_PauseAudioDevice(this->devid, 0);
    }
}


SpeakerClicker::~SpeakerClicker() {
    if (!this->devid) {
        return;
    }
    SDL_CloseAudioDevice(this->devid);
}

void SpeakerClicker::tick() {
    if (!this->devid) {
        return;
    }

    if (!--this->t) {
        std::uint8_t amp;
        if (this->clicked) {
            amp = this->positive ? this->silence+100u : this->silence-100u;
            this->positive = !this->positive;
            this->clicked = false;
        } else {
            amp = silence;
        }
        const int failed = SDL_QueueAudio(this->devid, &amp, sizeof(amp));
        if (failed) {
            std::cerr << "SDL_QueueAudio failed " << SDL_GetError() << std::endl;
        }
        this->t = TICKS_PER_SAMPLE;
    }
}

void SpeakerClicker::click() {
    this->clicked = true;
}
