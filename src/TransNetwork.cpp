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
#include <iostream>
#include <set>
#include <string>
#include <memory>

TransNetwork::TransNetwork(std::istream& in, SegmentCache& segs, TransCache& transes) : segs(segs), transes(transes) {
    std::string c1, gate, c2;
    in >> c1 >> gate >> c2;
    while (in.good()) {
        this->transes.add(this->segs.getOrAdd(c1), this->segs.getOrAdd(gate), this->segs.getOrAdd(c2));
        in >> c1 >> gate >> c2;
    }

    StateCalculator::recalc(this->segs.all());
}
