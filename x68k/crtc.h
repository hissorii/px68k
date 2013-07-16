#ifndef _winx68k_crtc
#define _winx68k_crtc

#include "common.h"

#define	VSYNC_HIGH	180310L
#define	VSYNC_NORM	162707L

extern	BYTE	CRTC_Regs[48];
extern	BYTE	CRTC_Mode;
extern	WORD	CRTC_VSTART, CRTC_VEND;
extern	WORD	CRTC_HSTART, CRTC_HEND;
extern	DWORD	TextDotX, TextDotY;
extern	DWORD	TextScrollX, TextScrollY;
extern	BYTE	VCReg0[2];
extern	BYTE	VCReg1[2];
extern	BYTE	VCReg2[2];
extern	WORD	CRTC_IntLine;
extern	BYTE	CRTC_FastClr;
extern	BYTE	CRTC_DispScan;
extern	DWORD	CRTC_FastClrLine;
extern	WORD	CRTC_FastClrMask;
extern	BYTE	CRTC_VStep;
extern  int		HSYNC_CLK;

extern	DWORD	GrphScrollX[];
extern	DWORD	GrphScrollY[];

void CRTC_Init(void);

void CRTC_RasterCopy(void);

BYTE FASTCALL CRTC_Read(DWORD adr);
void FASTCALL CRTC_Write(DWORD adr, BYTE data);

BYTE FASTCALL VCtrl_Read(DWORD adr);
void FASTCALL VCtrl_Write(DWORD adr, BYTE data);

#endif
