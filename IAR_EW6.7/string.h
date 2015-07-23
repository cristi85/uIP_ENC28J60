#ifndef _STRING_H
#define _STRING_H

#include "types.h"

u8 strlen(const char* src);
u8 strncmp(const char *s1, const char *s2, u16 n);
char *strchr(char *string, char c);
char *strncpy(char *dest, const char *src, u16 n);
char *strrchr(const char *str, char c);
char* strcpy(char* dest, const char* src);

char* string_U32ToStr(u32, char*);
char* string_append(char*, const char*);
char* string_substr(char*, const char*, u8, u8);
u8 string_len(const char*);

#endif