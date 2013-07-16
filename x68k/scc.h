#ifndef _winx68k_scc
#define _winx68k_scc

#include "common.h"

void SCC_IntCheck(void);
void SCC_Init(void);
BYTE FASTCALL SCC_Read(DWORD adr);
void FASTCALL SCC_Write(DWORD adr, BYTE data);

extern signed char MouseX;
extern signed char MouseY;
extern BYTE MouseSt;

#endif
