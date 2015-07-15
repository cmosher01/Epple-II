;    Epple ][ System Demo ROM
;    Copyright (C) 2009 by Chris Mosher <chris@mosher.mine.nu>
;
;    This program is free software: you can redistribute it and/or modify
;    it under the terms of the GNU General Public License as published by
;    the Free Software Foundation, either version 3 of the License, or
;    (at your option) any later version.
;
;    This program is distributed in the hope that it will be useful,
;    but WITHOUT ANY WARRANTY, without even the implied warranty of
;    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
;    GNU General Public License for more details.
;
;    You should have received a copy of the GNU General Public License
;    along with this program. If not, see <http://www.gnu.org/licenses/>.
;
;
;
include(asm.m4h)
;
;
;
; constants
SCREENWID   = 40 ; width of text screen
SCREENHGT   = 24 ; height of text screen
;
;
;
; ASCII values (high bit set)
BS          = $88
CR          = $8D
SPACE       = $A0
;
;
;
; zero page addresses (our variables)
TMPW        = $00 ; $01
ADDR        = $02 ; $03
ADDR2       = $04 ; $05
TEXTROW     = $06 ; $07
TMP         = $08
COL         = $09
ROW         = $0A
INBUFC      = $0B
;
;
;
; page 2 (input buffer)
INBUF       = $0200
;
;
;
; I/O addresses ($C0xx)
READKEY     = $C000
CLEARKEY    = $C010
TEXTON      = $C051
MIXEDOFF    = $C052
PAGE2OFF    = $C054
HIRESOFF    = $C056






;
;
;
;;RESET
; Handles power-on or reset-button-press.
; Initializes CPU, screen; displays welcome message;
; invokes the shell.
;
RESET       NOP
            SEI
            CLD
            LDX #$FF
            TXS

            LDA TEXTON
            LDA MIXEDOFF
            LDA PAGE2OFF
            LDA HIRESOFF

            NOP

            JSR ENDSCR
            JSR ADVCUR
            JSR WELCOME
            JMP SHELL
;
;
;
;;LINEADDR
; Computes address of column 0 for text line
; (0-23) given in A, into TEXTROW.
; input:
;  A        000edcba
; output:
;  TEXTROW  aeded000 000001cb
; scratch:
;  X
;
LINEADDR    TAX
            LDA #$04
            STA TEXTROW+1
            TXA
            LSR
            AND #$03
            ORA TEXTROW+1
            STA TEXTROW+1

            BCC ZERO
            LDA #$80
            BNE STORELOW
ZERO        LDA #$00
STORELOW    STA TEXTROW
            TXA
            AND #$18
            TAX
            ORA TEXTROW
            STA TEXTROW
            TXA
            ASL
            ASL
            ORA TEXTROW
            STA TEXTROW
            RTS
;
;
;
;;CHARADDR
; Computes address of character at ROW,COL
; for use as STA (TEXTROW),Y
; input: ROW, COL
; output: TEXTROW, Y
; scratch: A,X
;
CHARADDR    LDA ROW
            JSR LINEADDR
            LDY COL
            RTS
;
;
;
;;ENDSCR
; Moves cursor to bottom right.
; output: ROW, COL
; scratch: A
;
ENDSCR      LDA #SCREENHGT-1
            STA ROW
            LDA #SCREENWID-1
            STA COL
            RTS
;
;
;
;;SCROLL
; Scrolls the entire screen up one line
; scratch: A. Y, X, TMP, TMPW, TEXTROW
;
SCROLL      LDA #0
            STA TMP ; row

LOOPROW     LDA TMP
            JSR LINEADDR
            LDA TEXTROW
            STA TMPW
            LDA TEXTROW+1
            STA TMPW+1
            INC TMP
            LDA TMP
            JSR LINEADDR

            LDY #SCREENWID-1
LOOPCOL     LDA (TEXTROW),Y
            STA (TMPW),Y
            DEY
            BPL LOOPCOL

            LDA TMP
            CMP #SCREENHGT-1
            BNE LOOPROW

            RTS
;
;
;
;;CLEARLINE
; Clears line given by A
; input: A
; scratch: X, Y, TEXTROW
;
CLEARLINE   JSR LINEADDR
            LDA #SPACE
            LDY #SCREENWID-1
LOOPCLR     STA (TEXTROW),Y
            DEY
            BPL LOOPCLR
            RTS
;
;
;
;;PRCURS
; Prints the cursor (inverses char at ROL,COL)
; input: ROW, COL
; scratch: TEXTROW, Y, A, X
;
PRCURS      JSR CHARADDR
            LDA #$60
            STA (TEXTROW),Y
            RTS
;
;
;
;;PRCHAR
; Prints a normal character in A at ROW, COL;
; advances cursor; scrolls if necessary
; input: A, ROW, COL
; scratch: TEXTROW, Y, X
;
PRCHAR      PHA
            JSR CHARADDR
            PLA

            STA (TEXTROW),Y

            JSR ADVCUR
            RTS
;
;
;
;;ADVCUR
; Advances cursor one position; scrolls if necessary.
;
ADVCUR      INC COL
            LDA COL
            CMP #SCREENWID
            BNE ADVOK

            JSR PRCR
            JSR PRLF

ADVOK       RTS
;
;
;
;;PRCR
; Prints a carraige return (goes to start of same line)
; output: COL
; scratch: A
;
PRCR        LDA #0
            STA COL
            RTS
;
;
;
;;PRLF
; Prints a line feed (goes down one line);
; scrolls if necessary
; input: ROW
; output: ROW
; scratch: A. Y, X, TMP, TMPW, TEXTROW
;
PRLF        INC ROW
            LDA ROW
            CMP #SCREENHGT
            BNE LFOK

            DEC ROW
            JSR SCROLL
            LDA #SCREENHGT-1
            JSR CLEARLINE
LFOK        RTS
;
;
;
;;PRBS
; Prints a back-space (goes left one char)
; input: COL, ROW
; output: COL, ROW
; scratch: A
;
PRBS        DEC COL
            BPL BSOK
            LDA #SCREENWID-1
            STA COL
            DEC ROW
            BPL BSOK
            INC ROW
            LDA #0
            STA COL
BSOK        RTS
;
;
;
;;PRPROMPT
; Prints the prompt character.
; input: ROW, COL
; scratch: A, TEXTROW, Y, X
;
PRPROMPT    JSR PRSTR
            HIASCII(`E2>')
            ASM_DATA(0)
            RTS
;
;
;
;;RDCHAR
; Reads a character (waits for one) into A
; (high bit will be set)
;
RDCHAR      LDA CLEARKEY
NOKEY       LDA READKEY
            BPL NOKEY
            RTS
;
;
;
;;WELCOME
;
WELCOME     JSR PRSTR
            HIASCII(`EPPLE ][ DEMO ROM [GPLV3]  ? FOR HELP')
            ASM_DATA(0)
            JSR PRCR
            JSR PRLF
            RTS
;
;
;
;;PRSTR
; Prints a string (in code seg of caller)
;
PRSTR       PLA
            STA TMPW
            PLA
            STA TMPW+1
            INC TMPW
            BNE NOC
            INC TMPW+1
NOC         LDY #0
            LDA (TMPW),Y
            TAY
            LDA TMPW+1
            PHA
            LDA TMPW
            PHA
            TYA
            BEQ DONE
            JSR PRCHAR
            JMP PRSTR
DONE

            RTS
;
;
;;CVTHEX
; Parses hex character ('0'-'9' or 'A'-'F') in A
; and shifts into low-order nibble of TMPW.
; Undefined op if A contains something else.
;
CVTHEX      CMP #HICHAR(`A')
            BCC DIGIT
            SBC #'A'-'9'-1
DIGIT
            AND #%00001111
            ASL
            ASL
            ASL
            ASL

            ASL
            ROL TMPW
            ROL TMPW+1
            ASL
            ROL TMPW
            ROL TMPW+1
            ASL
            ROL TMPW
            ROL TMPW+1
            ASL
            ROL TMPW
            ROL TMPW+1
            RTS
;
;
;;PRBYTE
; Prints byte in A as hex
;
PRBYTE      PHA
                ; ABCDxxxx
                ; 1011ABCD
            LSR
            LSR
            LSR
            LSR
            ORA #%10110000
            CMP #HICHAR(`9')+1
            BCC PRDIGIT
            ADC #'A'-'9'-2
PRDIGIT     JSR PRCHAR
            PLA
            AND #%00001111
            ORA #%10110000
            CMP #HICHAR(`9')+1
            BCC PRDIGIT2
            ADC #'A'-'9'-2
PRDIGIT2    JSR PRCHAR
            RTS
;
;
;
;;SHELL
;
SHELL       LDA #0
            STA INBUFC
            JSR PRPROMPT


K_LOOP      JSR PRCURS
            JSR RDCHAR

K_CR        CMP #CR
            BNE K_BS
            LDA #SPACE
            JSR PRCHAR
            JSR COMMAND
            JMP SHELL

K_BS        CMP #BS
            BNE K_CHAR
            DEC INBUFC
            BPL K_BS_OK
            INC INBUFC
            JMP K_LOOP
K_BS_OK     JSR PRBS
            LDA #SPACE
            JSR PRCHAR
            LDA #SPACE
            JSR PRCHAR
            JSR PRBS
            JSR PRBS
            JMP K_LOOP

K_CHAR      LDY INBUFC
            STA INBUF,Y
            INC INBUFC
            BNE K_CHAR_OK
            DEC INBUFC
            JMP K_LOOP
K_CHAR_OK   JSR PRCHAR
            JMP K_LOOP
;
;
;
;;COMMAND
;
COMMAND     JSR PRCR
            JSR PRLF



            LDY #0
            CPY INBUFC
            BNE PARS1
            RTS
PARS1       LDA INBUF,Y

CMD_H       CMP #HICHAR(`?')
            BEQ CMD_H_IMP
            JMP CMD_D
CMD_H_IMP
            JSR PRCR
            JSR PRLF

            JSR PRSTR
            HIASCII(`COMMAND        ACTION')
            ASM_DATA(0)
            JSR PRCR
            JSR PRLF

            JSR PRSTR
            HIASCII(`-------------  ------------------------')
            ASM_DATA(0)
            JSR PRCR
            JSR PRLF

            JSR PRSTR
            HIASCII(`D XXXX [YYYY]  DUMP MEMORY XXXX TO YYYY')
            ASM_DATA(0)
            JSR PRCR
            JSR PRLF

            JSR PRSTR
            HIASCII(`R XXXX         RUN AT XXXX')
            ASM_DATA(0)
            JSR PRCR
            JSR PRLF

            JSR PRSTR
            HIASCII(`S XXXX YY      SET MEMORY XXXX TO YY')
            ASM_DATA(0)
            JSR PRCR
            JSR PRLF

            JSR PRSTR
            HIASCII(`?              SHOW HELP')
            ASM_DATA(0)
            JSR PRCR
            JSR PRLF

            JSR PRSTR
            HIASCII(`[ENTER ALL VALUES IN HEX]')
            ASM_DATA(0)
            JSR PRCR
            JSR PRLF

            JSR PRCR
            JSR PRLF

            RTS

CMD_D       CMP #HICHAR(`D')
            BEQ CMD_D_IMP
            JMP CMD_R

CMD_D_IMP   LDA #0
            STA TMP

CMD_D_ARGS  LDA #0
            STA TMPW
            STA TMPW+1

            LDX #4
CMD_D_SP    INY
            CPY INBUFC
            BNE CMD_D_1
            LDA TMP
            BNE CMD_D_RUN
            RTS
CMD_D_1     LDA INBUF,Y
            CMP #SPACE
            BEQ CMD_D_SP
            JSR CVTHEX
            DEX
            BNE CMD_D_SP
            LDA TMP
            BNE CMD_D_RUN
            INC TMP
            LDA TMPW
            STA ADDR
            STA ADDR2
            LDA TMPW+1
            STA ADDR+1
            STA ADDR2+1
            JMP CMD_D_ARGS

CMD_D_RUN   LDA TMPW
            STA ADDR2
            LDA TMPW+1
            STA ADDR2+1

CMD_D_ADR   LDA ADDR+1
            JSR PRBYTE
            LDA ADDR
            JSR PRBYTE
            LDA #HICHAR(`:')
            JSR PRCHAR

CMD_D_BYT   LDA #SPACE
            JSR PRCHAR
            LDY #0
            LDA (ADDR),Y
            JSR PRBYTE
            INC ADDR
            BNE CMD_D_NOOV
            INC ADDR+1
CMD_D_CKRO  LDA ADDR
            BNE CMD_D_NOOV
            LDA ADDR+1
            BEQ CMD_D_END
CMD_D_NOOV  LDA ADDR+1
            CMP ADDR2+1
            BNE CMD_D_NEXT
            LDA ADDR
            CMP ADDR2
CMD_D_NEXT  BCC CMD_D_CR
            BEQ CMD_D_CR
CMD_D_END   JSR PRCR
            JSR PRLF
            RTS
CMD_D_CR    LDA ADDR
            AND #%00000111
            BNE CMD_D_BYT
            JSR PRCR
            JSR PRLF
            JMP CMD_D_ADR

CMD_R       CMP #HICHAR(`R')
            BNE CMD_S
            LDX #4
CMD_R_SP    INY
            CPY INBUFC
            BNE CMD_R_1
            RTS
CMD_R_1     LDA INBUF,Y
            CMP #SPACE
            BEQ CMD_R_SP
            JSR CVTHEX
            DEX
            BNE CMD_R_SP
            JMP (TMPW)

CMD_S       CMP #HICHAR(`S')
            BEQ CMD_S_IMP
            RTS
CMD_S_IMP   LDX #4
CMD_S_SP    INY
            CPY INBUFC
            BNE CMD_S_1
            RTS
CMD_S_1     LDA INBUF,Y
            CMP #SPACE
            BEQ CMD_S_SP
            JSR CVTHEX
            DEX
            BNE CMD_S_SP
            LDA TMPW
            STA ADDR
            LDA TMPW+1
            STA ADDR+1

            LDX #2
CMD_S_SP2   INY
            CPY INBUFC
            BNE CMD_S_2
            RTS
CMD_S_2     LDA INBUF,Y
            CMP #SPACE
            BEQ CMD_S_SP2
            JSR CVTHEX
            DEX
            BNE CMD_S_SP2
            LDA TMPW
            LDY #0
            STA (ADDR),Y
            RTS
;
;
;
;;VECTORS
;
VECTORS     RTS
            JMP RESET
            ASM_ADDR(RESET)
            ASM_ADDR(RESET)
