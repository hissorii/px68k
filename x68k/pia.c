// ---------------------------------------------------------------------------------------
//  PIA.C - uPD8255（必要最低限のみ）
// ---------------------------------------------------------------------------------------

#include "common.h"
#include "joystick.h"
#include "pia.h"
#include "adpcm.h"
#include "m68000.h"

typedef struct {
	BYTE PortA;
	BYTE PortB;
	BYTE PortC;
	BYTE Ctrl;
} PIA;

static PIA pia;

// -----------------------------------------------------------------------
//   初期化
// -----------------------------------------------------------------------
void PIA_Init(void)
{
	pia.PortA = 0xff;
	pia.PortB = 0xff;
	pia.PortC = 0x0b;
	pia.Ctrl = 0;
}


// -----------------------------------------------------------------------
//   I/O Write
// -----------------------------------------------------------------------
void FASTCALL PIA_Write(DWORD adr, BYTE data)
{
	BYTE mask, bit, portc = pia.PortC;
	if ( adr==0xe9a005 ) {
		portc = pia.PortC;
		pia.PortC = data;
		if ( (portc&0x0f)!=(pia.PortC&0x0f) ) ADPCM_SetPan(pia.PortC&0x0f);
		if ( (portc&0x10)!=(pia.PortC&0x10) ) Joystick_Write(0, (BYTE)((data&0x10)?0xff:0x00));
		if ( (portc&0x20)!=(pia.PortC&0x20) ) Joystick_Write(1, (BYTE)((data&0x20)?0xff:0x00));
	} else if ( adr==0xe9a007 ) {
		if ( !(data&0x80) ) {
			portc = pia.PortC;
			bit = (data>>1)&7;
			mask = 1<<bit;
			if ( data&1 )
				pia.PortC |= mask;
			else
				pia.PortC &= ~mask;
			if ( (portc&0x0f)!=(pia.PortC&0x0f) ) ADPCM_SetPan(pia.PortC&0x0f);
			if ( (portc&0x10)!=(pia.PortC&0x10) ) Joystick_Write(0, (BYTE)((data&1)?0xff:0x00));
			if ( (portc&0x20)!=(pia.PortC&0x20) ) Joystick_Write(1, (BYTE)((data&1)?0xff:0x00));
		}
	} else if ( adr==0xe9a001 ) {
		Joystick_Write(0, data);
	} else if (adr == 0xe9a003) {
		Joystick_Write(1, data);
	}
}


// -----------------------------------------------------------------------
//   I/O Read
// -----------------------------------------------------------------------
BYTE FASTCALL PIA_Read(DWORD adr)
{
	BYTE ret=0xff;
	if ( adr==0xe9a001 )
		ret = Joystick_Read(0);
	if ( adr==0xe9a003 )
		ret = Joystick_Read(1);
	if ( adr==0xe9a005 )
		ret = pia.PortC;
	return ret;
}
