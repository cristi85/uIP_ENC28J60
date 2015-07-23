#include "types.h"

void memset(void *memstart, u8 val, u8 num)
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