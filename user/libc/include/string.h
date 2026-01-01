#ifndef __STRING_H
#define __STRING_H

#include <types.h>

size strlen(const char *s);
bool streq(const char *a, const char *b);
char *strchr(const char *s, int c);
char *strtok(char *str, const char *delim);

#endif