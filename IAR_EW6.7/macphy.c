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
//=        Filename: macphy.c
//=
//=        Compiler: Rowley CrossWorks C compiler
//=
//=        Hardware: MAXQ2000 Evaluation Kit (Rev B)
//=
//===========================================================================

#include "macphy.h"
#include "macphy_priv.h"
#include "types.h"
#include "serial.h"

/* Packet buffer for uip */
/* MAX_ETH_FRAMELEN for ethernet frame, 6 bytes of status, 2 bytes of alignment+uip extras */
unsigned char pkt_buf[MAX_ETH_FRAMELEN + 6 + 2];

/* Stuff for uIP. Don't want to sully ourselves with the entire uip.h */
#define UIP_IPH_LEN    20    /* Size of IP header */
#define UIP_UDPH_LEN    8    /* Size of UDP header */
#define UIP_TCPH_LEN   20    /* Size of TCP header */
#define UIP_IPUDPH_LEN (UIP_UDPH_LEN + UIP_IPH_LEN)    /* Size of IP +
							  UDP
							  header */
#define UIP_IPTCPH_LEN (UIP_TCPH_LEN + UIP_IPH_LEN)    /* Size of IP +
							  TCP
							  header */
#define UIP_TCPIP_HLEN UIP_IPTCPH_LEN
#define UIP_LLH_LEN     14 /* Ethernet */

extern uint16_t uip_len;
extern void *uip_appdata;
extern unsigned char *uip_buf = &pkt_buf[6];

extern u8 spi_txrx (u8);
extern void spi_ss(unsigned char);

u8 rev_ID;

/*
 * Plan of attack: 
 *
 * RX buffer is first 4k of chip (0x0000 - 0x0fff)
 *
 * TX frames may live anywhere in 0x1000 - 0x1fff
 * 
 */

void macphy_init(unsigned char *macaddr)
{
  unsigned char x;
  
  /* Print out the revision ID of this chip */
  macphy_setbank(3);
  rev_ID = macphy_eth_readreg(EREVID);
  Serial_PrintString("ENC28J60 chip revision: ");
  Serial_PrintByte(rev_ID, TRUE);
  Serial_PrintString("\r\n");

  /* Set bank 0 */
  macphy_setbank(0);

  /* Set the receive buffer pointers */
  macphy_writereg(ERXSTL, 0x00);
  macphy_writereg(ERXSTH, 0x00);

  macphy_writereg(ERXNDL, 0xff);
  macphy_writereg(ERXNDH, 0x0f);

  /* Set the RX RD pointer to an odd address to work around B5 errata #11 */
  macphy_writereg(ERXRDPTL, 0xff);
  macphy_writereg(ERXRDPTH, 0x0f);

  /* Set the initial SPI read pointer location */
  macphy_writereg(ERDPTL, 0x00);
  macphy_writereg(ERDPTH, 0x00);

  /* Set the MAC TX start pointer */
  macphy_writereg(ETXSTL, 0x00);
  macphy_writereg(ETXSTH, 0x10);

  /* Set ECON2.7 to automatically increment read pointer in circular buffer */
  macphy_setbit(ECON2, AUTOINC);

  /* Select receive filters for my MAC address and broadcast*/
  /* ERXFCON lives in bank 1 */
  macphy_setbank(1);

  macphy_writereg(ERXFCON, UCEN | CRCEN | BCEN);

  /* Wait for oscillator ready, but don't hang here! */
  x = 0x00;
  while ((x & 0x01) != 0) {
    x = macphy_eth_readreg(ESTAT);
  }

  /* MAC Init */

  /* Enable MAC to receive frames (MACON1.0) */
  macphy_setbank(2);
  
  macphy_writereg(MACON1, TXPAUS | RXPAUS | MARXEN);

  /* Set up padding, CRC, and duplex (MACON3) */
  /* Pad to 60-bytes, append valid CRC, FRMLNEN */
  macphy_writereg(MACON3, PADCFG0 | TXCRCEN  | FRMLNEN );

  /* Configure backoff/deferral settings (MACON4) */
  /* We accept reset defaults */

  /* Set maximum ethernet frame length of 800 (MAMXFL) */
  macphy_writereg(MAMXFLL, MAX_ETH_FRAMELEN & 0xff);
  macphy_writereg(MAMXFLH, MAX_ETH_FRAMELEN >> 8);

  /* Configure inter-packet gap lengths (MABBIPG, MAIPGL, MAIPGH) */
  macphy_writereg(MABBIPG, 0x12);

  macphy_writereg(MAIPGL, 0x12);
  macphy_writereg(MAIPGH, 0x0c);

  /* Retransmission and Collision windows */
  /* We accept reset defaults */

  /* Configure MAC address */
  macphy_setbank(3);

  macphy_writereg(MAADR1, macaddr[0]);
  macphy_writereg(MAADR2, macaddr[1]);
  macphy_writereg(MAADR3, macaddr[2]);
  macphy_writereg(MAADR4, macaddr[3]);
  macphy_writereg(MAADR5, macaddr[4]);
  macphy_writereg(MAADR6, macaddr[5]);

  /* PHY Init */
  macphy_setbank(2);

  /* Set PHCON1 subregister for half duplex. */
  macphy_writereg(MIREGADR, PHCON1); /* MIREGADR write */
  macphy_writereg(MIWRL, 0x00); /* MIWRL write */
  macphy_writereg(MIWRH, 0x00); /* MIWRH write */

  /* Set PHCON2 subregister for half-duplex loopback inhibit */
  /*  (ie. don't receive copies of packets we transmit)      */
  macphy_writereg(MIREGADR, PHCON2); /* MIREGADR write */
  macphy_writereg(MIWRL, 0x00); /* MIWRL write */
  macphy_writereg(MIWRH, 0x01); /* MIWRH write */

  /* Set up LEDs for link status on A and rx/tx on B */
  macphy_writereg(MIREGADR, PHLCON); /* MIREGADR write */
  macphy_writereg(MIWRL, 0x72); /* MIWRL write */
  macphy_writereg(MIWRH, 0x34); /* MIWRH write */

}

