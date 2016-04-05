#ifndef __UTILS_H__
#define __UTILS_H__

#include <stddef.h>
#include <stdint.h>

int strncmp(const char *s1, const char *s2, size_t n);

void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);

uint32_t swap_be32(uint32_t val);

#endif /* __UTILS_H__ */
