#include "board.h"
#include "types.h"
#include "config.h"
#include "errors.h"
#include "timeout.h"
#include "stm32f0xx_it.h"

#define SPI_DR16     *(u16*)0x4000380C
#define SPI_DR8      *(u8*)0x4000380C
#define CS_HIGH()    (GPIOB->BSRR = GPIO_Pin_12)
#define CS_LOW()     (GPIOB->BRR  = GPIO_Pin_12)

typedef u32 clock_time_t;

void Config_UART1(void);
void Config_TIM1(void);
void Config_TIM2(void);
void Config_TIM2_cyclic_int(void);
void Config_TIM3(void);
void Config_TIM6(void);
void Config_GPIO(void);
void Config_TIM14(void);
void Config_TIM15(void);
void Config_COMP1(void);
void Config_ADC1_DMA(void);
void Config_SPI1(void);
void Config_SPI2(void);
void Config_DAC_DMA(void);

void Config()
{
  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  Config_GPIO();
  Config_UART1();
  //Config_TIM1();   /* Configure TIM1_CH1 as PWM output on PA8 (PWM1 Output) */
  //Config_TIM2();   /* Configure TIM2_CH4 as PWM output on PA3 (PWM2 Output) */
  Config_TIM2_cyclic_int();  /* Configure TIM2 to generate periodic INT @ 1 second */
  Config_TIM3();     /* Periodic 1ms interrupt */
  Config_TIM6();     /* Periodic DMA->DAC triggering */
  Config_TIM14();
  Config_TIM15();    /* for delay_10us */
  //Config_ADC1_DMA();
  //Config_SPI1();
  Config_SPI2();
  //Config_DAC_DMA();
}
void Config_ADC1_DMA()
{
  ADC_InitTypeDef ADC_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* ADC1 DeInit */  
  ADC_DeInit(ADC1);
  
  /* ADC1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  
  /* DMA1 clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);
  
  /* DMA1 Channel1 Config */
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)0x00000000;
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)0x00000000;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 2;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  
  /* DMA1 Channel1 enable */
  DMA_Cmd(DMA1_Channel1, ENABLE);
  
  /* Enable the DMA channel 1 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable Transfer Complete IT notification for DMA1 Channel 1 */
  DMA_ITConfig(DMA1_Channel1, DMA1_IT_TC1, ENABLE);
  
  /* ADC DMA request in circular mode */
  ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);
  
  /* Enable ADC_DMA */
  ADC_DMACmd(ADC1, ENABLE);  
  
  /* Initialize ADC structure */
  ADC_StructInit(&ADC_InitStructure);
  
  /* Configure the ADC1 in continous mode withe a resolutuion equal to 12 bits  */
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Backward;
  ADC_Init(ADC1, &ADC_InitStructure); 
  
  /* Convert the ADC1 Vref  with 55.5 Cycles as sampling time */ 
  ADC_ChannelConfig(ADC1, ADC_Channel_Vrefint, ADC_SampleTime_55_5Cycles); 
  ADC_VrefintCmd(ENABLE);
  /* Convert the ADC1 ADC_Channel_5  with 239.5 Cycles as sampling time */ 
  ADC_ChannelConfig(ADC1, ADC_Channel_5, ADC_SampleTime_239_5Cycles);
  
  /* ADC Calibration */
  ADC_GetCalibrationFactor(ADC1);
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);     
  
  /* Wait the ADCEN flag */
  Timeout_SetTimeout1(50);   //Set timeout1 to 50ms
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN) && !Timeout_IsTimeout1()); 
  if(Timeout_IsTimeout1()) Errors_SetError(ERROR_ADC_INIT);
  else Errors_ResetError(ERROR_ADC_INIT);
  
  /* ADC1 regular Software Start Conv */ 
  ADC_StartOfConversion(ADC1);
}

