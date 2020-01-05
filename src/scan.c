#include <string.h>
#include <stdio.h>
#include "types.h"
#include "strings.h"
#include "scan.h"

ScanLog scan_log;

int get_scan_speed(int id)
{
	StringTableEntry* entry = get_scan_log_entry(id, &scan_log);
	if(entry)
		return 10 * (entry->speed & 7);
	else
		return 60;
}

StringTableEntry* get_scan_log_entry(int id, ScanLog* log)
{
	char name[5];

	sprintf(name, "L%03d", id);
	return get_scan_log_entry_by_name(log, name);
}

StringTableEntry* get_scan_log_entry_by_name(ScanLog* log, const char* name)
{
	char* p;
	int i;
	char buf[5];

	strcpy(buf, "0000");
	for(i = 0, p = buf; i < 4; i++, p++)
		*p = name[3 - i];

	return search_scan_log_entry(log, *(unsigned int *)buf);
}

StringTableEntry* search_scan_log_entry(ScanLog* log, unsigned int id)
{
	unsigned int length;
	unsigned int idx;
	StringTableEntry* entries;
	unsigned int ptr;
	unsigned int string_id;

	length = log->count;
	idx = 0;
	if(!length)
		return NULL;

	entries = log->entries;
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
	return &entries[ptr];
}
