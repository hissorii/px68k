#ifndef _winx68k_sysport
#define _winx68k_sysport

#include "common.h"

extern	BYTE	SysPort[7];

void SysPort_Init(void);
BYTE FASTCALL SysPort_Read(DWORD adr);
void FASTCALL SysPort_Write(DWORD adr, BYTE data);

#endif