void macphy_startrx(void)
{
  /* Enable writing of received packets into receive buffer */

  macphy_setbit(ECON1, RXEN);
}

void macphy_stoprx(void)
{
  /* Disable writing of received packets into receive buffer */

  macphy_clearbit(ECON1, RXEN);
}

void macphy_pktdec(void)
{
  /* Set the ECON2.PKTDEC bit to indicate we've processed a packet */

  macphy_setbit(ECON2, PKTDEC);
}

u8 macphy_pktcnt(void)
{
  u32 cnt;

  macphy_setbank(1);

  /* Read the PKTCNT register and return contents */
  cnt = macphy_eth_readreg(EPKTCNT);

  return cnt;
}

void macphy_setbit(u8 reg, u8 val)
{
  /* CAUTION: You must set the proper bank before calling this routine */

  spi_ss(0);
  spi_txrx(CMD_BFS(reg));
  spi_txrx(val);
  spi_ss(1);
}

void macphy_clearbit(u8 reg, u8 val)
{
  /* CAUTION: You must set the proper bank before calling this routine */

  spi_ss(0);
  spi_txrx(CMD_BFC(reg));
  spi_txrx(val);
  spi_ss(1);
}

void macphy_writereg(u8 reg, u8 val)
{  
  /* CAUTION: You must set the proper bank before calling this routine */

  spi_ss(0);
  spi_txrx(CMD_WCR(reg));
  spi_txrx(val);
  spi_ss(1);
}

u8 macphy_eth_readreg(u8 reg)
{
  u8 x;

  /* CAUTION: You must set the proper bank before calling this routine */

  spi_ss(0);
  spi_txrx(CMD_RCR(reg));
  x = spi_txrx(0xff);
  spi_ss(1);

  return x;
}

u8 macphy_mac_readreg(u8 reg)
{
  u8 x;

  /* CAUTION: You must set the proper bank before calling this routine */

  spi_ss(0);
  spi_txrx(CMD_RCR(reg));
  spi_txrx(0xff); /* First byte out is dummy */
  x = spi_txrx(0xff);
  spi_ss(1);

  return x;
}

