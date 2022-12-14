/* 
 * File:   TransNetwork.h
 * Author: cmosher
 *
 * Created on December 11, 2013, 10:44 AM
 */

#ifndef TRANSNETWORK_H
#define	TRANSNETWORK_H

#include <filesystem>
#include <set>
#include <memory>

class TransCache;
class SegmentCache;
class Common;
class Trans;

class TransNetwork final {
public:

    TransNetwork(std::filesystem::path& readFromHere, SegmentCache& segs, TransCache& transes);

private:

    TransNetwork(const TransNetwork&) = delete;
    TransNetwork& operator=(const TransNetwork&) = delete;

    SegmentCache& segs;
    TransCache& transes;

    friend Common;
};

#endif	/* TRANSNETWORK_H */
