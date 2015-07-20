/**
  ******************************************************************************
  * @file    TIM_Time_Base/stm32f0xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    23-March-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "types.h"
#include "stm32f0xx_it.h"
#include "config.h"

//-----------------
/** @addtogroup STM32F0_Discovery_Peripheral_Examples
  * @{
  */

/** @addtogroup TIM_Time_Base
  * @{
  */

/* Periodic Tasks */
#define CNTVAL_50MS  (u16)50
_Bool FLAG_50ms = FALSE;
u16 cnt_flag_50ms = 0;
#define CNTVAL_250MS  (u16)250
_Bool FLAG_250ms = FALSE;
u16 cnt_flag_250ms = 0;
#define CNTVAL_500MS  (u16)500
_Bool FLAG_500ms = FALSE;
u16 cnt_flag_500ms = 0;
#define CNTVAL_1000MS (u16)1000
_Bool FLAG_1000ms = FALSE;
u16 cnt_flag_1000ms = 0;
/*================*/

/* Buttons debouncing and repetition delay */
#define DIG_IN_DEB_TIME   (u8)15     /* 30ms digital input debounce time */
#define BTN_DELAY  (u8)150   //300ms
u8 btn1_0_cnt = 0;
u8 btn1_1_cnt = 0;
volatile u8 BTN1_DEB_STATE = BTN_DEPRESSED;
u8 btn2_0_cnt = 0;
u8 btn2_1_cnt = 0;
volatile u8 BTN2_DEB_STATE = BTN_DEPRESSED;
u8 btn3_0_cnt = 0;
u8 btn3_1_cnt = 0;
volatile u8 BTN3_DEB_STATE = BTN_DEPRESSED;
u8 btn4_0_cnt = 0;
u8 btn4_1_cnt = 0;
volatile u8 BTN4_DEB_STATE = BTN_DEPRESSED;

_Bool BTN1_DELAY_FLAG = FALSE;
u8 btn1_delay_cnt = 0;
u16 BTN1_press_timer = 0;
_Bool BTN2_DELAY_FLAG = FALSE;
u8 btn2_delay_cnt = 0;
u16 BTN2_press_timer = 0;
_Bool BTN3_DELAY_FLAG = FALSE;
u8 btn3_delay_cnt = 0;
u16 BTN3_press_timer = 0;
_Bool BTN4_DELAY_FLAG = FALSE;
u8 btn4_delay_cnt = 0;
u16 BTN4_press_timer = 0;
/*========================================*/

_Bool flag_stopDMA = TRUE;

// ===== UART Receive =====
#define UART_TIMEOUT (u8)100   /* 100ms */
UART_CMD_T UART_CMD;
u8 UARTrcvbuffidx = 0;
u8 UARTdatachksum;
u8 UARTcmd;
u8 UARTdatalen;
u8 UARTtimeoutcnt = 0;
_Bool FLAG_UART_timeout_started = FALSE;

typedef enum {
          UART_RCV_CMD            = 0,
          UART_RCV_DATALEN_B1     = 1,
          UART_RCV_DATALEN_B2     = 2,
          UART_RCV_HIGHSPEED_DATA = 4,
          UART_RCV_DATA           = 5,
          UART_RCV_CHKSUM         = 6
} UART_RcvState_t;
UART_RcvState_t UART_rcv_state = UART_RCV_CMD;
_Bool FLAG_UART_cmd_rcv = FALSE;
// ===== END UART Receive =====

/* Public variables */
volatile _Bool FLAG_UARTcmdRcv = FALSE;

u32 current_time_sec = 0;

