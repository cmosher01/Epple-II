/* 
 * File:   Emu6502.h
 * Author: Christopher
 *
 * Created on December 15, 2013, 12:43 AM
 */

#ifndef EMU6502_H
#define	EMU6502_H

#include "Cpu6502Helper.h"
#include "Cpu6502.h"
#include "Trace.h"
#include "Common.h"
#include "TransNetwork.h"
#include "TransCache.h"
#include "SegmentCache.h"
#include <istream>

class AddressBus;

class Emu6502 {
public:

    Emu6502(std::istream& transistors, AddressBus& mem) : tn(transistors, segs, transes), c(tn), trace(segs, transes, c), cpu(mem, trace, c), cpuhelper(cpu, c) {
    }

    virtual ~Emu6502() {
    }

    void powerOn() {
        this->cpuhelper.powerOn();
    }

    void tick() {
        this->cpuhelper.tick();
    }

    void reset() {
        this->cpuhelper.reset();
    }

private:

    Emu6502(const Emu6502&) = delete;
    Emu6502 operator=(const Emu6502&) = delete;

    SegmentCache segs;
    TransCache transes;
    TransNetwork tn;
    Common c;
    Trace trace;
    Cpu6502 cpu;
    Cpu6502Helper cpuhelper;
};

#endif	/* EMU6502_H */
