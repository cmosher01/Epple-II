/* 
 * File:   Cpu6502Helper.cpp
 * Author: Christopher
 * 
 * Created on December 12, 2013, 10:22 PM
 */

#include "Cpu6502Helper.h"
#include "Cpu6502.h"
#include "Common.h"
#include "SegmentTypes.h"

void Cpu6502Helper::powerOn() {
    PinSettings ps;

    // set voltage supply and ground.
    ps.insert(std::make_pair(this->common.VCC, true));
    ps.insert(std::make_pair(this->common.VSS, false));

    // don't do the set-overflow overriding functionality
    ps.insert(std::make_pair(this->common.SO, false));

    // ready to run (i.e., do not do single-stepping of instructions)
    ps.insert(std::make_pair(this->common.RDY, true));

    // pull up to indicate that we are not interrupting now
    ps.insert(std::make_pair(this->common.IRQ, true));
    ps.insert(std::make_pair(this->common.NMI, true));


    /*
     * RES_BAR pin means "not resetting". Since it is a negated pin, pulling it low means "resetting"
     * and pulling it high means "not resetting" or equivalently "running".
     */

    /*
     * RES_BAR false: resetting now (i.e., in power-up now; pull high to begin normal operation)
     * We want to hold RES_BAR low for a while, indicating power-up phase during which the
     * CPU does not start up normal operations yet. The caller can set RES_BAR high (by calling
     * reset) whenever he is ready to start the CPU running.
     */
    ps.insert(std::make_pair(this->common.RES, false));

    this->cpu.setPins(ps);

    this->nextPhase = true;
}

void Cpu6502Helper::tick() {
    step();
    step();
}

void Cpu6502Helper::step() {
    /*
     * We cheat a little bit here: instead of requiring the
     * caller to toggle clock-zero pin, we let him just call
     * "step" and *we* keep track of which phase we are in.
     * To do this, we just use the CLK0 segment value (as
     * a kind of temporary variable), and just toggle it in
     * order to know which phase we are going into.
     * 
     * The real 6502, of course, does not do this.
     */
    this->nextPhase = !this->nextPhase;

    this->cpu.clock(this->nextPhase);
}

void Cpu6502Helper::reset() {
    this->cpu.setPins(PinSettings{std::make_pair(this->common.RES, true)});
}
