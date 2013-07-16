#ifndef _winx68k_sasi
#define _winx68k_sasi

#include "common.h"

void SASI_Init(void);
BYTE FASTCALL SASI_Read(DWORD adr);
void FASTCALL SASI_Write(DWORD adr, BYTE data);
int SASI_IsReady(void);

extern char SASI_Name[16][MAX_PATH];

#endif //_winx68k_sasi
