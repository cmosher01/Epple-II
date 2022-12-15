/*
 * File:   Cpu6502.cpp
 * Author: Christopher
 *
 * Created on December 12, 2013, 10:14 PM
 */

#include "Cpu6502.h"
#include "addressbus.h"
#include "StateCalculator.h"
#include "Trace.h"
#include "Common.h"
#include "trans.h"



//#define TRACEREG 1
//#define TRACESEG 1




void Cpu6502::setPins(const PinSettings& ps) {
    SegmentSet rec;
    for (auto p : ps) {
        p.first->set(p.second);
        rec.insert(p.first);
    }
    StateCalculator::recalc(rec);
}

void Cpu6502::clock(bool phase) {
    setPins(PinSettings{std::make_pair(common.CLK0,phase)});
    rw();

    // TODO turn these into trace log settings
#ifdef TRACEREG
    this->trace.dumpRegisters();
#endif

#ifdef TRACESEG
    this->trace.dumpSegments();
#endif
}

void Cpu6502::rw() {
    /* database read/write happens (only) during Clock Phase 2 */
    if (common.CLK2OUT->on) {
        readData();
        writeData();
    }
}

void Cpu6502::readData() {
    if (this->common.RW->on) {
        setPins(this->common.getDataPinSettings(this->addressBus.read(this->common.rAddr())));
    }
}

void Cpu6502::writeData() {
    if (!this->common.RW->on) {
        this->addressBus.write(this->common.rAddr(), this->common.rData());
    }
}
