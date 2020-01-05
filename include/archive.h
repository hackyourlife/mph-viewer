#ifndef __ARCHIVE_H__
#define __ARCHIVE_H__

void LoadArchive(const char* name, const char* filename);
int LoadFromArchive(void** out, const char* name, const char* filename);

#endif
