#ifndef winx68k_fileio_h
#define winx68k_fileio_h

#include "common.h"
#include "dosio.h"

#define	FILEH		HANDLE

#define	FSEEK_SET	0
#define	FSEEK_CUR	1
#define	FSEEK_END	2

LPSTR getFileName(LPSTR filename);
//#define	getFileName	GetFileName

FILEH	File_Open(BYTE *filename);
FILEH	File_Create(BYTE *filename);
DWORD	File_Seek(FILEH handle, long pointer, short mode);
DWORD	File_Read(FILEH handle, void *data, DWORD length);
DWORD	File_Write(FILEH handle, void *data, DWORD length);
short	File_Close(FILEH handle);
short	File_Attr(BYTE *filename);
#define	File_Open	file_open
#define	File_Create	file_create
#define	File_Seek	file_seek
#define	File_Read	file_lread
#define	File_Write	file_lwrite
#define	File_Close	file_close
#define	File_Attr	file_attr

void	File_SetCurDir(BYTE *exename);
FILEH	File_OpenCurDir(BYTE *filename);
FILEH	File_CreateCurDir(BYTE *filename);
short	File_AttrCurDir(BYTE *filename);
#define	File_SetCurDir		file_setcd
#define	File_OpenCurDir		file_open_c
#define	File_CreateCurDir	file_create_c
#define	File_AttrCurDir		file_attr_c

#endif
