#ifndef _winx68k_cdrom
#define _winx68k_cdrom

#include "common.h"

typedef struct {
	unsigned char reserved;
	unsigned char adr;
	unsigned char trackno;
	unsigned char reserved1;
	unsigned char addr[4];
} TOCENTRY;

typedef struct {
	unsigned char size[2];
	unsigned char first;
	unsigned char last;
	TOCENTRY track[100];
} TOC;

void CDROM_Init(void);
void CDROM_Cleanup(void);
BYTE FASTCALL CDROM_Read(DWORD adr);
void FASTCALL CDROM_Write(DWORD adr, BYTE data);

extern BYTE CDROM_ASPIChecked;

#endif

