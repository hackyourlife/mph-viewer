#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"

void fatal_error(const char* fmt, ...)
{
	char msg[0x400];
	va_list args;
	va_start(args, fmt);
	vsnprintf(msg, sizeof(msg), fmt, args);
	va_end(args);
	printf("FATAL ERROR!\n%s", msg);
	exit(1);
}
