#ifndef _winx68k_bg
#define _winx68k_bg

#include "common.h"

extern	BYTE	BG_DrawWork0[1024*1024];
extern	BYTE	BG_DrawWork1[1024*1024];
extern	DWORD	BG0ScrollX, BG0ScrollY;
extern	DWORD	BG1ScrollX, BG1ScrollY;
extern	DWORD	BG_AdrMask;
extern	BYTE	BG_CHRSIZE;
extern	BYTE	BG_Regs[0x12];
extern	WORD	BG_BG0TOP;
extern	WORD	BG_BG1TOP;
extern	long	BG_HAdjust;
extern	long	BG_VLINE;
extern	DWORD	VLINEBG;

extern	BYTE	Sprite_DrawWork[1024*1024];
extern	WORD	BG_LineBuf[1600];

void BG_Init(void);

BYTE FASTCALL BG_Read(DWORD adr);
void FASTCALL BG_Write(DWORD adr, BYTE data);

void FASTCALL BG_DrawLine(int opaq, int gd);

#endif
