/* 
 * File:   StateCalculator.h
 * Author: Christopher
 *
 * Created on December 12, 2013, 8:29 PM
 */

#ifndef STATECALCULATOR_H
#define	STATECALCULATOR_H

#include <set>
#include "SegmentTypes.h"

class Trans;

class StateCalculator final {
public:

    static void recalc(const SegmentSet& rSeg);

private:

    StateCalculator() {
    }

    StateCalculator(const StateCalculator&) = delete;
    StateCalculator& operator=(const StateCalculator&) = delete;

    void recalcNode(Segment* seg);
    void setSeg(Segment* s, const bool on);
    void setTrans(Trans* t, const bool on);

    SegmentSet segs;
};

#endif	/* STATECALCULATOR_H */
