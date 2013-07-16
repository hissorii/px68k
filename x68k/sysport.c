// ---------------------------------------------------------------------------------------
//  SYSPORT.C - X68k System Port
// ---------------------------------------------------------------------------------------

#include "common.h"
#include "prop.h"
#include "sysport.h"
#include "palette.h"

BYTE	SysPort[7];

// -----------------------------------------------------------------------
//   初期化
// -----------------------------------------------------------------------
void SysPort_Init(void)
{
	int i;
	for (i=0; i<7; i++) SysPort[i]=0;
}


// -----------------------------------------------------------------------
//   らいと
// -----------------------------------------------------------------------
void FASTCALL SysPort_Write(DWORD adr, BYTE data)
{
	switch(adr)
	{
	case 0xe8e001:
		if (SysPort[1]!=(data&15))
		{
			SysPort[1] = data & 15;
			Pal_ChangeContrast(SysPort[1]);
		}
		break;
	case 0xe8e003:
		SysPort[2] = data & 0x0b;
		break;
	case 0xe8e005:
		SysPort[3] = data & 0x1f;
		break;
	case 0xe8e007:
		SysPort[4] = data & 0x0e;
		break;
	case 0xe8e00d:
		SysPort[5] = data;
		break;
	case 0xe8e00f:
		SysPort[6] = data & 15;
		break;
	}
}


// -----------------------------------------------------------------------
//   りーど
// -----------------------------------------------------------------------
BYTE FASTCALL SysPort_Read(DWORD adr)
{
	BYTE ret=0xff;

	switch(adr)
	{
	case 0xe8e001:
		ret = SysPort[1];
		break;
	case 0xe8e003:
		ret = SysPort[2];
		break;
	case 0xe8e005:
		ret = SysPort[3];
		break;
	case 0xe8e007:
		ret = SysPort[4];
		break;
	case 0xe8e00b:		// 10MHz:0xff、16MHz:0xfe、030(25MHz):0xdcをそれぞれ返すらしい
		switch(Config.XVIMode)
		{
		case 1:			// XVI or RedZone
		case 2:
			ret = 0xfe;
			break;
		case 3:			// 030
			ret = 0xdc;
			break;
		default:		// 10MHz
			ret = 0xff;
			break;
		}
		break;
	case 0xe8e00d:
		ret = SysPort[5];
		break;
	case 0xe8e00f:
		ret = SysPort[6];
		break;
	}

	return ret;
}
