#include <string.h>

#include "error.h"
#include "heap.h"
#include "io.h"
#include "archive.h"

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
	char name[32];
	char* file;
	char* slash = strchr(filename, '/');
	if(!slash)
		fatal_error("LoadFileFromArchive: Failed to get archive name from file %s!\n", filename);

	memcpy(name, filename, (int) (slash - filename));
	name[(int) (slash - filename)] = 0;
	file = slash + 1;

	return LoadFromArchive(out, name, file);
}
