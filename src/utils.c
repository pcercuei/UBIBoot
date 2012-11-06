
#include <string.h>

/* strncmp of the poor */
int strncmp(const char *s1, const char *s2, size_t n)
{
	while (n--) {
		if (*s1++ != *s2++)
			return (*s1 < *s2) ? -1 : 1;
	}
	return 0;
}

size_t strlen(const char *s)
{
	size_t i = 0;

	while(*s++)
		i++;
	return i + 1;
}

void * memcpy(void *dest, const void *src, size_t n)
{
	unsigned char *d = dest;
	const unsigned char *s = src;

	while (n) {
		*d++ = *s++;
		n -= sizeof(unsigned char);
	}
	return dest;
}
