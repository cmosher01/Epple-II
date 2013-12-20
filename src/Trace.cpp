/* 
 * File:   Trace.cpp
 * Author: cmosher
 * 
 * Created on December 12, 2013, 3:39 PM
 */

#include "Trace.h"
#include "TransCache.h"
#include "SegmentCache.h"
#include "Common.h"
#include "trans.h"
#include <iostream>
#include <iomanip>

static void pHex(const unsigned long x, const int width) {
    std::cout << std::setw(width) << std::setfill('0') << std::hex << x << std::dec;
}

static void pHexb(const unsigned char x) {
    pHex(x, 2);
}

static void pHexw(const unsigned short x) {
    pHex(x, 4);
}

void Trace::dumpSegments() const {
    for (auto sp : this->segs) {
        Segment* seg = sp.second.get();
        if (seg->pull == Pull::UP) {
            std::cout << (seg->on ? "U" : "u");
        } else if (seg->pull == Pull::DOWN) {
            std::cout << (seg->on ? "D" : "d");
        } else {
            std::cout << (seg->on ? "F" : "f");
        }
    }
    std::cout << std::endl;
}

void Trace::dumpRegisters() const {
    std::cout << "A";
    pHexb(this->common.rA());

    std::cout << " X";
    pHexb(this->common.rX());

    std::cout << " Y";
    pHexb(this->common.rY());

    std::cout << " ";
    std::cout << (this->common.P7->on ? "N" : "n");
    std::cout << (this->common.P6->on ? "V" : "v");
    std::cout << ".";
    std::cout << (this->common.P4->on ? "B" : "b");
    std::cout << (this->common.P3->on ? "D" : "d");
    std::cout << (this->common.P2->on ? "I" : "i");
    std::cout << (this->common.P1->on ? "Z" : "z");
    std::cout << (this->common.P0->on ? "C" : "c");

    std::cout << " S";
    pHexb(this->common.rS());

    std::cout << " PC";
    pHexw(this->common.rPC());

    if (this->common.CLK1OUT->on) {
        std::cout << "  PH1  ";
    }
    if (this->common.CLK2OUT->on) {
        std::cout << "  PH2";
        if (this->common.RW->on) {
            std::cout << " R";
        } else {
            std::cout << " W";
        }
    }
    if (!(this->common.CLK1OUT->on || this->common.CLK2OUT->on)) {
        std::cout << "  PH-  ";
    }

    std::cout << " DB";
    pHexb(this->common.rData());

    std::cout << " AB";
    pHexw(this->common.rAddr());

    std::cout << std::endl;
}

void Trace::dumpTransistors() const {
    /* count depletion-mode MOSFETs */
    int cd(0);
    for (auto sp : this->segs) {
        Segment* seg = sp.second.get();
        if (seg->dmos) {
            ++cd;
        }
    }
    
    std::cout << "eMOSFETs: " << this->transes.size() << ", dMOSFETs: " << cd << std::endl;
}
