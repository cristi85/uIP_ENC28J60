#ifndef _SERIAL_H_
#define _SERIAL_H_

void Serial_PrintByte(u8 data, _Bool prefix);
void Serial_PrintString(char* src);
void Serial_PrintBinary(u16 num);
void Serial_PrintNumber(s32 num);
void Serial_PrintChar(char ch);

#endif