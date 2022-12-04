YSAV1   =   $35
CR      =   $8D
STKBAS  =   $0100
BUF     =   $0200
READC   =   $C080
LATCHC  =   $C081

CLOCK   PHP
        NOP
        PLP
        CLV
        CLI
        SEC
        BVS *+2+$18
        BVC READCL
        NOP

WRITECL ORA ($6F,X)
        RTS

READCL  LDA #$60
        STA YSAV1
        JSR YSAV1
        TSX
        LDA STKBAS,X
        ASL
        ASL
        ASL
        ASL
        TAX

        LDY #0
LOOP    LDA LATCHC,X
REREAD  LDA READC,X
        BPL REREAD
        STA BUF,Y
        INY
        CMP #CR
        BNE LOOP

        RTS