void Config_GPIO()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Enable or disable the AHB peripheral clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
  
  /* BTN1 - digital INPUT */
  GPIO_InitStructure.GPIO_Pin = BTN1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(BTN_PORT, &GPIO_InitStructure);
  
   /* USART - digital I/O */
  GPIO_InitStructure.GPIO_Pin =  USART_RX_PIN | USART_TX_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(USART_PORT, &GPIO_InitStructure);

  /* ======QVGA 2.2 TFT SPI====== */
  GPIO_InitStructure.GPIO_Pin = ILI9341_DC_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(ILI9341_DC_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = ILI9341_RST_PIN;
  GPIO_Init(ILI9341_RST_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = ILI9341_CS_PIN;
  //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(ILI9341_CS_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = ILI9341_MOSI_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_Init(ILI9341_MOSI_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = ILI9341_SCK_PIN;
  GPIO_Init(ILI9341_SCK_PORT, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(ILI9341_CS_PORT,   GPIO_PinSource4, GPIO_AF_0);
  GPIO_PinAFConfig(ILI9341_SCK_PORT,  GPIO_PinSource5, GPIO_AF_0);
  GPIO_PinAFConfig(ILI9341_MOSI_PORT, GPIO_PinSource7, GPIO_AF_0);
  /* ======END QVGA 2.2 TFT SPI====== */
}

/* Configure TIM1_CH1 as PWM output on PA8 (PWM1 Output) */
void Config_TIM1()
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  TIM_OCInitTypeDef TIM_OCInitStruct;
  /* TIM2 clock enable */
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM1, ENABLE);
  
  TIM_TimeBaseInitStruct.TIM_Prescaler = 4800;                       // This parameter can be a number between 0x0000 and 0xFFFF
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;       // This parameter can be a value of @ref TIM_Counter_Mode
  TIM_TimeBaseInitStruct.TIM_Period = 10000;                         // This parameter must be a number between 0x0000 and 0xFFFF, fclk=10k, 10000->T=1s
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;           // This parameter can be a value of @ref TIM_Clock_Division_CKD
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;                  // This parameter is valid only for TIM1
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);
  
  TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Disable;
  TIM_OCInitStruct.TIM_Pulse = 5000;                                 // Duty cycle (compared to TIM_Period)
  TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStruct.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;
  TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
  TIM_OC1Init(TIM1, &TIM_OCInitStruct);
  
  TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
  
  /* TIM1 enable counter */
  TIM_Cmd(TIM1, ENABLE);
  
  /* TIM1 enable output */
  //TIM_CtrlPWMOutputs(TIM1, ENABLE)
}

/* Configure TIM2_CH4 as PWM output on PA3 (PWM2 Output) */
void Config_TIM2()
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  TIM_OCInitTypeDef TIM_OCInitStruct;
  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  
  TIM_TimeBaseInitStruct.TIM_Prescaler = 48;                         // This parameter can be a number between 0x0000 and 0xFFFF
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;       // This parameter can be a value of @ref TIM_Counter_Mode
  TIM_TimeBaseInitStruct.TIM_Period = 1000000;                       // This parameter must be a number between 0x0000 and 0xFFFF, fclk=1M, 1000000->T=1s
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;           // This parameter can be a value of @ref TIM_Clock_Division_CKD
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;                  // This parameter is valid only for TIM1
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
  
  TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Disable;
  TIM_OCInitStruct.TIM_Pulse = 500000;                              // Duty cycle (compared to TIM_Period)
  TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStruct.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;
  TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
  TIM_OC4Init(TIM2, &TIM_OCInitStruct);
  
  TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
  
  /* TIM2 enable counter */
  TIM_Cmd(TIM2, ENABLE);
  
  /* TIM2 enable output */
  //TIM_CtrlPWMOutputs(TIM2, ENABLE)
}

void Config_TIM2_cyclic_int()
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  
  TIM_TimeBaseInitStruct.TIM_Prescaler = 8000;                       // This parameter can be a number between 0x0000 and 0xFFFF
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;       // This parameter can be a value of @ref TIM_Counter_Mode
  TIM_TimeBaseInitStruct.TIM_Period = 1000;                          // This parameter must be a number between 0x0000 and 0xFFFF, fclk=1M, 1000000->T=1s
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;           // This parameter can be a value of @ref TIM_Clock_Division_CKD
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;                  // This parameter is valid only for TIM1
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
  
  /* TIM2 enable counter */
  TIM_Cmd(TIM2, ENABLE);
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}

/* Periodic 1ms interrupt */
void Config_TIM3()
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  NVIC_InitTypeDef NVIC_InitStructure;
  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  
  /* Enable the TIM3 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  TIM_TimeBaseInitStruct.TIM_Prescaler = 48;                         // This parameter can be a number between 0x0000 and 0xFFFF
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;       // This parameter can be a value of @ref TIM_Counter_Mode
  TIM_TimeBaseInitStruct.TIM_Period = 1000;                          // This parameter must be a number between 0x0000 and 0xFFFF
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;           // This parameter can be a value of @ref TIM_Clock_Division_CKD
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x00;               //This parameter is valid only for TIM1
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);
  /* Enable the TIM3 Update Interrupt Request */
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
}

void Config_TIM14()
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  /* TIM14 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
  
  TIM_TimeBaseInitStruct.TIM_Prescaler = 48000;                      // This parameter can be a number between 0x0000 and 0xFFFF
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;       // This parameter can be a value of @ref TIM_Counter_Mode
  TIM_TimeBaseInitStruct.TIM_Period = 0xFFFF;                        // This parameter must be a number between 0x0000 and 0xFFFF
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;           // This parameter can be a value of @ref TIM_Clock_Division_CKD
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x00;               // This parameter is valid only for TIM1
  TIM_TimeBaseInit(TIM14, &TIM_TimeBaseInitStruct);
  /* enable counter */
  TIM_Cmd(TIM14, ENABLE);
}

