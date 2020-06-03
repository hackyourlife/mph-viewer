#include <string.h>
#include <strings.h>

#include "error.h"
#include "heap.h"
#include "io.h"
#include "archive.h"
#include "lzss.h"
#include "endianess.h"

typedef struct {
	u8	name[32];
	u32	offset;
	u32	size;
	u32	unknown[6];
} ArchiveEntry;

typedef struct Archive Archive;

struct Archive {
	Archive*	next;
	char		name[32];
	int		entry_count;
	void*		data;
};

static Archive* archives = NULL;

Archive* FindArchive(const char* name)
{
	Archive* arc;
	for(arc = archives; arc; arc = arc->next) {
		if(!strcmp(arc->name, name))
			return arc;
	}
	return NULL;
}

void LoadArchive(const char* name, const char* filename)
{
	int i;
	void* arc = NULL;
	u8* data;

	Archive* archive = FindArchive(name);
	if(archive)
		return;

	printf("registering archive %s\n", name);

	LoadFile(&arc, filename);
	LZDecode((void**)&data, arc);
	free_to_heap(arc);

	if(memcmp(data, "SNDFILE", 7))
		fatal_error("not a valid archive!");

	archive = (Archive*) alloc_from_heap(sizeof(Archive));
	strcpy(archive->name, name);
	archive->next = archives;
	archive->data = data;
	archive->entry_count = data[11];

	for(i = 0; i < archive->entry_count; i++) {
		ArchiveEntry* entry = (ArchiveEntry*) (data + 32 + i * sizeof(ArchiveEntry));
		entry->size = get32bit_BE((u8*) &entry->size);
		entry->offset = get32bit_BE((u8*) &entry->offset);
	}

	archives = archive;
}

int LoadFromArchive(void** out, const char* name, const char* filename)
{
	int i;
	Archive* arc = FindArchive(name);
	u8* data;

	if(!arc)
		fatal_error("LoadFromArchive: Cannot find archive %s!\n", name);

	data = (u8*) arc->data;

	for(i = 0; i < arc->entry_count; i++) {
		ArchiveEntry* entry = (ArchiveEntry*) (data + 32 + i * sizeof(ArchiveEntry));
		if(!strcasecmp(entry->name, filename)) {
			int size = entry->size;
			*out = alloc_from_heap(size);
			if(!*out)
				fatal_error("LoadFromArchive: Not enough memory to allocate %d bytes!\n", size);
			memcpy(*out, data + entry->offset, size);
			return size;
		}
	}

	fatal_error("LoadFromArchive: Cannot find file %s in archive %s\n", filename, name);
}
