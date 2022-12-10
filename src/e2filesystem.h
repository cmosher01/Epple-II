/*
    epple2
    Copyright (C) 2022 by Christopher A. Mosher <cmosher01@gmail.com>

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

/*
 * File:   e2filesystem.cpp
 * Author: Christopher Alan Mosher, Shelton, Connecticut
 *
 * Created on December 7, 2022, 8:24 PM
 */

#ifndef E2FILESYSTEM_H
#define E2FILESYSTEM_H

#include <wx/string.h>

#include <filesystem>

std::filesystem::path valid_input_file(const std::filesystem::path path, const std::filesystem::path base = std::filesystem::path{});
std::filesystem::path path_from_string(const wxString& s);

#endif /* E2FILESYSTEM_H */
