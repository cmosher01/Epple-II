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

#include "e2filesystem.h"

#include <boost/log/trivial.hpp>

#include <system_error>

static void log_ec(const std::error_code& ec) {
    BOOST_LOG_TRIVIAL(warning) << "error from native file-system: " << ec.value() << " " << ec.message();
}

std::filesystem::path valid_input_file(const std::filesystem::path path, const std::filesystem::path base) {
    std::filesystem::path p(path);

    std::error_code ec;



    BOOST_LOG_TRIVIAL(info) << "will check for file specified as: " << p.c_str();
    if (p.is_relative()) {
        BOOST_LOG_TRIVIAL(info) << "will check for file at relative path: " << p.c_str();

        std::filesystem::path b(base);
        if (b.empty()) {
            BOOST_LOG_TRIVIAL(info) << "no base path found; will use current path...";
            b = std::filesystem::current_path(ec);
            if (ec) {
                log_ec(ec);
                b.clear();
            }
        }

        BOOST_LOG_TRIVIAL(info) << "...relative to base path: " << b.c_str();
        p = b / p;
    }



    p = p.lexically_normal();
    BOOST_LOG_TRIVIAL(info) << "checking for file at absolute path: " << p.c_str();
    bool exists = std::filesystem::exists(p, ec);
    if (ec) {
        log_ec(ec);
        exists = false;
    }

    if (!exists) {
        BOOST_LOG_TRIVIAL(warning) << "can't find file: " << p.c_str();
        p = std::filesystem::weakly_canonical(p, ec);
        if (ec) {
            log_ec(ec);
            BOOST_LOG_TRIVIAL(warning) << "can't determine canonical path.";
        } else {
            BOOST_LOG_TRIVIAL(info) << "...canonical file path was: " << p.c_str();
        }
        p.clear();
        return p;
    }



    BOOST_LOG_TRIVIAL(info) << "found file: " << p.c_str();

    std::filesystem::path cp = std::filesystem::canonical(p, ec);
    if (ec) {
        log_ec(ec);
        BOOST_LOG_TRIVIAL(warning) << "can't determine canonical path, continuing anyway...";
    } else {
        BOOST_LOG_TRIVIAL(info) << "...canonical file path was: " << cp;
    }



    bool regular = std::filesystem::is_regular_file(p, ec);
    if (ec) {
        log_ec(ec);
        regular = false;
    }

    if (!regular) {
        BOOST_LOG_TRIVIAL(warning) << "not a regular file";
        p.clear();
        return p;
    }

    return p;
}



std::filesystem::path path_from_string(const wxString& s) {
    return std::filesystem::path{s.wc_str()};
}
