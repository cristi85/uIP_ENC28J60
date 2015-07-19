
/* Clock hardware-specific implementation on the MAXQ2000 using the RTC */

#ifndef __CLOCK_ARCH_H__
#define __CLOCK_ARCH_H__

typedef unsigned long clock_time_t;

/* The RTC has a sub-second counter as well, but 32 bits of seconds plus  */
/*  the 8 bits of RTSS would have to be stored as a 64-bit entity         */

/*  For now, accept the loss of resolution and hope it doesn't bite us in */
/*  the ass later.                                                        */

#define CLOCK_CONF_SECOND 16

#endif /* __CLOCK_ARCH_H__ */
