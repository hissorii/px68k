// ---------------------------------------------------------------------------------------
//  MFP.C - MFP (Multi-Function Periferal)
// ---------------------------------------------------------------------------------------

#include "mfp.h"
#include "irqh.h"
#include "crtc.h"
#include "m68000.h"
#include "winx68k.h"
#include "keyboard.h"

extern BYTE traceflag;
BYTE testflag=0;
BYTE LastKey = 0;

BYTE MFP[24];
BYTE Timer_TBO = 0;
static BYTE Timer_Reload[4] = {0, 0, 0, 0};
static int Timer_Tick[4] = {0, 0, 0, 0};
static const int Timer_Prescaler[8] = {1, 10, 25, 40, 125, 160, 250, 500};

// -----------------------------------------------------------------------
//   優先割り込みのチェックをし、該当ベクタを返す
// -----------------------------------------------------------------------
DWORD FASTCALL MFP_IntCallback(BYTE irq)
{
	BYTE flag;
	DWORD vect;
	int offset = 0;
	IRQH_IRQCallBack(irq);
	if (irq!=6) return (DWORD)-1;
	for (flag=0x80, vect=15; flag; flag>>=1, vect--)
	{
		if ((MFP[MFP_IPRA]&flag)&&(MFP[MFP_IMRA]&flag)&&(!(MFP[MFP_ISRA]&flag)))
			break;
	}
	if (!flag)
	{
		offset = 1;
		for (flag=0x80, vect=7; flag; flag>>=1, vect--)
		{
			if ((MFP[MFP_IPRB]&flag)&&(MFP[MFP_IMRB]&flag)&&(!(MFP[MFP_ISRB]&flag)))
				break;
		}
	}
	if (!flag)
	{
		Error("MFP Int w/o Request. Default Vector(-1) has been returned.");
		return (DWORD)-1;
	}

	MFP[MFP_IPRA+offset] &= (~flag);
	if (MFP[MFP_VR]&8)
		MFP[MFP_ISRA+offset] |= flag;
	vect |= (MFP[MFP_VR]&0xf0);
	for (flag=0x80; flag; flag>>=1)
	{
		if ((MFP[MFP_IPRA]&flag)&&(MFP[MFP_IMRA]&flag)&&(!(MFP[MFP_ISRA]&flag)))
		{
			IRQH_Int(6, &MFP_IntCallback);
			break;
		}
		if ((MFP[MFP_IPRB]&flag)&&(MFP[MFP_IMRB]&flag)&&(!(MFP[MFP_ISRB]&flag)))
		{
			IRQH_Int(6, &MFP_IntCallback);
			break;
		}
	}
	return vect;
}


// -----------------------------------------------------------------------
//   割り込みが取り消しになってないか調べます
// -----------------------------------------------------------------------
void MFP_RecheckInt(void)
{
	BYTE flag;
	IRQH_IRQCallBack(6);
	for (flag=0x80; flag; flag>>=1)
	{
		if ((MFP[MFP_IPRA]&flag)&&(MFP[MFP_IMRA]&flag)&&(!(MFP[MFP_ISRA]&flag)))
		{
			IRQH_Int(6, &MFP_IntCallback);
			break;
		}
		if ((MFP[MFP_IPRB]&flag)&&(MFP[MFP_IMRB]&flag)&&(!(MFP[MFP_ISRB]&flag)))
		{
			IRQH_Int(6, &MFP_IntCallback);
			break;
		}
	}
}


// -----------------------------------------------------------------------
//   割り込み発生
// -----------------------------------------------------------------------
void MFP_Int(int irq)		// 'irq' は 0が最優先（HSYNC/GPIP7）、15が最下位（ALARM）
{				// ベクタとは番号の振り方が逆になるので注意〜
	BYTE flag = 0x80;
	if (irq<8)
	{
		flag >>= irq;
		if (MFP[MFP_IERA]&flag)
		{
			MFP[MFP_IPRA] |= flag;
			if ((MFP[MFP_IMRA]&flag)&&(!(MFP[MFP_ISRA]&flag)))
			{
				IRQH_Int(6, &MFP_IntCallback);
			}
		}
	}
	else
	{
		irq -= 8;
		flag >>= irq;
		if (MFP[MFP_IERB]&flag)
		{
			MFP[MFP_IPRB] |= flag;
			if ((MFP[MFP_IMRB]&flag)&&(!(MFP[MFP_ISRB]&flag)))
			{
				IRQH_Int(6, &MFP_IntCallback);
			}
		}
	}
}


