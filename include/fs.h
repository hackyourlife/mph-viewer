#ifndef __FS_H__
#define __FS_H__

#include <stdio.h>
#include "types.h"

typedef enum {
	/* Asynchronous commands*/
	FS_COMMAND_ASYNC_BEGIN = 0,
	FS_COMMAND_READFILE = FS_COMMAND_ASYNC_BEGIN,
	FS_COMMAND_WRITEFILE,
	FS_COMMAND_ASYNC_END,

	/* Synchronous commands*/
	FS_COMMAND_SYNC_BEGIN = FS_COMMAND_ASYNC_END,
	FS_COMMAND_SEEKDIR = FS_COMMAND_SYNC_BEGIN,
	FS_COMMAND_READDIR,
	FS_COMMAND_FINDPATH,
	FS_COMMAND_GETPATH,
	FS_COMMAND_OPENFILEFAST,
	FS_COMMAND_OPENFILEDIRECT,
	FS_COMMAND_CLOSEFILE,
	FS_COMMAND_SYNC_END,

	/* Archive messages*/
	FS_COMMAND_STATUS_BEGIN = FS_COMMAND_SYNC_END,
	FS_COMMAND_ACTIVATE = FS_COMMAND_STATUS_BEGIN,
	FS_COMMAND_IDLE,
	FS_COMMAND_SUSPEND,
	FS_COMMAND_RESUME,
	FS_COMMAND_STATUS_END,

	FS_COMMAND_INVALID
} FSCommandType;

typedef enum {
	FS_SEEK_SET,
	FS_SEEK_CUR,
	FS_SEEK_END
} FSSeekFileMode;

struct FSFile;
typedef struct FSFile FSFile;

typedef struct {
	FSFile*	prev;
	FSFile*	next;
} FSFileLink;

typedef struct {
	void*	head;
	void*	tail;
} OSThreadQueue;

struct FSFile {
	FSFileLink link;
	void*	arc;
	int	stat;
	int	command;
	int	error;
	OSThreadQueue queue;

	/* handle property*/
	union {
	/* file-handle property*/
		struct {
			u32	own_id;		/* own file-id*/
			u32	top;		/* image-top*/
			u32	bottom;		/* image-bottom*/
			u32	pos;		/* current position*/
		}
		file;
	} prop;

	int	arg;

	/* PC only */
	FILE*	stdio_file;
};

static inline u32 FS_GetLength(const FSFile* p_file)
{
	return p_file->prop.file.bottom - p_file->prop.file.top;
}

static inline u32 FS_GetPosition(const FSFile *p_file)
{
	return p_file->prop.file.pos - p_file->prop.file.top;
}

void	FS_InitFile(FSFile* p_file);
BOOL	FS_OpenFile(FSFile* p_file, const char* path);
BOOL	FS_CloseFile(FSFile* p_file);
int	FS_ReadFile(FSFile* p_file, void* dst, int len);
BOOL	FS_SeekFile(FSFile *p_file, s32 offset, FSSeekFileMode origin);

/* internal */
BOOL	FSi_SendCommand(FSFile *p_file, FSCommandType command);

#endif
