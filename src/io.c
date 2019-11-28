#include "error.h"
#include "heap.h"
#include "io.h"

int LoadFile(void** out, const char* filename)
{
	FSFile file;
	int size;

	FS_InitFile(&file);
	if(!FS_OpenFile(&file, filename))
		fatal_error("LoadFile: Cannot open file %s!\n", filename);

	size = FS_GetLength(&file);
	*out = alloc_from_heap(size);

	if(!*out)
		fatal_error("LoadFile: Not enough memory to allocate %d bytes!\n", size);
	if(size != FS_ReadFile(&file, *out, size))
		fatal_error("LoadFile: Failed to read the file %s!\n", filename);

	FS_CloseFile(&file);
	return size;
}

int LoadFileFromArchive(void** out, const char* filename)
{
	char name[256];
	sprintf(name, "archives/data/%s", filename);
	printf("loading file %s from extracted archive\n", filename);
	return LoadFile(out, name);
}
