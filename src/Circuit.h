/* 
 * File:   Circuit.h
 * Author: Christopher
 *
 * Created on December 12, 2013, 7:04 PM
 */

#ifndef CIRCUIT_H
#define	CIRCUIT_H

#include "SegmentTypes.h"
#include <set>

/*
 * Builds a circuit, given one segment in that circuit.
 * Extends the given segment by traversing the ON
 * transistors it is connected to, recursively, until
 * hitting ground and voltage supply. Provides iterators
 * for retrieving all the circuit's segments, and a method
 * to get the ON value of the circuit.
 */
class Circuit final {
public:

    Circuit(Segment* extendFrom) {
        extend(extendFrom);
    }

    bool getValue();

    SegmentSet::iterator begin() {
        return this->segs.begin();
    }

    SegmentSet::iterator end() {
        return this->segs.end();
    }

private:

    Circuit(const Circuit&) = delete;
    Circuit& operator=(const Circuit&) = delete;

    void extend(Segment* extendFrom);

    SegmentSet segs;
};

#endif	/* CIRCUIT_H */
