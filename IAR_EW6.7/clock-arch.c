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
//=        Filename: clock-arch.c
//=
//=        Compiler: Rowley CrossWorks C compiler
//=
//=        Hardware: MAXQ2000 Evaluation Kit (Rev B)
//=
//===========================================================================

/* Clock hardware-specific implementation on the MAXQ2000 using the RTC */

#include "uip/clock.h"
#include "clock-arch.h"
#include "maxq2000.h"
#include "types.h"

void clock_init(void)
{
  /* Disable the clock in case it's already started */
  RCNT = 0x8000;

  /* Turn off 32kHz disable bit */
  RCNT = 0x0000;
  
  /* Turn on Write Enable bit -- must be done separately */
  RCNT = 0x8000;

  /* Wait for the BUSY bit to clear */
  while (RCNT & 0x0008);

  /* Clear all counters */
  RTSS = 0x00;

  /* Wait for the BUSY bit to clear */
  while (RCNT & 0x0008);
  RTSL = 0x0000;

  /* Wait for the BUSY bit to clear */
  while (RCNT & 0x0008);
  RTSH = 0x0000;

  /* Wait for the BUSY bit to clear */
  while (RCNT & 0x0008);

  /* Start the clock by setting Real Time Clock Enable bit */
  RCNT = 0x8001;
}

u32 clock_time(void)
{
  u32 seconds;

  /* Wait for the Ready bit to go low, signifying the start of an update */
  while (RCNT & 0x0010);

  /* Now wait for it to go high */
  while (!(RCNT & 0x0010));

  /* Read the values */
  seconds = (u32)(RTSH & 0x0fff) << 16;
  // seconds <<= 16;
  seconds += (RTSL << 4);
  seconds += (RTSS >> 4);

  return seconds;
}
