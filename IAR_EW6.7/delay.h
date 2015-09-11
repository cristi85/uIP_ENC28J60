#ifndef _DELAY_H_
#define _DELAY_H_

#include "board.h"
#include "types.h"

#define I2C_DELAY      (u8)2 
#define SCL_SDA_DELAY  (u8)2  


/* Name:       Delay_us - Macro to delay multiple of 1us 
   Param:      us - size=u16
   Dependency: uses TIM14 that has to be configured to overflow at 1us 
*/
#define DELAY_US(us) TIM14->ARR = us-1;                    \
                     TIM14->CNT = 0;                       \
                     TIM14->SR &= ~TIM_SR_UIF;             \
                     while((TIM14->SR & TIM_SR_UIF)==0){}  \
                     TIM14->SR &= ~TIM_SR_UIF;  

//Uses TIM15, configure 10us overflow clock must be activated before
void delay_10us(u16);

#endif