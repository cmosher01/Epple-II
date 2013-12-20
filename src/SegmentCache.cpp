/* 
 * File:   SegmentCache.cpp
 * Author: cmosher
 * 
 * Created on December 10, 2013, 9:56 PM
 */

#include "SegmentCache.h"
#include "trans.h"
#include <memory>
#include <string>
#include <map>

bool SegmentCache::cached(const std::string& id) const {
    return this->cache.find(id) != this->cache.end();
}

Segment* SegmentCache::getOrAdd(const std::string& id) {
    if (!cached(id)) {
        this->cache[id] = std::make_shared<Segment>(id);

        /*
         * We want to optimize VSS and VCC checking in
         * the rest of the program, so we check here
         * for those two special cases, and flag them.
         * So, for example, checking (s->vss) is equivalent
         * to checking (s==VCC).
         */
        if (id == "-vss") {
            this->cache[id]->vss = true;
        } else if (id == "+vcc") {
            this->cache[id]->vcc = true;
        }
    }

    return get(id);
}

Segment* SegmentCache::get(const std::string& id) const {
    if (!cached(id)) {
        throw "Cannot find segment: " + id;
    }

    return this->cache.at(id).get();
}

SegmentSet SegmentCache::all() const {
    SegmentSet s;
    for (auto i : this->cache) {
        s.insert(i.second.get());
    }
    return s;
}
