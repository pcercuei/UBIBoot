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
	const unsigned char *s = src;

	while (n--)
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

		while (n--)
			*--d = *--s;
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
