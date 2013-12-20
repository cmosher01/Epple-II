/* 
 * File:   Common.cpp
 * Author: Christopher
 * 
 * Created on December 14, 2013, 8:47 PM
 */

#include "Common.h"
#include "TransNetwork.h"
#include "SegmentCache.h"
#include "trans.h"

Common::Common(const TransNetwork& tn) :
Common(
    tn.segs.get("-vss"), tn.segs.get("+vcc"),
    tn.segs.get("-clk0"),
    tn.segs.get("-irq"), tn.segs.get("-res"), tn.segs.get("-nmi"),
    tn.segs.get("+rdy"), tn.segs.get("+so"),
    tn.segs.get("-db0"), tn.segs.get("-db1"), tn.segs.get("-db2"), tn.segs.get("-db3"), tn.segs.get("-db4"), tn.segs.get("-db5"), tn.segs.get("-db6"), tn.segs.get("-db7"),
    tn.segs.get("-ab0"), tn.segs.get("-ab1"), tn.segs.get("-ab2"), tn.segs.get("-ab3"), tn.segs.get("-ab4"), tn.segs.get("-ab5"), tn.segs.get("-ab6"), tn.segs.get("-ab7"),
    tn.segs.get("-ab8"), tn.segs.get("-ab9"), tn.segs.get("-ab10"), tn.segs.get("-ab11"), tn.segs.get("-ab12"), tn.segs.get("-ab13"), tn.segs.get("-ab14"), tn.segs.get("-ab15"),
    tn.segs.get("-rw"), tn.segs.get("-sync"),
    tn.segs.get("-clk1out"), tn.segs.get("-clk2out"),
    tn.segs.get("-a0"), tn.segs.get("-a1"), tn.segs.get("-a2"), tn.segs.get("-a3"), tn.segs.get("-a4"), tn.segs.get("-a5"), tn.segs.get("-a6"), tn.segs.get("-a7"),
    tn.segs.get("-x0"), tn.segs.get("-x1"), tn.segs.get("-x2"), tn.segs.get("-x3"), tn.segs.get("-x4"), tn.segs.get("-x5"), tn.segs.get("-x6"), tn.segs.get("-x7"),
    tn.segs.get("-y0"), tn.segs.get("-y1"), tn.segs.get("-y2"), tn.segs.get("-y3"), tn.segs.get("-y4"), tn.segs.get("-y5"), tn.segs.get("-y6"), tn.segs.get("-y7"),
    tn.segs.get("-pcl0"), tn.segs.get("-pcl1"), tn.segs.get("-pcl2"), tn.segs.get("-pcl3"), tn.segs.get("-pcl4"), tn.segs.get("-pcl5"), tn.segs.get("-pcl6"), tn.segs.get("-pcl7"),
    tn.segs.get("-pch0"), tn.segs.get("-pch1"), tn.segs.get("-pch2"), tn.segs.get("-pch3"), tn.segs.get("-pch4"), tn.segs.get("-pch5"), tn.segs.get("-pch6"), tn.segs.get("-pch7"),
    tn.segs.get("+Pout0"), tn.segs.get("+Pout1"), tn.segs.get("+Pout2"), tn.segs.get("+Pout3"), tn.segs.get("+Pout4"), /*no P5 */tn.segs.get("+Pout6"), tn.segs.get("+Pout7"),
    tn.segs.get("-s0"), tn.segs.get("-s1"), tn.segs.get("-s2"), tn.segs.get("-s3"), tn.segs.get("-s4"), tn.segs.get("-s5"), tn.segs.get("-s6"), tn.segs.get("-s7")) {
}

unsigned short Common::rAddr() const {
    return Segment::asWord(this->AB15, this->AB14, this->AB13, this->AB12, this->AB11, this->AB10, this->AB9, this->AB8, this->AB7, this->AB6, this->AB5, this->AB4, this->AB3, this->AB2, this->AB1, this->AB0);
}

unsigned char Common::rData() const {
    return Segment::asByte(this->DB7, this->DB6, this->DB5, this->DB4, this->DB3, this->DB2, this->DB1, this->DB0);
}

unsigned char Common::rA() const {
    return Segment::asByte(this->A7, this->A6, this->A5, this->A4, this->A3, this->A2, this->A1, this->A0);
}

unsigned char Common::rX() const {
    return Segment::asByte(this->X7, this->X6, this->X5, this->X4, this->X3, this->X2, this->X1, this->X0);
}

unsigned char Common::rY() const {
    return Segment::asByte(this->Y7, this->Y6, this->Y5, this->Y4, this->Y3, this->Y2, this->Y1, this->Y0);
}

unsigned char Common::rS() const {
    return Segment::asByte(this->S7, this->S6, this->S5, this->S4, this->S3, this->S2, this->S1, this->S0);
}

unsigned short Common::rPC() const {
    return Segment::asWord(this->PCH7, this->PCH6, this->PCH5, this->PCH4, this->PCH3, this->PCH2, this->PCH1, this->PCH0, this->PCL7, this->PCL6, this->PCL5, this->PCL4, this->PCL3, this->PCL2, this->PCL1, this->PCL0);
}

PinSettings Common::getDataPinSettings(const unsigned char data) const {
    unsigned char x = data;

    PinSettings ps;

    ps.insert(std::make_pair(this->DB0,x & 1));
    x >>= 1;
    ps.insert(std::make_pair(this->DB1,x & 1));
    x >>= 1;
    ps.insert(std::make_pair(this->DB2,x & 1));
    x >>= 1;
    ps.insert(std::make_pair(this->DB3,x & 1));
    x >>= 1;
    ps.insert(std::make_pair(this->DB4,x & 1));
    x >>= 1;
    ps.insert(std::make_pair(this->DB5,x & 1));
    x >>= 1;
    ps.insert(std::make_pair(this->DB6,x & 1));
    x >>= 1;
    ps.insert(std::make_pair(this->DB7,x & 1));

    return ps;
}