// -----------------------------------------------------------------------
//   初期化
// -----------------------------------------------------------------------
void MFP_Init(void)
{
	int i;
	static const BYTE initregs[24] = {
		0x7b, 0x06, 0x00, 0x18, 0x3e, 0x00, 0x00, 0x00,
		0x00, 0x18, 0x3e, 0x40, 0x08, 0x01, 0x77, 0x01,
		0x0d, 0xc8, 0x14, 0x00, 0x88, 0x01, 0x81, 0x00
	};
	memcpy(MFP, initregs, 24);
	for (i=0; i<4; i++) Timer_Tick[i] = 0;
}


// -----------------------------------------------------------------------
//   I/O Read
// -----------------------------------------------------------------------
BYTE FASTCALL MFP_Read(DWORD adr)
{
	BYTE reg;
	BYTE ret = 0;
	int hpos;

	if (adr>0xe8802f) return ret;		// ばすえらー？

	if (adr&1)
	{
		reg=(BYTE)((adr&0x3f)>>1);
		switch(reg)
		{
		case MFP_GPIP:
			if ( (vline>=CRTC_VSTART)&&(vline<CRTC_VEND) )
				ret = 0x13;
			else
				ret = 0x03;
			hpos = (int)(ICount%HSYNC_CLK);
			if ( (hpos>=((int)CRTC_Regs[5]*HSYNC_CLK/CRTC_Regs[1]))&&(hpos<((int)CRTC_Regs[7]*HSYNC_CLK/CRTC_Regs[1])) )
				ret &= 0x7f;
			else
				ret |= 0x80;
			if (vline!=CRTC_IntLine)
				ret |= 0x40;
			break;
		case MFP_UDR:
			ret = LastKey;
			KeyIntFlag = 0;
			break;
		case MFP_RSR:
			if (KeyBufRP!=KeyBufWP)
				ret = MFP[reg] & 0x7f;
			else
				ret = MFP[reg] | 0x80;
			break;
		default:
			ret = MFP[reg];
		}
		return ret;
	}
	else
		return 0xff;
}


// -----------------------------------------------------------------------
//   I/O Write
// -----------------------------------------------------------------------
void FASTCALL MFP_Write(DWORD adr, BYTE data)
{
	BYTE reg;
	if (adr>0xe8802f) return;
/*if(adr==0xe88009){
FILE* fp = fopen("_mfp.txt", "a");
fprintf(fp, "Write   Adr=$%08X  Data=$%02X   PC=$%08X\n", adr, data, regs.pc);
fclose(fp);
}*/
	if (adr&1)
	{
		reg=(BYTE)((adr&0x3f)>>1);

		switch(reg)
		{
		case MFP_IERA:
		case MFP_IERB:
			MFP[reg] = data;
			MFP[reg+2] &= data;  // 禁止されたものはIPRA/Bを落とす
			MFP_RecheckInt();
			break;
		case MFP_IPRA:
		case MFP_IPRB:
		case MFP_ISRA:
		case MFP_ISRB:
			MFP[reg] &= data;
			MFP_RecheckInt();
			break;
		case MFP_IMRA:
		case MFP_IMRB:
			MFP[reg] = data;
			MFP_RecheckInt();
			break;
		case MFP_TSR:
			MFP[reg] = data|0x80; // Txは常にEnableに
			break;
		case MFP_TADR:
			Timer_Reload[0] = MFP[reg] = data;
			break;
		case MFP_TACR:
			MFP[reg] = data;
			break;
		case MFP_TBDR:
			Timer_Reload[1] = MFP[reg] = data;
			break;
		case MFP_TBCR:
			MFP[reg] = data;
			if ( MFP[reg]&0x10 ) Timer_TBO = 0;
			break;
		case MFP_TCDR:
			Timer_Reload[2] = MFP[reg] = data;
			break;
		case MFP_TDDR:
			Timer_Reload[3] = MFP[reg] = data;
			break;
		case MFP_TCDCR:
			MFP[reg] = data;
			break;
		case MFP_UDR:
			break;
		default:
			MFP[reg] = data;
		}
	}
/*{
FILE* fp = fopen("_mfp.txt", "a");
fprintf(fp, "MFP Timer - A:$%08X TACR:$%02X TADR:$%02X\n", Timer_Count[0], MFP[MFP_TACR], MFP[MFP_TADR]);
fprintf(fp, "MFP Timer - B:$%08X TBCR:$%02X TBDR:$%02X\n", Timer_Count[1], MFP[MFP_TBCR], MFP[MFP_TBDR]);
fprintf(fp, "MFP Timer - C/D:$%08X $%08X TCDCR:$%02X TCDR:$%02X TDDR:$%02X\n", Timer_Count[2], Timer_Count[3], MFP[MFP_TCDCR], MFP[MFP_TCDR], MFP[MFP_TDDR]);
fclose(fp);
}*/
}


