#ifndef __SCAN_H__
#define __SCAN_H__

#include "strings.h"

typedef struct {
	char			filename[32];
	int			count;
	int			size;
	int			flags;
	StringTableEntry*	entries;
} ScanLog;

typedef struct {
	u32 count;
	u32 size;
} ScanLogHeader;

typedef struct {
	int		category;
	char		title[22];
	char		category_name[18];
	char		log_pages[16][129];
	unsigned char	page_count;
	int		scan_frames;
} CScan;

int get_scan_speed(int id);
StringTableEntry* get_scan_log_entry(int id, ScanLog* log);
StringTableEntry* get_scan_log_entry_by_name(ScanLog* log, const char* name);
StringTableEntry* search_scan_log_entry(ScanLog* log, unsigned int id);

#endif
