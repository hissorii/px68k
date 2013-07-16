#ifndef _winx68k_dmac
#define _winx68k_dmac

#include "common.h"

typedef struct
{
	BYTE CSR;		// 00
	BYTE CER;
	BYTE dmy0[2];
	BYTE DCR;		// 04
	BYTE OCR;
	BYTE SCR;
	BYTE CCR;
	BYTE dmy1[2];	// 08
	WORD MTC;
	DWORD MAR;		// 0C
	BYTE dmy2[4];	// 10
	DWORD DAR;		// 14
	BYTE dmy3[2];	// 18
	WORD BTC;
	DWORD BAR;		// 1C
	BYTE dmy4[5];	// 20
	BYTE NIV;
	BYTE dmy5;
	BYTE EIV;
	BYTE dmy6;		// 28
	BYTE MFC;
	BYTE dmy7[3];
	BYTE CPR;
	BYTE dmy8[3];
	BYTE DFC;
	BYTE dmy9[7];
	BYTE BFC;
	BYTE dmya[5];
	BYTE GCR;

} dmac_ch;

extern dmac_ch	DMA[4];

DWORD FASTCALL DMA_Int(BYTE irq);
BYTE FASTCALL DMA_Read(DWORD adr);
void FASTCALL DMA_Write(DWORD adr, BYTE data);

int FASTCALL DMA_Exec(int ch);
void DMA_Init(void);
void DMA_SetReadyCB(int ch, int (*func)(void));

#endif //_winx68k_dmac
