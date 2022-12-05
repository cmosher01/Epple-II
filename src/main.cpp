/*
    epple2
    Copyright (C) 2008, 2022 by Christopher A. Mosher <cmosher01@gmail.com>

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
#include "emulator.h"
#include "configep2.h"
#include "gui.h"
#include "e2const.h"
#include "E2wxApp.h"

#include <wx/app.h>

#include <thread>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <cstdio>
#include <cstddef>



static std::string parse_args(int argc, char* argv[]) {
    if (argc > 2) {
        throw std::runtime_error("usage: epple2 [config-file]" );
    }

    if (argc <= 1) {
        return std::string();
    }

    return std::string(argv[1]);
}


static int fake_argc(0);
static char fake_prog[] = "epple2";
static char *fake_argv[] { fake_prog };

static int runWx() {
    return wxEntry(fake_argc, fake_argv);
}

static int runSdl(const std::string config_file) {
    GUI gui;

    std::thread thread_wx(runWx);

    std::unique_ptr<Emulator> emu(new Emulator());

    Config cfg(config_file);
    emu->config(cfg);

    emu->init();

    const int ret = emu->run();

    if (wxApp::GetInstance() != nullptr) {
        wxApp::GetInstance()->ExitMainLoop();
    }
    thread_wx.join();

    return ret;
}



#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char *argv[]) {
    ::setbuf(stdout, nullptr);
    ::setbuf(stderr, nullptr);

    const int x = E2Const::test();
    if (x != -1) {
        std::cerr << x << std::endl;
        throw std::runtime_error("bad constant in e2const.h" );
    }

    const std::string config_file = parse_args(argc, argv);

    const int ret = runSdl(config_file);

    return ret;
}
