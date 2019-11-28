#include <stdlib.h>

#include "heap.h"

void* alloc_from_heap(unsigned int size)
{
	return malloc(size);
}

void free_to_heap(void* ptr)
{
	free(ptr);
}

void check_heap()
{
}
