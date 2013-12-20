/* 
 * File:   trans.h
 * Author: cmosher
 *
 * Created on December 10, 2013, 2:37 PM
 */

#ifndef TRANS_H
#define	TRANS_H

#include "SegmentTypes.h"
#include <set>
#include <string>



class Trans;

enum class Pull { UP, DOWN, FLOAT };

class Segment final {
private:
    const std::string id;

public:
    std::set<Trans*> gates;
    std::set<Trans*> c1c2s;
    bool dmos;
    bool vss;
    bool vcc;

    Pull pull;
    bool on;



    Segment(const std::string& id) : id(id), dmos(id[0]=='+'), vss(false), vcc(false), pull(dmos ? Pull::UP : Pull::FLOAT), on(false) {
    }



    void set(const bool up) {
        this->pull = (up ? Pull::UP : Pull::DOWN);
    }

    bool operator<(const Segment& that) const { return this->id < that.id; }



    static unsigned char asByte(Segment* b7, Segment* b6, Segment* b5, Segment* b4, Segment* b3, Segment* b2, Segment* b1, Segment* b0) {
        return b7->on << 0x7 | b6->on << 0x6 | b5->on << 0x5 | b4->on << 0x4 | b3->on << 0x3 | b2->on << 0x2 | b1->on << 0x1 | b0->on;
    }

    static unsigned short asWord(Segment* bf, Segment* be, Segment* bd, Segment* bc, Segment* bb, Segment* ba, Segment* b9, Segment* b8, Segment* b7, Segment* b6, Segment* b5, Segment* b4, Segment* b3, Segment* b2, Segment* b1, Segment* b0) {
        return bf->on << 0xf | be->on << 0xe | bd->on << 0xd | bc->on << 0xc | bb->on << 0xb | ba->on << 0xa | b9->on << 0x9 | b8->on << 0x8 | b7->on << 0x7 | b6->on << 0x6 | b5->on << 0x5 | b4->on << 0x4 | b3->on << 0x3 | b2->on << 0x2 | b1->on << 0x1 | b0->on;
    }

private:

    Segment(const Segment&) = delete;
    Segment& operator=(const Segment&) = delete;
};







class Trans final {
public:

    Segment* c1;
    Segment* c2;

    bool on;



    Trans(Segment* c1, Segment* gate, Segment* c2) : c1(c1), c2(c2), on(false) {
        c1->c1c2s.insert(this);
        gate->gates.insert(this);
        c2->c1c2s.insert(this);
    }

private:

    Trans(const Trans&) = delete;
    Trans& operator=(const Trans&) = delete;
};

#endif	/* TRANS_H */
