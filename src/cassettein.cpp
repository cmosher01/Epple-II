/*
    epple2

    Copyright © 2008, 2019, Christopher Alan Mosher, Shelton, CT, USA. <cmosher01@gmail.com>

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
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_log.h>
#include "tinyfiledialogs.h"
#include "cassettein.h"
#include "e2const.h"
#include <vector>
#include <cmath>

CassetteIn::CassetteIn(ScreenImage& gui):
    Cassette(gui),
    samp(nullptr),
    samp_siz(0),
    positive(false),
    slope_was(0) {
    this->gui.setCassetteInFile("[empty]");
}

CassetteIn::~CassetteIn() {
}

std::string CassetteIn::port() {
    return "IN";
}

void CassetteIn::note_pos() {
    std::cout << "cassette: ";
    std::cout << "t=" << this->t;
    std::cout << ", ";
    std::cout << "t_act=" << this->t_active;
    if (this->playing) {
        std::cout << "+";
    }
    std::cout << std::endl;
}

static const float MOVEMENT_THRESHOLD = 0.001f;

/* prev --> curr: rising=+1, falling=-1, flat=0 */
static std::int_fast8_t slope(const float prev, const float curr) {
    if (abs(curr-prev) < MOVEMENT_THRESHOLD) {
        return 0;
    }
    return prev < curr ? +1 : -1;
}

//static std::uint_fast32_t prev_p = 0;
//static std::uint_fast32_t ccc = 0;

void CassetteIn::tick() {
    Cassette::tick();
    if (this->t % 10) {
        return;
    }

    if (this->playing) {
        const std::uint_fast32_t p = this->t/10 > 0 ? this->t/10 : 1;

        if (this->samp_siz <= p) {
            note("END OF TAPE");
            this->t = this->samp_siz*10;
            this->t_active = this->t;
            this->playing = false;
            this->gui.setCassettePos(this->samp_siz,this->samp_siz);
            note_pos();
            return;
        }

        this->gui.setCassettePos(p,this->samp_siz);



        /*
         * Reconstruct original square wave signal (from the Apple cassette output port)
         * from the cassette tape's (lagging) sine wave. Assume the cassette's recorded voltage level
         * lagged from its input voltage. Therefore, here we check for the sine wave BEGINNING
         * to react to what we assume is the input voltage's (square wave) swing, and we use
         * that as the signal to send up to the emulator.
         *
         * ---+             +-----
         *    .\           /
         *    . \         /
         *    .  \       /
         *    .   +-----+
         *    .         .
         *    .         .
         * ---+         +---------
         *    |         |
         *    |         |
         *    |         |
         *    +---------+
         */
        std::int_fast8_t slope_is = slope(this->samp[p-1], this->samp[p]);
        if (slope_is != 0 && slope_is != this->slope_was) {
            this->positive = !this->positive;
//            std::printf("%d ", (p-prev_p));
//            if (!(++ccc % 40)) {
//                std::printf("\n");
//            }
//            prev_p = p;
            this->slope_was = slope_is;
        }
    }
}

bool CassetteIn::input() {
    if (!isLoaded()) {
        return false;
    }

    if (!this->playing) {
        if (this->t/10 < this->samp_siz) {
            note("PLAY");
            this->playing = true;
            note_pos();
        }
    }

    if (this->playing) {
        this->t_active = this->t;
    }

    return this->positive;
}

void CassetteIn::rewind() {
    if (!isLoaded()) {
        return;
    }
    note_pos();

    note("REWIND");
    this->t = 0;
    this->t_active = 0;
    this->slope_was = 0;
    this->positive = false;
    this->playing = false;
    note_pos();

    this->gui.setCassettePos(this->t/10,this->samp_siz);
}

void CassetteIn::tone() {
    if (!isLoaded()) {
        return;
    }
    note_pos();

    note("FAST FORWARD TO TONE");

    this->playing = false;

    const unsigned int HEAD_SAMPLES = 1020;
    std::int_fast8_t slope_was = 0;
    std::uint_fast32_t i_was = 0;
    std::uint_fast32_t c_head = 0;

    const std::uint_fast32_t start = this->t/10 > 0 ? this->t/10 : 1;
    for (std::uint_fast32_t i = start; i < this->samp_siz; ++i) {
        std::int_fast8_t slope_is = slope(this->samp[i-1], this->samp[i]);
        if (slope_is) {
            if (slope_is != slope_was) {
//                std::printf("%d ", (i-prev_p));
//                if (!(++ccc % 40)) {
//                    std::printf("\n");
//                }
//                prev_p = i;
                this->positive = !this->positive;
                this->slope_was = slope_is;
                const std::uint_fast32_t d = i-i_was;
                if (59u <= d && d <= 71u) {
                    ++c_head;
//                    std::printf("=====[%d]====", c_head);
                    // fast-forward to the header tone
                    if (HEAD_SAMPLES <= c_head) {
//                        prev_p = i;
                        this->t = i*10-1;
                        this->t_active = this->t;
//                        ccc = 0;
                        break;
                    }
                } else {
                    c_head = 0;
                }
                i_was = i;
                slope_was = slope_is;
            }
        }
    }
    note_pos();
    this->gui.setCassettePos(this->t/10,this->samp_siz);
}

bool CassetteIn::load(const std::string& filePath) {
    SDL_AudioSpec wav_spec;
    std::uint8_t *wav_buffer;
    std::uint32_t wav_length;

    if (SDL_LoadWAV(filePath.c_str(), &wav_spec, &wav_buffer, &wav_length) == nullptr) {
        SDL_Log("Error: %s ; file: %s\n", SDL_GetError(), filePath.c_str());
        return false;
    }
//    std::printf("opened WAVE file %s\n", filePath.c_str());
//    std::printf("    buffer size: %d bytes\n", wav_length);
//    std::printf("    sample rate: %dHz\n", wav_spec.freq);
//    std::printf("    sample datatype: %04X\n", wav_spec.format);
//    std::printf("    channels: %d\n", wav_spec.channels);
//    std::printf("    silence value: %d\n", wav_spec.silence);
//    std::printf("    sample count: %d\n", wav_spec.samples);

    if (isLoaded()) {
        if (!eject()) {
            return false;
        }
	}

    /* convert input sample to floating-point, at rate of 10 CPU cycles per sample, for easy calculation */
    SDL_AudioCVT cvt;
    SDL_BuildAudioCVT(&cvt, wav_spec.format, wav_spec.channels, wav_spec.freq, AUDIO_F32, 1, E2Const::AVG_CPU_HZ/10);
    cvt.len = wav_length;
    cvt.buf = reinterpret_cast<std::uint8_t*>(std::malloc(cvt.len_mult * cvt.len));
    std::memcpy(cvt.buf, wav_buffer, cvt.len);
    SDL_FreeWAV(wav_buffer);

    SDL_ConvertAudio(&cvt);
    this->samp = reinterpret_cast<float*>(cvt.buf);
    this->samp_siz = cvt.len_cvt/4u;

    note("LOAD");
    note_pos();

    this->file = filePath;
    this->gui.setCassetteInFile(filePath);

    rewind();
    tone();

	return true;
}

bool CassetteIn::eject() {
    const bool ok = Cassette::eject();
    if (ok) {
        this->gui.setCassetteInFile("[empty]");
        this->gui.setCassettePos(0,0);
        std::free(this->samp);
        this->samp_siz = 0;
    }
    return ok;
}