/* External variables */
extern _Bool Timeout_istout1;
extern _Bool Timeout_istout2;
extern u16 Timeout_toutcnt1;
extern u16 Timeout_toutcnt2;
extern u16 Timeout_tout1;
extern u16 Timeout_tout2;
extern const volatile struct CalibData CAL;
/*====================*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/
void DMA1_Channel2_3_IRQHandler()
{
 while(1);
}
void TIM2_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM2, TIM_IT_Update))
  {
    current_time_sec++;
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  }
}

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles TIM3 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)  
{
  if(TIM_GetITStatus(TIM3, TIM_IT_Update))  //1ms 
  {
    /* ===== CKECK PERIODIC TASKS FLAGS ===== */
    if(cnt_flag_50ms < U16_MAX) cnt_flag_50ms++;
    if(cnt_flag_50ms >= CNTVAL_50MS) 
    {
      cnt_flag_50ms = 0;
      FLAG_50ms = TRUE;
    }
    if(cnt_flag_250ms < U16_MAX) cnt_flag_250ms++;
    if(cnt_flag_250ms >= CNTVAL_250MS) 
    {
      cnt_flag_250ms = 0;
      FLAG_250ms = TRUE;
    }
    if(cnt_flag_500ms < U16_MAX) cnt_flag_500ms++;
    if(cnt_flag_500ms >= CNTVAL_500MS) 
    {
      cnt_flag_500ms = 0;
      FLAG_500ms = TRUE;
    }
    if(cnt_flag_1000ms < U16_MAX) cnt_flag_1000ms++;
    if(cnt_flag_1000ms >= CNTVAL_1000MS) 
    {
      cnt_flag_1000ms = 0;
      FLAG_1000ms = TRUE;
    }
    /* ===== CHECK TIMEOUTS ===== */
    if(!Timeout_istout1)
    {
      Timeout_toutcnt1++;
      if(Timeout_toutcnt1 >= Timeout_tout1) Timeout_istout1 = TRUE;
    }
    if(!Timeout_istout2)
    {
      Timeout_toutcnt2++;
      if(Timeout_toutcnt2 >= Timeout_tout2) Timeout_istout2 = TRUE;
    }
    /* ========== DEBOUNCE INPUTS ========== 2MS */
    /* Debounce BTN1 */
    if(!BTN1_STATE)
    {
      if(btn1_0_cnt < U8_MAX) btn1_0_cnt++;
      btn1_1_cnt = 0;
      if(btn1_0_cnt >= DIG_IN_DEB_TIME)
      {
        BTN1_DEB_STATE = BTN_PRESSED;
      }
    }
    else
    {
      if(btn1_1_cnt < U8_MAX) btn1_1_cnt++;
      btn1_0_cnt = 0;
      if(btn1_1_cnt >= DIG_IN_DEB_TIME)
      {
        BTN1_DEB_STATE = BTN_DEPRESSED;
        BTN1_press_timer = 0;
      }
    }
   
    /* Record button press time */
    if(BTN1_DEB_STATE == BTN_PRESSED)
    {
      if(BTN1_press_timer < U16_MAX) BTN1_press_timer++;
    }
    /* Process button repetition rate delays */
    if(!BTN1_DELAY_FLAG)
    {
      btn1_delay_cnt++;
      if(btn1_delay_cnt == BTN_DELAY)
      {
        btn1_delay_cnt = 0;
        BTN1_DELAY_FLAG = TRUE;
      }
    }
    /* ======================================= */
    // ====== CHECK UART COMMUNICATION TIMEOUT ======
    if(UARTtimeoutcnt < U8_MAX) UARTtimeoutcnt++;
    if(UARTtimeoutcnt >= UART_TIMEOUT)
    {
      UART_rcv_state = UART_RCV_CMD;
    }
    
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);        // clear TIM1 CC2 interrupt flag
  }
}

void USART1_IRQHandler(void)
{
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
    u8 uart_rx = USART_ReceiveData(USART1);
    UARTtimeoutcnt = 0;     //reset timeout counter
    if(!FLAG_UART_cmd_rcv)  //if previously received command was processed by application, store in buffer, else discard sent data
    {
      switch(UART_rcv_state)
      {
        case UART_RCV_CMD:
        {
          UART_CMD.CMD = uart_rx;
          UARTrcvbuffidx = 0;
          UART_rcv_state = UART_RCV_DATALEN_B1;
          break;
        }
        case UART_RCV_DATALEN_B1:
        {
          UART_CMD.DATAlen = (uart_rx << 8);
          UART_rcv_state = UART_RCV_DATALEN_B2;
          break;
        }
        case UART_RCV_DATALEN_B2:
        {
          UART_CMD.DATAlen |= (uart_rx);
          if(UART_CMD.CMD == 0xFF) UART_rcv_state = UART_RCV_HIGHSPEED_DATA;
          else
          {
            if(UART_CMD.DATAlen == 0)
            {
              UART_rcv_state = UART_RCV_CHKSUM;
            }
            else if(UART_CMD.DATAlen > UARTBUFFSIZE)
            {
              UART_rcv_state = UART_RCV_CMD;
              //acknowledge data length too long
              while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
              USART_SendData(USART1, 0x3F);
            }
            else UART_rcv_state = UART_RCV_DATA;
          }
          break;
        }
        case UART_RCV_HIGHSPEED_DATA:
        {
          if(UARTrcvbuffidx++ < UART_CMD.DATAlen)
          {
            //send uart_rx to LCD via SPI
            while(SPI1->SR & SPI_SR_BSY);
            *(u8*)0x4001300C = uart_rx;
            /*while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
            USART_SendData(USART1, uart_rx);*/
            if(UARTrcvbuffidx == UART_CMD.DATAlen)
            {
              //send message received acknowledge
              //while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
              //USART_SendData(USART1, 0x10);
              UART_rcv_state = UART_RCV_CMD;
            }
          }
          break;
        }
        case UART_RCV_DATA:
        {
          if(UARTrcvbuffidx < UART_CMD.DATAlen)
          {
            UART_CMD.DATA[UARTrcvbuffidx++] = uart_rx;
            if(UARTrcvbuffidx == UART_CMD.DATAlen)
            {
              UART_rcv_state = UART_RCV_CHKSUM;
            }
          }
          break;
        }
        case UART_RCV_CHKSUM:
        {
          u8 calcchksum = 0, i;
          UARTdatachksum = uart_rx;
          //calculate received data checksum
          for(i = 0; i < UARTrcvbuffidx; i++)
          {
            calcchksum += UART_CMD.DATA[i];
          }
          calcchksum = (u8)(~calcchksum);
          //compare with received checksum
          if(calcchksum == UARTdatachksum)
          {
            FLAG_UART_cmd_rcv = TRUE;
            //send message received correct acknowledge
            while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
            USART_SendData(USART1, 0x10);
          }
          else
          {
            //send message received ok but checksum failed
            while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
            USART_SendData(USART1, 0x4F);
          }
          UART_rcv_state = UART_RCV_CMD;
        
          break;
        }
        default : break;
      }
    }
    else
    {
      // if previously received command was not processed by application ,discard data and send information byte
      while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
      USART_SendData(USART1, 0x5F);
    }
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
  }
}
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
