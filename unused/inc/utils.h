#ifndef UTILS_H
#define UTILS_H

#include "famine.h"


#define ELF_32 1
#define ELF_64 2
#define PT_LOAD 1

void *ft_memcpy(void *dest, const void *src, size_t n);
void *ft_memset(void *s, int c, size_t n);
int ft_memcmp(const void *s1, const void *s2, size_t n);

int ft_strlen(const char *str);

void print_hex(void *data, size_t size);

void free_data(void);


#endif
