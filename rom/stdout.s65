YSAV1   =   $35
STKBAS  =   $0100
LATCHC  =   $C080
COUT    =   $FDF0
MONRTS  =   $FF58

STDOUT  STY YSAV1
        PHA
        TXA
        PHA

        JSR MONRTS
        TSX
        LDA STKBAS,X
        ASL
        ASL
        ASL
        ASL
        TAX

        PLA
        TAY
        PLA

        STA LATCHC,X

        PHA
        TYA
        TAX
        PLA
        LDY YSAV1

        JMP COUT
