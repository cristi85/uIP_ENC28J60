#include "types.h"

void Serial_PrintChar(char ch)
{
  while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
  USART_SendData(USART1, ch);
}

void Serial_PrintByte(u8 num, _Bool prefix)
{
  if(prefix)
  {
    while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
    USART_SendData(USART1, '0');
    while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
    USART_SendData(USART1, 'x');
  }
  if(((num & 0xF0) >> 4) < 10) 
  {
    while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
    USART_SendData(USART1, ((num & 0xF0) >> 4) + 48);
  }
  else 
  {
    while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
    USART_SendData(USART1, ((num & 0xF0) >> 4) + 55);
  }
  if((num & 0x0F) < 10)
  {
    while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
    USART_SendData(USART1, (num & 0x0F) + 48);
  }
  else
  { 
    while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
    USART_SendData(USART1, (num & 0x0F) + 55);
  }
}

void Serial_PrintString(char* src)
{
  while(*src)
  {
    while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
    USART_SendData(USART1, *src);
    src++;
  }
}

void Serial_PrintBinary(u16 num)
{
  if (num == 0) 
  {
	while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
    USART_SendData(USART1, '0');
  }
  else 
  {
    u16 mask = 0x8000;
    u8 i=0;
    while(!(num & mask) && i < 16)  // skip leading zeroes
    {
      mask >>= 1;
      i++;
    }
    for(;i<16;i++)
    {
      if(num & mask) 
	  {
		while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
        USART_SendData(USART1, '0');
	  }
      else
	  {
		while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
        USART_SendData(USART1, '1');
	  }
      mask >>= 1;
    }
  }
}

void Serial_PrintNumber(s32 num)
{
  if (num == 0) 
  {
	while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
    USART_SendData(USART1, '0');
  }
  else 
  {
    static u8 numstr[10];
    u8 CurPos=0;
    if(num < 0)
    {
      while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
      USART_SendData(USART1, '-');
      num = -num;
    }
    while (num != 0)
    {
      numstr[CurPos] = (u8)((u8)(num%10) + 48);
      num /= 10;
      CurPos++;
    }

    while (CurPos--) 
	{
	  while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
      USART_SendData(USART1, numstr[CurPos] + 48);
	}
  }
}