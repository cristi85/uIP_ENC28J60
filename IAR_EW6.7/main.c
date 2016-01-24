/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "types.h"
#include "integer.h"
#include "config.h"
#include "stm32f0xx_it.h"
#include "delay.h"
#include "errors.h"
#include "config.h"
#include "timeout.h"
#include "string.h"
#include "uip/clock.h"
#include "macphy.h"
#include "uip/uip.h"
#include "uip/timer.h"
#include "uip/uip_arp.h"
#include "serial.h"
#include "webserver\httpd.h"

#define UART_CMD_PREPARE_TFT  (u8)0x01
#define UART_CMD_2  (u8)0x10
#define UART_CMD_3  (u8)0x02

extern void memcpy(u8*, const u8*, u8);
extern void Util_SendUART(u8*, u8);

/* Calibration Data */
const struct CalibData CAL = 
  {
    100,    // LCD Backlight Brightness
    16      //number of samples when averaging ADC measured data
  };

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static _Bool FLAG_BTN1_lock = FALSE;

/* uIP stack */
unsigned char macaddr[] = {0x02, 0x01, 0x02, 0x03, 0x04, 0x05}; 
struct uip_eth_addr uipmacaddr = {{0x02, 0x01, 0x02, 0x03, 0x04, 0x05}};
uip_ipaddr_t ipaddr;
/* Re-transmit buffer length */
u32 xmitlen;
extern unsigned char pkt_buf[];
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

u16* ptr_BUF_type;
timer_t periodic_timer, arp_timer;
/* Private function prototypes -----------------------------------------------*/
void TASK_UARTCommands(void);


int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f0xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f0xx.c file
     */
  u32 i;
  
  Config();
  SystemCoreClockUpdate();
  Errors_Init();
  Serial_PrintString("Power-up\r\n");
  
  /* ===== ETHERNET ======= */
  /* Reset the ENC28J60 */
  macphy_reset();

  /* Enable ethernet */
  macphy_init(macaddr);

  /* Begin receiving packets */
  macphy_startrx();

  /* Taken almost verbatim from the uIP example code */
  timer_set(&periodic_timer, 1); /* 125uS periodic interrupts */
  timer_set(&arp_timer, 10);

  /* Initialize the uIP ARP subsystem */
  uip_arp_init();
  uip_setethaddr(uipmacaddr);

  /* Initialize the uIP stack proper */
  uip_init();

  /* Hardcode the IP address */
  uip_ipaddr(ipaddr, 192, 168, 0, 221);
  uip_sethostaddr(ipaddr);
	  
  uip_ipaddr(ipaddr, 255, 255, 252, 0);
  uip_setnetmask(ipaddr);

  /* Initialize the application to listen on TCP port 23 (telnet) */
  uip_listen(HTONS(23));
  
  /* Initialize WEB server */
  httpd_init();
  
  ptr_BUF_type = (u16*)&(BUF->type);
  /* ===== END ETHERNET ======= */
  
  while (1)
  {
    // ============= uIP ============== //
    
    /* Incoming packet? */
    if (macphy_pktcnt() > 0) {
      macphy_readpkt();
      uip_len = (pkt_buf[3] << 8) + pkt_buf[2];
    } else {
      uip_len = 0;
    }

    /* Outgoing packet? */
    if(uip_len > 0) {
      if(*ptr_BUF_type == htons(UIP_ETHTYPE_IP)) {
        uip_arp_ipin();
        uip_input();
        /* If the above function invocation resulted in data that
           should be sent out on the network, the global variable
           uip_len is set to a value > 0. */
        if(uip_len > 0) {
          uip_arp_out();
	 macphy_sendpkt();
        }
      } else if(*ptr_BUF_type == htons(UIP_ETHTYPE_ARP)) {
        uip_arp_arpin();
        /* If the above function invocation resulted in data that
           should be sent out on the network, the global variable
           uip_len is set to a value > 0. */
        if(uip_len > 0) {
	  macphy_sendpkt();
        }
      }

    } else if(timer_expired(&periodic_timer)) {
      timer_reset(&periodic_timer);
      for(i = 0; i < UIP_CONNS; i++) {
        uip_periodic(i);
        /* If the above function invocation resulted in data that
           should be sent out on the network, the global variable
           uip_len is set to a value > 0. */
        if(uip_len > 0) {
          uip_arp_out();
	  macphy_sendpkt();
        }
      }
     
      /* Call the ARP timer function every 10 seconds. */
      if(timer_expired(&arp_timer)) {
        timer_reset(&arp_timer);
        uip_arp_timer();
      }
    }
    
    // ============= END uP ========== //
    
    // ================== TASK cyclic 1000ms ===================
    if(FLAG_1000ms)
    {
      FLAG_1000ms = FALSE;
    }
    // ================== END TASK cyclic 1000ms ===================
    
    // ================== TASK cyclic 500ms ===================
    if(FLAG_500ms)
    {
      FLAG_500ms = FALSE;
    }
    // ================== END TASK cyclic 500ms ===================
    
    // ================== TASK cyclic 250ms ===================
    if(FLAG_250ms)
    {
      FLAG_250ms = FALSE;
    }
    // ================== END TASK cyclic 250ms ===================
    
    // ================== TASK cyclic 50ms ===================
    if(FLAG_50ms)
    {
      FLAG_50ms = FALSE;
    }
    // ================== END TASK cyclic 50ms ===================
    
    //=============== CHANGE CURRENT SCREEN ================
    // ============== PRESS BTN1 with key repetition lock =================
    if(BTN1_DEB_STATE == BTN_PRESSED && !FLAG_BTN1_lock)
    {
      FLAG_BTN1_lock = TRUE;
      
    }
    if(BTN1_DEB_STATE == BTN_DEPRESSED && FLAG_BTN1_lock)
    {
      FLAG_BTN1_lock = FALSE;
    }
    // ============== END PRESS BTN1 with key repetition lock =================
    
    
    // ============= UART COMMAND RECEIVED ==============
    if(FLAG_UART_cmd_rcv)
    {
      switch(UART_CMD.CMD)
      {
        default:
        {
          //acknowledge command not recognized
          while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
            USART_SendData(USART1, 0x8F);
          break;
        }
      }
      FLAG_UART_cmd_rcv = FALSE;
    }
    // ============= END UART COMMAND RECEIVED ==============
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
