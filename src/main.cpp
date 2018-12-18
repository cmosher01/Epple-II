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
#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#include <SDL2/SDL.h>
#include "emulator.h"
#include "configep2.h"
#include "gui.h"
#include "e2const.h"

#include <string>
#include <memory>
#include <stdexcept>

#include <iostream>
#include <iomanip>

static int run(const std::string& config_file) {
	GUI gui;

    std::unique_ptr<Emulator> emu(new Emulator());

	Config cfg(config_file);
	emu->config(cfg);

	emu->init();

	return emu->run();
}



//std::uint8_t dataBus;
//std::uint8_t lssrom[0x100];
//std::uint8_t lss13rom[0x100];
//std::uint8_t wp(0);
//std::uint8_t q7(0);
//std::uint8_t q6(0);
//std::uint8_t dreg(0);
//std::uint8_t seq(0x20);
//std::uint8_t cmd;
//std::uint8_t adr;
//std::uint8_t cprint(0);

//void debugLog(const std::uint8_t x) {
//    if (++cprint == 128 || x==0xD5u) {
//        cprint  = 0;
//        printf("\n");
//    }
//    printf("%02x", x);
//}

//void lss(const std::uint8_t cmd) {
//    if (cmd & 8) {
//        switch (cmd & 3) {
//            case 3: dreg = dataBus; break;
//            case 2: dreg >>= 1; dreg |= (wp << 7); break;
//            case 1: dreg <<= 1; dreg |= ((cmd & 4) >> 2); break;
//        }
//    } else {
//        debugLog(dreg);
//        dreg = 0;
//    }
//}

//void doTrack(std::uint8_t *ptrk, const std::uint16_t ctrk) {
//    std::uint8_t *pend(ptrk+ctrk);
//    while (ptrk < pend) {
//        for (std::uint8_t m(0x80); m; m >>= 1) {
//            std::uint8_t pls = (*ptrk & m) ? 1 : 0;
//            for (int i(0); i < 8; ++i) {
//                adr = q7<<3 | q6<<2 | (dreg>>7)<<1 | pls;
//                pls = 0;
//                cmd = lssrom[seq|adr];
//                seq = cmd & 0xF0;
//                lss(cmd);
//            }
//        }
//        ++ptrk;
//    }
//}




#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char* argv[]) {
    if (argc > 2) {
		throw std::runtime_error("usage: epple2 [config-file]" );
	}

	int x = E2Const::test();
    if (x != -1) {
		std::cerr << x << std::endl;
		throw std::runtime_error("bad constant in e2const.h" );
	}

	std::string config_file;
    if (argc > 1) {
		config_file = argv[1];
	}

	return run(config_file);
}
