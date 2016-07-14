#include "config.h"
#include "utils.h"

int strncmp(const char *s1, const char *s2, size_t n)
{
	while (n--) {
		unsigned char c1 = *s1, c2 = *s2;
		if (c1 != c2)
			return (int)c1 - (int)c2;
		if (c1 == '\0')
			return 0;
		s1++;
		s2++;
	}
	return 0;
}

void *memcpy(void *dest, const void *src, size_t n)
{
	unsigned char *d = dest;
	const unsigned char *s = src, *e = src + n;

	while (s != e)
		*d++ = *s++;
	return dest;
}

void *memmove(void *dest, const void *src, size_t n)
{
	if (dest <= src || src + n <= dest) {
		/* Copying in forward direction won't lose data. */
		return memcpy(dest, src, n);
	} else {
		/* Copying in backward direction won't lose data. */
		unsigned char *d = dest + n;
		const unsigned char *s = src + n;

		/* Note that n == 0 is handled by the other branch. */
		do { *--d = *--s; } while (s != src);
		return dest;
	}
}

uint32_t swap_be32(uint32_t val)
{
	return ((val & 0xFF000000) >> 24)
	     | ((val & 0x00FF0000) >> 8)
	     | ((val & 0x0000FF00) << 8)
	     | ((val & 0x000000FF) << 24);
}

void write_hex_digits(unsigned int value, char *last_digit)
{
	unsigned char *ptr = (unsigned char *) last_digit;
	do {
		unsigned char nb = value & 0xf;
		*ptr-- = nb >= 10 ? 'a' + nb - 10 : '0' + nb;
		value >>= 4;
	} while (value && (*ptr != 'x'));
}

void udelay(unsigned int us)
{
	unsigned int tmp = (CFG_CPU_SPEED / 1000000 / 2) * us;
	asm volatile (
		".set noreorder\n\t"
		"0:\n\t"
			"bnez %0,0b\n\t"
			"addiu %0, %0, -1\n\t"
		".set reorder\n\t"
			::"r"(tmp)
		);
}