void macphy_reset(void)
{

  spi_ss(0);
  spi_txrx(0xff); /* MAC/PHY soft reset */
  spi_ss(1);

}

void macphy_readback(void)
{
  /* Only cprintf function was used */
 
}

/* Sets the bank bits of the ENC28J60. Valid values are 0-3 */
void macphy_setbank(u8 bank)
{

  switch (bank) {
    case 0:  /* Clear the lower two bits */
      macphy_clearbit(ECON1, BSEL0|BSEL1);
      break;
    case 1:  /* Set bit 0, clear bit 1 */
      macphy_setbit(ECON1, BSEL0);
      macphy_clearbit(ECON1, BSEL1);
      break;
    case 2: /* Clear bit 0, set bit 1 */
      macphy_clearbit(ECON1, BSEL0);
      macphy_setbit(ECON1, BSEL1);
      break;
    case 3: /* Set the lower two bits */
      macphy_setbit(ECON1, BSEL0|BSEL1);
      break;
    default:
      /* Ignore */
      break;
  }
  
}

/* Reads packet from the ENC28J60 and decrements pktcnt */
void macphy_readpkt(void)
{
  u8 rlo, rhi;
  u32 i, plen, nxtpkt, rptr;

  macphy_setbank(0);
  /* Retrieve the rx pointer location */
  rlo = macphy_eth_readreg(ERDPTL);
  rhi = macphy_eth_readreg(ERDPTH);
  
  rptr = (rhi << 8) + rlo;

  spi_ss(0);
  spi_txrx(CMD_RBM);
  pkt_buf[0] = spi_txrx(0xff); /* Next packet pointer LSB */
  pkt_buf[1] = spi_txrx(0xff); /* Next packet pointer MSB */

  pkt_buf[2] = spi_txrx(0xff); /* status[7:0] */
  pkt_buf[3] = spi_txrx(0xff); /* status[15:8] */
  pkt_buf[4] = spi_txrx(0xff); /* status[23:16] */
  pkt_buf[5] = spi_txrx(0xff); /* status[31:24] */
  /* Calculate the remaining length of this packet */
  nxtpkt = plen = (pkt_buf[1] << 8) + pkt_buf[0];
  if (nxtpkt < rptr) {
    /* Wrapping case */
    /* Explanation: RX buffer ends at byte 0xfff, so bytes between rptr and 0x1000 are */
    /* part of the packet, plus the stuff that wrapped around to nxtpkt. Since plen holds */
    /* nxtpkt already, we just need to add in the bytes before the wrap */
    plen += (0x1000 - rptr);
  } else {
    /* Normal case */
    plen -= rptr;
  }

  if (plen > (MAX_ETH_FRAMELEN + 6 + 1)) {
    //cprintf(C" - !! - PACKET LENGTH CORRUPTION - !! -\r\n");
    while (1);
  }
  /* Could be off by one here due to the word alignment. If so, go ahead and accept. */
  if ((plen != ((pkt_buf[3] << 8) + pkt_buf[2] + 6)) &&
      (plen != ((pkt_buf[3] << 8) + pkt_buf[2] + 7))) {
    //cprintf(C" - !! - PACKET LENGTH MISMATCH - !! -\r\n");
    //cprintf(C" Status Vector said 0x%02x%02x (+6/+7), plen = 0x%04x\r\n", pkt_buf[3], pkt_buf[2], plen);
    //cprintf(C" Next packet pointer = 0x%02x%02x\r\n", pkt_buf[1], pkt_buf[0]);
    //cprintf(C" SPI Read Pointer = 0x%04x\r\n", rptr);
    macphy_readback();
  }
  /* Ethernet frame follows here */
  for (i = 6; i < plen; i++) {
    pkt_buf[i] = spi_txrx(0xff);
  }
  spi_ss(1);

  /* Free up space by moving ERXRDPT forward and setting the PKTDEC bit */
  if (nxtpkt > 0xfff) {
    //cprintf(C" - !! - NEXT PACKET POINTER CORRUPTION - !! -\r\n");
    while (1);
  }
  nxtpkt -= 1;
  if (nxtpkt > 0xfff) {
    /* Wrap around. Fix up. */
    nxtpkt = 0xfff;
  }

  macphy_writereg(ERXRDPTL, nxtpkt & 0xff); 
  macphy_writereg(ERXRDPTH, nxtpkt >> 8);
  macphy_pktdec();
}


