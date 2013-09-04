#ifndef _winx68k_memory
#define _winx68k_memory

#include "../x11/common.h"

#define	Memory_ReadB		cpu_readmem24
#define Memory_ReadW		cpu_readmem24_word
#define Memory_ReadD		cpu_readmem24_dword

#define	Memory_WriteB		cpu_writemem24
#define Memory_WriteW		cpu_writemem24_word
#define Memory_WriteD		cpu_writemem24_dword

extern	BYTE*	IPL;
extern	BYTE*	MEM;
extern	BYTE*	OP_ROM;
extern	BYTE*	FONT;
extern  BYTE    SCSIIPL[0x2000];
extern  BYTE    SRAM[0x4000];
extern  BYTE    GVRAM[0x80000];
extern  BYTE   TVRAM[0x80000];


extern	DWORD	BusErrFlag;
extern	DWORD	BusErrAdr;
extern	DWORD	MemByteAccess;

void FASTCALL Memory_ErrTrace(void);
void FASTCALL Memory_IntErr(int i);

void Memory_Init(void);
BYTE FASTCALL Memory_ReadB(DWORD adr);
WORD FASTCALL Memory_ReadW(DWORD adr);
DWORD FASTCALL Memory_ReadD(DWORD adr);

BYTE FASTCALL dma_readmem24(DWORD adr);
WORD FASTCALL dma_readmem24_word(DWORD adr);
DWORD FASTCALL dma_readmem24_dword(DWORD adr);

void FASTCALL Memory_WriteB(DWORD adr, BYTE data);
void FASTCALL Memory_WriteW(DWORD adr, WORD data);
void FASTCALL Memory_WriteD(DWORD adr, DWORD data);

void FASTCALL dma_writemem24(DWORD adr, BYTE data);
void FASTCALL dma_writemem24_word(DWORD adr, WORD data);
void FASTCALL dma_writemem24_dword(DWORD adr, DWORD data);

void FASTCALL cpu_setOPbase24(DWORD adr);

void FASTCALL Memory_SetSCSIMode(void);

#endif