short timertrace = 0;
//static int TimerACounted = 0;
// -----------------------------------------------------------------------
//   たいまの時間を進める（も少し奇麗に書き直そう……）
// -----------------------------------------------------------------------
void FASTCALL MFP_Timer(long clock)
{
	if ( (!(MFP[MFP_TACR]&8))&&(MFP[MFP_TACR]&7) ) {
		int t = Timer_Prescaler[MFP[MFP_TACR]&7];
		Timer_Tick[0] += clock;
		while ( Timer_Tick[0]>=t ) {
			Timer_Tick[0] -= t;
			MFP[MFP_TADR]--;
			if ( !MFP[MFP_TADR] ) {
				MFP[MFP_TADR] = Timer_Reload[0];
				MFP_Int(2);
			}
		}
	}

	if ( MFP[MFP_TBCR]&7 ) {
		int t = Timer_Prescaler[MFP[MFP_TBCR]&7];
		Timer_Tick[1] += clock;
		while ( Timer_Tick[1]>=t ) {
			Timer_Tick[1] -= t;
			MFP[MFP_TBDR]--;
			if ( !MFP[MFP_TBDR] ) {
				MFP[MFP_TBDR] = Timer_Reload[1];
				MFP_Int(7);
			}
		}
	}

	if ( MFP[MFP_TCDCR]&0x70 ) {
		int t = Timer_Prescaler[(MFP[MFP_TCDCR]&0x70)>>4];
		Timer_Tick[2] += clock;
		while ( Timer_Tick[2]>=t ) {
			Timer_Tick[2] -= t;
			MFP[MFP_TCDR]--;
			if ( !MFP[MFP_TCDR] ) {
				MFP[MFP_TCDR] = Timer_Reload[2];
				MFP_Int(10);
			}
		}
	}

	if ( MFP[MFP_TCDCR]&7 ) {
		int t = Timer_Prescaler[MFP[MFP_TCDCR]&7];
		Timer_Tick[3] += clock;
		while ( Timer_Tick[3]>=t ) {
			Timer_Tick[3] -= t;
			MFP[MFP_TDDR]--;
			if ( !MFP[MFP_TDDR] ) {
				MFP[MFP_TDDR] = Timer_Reload[3];
				MFP_Int(11);
			}
		}
	}
}


void FASTCALL MFP_TimerA(void)
{
	if ( (MFP[MFP_TACR]&15)==8 ) {					// いべんとかうんともーど（VDispでカウント）
		if ( MFP[MFP_AER]&0x10 ) {				// VDisp割り込みとタイミングが違ってるのが気になるといえば気になる（ぉぃ
			if (vline==CRTC_VSTART) MFP[MFP_TADR]--;	// 本来は同じだと思うんだけどなぁ……それじゃ動かんし（汗
		} else {
			if ( CRTC_VEND>=VLINE_TOTAL ) {
				if ( (long)vline==(VLINE_TOTAL-1) ) MFP[MFP_TADR]--;	// 表示期間の終わりでカウントらしひ…（ろーどす）
			} else {
				if ( vline==CRTC_VEND ) MFP[MFP_TADR]--;
			}
		}
		if ( !MFP[MFP_TADR] ) {
			MFP[MFP_TADR] = Timer_Reload[0];
			MFP_Int(2);
		}
	}
}
