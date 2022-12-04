YSAV1   =   $35
EOF     =   $FF
STKBAS  =   $0100
READC   =   $C080
LATCHC  =   $C081
KEYIN   =   $FD1B
MONRTS  =   $FF58

STDIN   TXA
        PHA
        JSR MONRTS
        TSX
        LDA STKBAS,X
        ASL
        ASL
        ASL
        ASL
        TAX

        LDA LATCHC,X
REREAD  LDA READC,X
        BPL REREAD
        STA YSAV1
        CMP #EOF
        BNE EXIT
        JSR KEYIN
        STA YSAV1
EXIT    PLA
        TAX
        LDA YSAV1
        RTS
