#include <stdlib.h>

#include "fs.h"

void FS_InitFile(FSFile* p_file)
{
	p_file->link.prev = 0;
	p_file->link.next = p_file->link.prev;
	p_file->queue.tail = 0;
	p_file->queue.head = p_file->queue.tail;
	p_file->arc = 0;
	p_file->command = 14;
	p_file->stat = 0;
}

BOOL FS_OpenFile(FSFile* p_file, const char* path)
{
	char buf[256];
	size_t size;
	sprintf(buf, "data/%s", path);
	p_file->stdio_file = fopen(buf, "rb");
	if(!p_file->stdio_file)
		return FALSE;
	fseek(p_file->stdio_file, 0, SEEK_END);
	size = ftell(p_file->stdio_file);
	fseek(p_file->stdio_file, 0, SEEK_SET);
	p_file->prop.file.bottom = (int) size;
	p_file->prop.file.top = 0;
	p_file->prop.file.pos = 0;
	return TRUE;
}

BOOL FS_CloseFile(FSFile* p_file)
{
	if(!FSi_SendCommand(p_file, FS_COMMAND_CLOSEFILE))
		return FALSE;
	p_file->arc = NULL;
	p_file->command = 14;
	p_file->stat &= 0xFFFFFFCF;
	return TRUE;
}

int FS_ReadFile(FSFile* p_file, void* dst, int len)
{
	size_t sz = fread(dst, 1, len, p_file->stdio_file);
	p_file->prop.file.pos = ftell(p_file->stdio_file);
	return (int) sz;
}

BOOL FS_SeekFile(FSFile *p_file, s32 offset, FSSeekFileMode origin)
{
	switch(origin) {
		case FS_SEEK_SET:
			offset += p_file->prop.file.top;
			break;
		case FS_SEEK_CUR:
			offset += p_file->prop.file.pos;
			break;
		case FS_SEEK_END:
			offset += p_file->prop.file.bottom;
			break;
		default:
			return FALSE;
	}

	if(offset < (s32) p_file->prop.file.top)
		offset = (s32) p_file->prop.file.top;
	if(offset > (s32) p_file->prop.file.bottom)
		offset = (s32) p_file->prop.file.bottom;
	p_file->prop.file.pos = (u32) offset;

	fseek(p_file->stdio_file, FS_GetPosition(p_file), SEEK_SET);

	return TRUE;
}

/* PC version */
BOOL FSi_SendCommand(FSFile *p_file, FSCommandType command)
{
	switch(command) {
		case FS_COMMAND_CLOSEFILE:
			if(p_file->stdio_file) {
				fclose(p_file->stdio_file);
				p_file->stdio_file = NULL;
				return TRUE;
			} else
				return FALSE;
			break;
		default:
			return FALSE;
	}
}
