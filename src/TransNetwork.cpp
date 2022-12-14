/*
 * File:   TransNetwork.cpp
 * Author: cmosher
 *
 * Created on December 11, 2013, 10:44 AM
 */

#include "TransNetwork.h"
#include "TransCache.h"
#include "SegmentCache.h"
#include "StateCalculator.h"
#include "trans.h"
#include "E2wxApp.h"
#include "e2filesystem.h"

#include <iostream>
#include <set>
#include <string>
#include <memory>
#include <fstream>

TransNetwork::TransNetwork(std::filesystem::path& p, SegmentCache& segs, TransCache& transes) : segs(segs), transes(transes) {
    std::filesystem::path inpath = valid_input_file(p, wxGetApp().GetResDir());

    std::ifstream in(inpath);

    std::string c1, gate, c2;
    in >> c1 >> gate >> c2;
    while (in.good()) {
        this->transes.add(this->segs.getOrAdd(c1), this->segs.getOrAdd(gate), this->segs.getOrAdd(c2));
        in >> c1 >> gate >> c2;
    }

    StateCalculator::recalc(this->segs.all());
}
