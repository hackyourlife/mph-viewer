#include <stdlib.h>
#include <stdio.h>

#include "os.h"

void OS_Terminate(void)
{
	printf("OS_Terminate\n");
	exit(1);
}
