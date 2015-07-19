//===========================================================================
//=
//=  Copyright (C) 2008 Maxim Integrated Products
//=  All rights Reserved. Printed in U.S.A.
//=
//=  Permission is hereby granted, free of charge, to any person obtaining a
//=  copy of this software and associated documentation files (the 
//=  "Software"), to deal in the Software without restriction, including
//=  without limitation the rights to use, copy, modify, merge, publish,
//=  distribute, sublicense, and/or sell copies of the Software, and to
//=  permit persons to whom the Software is furnished to do so, subject to
//=  the following conditions:
//=  
//=  The above copyright notice and this permission notice shall be included
//=  in all copies or substantial portions of the Software source code.
//=  
//=  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//=  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//=  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//=  IN NO EVENT SHALL MAXIM INTEGRATED PRODUCTS BE LIABLE FOR ANY CLAIM, 
//=  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
//=  OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR 
//=  THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//=
//=  Except as contained in this notice, the name of Maxim Integrated Products
//=  shall not be used except as stated in the Maxim Branding Policy.
//=
//=     Description: MAXQ2000 uIP Ethernet Example Application
//=                 
//=        Filename: macphy_priv.h
//=
//=        Compiler: Rowley CrossWorks C compiler
//=
//=        Hardware: MAXQ2000 Evaluation Kit (Rev B)
//=
//===========================================================================

#define MAX_ETH_FRAMELEN 500 /* bytes */

/* Commands sent as the first byte after slave select */
#define CMD_RCR(x) (x)
#define CMD_RBM 0x3a
#define CMD_WCR(x) (0x40 | x)
#define CMD_WBM 0x7a
#define CMD_BFS(x) (0x80 | x)
#define CMD_BFC(x) (0xa0 | x)

/* Bank 0 Registers */
#define ERDPTL    0x00
#define ERDPTH    0x01
#define EWRPTL    0x02
#define EWRPTH    0x03
#define ETXSTL    0x04
#define ETXSTH    0x05
#define ETXNDL    0x06
#define ETXNDH    0x07
#define ERXSTL    0x08
#define ERXSTH    0x09
#define ERXNDL    0x0a
#define ERXNDH    0x0b
#define ERXRDPTL  0x0c
#define ERXRDPTH  0x0d
#define ERXWRPTL  0x0e
#define ERXWRPTH  0x0f
#define EDMASTL   0x10
#define EDMASTH   0x11
#define EDMANDL   0x12
#define EDMANDH   0x13
#define EDMADSTL  0x14
#define EDMADSTH  0x15
#define EDMACSL   0x16
#define EDMACSH   0x17

/* Bank 1 Registers */
#define EHT0      0x00
#define EHT1      0x01
#define EHT2      0x02
#define EHT3      0x03
#define EHT4      0x04
#define EHT5      0x05
#define EHT6      0x06
#define EHT7      0x07
#define EPMM0     0x08
#define EPMM1     0x09
#define EPMM2     0x0a
#define EPMM3     0x0b
#define EPMM4     0x0c
#define EPMM5     0x0d
#define EPMM6     0x0e
#define EPMM7     0x0f
#define EPMCSL    0x10
#define EPMCSH    0x11
#define EPMOL     0x14
#define EPMOH     0x15
#define ERXFCON   0x18
#define EPKTCNT   0x19

/* Bank 2 Registers */
#define MACON1    0x00
#define MACON3    0x02
#define MACON4    0x03
#define MABBIPG   0x04
#define MAIPGL    0x06
#define MAIPGH    0x07
#define MACLCON1  0x08
#define MACLCON2  0x09
#define MAMXFLL   0x0a
#define MAMXFLH   0x0b
#define MICMD     0x12
#define MIREGADR  0x14
#define MIWRL     0x16
#define MIWRH     0x17
#define MIRDL     0x18
#define MIRDH     0x19

/* Bank 3 Registers */
#define MAADR5    0x00
#define MAADR6    0x01
#define MAADR3    0x02
#define MAADR4    0x03
#define MAADR1    0x04
#define MAADR2    0x05
#define EBSTSD    0x06
#define EBSTCON   0x07
#define EBSTCSL   0x08
#define EBSTCSH   0x09
#define MISTAT    0x0a
#define EREVID    0x12
#define ECOCON    0x15
#define EFLOCON   0x17
#define EPAUSL    0x18
#define EPAUSH    0x19

/* Registers which exist in all banks */
#define EIE       0x1b
#define EIR       0x1c
#define ESTAT     0x1d
#define ECON2     0x1e
#define ECON1     0x1f

/* PHY subregisters */
#define PHCON1    0x00
#define PHSTAT1   0x01
#define PHID1     0x02
#define PHID2     0x03
#define PHCON2    0x10
#define PHSTAT2   0x11
#define PHIE      0x12
#define PHIR      0x13
#define PHLCON    0x14

/* Some commonly-used bitfields */
/* ESTAT */
#define CLKRDY    0x01
#define TXABRT    0x02
#define RXBUSY    0x04
#define LATECOL   0x10
#define BUFFER    0x40
#define INT       0x80

/* ECON1 */
#define TXRST     0x80
#define RXRST     0x40
#define DMAST     0x20
#define CSUMEN    0x10
#define TXRTS     0x08
#define RXEN      0x04
#define BSEL1     0x02
#define BSEL0     0x01

/* ECON2 */
#define AUTOINC   0x80
#define PKTDEC    0x40
#define PWRSV     0x20
#define VRPS      0x08

/* ERXFCON */
#define UCEN      0x80
#define ANDOR     0x40
#define CRCEN     0x20
#define PMEN      0x10
#define MPEN      0x08
#define HTEN      0x04
#define MCEN      0x02
#define BCEN      0x01

/* MACON1 */
#define TXPAUS    0x08
#define RXPAUS    0x04
#define PASSALL   0x02
#define MARXEN    0x01

/* MACON3 */
#define PADCFG2   0x80
#define PADCFG1   0x40
#define PADCFG0   0x20
#define TXCRCEN   0x10
#define PHDREN    0x08
#define HFRMEN    0x04
#define FRMLNEN   0x02
#define FULDPX    0x01

/* EIR */
#define PKTIF     0x40
#define DMAIF     0x20
#define LINKIF    0x10
#define TXIF      0x08
#define TXERIF    0x02
#define RXERIF    0x01
