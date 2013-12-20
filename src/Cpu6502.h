/* 
 * File:   Cpu6502.h
 * Author: Christopher
 *
 * Created on December 12, 2013, 10:14 PM
 */

#ifndef CPU6502_H
#define	CPU6502_H

#include "SegmentTypes.h"
#include "Trace.h"

class AddressBus;
class Trace;
class Common;

class Cpu6502 final {
public:

    Cpu6502(AddressBus& addressBus, Trace& trace, Common& common) : addressBus(addressBus), trace(trace), common(common) {
#if 0
        trace.dumpTransistors();
#endif
    }

    void setPins(const PinSettings& ps);
    void clock(bool phase);


private:

    Cpu6502(const Cpu6502&) = delete;
    Cpu6502& operator=(const Cpu6502&) = delete;

    void rw();
    void readData();
    void writeData();

    AddressBus& addressBus;
    Trace& trace;
    Common& common;
};

#endif	/* CPU6502_H */
