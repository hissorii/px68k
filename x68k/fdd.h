#ifndef _winx68k_fdd
#define _winx68k_fdd

#include "common.h"

typedef struct {
	BYTE c;
	BYTE h;
	BYTE r;
	BYTE n;
} FDCID;

enum {
	FD_Non = 0,
	FD_XDF,
	FD_D88,
	FD_DIM,
};

DWORD FASTCALL FDD_Int(BYTE irq);
void FDD_SetFD(int drive, char* filename, int readonly);
void FDD_EjectFD(int drive);
void FDD_Init(void);
void FDD_Cleanup(void);
void FDD_Reset(void);
void FDD_SetFDInt(void);
int FDD_Seek(int drv, int trk, FDCID* id);
int FDD_ReadID(int drv, FDCID* id);
int FDD_WriteID(int drv, int trk, unsigned char* buf, int num);
int FDD_Read(int drv, FDCID* id, unsigned char* buf);
int FDD_ReadDiag(int drv, FDCID* id, FDCID* retid, unsigned char* buf);
int FDD_Write(int drv, FDCID* id, unsigned char* buf, int del);
int FDD_IsReady(int drv);
int FDD_IsReadOnly(int drv);
int FDD_GetCurrentID(int drv, FDCID* id);
void FDD_SetReadOnly(int drv);
void FDD_SetEMask(int drive, int emask);
void FDD_SetAccess(int drive);
void FDD_SetBlink(int drive, int blink);

#endif


