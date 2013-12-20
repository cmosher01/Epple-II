/* 
 * File:   SegmentCache.h
 * Author: Christopher
 *
 * Created on December 10, 2013, 9:56 PM
 */

#ifndef SEGMENTCACHE_H
#define	SEGMENTCACHE_H

#include "SegmentTypes.h"
#include <map>
#include <set>
#include <string>
#include <memory>

class Common;

class SegmentCache final {
public:

    SegmentCache() {
    }

    Segment* getOrAdd(const std::string& id);

    SegmentSet all() const;



    typedef std::map<const std::string, std::shared_ptr<Segment>> Map;

    Map::const_iterator begin() const {
        return this->cache.begin();
    }

    Map::const_iterator end() const {
        return this->cache.end();
    }

    Map::size_type size() const {
        return this->cache.size();
    }

private:

    Map cache;

    SegmentCache(const SegmentCache&) = delete;
    SegmentCache& operator=(const SegmentCache&) = delete;

    Segment* get(const std::string& id) const;
    bool cached(const std::string& id) const;

    friend Common;
};

#endif	/* SEGMENTCACHE_H */
