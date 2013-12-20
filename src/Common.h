/* 
 * File:   Common.h
 * Author: Christopher
 *
 * Created on December 14, 2013, 8:47 PM
 */

#ifndef COMMON_H
#define	COMMON_H

#include "SegmentTypes.h"

class TransNetwork;

class Common final {
public:

    Segment * const VSS, * const VCC;
    Segment * const CLK0;
    Segment * const IRQ, * const RES, * const NMI;
    Segment * const RDY, * const SO;
    Segment * const DB0, * const DB1, * const DB2, * const DB3, * const DB4, * const DB5, * const DB6, * const DB7;
    Segment * const AB0, * const AB1, * const AB2, * const AB3, * const AB4, * const AB5, * const AB6, * const AB7, * const AB8, * const AB9, * const AB10, * const AB11, * const AB12, * const AB13, * const AB14, * const AB15;
    Segment * const RW, * const SYNC;
    Segment * const CLK1OUT, * const CLK2OUT;
    Segment * const A0, * const A1, * const A2, * const A3, * const A4, * const A5, * const A6, * const A7;
    Segment * const X0, * const X1, * const X2, * const X3, * const X4, * const X5, * const X6, * const X7;
    Segment * const Y0, * const Y1, * const Y2, * const Y3, * const Y4, * const Y5, * const Y6, * const Y7;
    Segment * const PCL0, * const PCL1, * const PCL2, * const PCL3, * const PCL4, * const PCL5, * const PCL6, * const PCL7;
    Segment * const PCH0, * const PCH1, * const PCH2, * const PCH3, * const PCH4, * const PCH5, * const PCH6, * const PCH7;
    Segment * const P0, * const P1, * const P2, * const P3, * const P4, /* no P5 */ * const P6, * const P7;
    Segment * const S0, * const S1, * const S2, * const S3, * const S4, * const S5, * const S6, * const S7;



    Common(const TransNetwork& segs);

    unsigned char rA() const;
    unsigned char rX() const;
    unsigned char rY() const;
    unsigned char rS() const;
    unsigned short rPC() const;
    unsigned short rAddr() const;
    unsigned char rData() const;

    PinSettings getDataPinSettings(const unsigned char data) const;



private:

    Common(
            Segment* VSS, Segment* VCC,
            Segment* CLK0,
            Segment* IRQ, Segment* RES, Segment* NMI,
            Segment* RDY, Segment* SO,
            Segment* DB0, Segment* DB1, Segment* DB2, Segment* DB3, Segment* DB4, Segment* DB5, Segment* DB6, Segment* DB7,
            Segment* AB0, Segment* AB1, Segment* AB2, Segment* AB3, Segment* AB4, Segment* AB5, Segment* AB6, Segment* AB7,
            Segment* AB8, Segment* AB9, Segment* AB10, Segment* AB11, Segment* AB12, Segment* AB13, Segment* AB14, Segment* AB15,
            Segment* RW, Segment* SYNC,
            Segment* CLK1OUT, Segment* CLK2OUT,
            Segment* A0, Segment* A1, Segment* A2, Segment* A3, Segment* A4, Segment* A5, Segment* A6, Segment* A7,
            Segment* X0, Segment* X1, Segment* X2, Segment* X3, Segment* X4, Segment* X5, Segment* X6, Segment* X7,
            Segment* Y0, Segment* Y1, Segment* Y2, Segment* Y3, Segment* Y4, Segment* Y5, Segment* Y6, Segment* Y7,
            Segment* PCL0, Segment* PCL1, Segment* PCL2, Segment* PCL3, Segment* PCL4, Segment* PCL5, Segment* PCL6, Segment* PCL7,
            Segment* PCH0, Segment* PCH1, Segment* PCH2, Segment* PCH3, Segment* PCH4, Segment* PCH5, Segment* PCH6, Segment* PCH7,
            Segment* P0, Segment* P1, Segment* P2, Segment* P3, Segment* P4, /* no P5 */ Segment* P6, Segment* P7,
            Segment* S0, Segment* S1, Segment* S2, Segment* S3, Segment* S4, Segment* S5, Segment* S6, Segment* S7) :
    VSS(VSS), VCC(VCC),
    CLK0(CLK0),
    IRQ(IRQ), RES(RES), NMI(NMI),
    RDY(RDY), SO(SO),
    DB0(DB0), DB1(DB1), DB2(DB2), DB3(DB3), DB4(DB4), DB5(DB5), DB6(DB6), DB7(DB7),
    AB0(AB0), AB1(AB1), AB2(AB2), AB3(AB3), AB4(AB4), AB5(AB5), AB6(AB6), AB7(AB7),
    AB8(AB8), AB9(AB9), AB10(AB10), AB11(AB11), AB12(AB12), AB13(AB13), AB14(AB14), AB15(AB15),
    RW(RW), SYNC(SYNC),
    CLK1OUT(CLK1OUT), CLK2OUT(CLK2OUT),
    A0(A0), A1(A1), A2(A2), A3(A3), A4(A4), A5(A5), A6(A6), A7(A7),
    X0(X0), X1(X1), X2(X2), X3(X3), X4(X4), X5(X5), X6(X6), X7(X7),
    Y0(Y0), Y1(Y1), Y2(Y2), Y3(Y3), Y4(Y4), Y5(Y5), Y6(Y6), Y7(Y7),
    PCL0(PCL0), PCL1(PCL1), PCL2(PCL2), PCL3(PCL3), PCL4(PCL4), PCL5(PCL5), PCL6(PCL6), PCL7(PCL7),
    PCH0(PCH0), PCH1(PCH1), PCH2(PCH2), PCH3(PCH3), PCH4(PCH4), PCH5(PCH5), PCH6(PCH6), PCH7(PCH7),
    P0(P0), P1(P1), P2(P2), P3(P3), P4(P4), /* no P5 */ P6(P6), P7(P7),
    S0(S0), S1(S1), S2(S2), S3(S3), S4(S4), S5(S5), S6(S6), S7(S7) {
    }
};

#endif	/* COMMON_H */