/* Transfers packet to ENC28J60 and initiates packet transmission */
void macphy_sendpkt(void)
{
  u16 i;
  u8 x;
  u8 rlo, rhi;
  u8 status[7];
  //u8* ptr_uip_appdata;
  //ptr_uip_appdata = (u8 *)(uip_appdata);

  /* Everything we need is in bank 0 */
  macphy_setbank(0);

  /* Retrieve the rx pointer location -- set it back after we're done */
  rlo = macphy_eth_readreg(ERDPTL);
  rhi = macphy_eth_readreg(ERDPTH);

  macphy_clearbit(ESTAT, TXABRT);
  macphy_clearbit(ESTAT, LATECOL);

  /* This code catches a potential silicon errata with the ENC28J60 */
  x = macphy_eth_readreg(ECON1);
  while (x & TXRTS) {
    x = macphy_eth_readreg(EIR);
    if (x & TXERIF) {
      /* Chip is stuck. Reset */
      //cprintf(C" -> MAC TX is stuck. Reset. \r\n");
      Serial_PrintString("MAC TX is stuck. Reset. \r\n");
      macphy_setbit(ECON1, TXRST);
      macphy_clearbit(ECON1, TXRST);
      macphy_clearbit(EIR, TXERIF);
    }
    x = macphy_eth_readreg(ECON1);
  }

  /* Set the SPI write buffer pointer location */
  macphy_writereg(EWRPTL, 0x00);
  macphy_writereg(EWRPTH, 0x10);
  
  //Serial_PrintString("macphy_sendpkt() uip_len=");
  //Serial_PrintNumber(uip_len);
  //Serial_PrintString("\r\n");
  /* Write the link-layer header to the chip */
  /* SPI Write Pointer Start (ETXSTL) already set by macphy_init() */
  spi_ss(0);
  spi_txrx(CMD_WBM);
  spi_txrx(0x00); /* "options" byte header needed by the ENC chip */

  for (i = 0; i < UIP_LLH_LEN; i++) {
    /* uip_buf starts at offset 6, first 6 bytes are from the old RX packet */
    spi_txrx(pkt_buf[6+i]);
  }
  if (uip_len <= UIP_LLH_LEN + UIP_TCPIP_HLEN) {
    /* Probably an ARP update or somesuch. No need to send appdata */
    for (i = 0; i < uip_len - UIP_LLH_LEN; i++) {
      spi_txrx(pkt_buf[6+UIP_LLH_LEN+i]);
    }
  } else {
    /* We have link-layer header, IP header, and appdata */
    for (i = 0; i < UIP_TCPIP_HLEN; i++) {
      /* IP header */
      spi_txrx(pkt_buf[6+UIP_LLH_LEN+i]);
    }
    for (i = 0; i < uip_len - UIP_TCPIP_HLEN - UIP_LLH_LEN; i++) {
      spi_txrx(((u8 *)uip_appdata)[i]);
      //spi_txrx(*ptr_uip_appdata);
      //ptr_uip_appdata++;
    }
  }
  spi_ss(1);

  /* Set the MAC TX end pointer */
  macphy_writereg(ETXNDL, (0x1000 + uip_len) & 0x00ff);
  macphy_writereg(ETXNDH, (0x1000 + uip_len) >> 8);

  /* Fire and forget! */
  macphy_setbit(ECON1, TXRTS);

  /* Just in case, clear the transmit abort and late collision flags */
  macphy_clearbit(ESTAT, TXABRT);
  macphy_clearbit(ESTAT, LATECOL);

}
