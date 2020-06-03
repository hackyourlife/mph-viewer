#include <string.h>
#include "utils.h"

#ifdef __linux__
char* strrev(char* s)
{
	size_t i, j;
	size_t len = strlen(s);
	for(i = 0, j = len - 1; i < j; i++, j--) {
		char x = s[i];
		s[i] = s[j];
		s[j] = x;
	}
	return s;
}
#endif

char* itoa(int value, char* buf, int base)
{
	int x;
	int sign;
	int i;
	int rem;

	x = value;
	sign = 0;
	i = 0;

	if(value < 0) {
		x = -value;
		sign = 1;
	}

	do {
		rem = x % base;
		x /= base;
		if(rem <= 9)
			buf[i++] = rem + '0';
		else
			buf[i++] = rem + '7';
	} while(x);

	if(sign)
		buf[i++] = '-';

	buf[i] = 0;
	strrev(buf);

	return buf;
}
