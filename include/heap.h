#ifndef __HEAP_H__
#define __HEAP_H__

void* alloc_from_heap(unsigned int size);
void free_to_heap(void* ptr);
void check_heap();

#endif
