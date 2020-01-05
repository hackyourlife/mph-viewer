#ifndef __STRINGS_H__
#define __STRINGS_H__

#include "types.h"

typedef struct {
	int			id;
	const char*		string;
	u16			length;
	u8			speed;
	char			category;
} StringTableEntry;

typedef struct {
	u32			id;
	u32			string;
	u16			length;
	u8			speed;
	char			category;
} RawStringTableEntry;

typedef struct {
	int			length;
	StringTableEntry*	entries;
} StringTable;

typedef struct {
	u32			length;
	RawStringTableEntry	data[1];
} StringTableFile;

extern char current_language;

const char* get_stringtable_folder();
void load_stringtable(const char* folder, const char *name, StringTable* dst);
const char* get_string_by_id(StringTable* strings, const char* id);
const char* get_string_by_id_and_type(char type, u16 id, StringTable* strings);


#endif
