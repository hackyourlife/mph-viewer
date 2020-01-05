#include <string.h>
#include "utils.h"

#ifdef __linux__


#include <string.h>

char* strrev(char* str)
{
	if(!str || ! *str)
		return str;

	int i = strlen(str) - 1, j = 0;

	char ch;
	while(i > j) {
		ch = str[i];
		str[i] = str[j];
		str[j] = ch;
		i--;
		j++;
	}
	return str;
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
