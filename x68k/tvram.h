#ifndef _winx68k_tvram
#define _winx68k_tvram

#include "common.h"

extern	BYTE	TVRAM[0x80000];
extern	BYTE	TextDrawWork[1024*1024];
extern	BYTE	TextDirtyLine[1024];
//extern	WORD	Text_LineBuf[1024];
extern	BYTE	Text_TrFlag[1024];

void TVRAM_SetAllDirty(void);

void TVRAM_Init(void);
void TVRAM_Cleanup(void);

BYTE FASTCALL TVRAM_Read(DWORD adr);
void FASTCALL TVRAM_Write(DWORD adr, BYTE data);
void FASTCALL TVRAM_RCUpdate(void);
void FASTCALL Text_DrawLine(int opaq);

#endif
