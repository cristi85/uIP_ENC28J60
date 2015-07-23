#include "types.h"

void macphy_init(u8 *macaddr);
void macphy_startrx(void);
void macphy_stoprx(void);
u8 macphy_pktcnt(void);
void macphy_reset(void);
void macphy_setbank(u8 bank);
u8 macphy_eth_readreg(u8 reg);
u8 macphy_mac_readreg(u8 reg);
void macphy_readback(void);
void macphy_readpkt(void);
void macphy_sendpkt(void);
void macphy_writereg(u8 reg, u8 val);
u8 macphy_readreg(u8 reg);
void macphy_setbit(u8 reg, u8 val);
void macphy_clearbit(u8 reg, u8 val);
u8 macphy_eth_readreg(u8 reg);
