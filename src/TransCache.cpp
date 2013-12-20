/* 
 * File:   TransCache.cpp
 * Author: Christopher
 * 
 * Created on December 15, 2013, 1:39 PM
 */

#include "TransCache.h"
#include "trans.h"

void TransCache::add(Segment* c1, Segment* gate, Segment* c2) {
    this->cache.insert(std::make_shared<Trans>(c1,gate,c2));
}
