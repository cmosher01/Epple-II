/*
    epple2

    Copyright © 2018, Christopher Alan Mosher, Shelton, CT, USA. <cmosher01@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY, without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "lss.h"

#include <algorithm>
#include <iterator>

static void setcmd(std::uint8_t lssrom[], std::uint8_t x, std::uint8_t cmd) {
    lssrom[x] = (lssrom[x] & 0xF0u) | cmd;
}

static void setseq(std::uint8_t lssrom[], std::uint8_t x, std::uint8_t seq) {
    lssrom[x] = (lssrom[x] & 0x0Fu) | seq;
}

static void setbth(std::uint8_t lssrom[], std::uint8_t x, std::uint8_t both) {
    lssrom[x] = both;
}

static void inst(std::uint8_t seq, std::uint8_t inst) {
    const std::uint8_t next_seq((inst & 0xF0u) >> 4);
    const bool stdseq((seq == 0x0Fu && next_seq == 0x00u) || next_seq == seq+1);
    const std::uint8_t cmd(inst & 0x0Fu);

    printf("\x1b[31m");
    if (cmd & 8u) {
        switch (cmd & 3u) {
        case 3:
            printf("LDR");
            break;
        case 2:
            printf("SRP");
            break;
        case 1:
            if ((cmd & 4u) >> 2) {
                printf("SL1");
            } else {
                printf("SL0");
            }
            break;
        default:
            printf("\x1b[0m...");
        }
    } else {
        printf("CLR");
    }
    printf("\x1b[0m");

    if (stdseq) {
        printf("  ");
    } else {
        printf(":\x1b[34m%X\x1b[0m", next_seq);
    }
    printf(" ");
}

static void showua2seq(std::uint8_t lssrom[], std::uint8_t seq) {
    const std::uint8_t s(seq >> 4);
    printf("%1X: | ", s);
    inst(s,lssrom[seq|0x9]);
    inst(s,lssrom[seq|0xB]);
    inst(s,lssrom[seq|0xD]);
    inst(s,lssrom[seq|0xF]);
    printf("| ");
    inst(s,lssrom[seq|0x8]);
    inst(s,lssrom[seq|0xA]);
    inst(s,lssrom[seq|0xC]);
    inst(s,lssrom[seq|0xE]);
    printf("| ");
    inst(s,lssrom[seq|0x1]);
    inst(s,lssrom[seq|0x0]);
    inst(s,lssrom[seq|0x3]);
    inst(s,lssrom[seq|0x2]);
    printf("| ");
    inst(s,lssrom[seq|0x5]);
    inst(s,lssrom[seq|0x4]);
    inst(s,lssrom[seq|0x7]);
    inst(s,lssrom[seq|0x6]);
    printf("\n");
    if (s == 7) {
        printf("   +-------------------------+-------------------------+-------------------------+------------------------\n");
    }
}

LSS::LSS(bool use13SectorDos32LSS):
    use13Sector(use13SectorDos32LSS) {
    /*
     * LSS P6 ROM is stored here with different addressing bits
     * than in the original hardware, just for ease of understanding.
     *
     * Addressing bits:
     * S3 S2 S1 S0: 4-bit sequence number, 0x00-0x0F
     * P: read pulse (from disk) indicator (1=pulse, 0=no pulse)
     * QA: MSB of controller card data register
     * Q7: READ/WRITE ($C08E/F) soft switch
     * Q6: SHIFT/LOAD ($C08C/D) soft switch
     *
     * Original hardware addressing: S3 S2 S0  P Q7 Q6 QA S1
     *
     * Addressing used here:         S3 S2 S1 S0 Q7 Q6 QA  P
     */

    // First fill in some reasonably global default values
    for (std::uint8_t s(0u); s < 0x10u; ++s) {
        std::uint8_t seq(s << 4u);
        for (std::uint8_t adr(0u); adr < 0x10u; ++adr) {
            lssrom[seq|adr] = seq+0x18u;
            if ((adr & 0xCu) == 4u) {
                lssrom[seq|adr] = 0x0Au;
            }
            if (adr == 1u || adr == 3u) {
                lssrom[seq|adr] = 0xD8u;
            }
        }
    }

    setcmd(lssrom,0x10u,0x0Du);
    setbth(lssrom,0x90u,0x29u);
    setcmd(lssrom,0xA0u,0x0Du);
    setbth(lssrom,0xB0u,0x59u);
    setcmd(lssrom,0xC0u,0x09u);
    setseq(lssrom,0xD0u,0x00u);
    setcmd(lssrom,0xE0u,0x0Du);
    setbth(lssrom,0xF0u,0x4Du);

    setseq(lssrom,0x01u,0x10u);
    setbth(lssrom,0x11u,0x2Du);
    setbth(lssrom,0xA1u,0xCDu);
    setcmd(lssrom,0xB1u,0x09u);
    setcmd(lssrom,0xC1u,0x09u);
    setbth(lssrom,0xE1u,0xFDu);
    setcmd(lssrom,0xF1u,0x0Du);

    setseq(lssrom,0x12u,0x30u);
    setseq(lssrom,0x22u,0x20u);
    setbth(lssrom,0xC2u,0xA0u);
    setbth(lssrom,0xF2u,0xE0u);

    setseq(lssrom,0x03u,0x10u);
    setseq(lssrom,0x13u,0x30u);
    setseq(lssrom,0x23u,0x00u);
    setseq(lssrom,0x33u,0x40u);
    setseq(lssrom,0xD3u,0xE0u);
    setseq(lssrom,0xE3u,0xF0u);
    setbth(lssrom,0xF3u,0xE0u);



    setcmd(lssrom,0x28u,0x09u);
    setseq(lssrom,0x78u,0x00u);
    setcmd(lssrom,0xA8u,0x09u);
    setseq(lssrom,0xF8u,0x80u);

    setcmd(lssrom,0x29u,0x09u);
    setseq(lssrom,0x79u,0x00u);
    setcmd(lssrom,0xA9u,0x09u);
    setseq(lssrom,0xF9u,0x80u);

    setcmd(lssrom,0x2Au,0x09u);
    setcmd(lssrom,0xAAu,0x09u);

    setcmd(lssrom,0x2Bu,0x09u);
    setcmd(lssrom,0xABu,0x09u);

    setcmd(lssrom,0x2Cu,0x0Bu);
    setseq(lssrom,0x7Cu,0x00u);
    setcmd(lssrom,0xACu,0x0Bu);
    setseq(lssrom,0xFCu,0x80u);

    setcmd(lssrom,0x2Du,0x0Bu);
    setseq(lssrom,0x7Du,0x00u);
    setcmd(lssrom,0xADu,0x0Bu);
    setseq(lssrom,0xFDu,0x80u);

    setcmd(lssrom,0x2Eu,0x0Bu);
    setcmd(lssrom,0xAEu,0x0Bu);

    setcmd(lssrom,0x2Fu,0x0Bu);
    setcmd(lssrom,0xAFu,0x0Bu);



    // build 13-sector LSS based on 16-sector LSS
    std::copy(std::begin(lssrom), std::end(lssrom), std::begin(lss13rom));

    setcmd(lss13rom,0x10u,0x08u);
    setseq(lss13rom,0x90u,0x00u);
    setseq(lss13rom,0xB0u,0x30u);
    setcmd(lss13rom,0xD0u,0x0Du);

    setseq(lss13rom,0x01u,0xD0u);
    setbth(lss13rom,0x11u,0xD8u);
    setbth(lss13rom,0xD1u,0x1Du);

    setseq(lss13rom,0x02u,0x00u);
    setseq(lss13rom,0x12u,0x20u);
    setseq(lss13rom,0x22u,0x30u);

    setseq(lss13rom,0x13u,0x20u);
    setseq(lss13rom,0x23u,0x30u);
    setseq(lss13rom,0x33u,0xD0u);

    if (use13Sector) {
        for (unsigned int seq = 0; seq < 0x100u; seq += 0x10u) {
            showua2seq(lss13rom,seq);
        }
    } else {
        for (unsigned int seq = 0; seq < 0x100u; seq += 0x10u) {
            showua2seq(lssrom,seq);
        }
    }
}

LSS::~LSS() {
}
