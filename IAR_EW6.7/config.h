#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "types.h"

#define ADC_Scan_Channels  (u8)2

extern void Config();

extern void Config_UART1(void);
extern void Config_TIM1(void);
extern void Config_TIM2(void);
extern void Config_TIM3(void);
extern void Config_GPIO(void);
extern void Config_TIM14(void);
extern void Config_TIM15(void);
extern void Config_COMP1(void);
extern void Config_ADC1_DMA(void);
extern void Config_SPI1(void);
extern void Config_SPI2(void);

#endif