void macphy_init(unsigned char *macaddr);
void macphy_startrx(void);
void macphy_stoprx(void);
unsigned int macphy_pktcnt(void);
void macphy_reset(void);
void macphy_setbank(unsigned int bank);
unsigned int macphy_eth_readreg(unsigned int reg);
unsigned int macphy_mac_readreg(unsigned int reg);
void macphy_readback(void);
void macphy_readpkt(void);
void macphy_sendpkt(void);
void macphy_writereg(unsigned int reg, unsigned int val);
unsigned int macphy_readreg(unsigned int reg);
void macphy_setbit(unsigned int reg, unsigned int val);
void macphy_clearbit(unsigned int reg, unsigned int val);
unsigned int macphy_eth_readreg(unsigned int reg);