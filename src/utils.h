#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define DIV_ROUND_UP(nb, div) ((nb + div - 1) / div)

int strncmp(const char *s1, const char *s2, size_t n);

void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);

uint32_t __bswap32(uint32_t x);
uint64_t __bswap64(uint64_t x);

/*
 * Writes the given value to a string as a hexadecimal number.
 * The string is given as a pointer to the last (least significant) digit
 * position. The string buffer should contain at least 8 places or have
 * an 'x' character mark the start of the number. This function does not write
 * any leading characters, so the buffer should be pre-filled with leading
 * characters (zeroes, spaces etc.).
 */
void write_hex_digits(unsigned int value, char *last_digit);

void udelay(unsigned int us);

bool ram_works(void);

#endif /* __UTILS_H__ */
