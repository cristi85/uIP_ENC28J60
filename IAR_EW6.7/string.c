#include "types.h"

/* 
Function name:
    strlen
Parameters:
    src - Pointer to the string
Return:
    u8 - Length of string (until '\0' is reached)
*/
u8 strlen(const char* src)
{
  u8 i = 0;
  while(*(src++)) i++;
  return i;
}

/* 
Function name:
    strncmp
Parameters:
    s1 - Pointer to first string
    s2 - Pointer to second string
Return:
  u8  0  strings equal in the first n bytes
     -1  s1 < s2 in the first n bytes
      1  s2 > s1 in the first n bytes
*/
u8 strncmp(const char *s1, const char *s2, u16 n)
{
  /*while(n > 0 && *string1 && *string2)
  {
    if(*string1 < *string2)
    {
      return -1;
    }
    else if(*string1 >*string2)
    {
      return 1;
    }
    n--;
    string1++;
    string2++;
  }
  return 0;*/
  while(n--)
        if(*s1++!=*s2++)
            return *(unsigned char*)(s1 - 1) - *(unsigned char*)(s2 - 1);
    return 0;
}

/* 
Function name:
    strncmp
Parameters:
    string - Pointer to string
Return:
  returns a pointer to the first occurrence of character c in string or a null pointer if no matching character is found
*/
char *strchr(char *string, char c)
{
  while(*string)
  {
    if(*string == c)
    {
      return string;
    }
    string++;
  }
  if(*string == c) return string;
  return 0;
}

/* 
Function name:
    strncpy
Parameters:
    dest - Pointer to destination string
    src  - Pointer to source string
Return:
  returns the final copy of the copied string
*/
char *strncpy(char *dest, const char *src, u16 n)
{
  while(*(src) && n > 0)
  {
    *(dest++) = *(src++);
    n--;
  }
  *dest = 0;
  return dest;
  
}

/* 
Function name:
    strrchr
Parameters:
    str - This is the C string
    c   - This is the character to be located
Return:
  returns a pointer to the last occurrence of character in str. If the value is not found, the function returns a null pointer
*/
char *strrchr(const char *str, char c)
{
  char* ret=0;
    do {
        if( *str == (char)c )
            ret = (char*)str;
    } while(*str++);
    return ret;
}

/* 
Function name:
    string_copy
Description:
    Copy contents of src string to dest string
Parameters:
    dest - Pointer to destination string
    src  - Pointer to source string
Return:
    char* - Pointer to destination string
*/
char* strcpy(char* dest, const char* src)
{
  char *ret = dest;
  while(*(src)) *(dest++) = *(src++);
  *dest = 0;
  return ret;
}

/* 
Function name:
    string_append
Description:
    Append to dest string the contents of src string
Parameters:
    dest - Pointer to destination string
    src  - Pointer to source string
Return:
    char* - Pointer to destination string
*/
char* string_append(char* dest, const char* src)
{
  while(*(dest)) dest++;
  while(*(src)) *(dest++) = *(src++);
  *dest = 0;
  return dest;
}

/* 
Function name:
    string_substr
Description:
    Get substring of scr in dest
Parameters:
    dest - Pointer to destination string
    src  - Pointer to source string
Return:
    void
*/
char* string_substr(char* dest, const char* src, u8 startindex, u8 endindex)
{
  u8 cnt;
  if(startindex >= endindex) return dest;
  cnt = endindex - startindex;
  src += startindex;
  while(*(src) && cnt--)
  {
    *dest = *(src++);
  }
  return dest;
}

/* 
Function name:
    string_U32ToStr
Description:
    Convert number to string
Parameters:
    number - Number to be converted
    dest    - Pointer to string which will hold the result of conversion
Return:
    u8 - String length
*/
char* string_U32ToStr(u32 number, char* dest) //dest len=11 for max u32
{
  u32 tmp = number;
  u8 i = 9, j =0, k = 0;
  while(tmp)
  {
    dest[i--] = tmp%10+'0';
    tmp /= 10;
    j++;
  }
  for(;j;j--, k++)
  {
    dest[k] = dest[k+i+1];
  }
  dest[k] = 0;
  return dest;
}