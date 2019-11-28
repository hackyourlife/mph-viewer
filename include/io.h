#ifndef __IO_H__
#define __IO_H__

#include "fs.h"

int LoadFile(void** out, const char* filename);
int LoadFileFromArchive(void** out, const char* filename);

#endif
