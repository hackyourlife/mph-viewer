#include <string.h>
#include <stdio.h>
#include "types.h"
#include "heap.h"
#include "strings.h"
#include "io.h"
#include "utils.h"

const char* stringtable_names[6] = {
		"stringTables",
		"stringTables_fr",
		"stringTables_gr",
		"stringTables_it",
		"stringTables_sp",
		"stringTables_jp"
};

char current_language = 0;

const char* get_stringtable_folder()
{
	return stringtable_names[current_language & 0x1F];
}

void parse_stringtable(StringTableEntry* entries, RawStringTableEntry* raw_entries, char *raw_data, int length)
{
	StringTableEntry* dst = entries;
	RawStringTableEntry* src = raw_entries;
	int i;

	for(i = 0; i < length; i++, src++, dst++) {
		dst->id = src->id;
		dst->length = src->length;
		dst->speed = src->speed;
		dst->category = src->category;
		dst->string = strdup(raw_data + src->string);
	}
}

void load_stringtable(const char* folder, const char* name, StringTable* dst)
{
	StringTableFile* out = NULL;
	char filename[64];

	sprintf(filename, "%s/%s", folder, name, 0);
	LoadFile((void **)&out, filename);
	dst->length = out->length;
	dst->entries = (StringTableEntry*) alloc_from_heap(dst->length * sizeof(StringTableEntry));
	parse_stringtable(dst->entries, out->data, (char*) out, dst->length & 0xFFFF);
	free_to_heap(out);
}

const char* search_string_by_id_internal(StringTable* strings, int id)
{
	unsigned int length;
	unsigned int idx;
	StringTableEntry* entries;
	unsigned int ptr;
	unsigned int string_id;

	length = strings->length;
	idx = 0;

	if(!length)
		return NULL;

	entries = strings->entries;
	while(1) {
		ptr = idx + ((length - idx) >> 1);
		string_id = entries[ptr].id;
		if(id == string_id)
			break;
		if(id <= string_id)
			length = idx + ((length - idx) >> 1);
		else
			idx = ptr + 1;
		if(idx >= length)
			return NULL;
	}

	return entries[ptr].string;
}

const char* get_string_by_id(StringTable* strings, const char* id)
{
	char* p;
	int i;
	char tmp;
	char reversed_id[5];

	p = reversed_id;
	strcpy(reversed_id, "0000");
	for(i = 0; i < 4; i++) {
		tmp = id[3 - i];
		*p++ = tmp;
	}

	return search_string_by_id_internal(strings, *(int *)reversed_id);
}

const char* get_string_by_id_and_type(char type, u16 id, StringTable* strings)
{
	int last_index;
	char* p;
	char* end;
	char tmp;
	char id32[5];
	char buf[5];

	id32[0] = type;
	strcpy(&id32[1], "000");
	itoa(id, buf, 10);
	last_index = strlen(buf) - 1;
	if(last_index >= 0) {
		p = &id32[3];
		end = &buf[last_index];
		do {
			tmp = *end--;
			--last_index;
			*p-- = tmp;
		}
		while(last_index >= 0);
	}

	return get_string_by_id(strings, id32);
}
