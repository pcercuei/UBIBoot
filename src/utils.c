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

size_t strlen(const char *s)
{
	size_t i = 0;

	while (*s++)
		i++;
	return i;
}

void *memcpy(void *dest, const void *src, size_t n)
{
	unsigned char *d = dest;
	const unsigned char *s = src;

	while (n--)
		*d++ = *s++;
	return dest;
}
