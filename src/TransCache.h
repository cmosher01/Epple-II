/* 
 * File:   TransCache.h
 * Author: Christopher
 *
 * Created on December 15, 2013, 1:39 PM
 */

#ifndef TRANSCACHE_H
#define	TRANSCACHE_H

#include <set>
#include <memory>

class Trans;
class Segment;

class TransCache final {
public:

    TransCache() {
    }

    void add(Segment* c1, Segment* gate, Segment* c2);

    typedef std::set<std::shared_ptr<Trans>> Set;

    Set::const_iterator begin() const {
        return this->cache.begin();
    }

    Set::const_iterator end() const {
        return this->cache.end();
    }

    Set::size_type size() const {
        return this->cache.size();
    }

private:

    TransCache(const TransCache&) = delete;
    TransCache& operator=(const TransCache&) = delete;

    Set cache;
};

#endif	/* TRANSCACHE_H */
