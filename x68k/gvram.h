#ifndef _winx68k_gvram
#define _winx68k_gvram

#include "common.h"

extern	BYTE	GVRAM[0x80000];
extern	WORD	Grp_LineBuf[1024];
extern	WORD	Grp_LineBufSP[1024];
extern	WORD	Grp_LineBufSP2[1024];

void GVRAM_Init(void);

void FASTCALL GVRAM_FastClear(void);

BYTE FASTCALL GVRAM_Read(DWORD adr);
void FASTCALL GVRAM_Write(DWORD adr, BYTE data);

void Grp_DrawLine16(void);
void FASTCALL Grp_DrawLine8(int page, int opaq);
void FASTCALL Grp_DrawLine4(DWORD page, int opaq);
void FASTCALL Grp_DrawLine4h(void);
void FASTCALL Grp_DrawLine16SP(void);
void FASTCALL Grp_DrawLine8SP(int page/*, int opaq*/);
void FASTCALL Grp_DrawLine4SP(DWORD page/*, int opaq*/);
void FASTCALL Grp_DrawLine4hSP(void);
void FASTCALL Grp_DrawLine8TR(int page, int opaq);
void FASTCALL Grp_DrawLine4TR(DWORD page, int opaq);
#endif

