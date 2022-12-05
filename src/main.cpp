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
#include "gui.h"

#include <wx/app.h>

#include <iostream>
#include <ostream>
#include <stdexcept>
#include <cstdio>
#include <cstddef>



#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char *argv[]) {
    ::setbuf(::stdout, nullptr);
    ::setbuf(::stderr, nullptr);

    const int x = E2Const::test();
    if (x != -1) {
        std::cerr << x << std::endl;
        throw std::runtime_error("bad constant in e2const.h" );
    }

    GUI gui;

    wxEntry(argc, argv);

    return 0;
}
