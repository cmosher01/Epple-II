/* 
 * File:   Circuit.cpp
 * Author: Christopher
 * 
 * Created on December 12, 2013, 7:04 PM
 */

#include "Circuit.h"
#include "trans.h"

/*
 * Adds segment extendFrom, and all segments electrically connected to it.
 * This happens recursively, but we don't recurse past ground or voltage supply.
 */
void Circuit::extend(Segment* extendFrom) {
    auto ret = this->segs.insert(extendFrom);
    if (!ret.second) {
        /* We've already processed this segment. */
        return;
    }

    /* Don't recurse past ground or voltage supply. */
    if (extendFrom->vss || extendFrom->vcc) {
        return;
    }

    /*
     * For every ON transistor this seg is connected to via a leg (source or
     * drain), add the seg that's connected to the OTHER leg of the transistor.
     * This is a RECURSIVE addition.
     * 
     * Also note that, upon system startup, all transistors are initialized
     * to OFF, so at the time of the very first recalcAll call, *no* other
     * segments will be added here.
     */
    for (auto t : extendFrom->c1c2s) {
        if (t->on) {
            if (t->c1 == extendFrom) {
                extend(t->c2);
            } else if (t->c2 == extendFrom) {
                extend(t->c1);
            }
        }
    }
}

/*
 * Upon system startup, this will return the "pullup" value of
 * each segment, except for VCC and VSS (which will of course be
 * ON and OFF respectively).
 */
bool Circuit::getValue() {
    /* If group contains ground, it's OFF, */
    for (auto s : this->segs) {
        if (s->vss) {
            return false;
        }
    }

    /*
     * otherwise, if any segment in the group is not floating,
     * return that segment as the value, otherwise if any floating
     * segment is ON, then return ON as the value.
     */
    for (auto s : this->segs) {
        if (s->pull != Pull::FLOAT) {
            return (s->pull == Pull::UP);
        } else if (s->on) {
            return true;
        }
    }

    /*
     * otherwise, if we get here, it means that all segments in the
     * group are floating and OFF, so return OFF as the value.
     */
    return false;
}
