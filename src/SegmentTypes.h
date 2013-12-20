/* 
 * File:   setpSeg.h
 * Author: Christopher
 *
 * Created on December 15, 2013, 12:16 AM
 */

#ifndef SETPSEG_H
#define	SETPSEG_H

#include "ptr_less.h"
#include <set>
#include <utility>

class Segment;

typedef std::set<Segment*,ptr_less<Segment>> SegmentSet;
typedef std::set<std::pair<Segment*, bool>> PinSettings;

#endif	/* SETPSEG_H */

