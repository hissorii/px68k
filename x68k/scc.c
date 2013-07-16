// ---------------------------------------------------------------------------------------
//  SCC.C - Z8530 SCC（マウスのみ）
// ---------------------------------------------------------------------------------------

#include "common.h"
#include "scc.h"
#include "m68000.h"
#include "irqh.h"
#include "mouse.h"

signed char MouseX = 0;
signed char MouseY = 0;
BYTE MouseSt = 0;

BYTE SCC_RegsA[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
BYTE SCC_RegNumA = 0;
BYTE SCC_RegSetA = 0;
BYTE SCC_RegsB[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
BYTE SCC_RegNumB = 0;
BYTE SCC_RegSetB = 0;
BYTE SCC_Vector = 0;
BYTE SCC_Dat[3] = {0, 0, 0};
BYTE SCC_DatNum = 0;


// -----------------------------------------------------------------------
//   わりこみ
// -----------------------------------------------------------------------
DWORD FASTCALL SCC_Int(BYTE irq)
{
	DWORD ret = (DWORD)(-1);
	IRQH_IRQCallBack(irq);
	if ( (irq==5)&&(!(SCC_RegsB[9]&2)) )
	{
		if (SCC_RegsB[9]&1)
		{
			if (SCC_RegsB[9]&0x10)
				ret = ((DWORD)(SCC_Vector&0x8f)+0x20);
			else
				ret = ((DWORD)(SCC_Vector&0xf1)+4);
		}
		else
			ret = ((DWORD)SCC_Vector);
	}

	return ret;
}


// -----------------------------------------------------------------------
//   割り込みのチェック
// -----------------------------------------------------------------------
void SCC_IntCheck(void)
{
	if ( (SCC_DatNum) && ((SCC_RegsB[1]&0x18)==0x10) && (SCC_RegsB[9]&0x08) )
	{
		IRQH_Int(5, &SCC_Int);
	}
	else if ( (SCC_DatNum==3) && ((SCC_RegsB[1]&0x18)==0x08) && (SCC_RegsB[9]&0x08) )
	{
		IRQH_Int(5, &SCC_Int);
	}
}


// -----------------------------------------------------------------------
//   初期化
// -----------------------------------------------------------------------
void SCC_Init(void)
{
	MouseX = 0;
	MouseY = 0;
	MouseSt = 0;
	SCC_RegNumA = 0;
	SCC_RegSetA = 0;
	SCC_RegNumB = 0;
	SCC_RegSetB = 0;
	SCC_Vector = 0;
	SCC_DatNum = 0;
}


// -----------------------------------------------------------------------
//   I/O Write
// -----------------------------------------------------------------------
void FASTCALL SCC_Write(DWORD adr, BYTE data)
{
	if (adr>=0xe98008) return;

	if ((adr&7) == 1)
	{
		if (SCC_RegSetB)
		{
			if (SCC_RegNumB == 5)
			{
				if ( (!(SCC_RegsB[5]&2))&&(data&2)&&(SCC_RegsB[3]&1)&&(!SCC_DatNum) )	// データが無い時だけにしやう（闇の血族）
				{			// マウスデータ生成
					Mouse_SetData();
					SCC_DatNum = 3;
					SCC_Dat[2] = MouseSt;
					SCC_Dat[1] = MouseX;
					SCC_Dat[0] = MouseY;
				}
			}
			else if (SCC_RegNumB == 2) SCC_Vector = data;
			SCC_RegSetB = 0;
			SCC_RegsB[SCC_RegNumB] = data;
/*{
FILE *fp;
fp=fopen("_scc.txt", "a");
fprintf(fp, "SCC  Reg[%d] = $%02X  @ $%08X\n", SCC_RegNumB, data, regs.pc);
fclose(fp);
}*/
			SCC_RegNumB = 0;
		}
		else
		{
			if (!(data&0xf0))
			{
				data &= 15;
				SCC_RegSetB = 1;
				SCC_RegNumB = data;
			}
			else
			{
				SCC_RegSetB = 0;
				SCC_RegNumB = 0;
			}
		}
	}
	else if ((adr&7) == 5)
	{
		if (SCC_RegSetA)
		{
			SCC_RegSetA = 0;
			switch (SCC_RegNumA)
			{
			case 2:
				SCC_RegsB[2] = data;
				SCC_Vector = data;
				break;
			case 9:
				SCC_RegsB[9] = data;
				break;
			}
		}
		else
		{
			data &= 15;
			if (data)
			{
				SCC_RegSetA = 1;
				SCC_RegNumA = data;
			}
			else
			{
				SCC_RegSetA = 0;
				SCC_RegNumA = 0;
			}
		}
	}
	else if ((adr&7) == 3)
	{
	}
	else if ((adr&7) == 7)
	{
	}
}


// -----------------------------------------------------------------------
//   I/O Read
// -----------------------------------------------------------------------
BYTE FASTCALL SCC_Read(DWORD adr)
{
	BYTE ret=0;

	if (adr>=0xe98008) return ret;

	if ((adr&7) == 1)
	{
		if (!SCC_RegNumB)
			ret = ((SCC_DatNum)?1:0);
		SCC_RegNumB = 0;
		SCC_RegSetB = 0;
	}
	else if ((adr&7) == 3)
	{
		if (SCC_DatNum)
		{
			SCC_DatNum--;
			ret = SCC_Dat[SCC_DatNum];
		}
	}
	else if ((adr&7) == 5)
	{
		switch(SCC_RegNumA)
		{
		case 0:
			ret = 4;			// 送信バッファ空（Xna）
			break;
		case 3:
			ret = ((SCC_DatNum)?4:0);
			break;
		}
		SCC_RegNumA = 0;
		SCC_RegSetA = 0;
	}
	return ret;
}
