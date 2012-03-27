
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
