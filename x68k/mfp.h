#ifndef _winx68k_mfp
#define _winx68k_mfp

#include "common.h"

extern	BYTE MFP[24];

#define MFP_GPIP	0
#define MFP_AER		1
#define MFP_DDR		2
#define MFP_IERA	3
#define MFP_IERB	4
#define MFP_IPRA	5
#define MFP_IPRB	6
#define MFP_ISRA	7
#define MFP_ISRB	8
#define MFP_IMRA	9
#define MFP_IMRB	10
#define MFP_VR		11
#define MFP_TACR	12
#define MFP_TBCR	13
#define MFP_TCDCR	14
#define MFP_TADR	15
#define MFP_TBDR	16
#define MFP_TCDR	17
#define MFP_TDDR	18
#define MFP_SCR		19
#define MFP_UCR		20
#define MFP_RSR		21
#define MFP_TSR		22
#define MFP_UDR		23

//extern long Timer_Prescaler[8];
extern long Timer_Count[4];
extern BYTE LastKey;
//extern BYTE KeyReadFlag;

void MFP_Init(void);
BYTE FASTCALL MFP_Read(DWORD adr);
void FASTCALL MFP_Write(DWORD adr, BYTE data);
void FASTCALL MFP_Timer(long clock);
void FASTCALL MFP_TimerA(void);
void MFP_Int(int irq);

#endif //_winx68k_mfp
