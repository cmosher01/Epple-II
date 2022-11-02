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
#include <SDL_audio.h>
#include <SDL_log.h>
#include "tinyfiledialogs.h"
#include "cassetteout.h"
#include "e2const.h"
#include <vector>
#include <cmath>

CassetteOut::CassetteOut(ScreenImage& gui):
    Cassette(gui) {
    this->gui.setCassetteOutFile("[empty]");
}


CassetteOut::~CassetteOut() {
}

std::string CassetteOut::port() {
    return "OUT";
}

//void CassetteOut::note_pos() {
//    std::cout << "cassette: ";
//    std::cout << "t=" << this->t;
//    std::cout << ", ";
//    std::cout << "t(in )=" << this->playing_t;
//    if (this->playing) {
//        std::cout << "*";
//    }
//    std::cout << ", ";
//    std::cout << "t(out)=" << this->recording_t;
//    if (this->recording) {
//        std::cout << "*";
//    }
//    if (this->samp_out.size()) {
//        std::cout << "[" << this->samp_out.size() << "]";
//    }
//    std::cout << std::endl;
//}

void CassetteOut::tick() {
    Cassette::tick();
}

void CassetteOut::output() {
    if (isLoaded()) {
        if (!this->playing) {
            this->playing = true;
            note("PLAY+RECORD");
    //        note_pos();
        } else {
            const std::uint32_t d = this->t-this->t_active;
            this->samp_out.push_back(d);
            this->modified = true;
            this->gui.setCassetteDirty(true);
        }
        this->t_active = this->t;
    }
}

bool CassetteOut::blank(const std::string& filePath) {
    if (!eject()) {
        return false;
    }
    std::ifstream in(filePath.c_str(),std::ios::binary|std::ios::in);
    if (in.is_open()) {
       in.close();
       std::cerr << "Error creating file; file already exists: " << filePath << std::endl;
       return false;
    }
    std::ofstream out(filePath.c_str(),std::ios::binary|std::ios::out);
    out.close();
    if (out) {
        this->file = filePath;
        this->gui.setCassetteOutFile(this->file);
        this->samp_out.clear();
    } else {
        std::cerr << "Error creating file: " << filePath << std::endl;
    }
    return (bool)out;
}

bool CassetteOut::eject() {
    const bool ok = Cassette::eject();
    if (ok) {
        this->gui.setCassetteOutFile("[empty]");
        this->samp_out.clear();
    }
    return ok;
}

bool CassetteOut::write() {
    std::ofstream out(this->file.c_str(), std::ios::binary);



    std::uint32_t c_sample = 0;
    for (std::uint32_t i = 0; i < this->samp_out.size(); ++i) {
        if (this->samp_out[i] < 3000) {
            c_sample += this->samp_out[i]/50;
        }
    }

    std::uint32_t longVal;
    std::uint16_t wordVal;

    out.write("RIFF", 4);
    out.write((char*)&(longVal = 36+c_sample), 4);
    out.write("WAVE", 4);



    out.write("fmt ", 4);
    out.write((char*)&(longVal = 16), 4);

    out.write((char*)&(wordVal = 1), 2); // PCM
    out.write((char*)&(wordVal = 1), 2); // mono, one channel

    out.write((char*)&(longVal = E2Const::AVG_CPU_HZ/50), 4); // samples per second
    out.write((char*)&(longVal = E2Const::AVG_CPU_HZ/50), 4); // byte rate (same)
    out.write((char*)&(wordVal = 1), 2); // alignment
    out.write((char*)&(wordVal = 8), 2); // bits per sample



    out.write("data", 4);
    out.write((char*)&(longVal = c_sample), 4);

    const float pi = acos(-1);
    bool positive = false;
    for (std::uint32_t i = 0; i < this->samp_out.size(); ++i) {
        if (this->samp_out[i] < 3000) {
            for (std::uint_fast8_t s = 0; s < this->samp_out[i]/50; ++s) {
                float x = sin(pi/2 + 50*pi*s/this->samp_out[i]);
                if (!positive) {
                    x = -1.0f*x;
                }
                x = round(128+128*x);
                if (x > 255.0f) {
                    x = 255.0f;
                }
                if (x < 0.0f) {
                    x = 0.0f;
                }
                std::uint8_t bx = x;
                out.write((char*)&bx, 1);
            }
        }
        positive = !positive;
    }



    out.flush();
    out.close();
    return (bool)out;
}