void Config_TIM15()
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  /* TIM15 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE);
  
  TIM_TimeBaseInitStruct.TIM_Prescaler = 48;                         // This parameter can be a number between 0x0000 and 0xFFFF
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;       // This parameter can be a value of @ref TIM_Counter_Mode
  TIM_TimeBaseInitStruct.TIM_Period = 10;                            // This parameter must be a number between 0x0000 and 0xFFFF
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;           // This parameter can be a value of @ref TIM_Clock_Division_CKD
  TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x00;               // This parameter is valid only for TIM1
  TIM_TimeBaseInit(TIM15, &TIM_TimeBaseInitStruct);
  //TIM_Cmd(TIM15, ENABLE);
}

void Config_UART1()
{
  USART_InitTypeDef USART_InitStruct;
  NVIC_InitTypeDef NVIC_InitStructure;
  /* Enable USART1 clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  /* Enable the USART1 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Configure GPIO pin alternate function UART1 RX TX */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);
  /* =================================== */
  USART_DeInit(USART1);
  USART_InitStruct.USART_BaudRate = 115200;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Init(USART1, &USART_InitStruct);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_Cmd(USART1, ENABLE);
}
void Config_SPI1()
{
  SPI_InitTypeDef  SPI_InitStructure;
  
  /* Enable the SPI periph */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  
  /* SPI configuration -------------------------------------------------------*/
  SPI_I2S_DeInit(SPI1);
  SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);
  SPI_SSOutputCmd(SPI1, ENABLE);
  SPI_Cmd(SPI1, ENABLE);
}
void Config_SPI2()
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
          
  /* GPIO configuration */
  //PB12 NSS  - user controlled
  //PB13 SCK  - controlled by SPI hardware
  //PB14 MISO - controlled by SPI hardware
  //PB15 MOSI - controlled by SPI hardware
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;       //NSS
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;      //SCK
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;      //MISO
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;      //MOSI
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_0);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_0);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_0);
  
  /* SPI2 configuration -------------------------------------------------------*/
  /* Enable the SPI2 periph */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
  SPI_I2S_DeInit(SPI2);
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;  // CS pin controlled by user SW
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;  //128: 375kHZ, 8: 6MHz, 4: 12MHz, 2: 24MHz
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI2, &SPI_InitStructure);
  SPI_Cmd(SPI2, ENABLE);
}
void Config_DAC_DMA()
{
  /*GPIO_InitTypeDef GPIO_InitStructure;
  DAC_InitTypeDef DAC_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  //PA4 DAC Output
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;       //DAC Output
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);
  
  // DMA1 Channel1 Config
  DMA_DeInit(DMA1_Channel3);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(DAC->DHR8R1);
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)wav_buff_start;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 1024;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel3, &DMA_InitStructure);
  
  // Enable the DMA channel 1 global Interrupt
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  // Enable Transfer Complete IT and Half Transfer notification for DMA1 Channel 1
  DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);
  DMA_ITConfig(DMA1_Channel3, DMA_IT_HT, ENABLE);
  
  // DMA1 Channel1 enable
  DMA_Cmd(DMA1_Channel3, ENABLE);
  DAC_Cmd(DAC_Channel_1, ENABLE);
  DAC_DMACmd(DAC_Channel_1, ENABLE);*/
}

void Config_TIM6()  /* Periodic DAC triggering */
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  /* TIM6 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
  
  TIM_TimeBaseInitStruct.TIM_Prescaler = 0;                          // F_timer = 48MHz
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;       // UP Counter
  TIM_TimeBaseInitStruct.TIM_Period = 0xFFFF;                        // 732.433 Overflows/s
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;           // Clock Division = 1
  TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStruct);
  TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
  TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
  TIM_DMACmd(TIM6, TIM_DMA_Update, ENABLE);
  TIM_Cmd(TIM6, DISABLE);
}

/* Exchange a byte */
u8 spi_txrx (u8 dat)
{
  SPI_DR8 = dat;
  while (SPI2->SR & SPI_SR_BSY) ;
  return (u8)SPI_DR8;
}

void spi_ss(unsigned char x)
{
  if (x > 0) 
  {
    CS_HIGH();
  } 
  else 
  {
    CS_LOW();
  }
} 

clock_time_t clock_time(void)
{
  clock_time_t seconds;
  seconds = (clock_time_t)current_time_sec;
  return seconds;
} 

void memset(u8* memstart, u8 val, u8 num)
{
  u8 i;
  u8* l_memstart;
  l_memstart = (u8*)memstart;
  for(i=0; i<num; i++)
  {
    *(l_memstart) = val;
    l_memstart++;
  }
}
void memcpy(void* dest, const void* src, u8 num)
{
  u8 i;
  u8* l_dest;
  u8* l_src;
  l_dest = (u8*)dest;
  l_src = (u8*)src;
  for(i=0; i<num; i++)
  {
    *(l_dest) = *(l_src);
    l_dest++;
    l_src++;
  }
}
