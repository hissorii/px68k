#ifndef _winx68k_pal
#define _winx68k_pal

#include "common.h"

extern	BYTE	Pal_Regs[1024];
extern	WORD	TextPal[256];
extern	WORD	GrphPal[256];
extern	WORD	Pal16[65536];

void Pal_SetColor(void);
void Pal_Init(void);

BYTE FASTCALL Pal_Read(DWORD adr);
void FASTCALL Pal_Write(DWORD adr, BYTE data);
void Pal_ChangeContrast(int num);

extern WORD Ibit, Pal_HalfMask, Pal_Ix2;

#endif

