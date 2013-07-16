// ---------------------------------------------------------------------------------------
//  SASI.C - Shugart Associates System Interface (SASI HDD)
// ---------------------------------------------------------------------------------------

#include "common.h"
#include "fileio.h"
#include "prop.h"
#include "status.h"
#include "m68000.h"
#include "ioc.h"
#include "sasi.h"
#include "irqh.h"

BYTE SASI_Buf[256];
BYTE SASI_Phase = 0;
DWORD SASI_Sector = 0;
DWORD SASI_Blocks = 0;
BYTE SASI_Cmd[6];
BYTE SASI_CmdPtr = 0;
WORD SASI_Device = 0;
BYTE SASI_Unit = 0;
short SASI_BufPtr = 0;
BYTE SASI_RW = 0;
BYTE SASI_Stat = 0;
BYTE SASI_Mes = 0;
BYTE SASI_Error = 0;
BYTE SASI_SenseStatBuf[4];
BYTE SASI_SenseStatPtr = 0;

extern int hddtrace;


int SASI_IsReady(void)
{
	if ( (SASI_Phase==2)||(SASI_Phase==3)||(SASI_Phase==9) )
		return 1;
	else
		return 0;
}


// -----------------------------------------------------------------------
//   わりこみ〜
// -----------------------------------------------------------------------
DWORD FASTCALL SASI_Int(BYTE irq)
{
	IRQH_IRQCallBack(irq);
if (hddtrace) {
FILE *fp;
fp=fopen("_trace68.txt", "a");
fprintf(fp, "Int (IRQ:%d)\n", irq);
fclose(fp);
}
	if (irq==1)
		return ((DWORD)IOC_IntVect+2);
	else
		return -1;
}


// -----------------------------------------------------------------------
//   初期化
// -----------------------------------------------------------------------
void SASI_Init(void)
{
	SASI_Phase = 0;
	SASI_Sector = 0;
	SASI_Blocks = 0;
	SASI_CmdPtr = 0;
	SASI_Device = 0;
	SASI_Unit = 0;
	SASI_BufPtr = 0;
	SASI_RW = 0;
	SASI_Stat = 0;
	SASI_Error = 0;
	SASI_SenseStatPtr = 0;
}


// -----------------------------------------------------------------------
//   し−く（リード時）
// -----------------------------------------------------------------------
short SASI_Seek(void)
{
	FILEH fp;

if (hddtrace) {
FILE *fp;
fp=fopen("_trace68.txt", "a");
fprintf(fp, "Seek  - Sector:%d  (Time:%08X)\n", SASI_Sector, timeGetTime());
fclose(fp);
}
	ZeroMemory(SASI_Buf, 256);
	fp = File_Open(Config.HDImage[SASI_Device*2+SASI_Unit]);
	if (!fp)
	{
		ZeroMemory(SASI_Buf, 256);
		return -1;
	}
	if (File_Seek(fp, SASI_Sector<<8, FSEEK_SET)!=(SASI_Sector<<8)) 
	{
		File_Close(fp);
		return 0;
	}
	if (File_Read(fp, SASI_Buf, 256)!=256)
	{
		File_Close(fp);
		return 0;
	}
	File_Close(fp);

	return 1;
}


// -----------------------------------------------------------------------
//   しーく（ライト時）
// -----------------------------------------------------------------------
short SASI_Flush(void)
{	FILEH fp;

	fp = File_Open(Config.HDImage[SASI_Device*2+SASI_Unit]);
	if (!fp) return -1;
	if (File_Seek(fp, SASI_Sector<<8, FSEEK_SET)!=(SASI_Sector<<8))
	{
		File_Close(fp);
		return 0;
	}
	if (File_Write(fp, SASI_Buf, 256)!=256)
	{
		File_Close(fp);
		return 0;
	}
	File_Close(fp);

if (hddtrace) {
FILE *fp;
fp=fopen("_trace68.txt", "a");
fprintf(fp, "Sec Write  - Sector:%d  (Time:%08X)\n", SASI_Sector, timeGetTime());
fclose(fp);
}
	return 1;
}


// -----------------------------------------------------------------------
//   I/O Read
// -----------------------------------------------------------------------
BYTE FASTCALL SASI_Read(DWORD adr)
{
	BYTE ret = 0;
	short result;

	if (adr==0xe96003)
	{
		if (SASI_Phase)
			ret |= 2;		// Busy
		if (SASI_Phase>1)
			ret |= 1;		// Req
		if (SASI_Phase==2)
			ret |= 8;		// C/D
		if ((SASI_Phase==3)&&(SASI_RW))	// SASI_RW=1:Read
			ret |= 4;		// I/O
		if (SASI_Phase==9)		// Phase=9:SenseStatus中
			ret |= 4;		// I/O
		if ((SASI_Phase==4)||(SASI_Phase==5))
			ret |= 0x0c;		// I/O & C/D
		if (SASI_Phase==5)
			ret |= 0x10;		// MSG
	}
	else if (adr ==0xe96001)
	{
		if ((SASI_Phase==3)&&(SASI_RW))	// データリード中〜
		{
			ret = SASI_Buf[SASI_BufPtr++];
			if (SASI_BufPtr==256)
			{
				SASI_Blocks--;
				if (SASI_Blocks)		// まだ読むブロックがある？
				{
					SASI_Sector++;
					SASI_BufPtr = 0;
					result = SASI_Seek();	// 次のセクタをバッファに読む
					if (!result)		// result=0：イメージの最後（＝無効なセクタ）なら
					{
						SASI_Error = 0x0f;
						SASI_Phase++;
					}
				}
				else
					SASI_Phase++;		// 指定ブロックのリード完了
			}
		}
		else if (SASI_Phase==4)				// Status Phase
		{
			if (SASI_Error)
				ret = 0x02;
			else
				ret = SASI_Stat;
			SASI_Phase++;
		}
		else if (SASI_Phase==5)				// MessagePhase
		{
			SASI_Phase = 0;				// 0を返すだけ〜。BusFreeに帰ります
		}
		else if (SASI_Phase==9)				// DataPhase(SenseStat専用)
		{
			ret = SASI_SenseStatBuf[SASI_SenseStatPtr++];
			if (SASI_SenseStatPtr==4)
			{
				SASI_Error = 0;
				SASI_Phase = 4;				// StatusPhaseへ
			}
		}
		if (SASI_Phase==4)
		{
			IOC_IntStat|=0x10;
			if (IOC_IntStat&8) IRQH_Int(1, &SASI_Int);
		}
	}

if (hddtrace&&((SASI_Phase!=3)||(adr!=0xe96001))) {
FILE *fp;
fp=fopen("_trace68.txt", "a");
fprintf(fp, "Read  - Adr:%08X  Ret:%02X  Phase:%d BufPtr:%d  (Time:%08X)  @ $%08X\n", adr, ret, SASI_Phase, SASI_BufPtr, timeGetTime(), regs.pc);
fclose(fp);
}

	StatBar_HDD((SASI_Phase)?2:0);

	return ret;
}


// コマンドのチェック。正直、InsideX68k内の記述ではちと足りない ^^;。
// 未記述のものとして、
//   - C2h（初期化系？）。Unit以外のパラメータは無し。DataPhaseで10個のデータを書きこむ。
//   - 06h（フォーマット？）。論理ブロック指定あり（21hおきに指定している）。ブロック数のとこは6が指定されている。
void SASI_CheckCmd(void)
{
	short result;
	SASI_Unit = (SASI_Cmd[1]>>5)&1;			// X68kでは、ユニット番号は0か1しか取れない

	switch(SASI_Cmd[0])
	{
	case 0x00:					// Test Drive Ready
		if (Config.HDImage[SASI_Device*2+SASI_Unit][0])
			SASI_Stat = 0;
		else
		{
			SASI_Stat = 0x02;
			SASI_Error = 0x7f;
		}
		SASI_Phase += 2;
		break;
	case 0x01:					// Recalibrate
		if (Config.HDImage[SASI_Device*2+SASI_Unit][0])
		{
			SASI_Sector = 0;
			SASI_Stat = 0;
		}
		else
		{
			SASI_Stat = 0x02;
			SASI_Error = 0x7f;
		}
		SASI_Phase += 2;
		break;
	case 0x03:					// Request Sense Status
		SASI_SenseStatBuf[0] = SASI_Error;
		SASI_SenseStatBuf[1] = (BYTE)((SASI_Unit<<5)|((SASI_Sector>>16)&0x1f));
		SASI_SenseStatBuf[2] = (BYTE)(SASI_Sector>>8);
		SASI_SenseStatBuf[3] = (BYTE)SASI_Sector;
		SASI_Error = 0;
		SASI_Phase=9;
		SASI_Stat = 0;
		SASI_SenseStatPtr = 0;
		break;
	case 0x04:					// Format Drive
		SASI_Phase += 2;
		SASI_Stat = 0;
		break;
	case 0x08:					// Read Data
		SASI_Sector = (((DWORD)SASI_Cmd[1]&0x1f)<<16)|(((DWORD)SASI_Cmd[2])<<8)|((DWORD)SASI_Cmd[3]);
		SASI_Blocks = (DWORD)SASI_Cmd[4];
		SASI_Phase++;
		SASI_RW = 1;
		SASI_BufPtr = 0;
		SASI_Stat = 0;
		result = SASI_Seek();
		if ( (result==0)||(result==-1) )
		{
//			SASI_Phase++;
			SASI_Error = 0x0f;
		}
		break;
	case 0x0a:					// Write Data
		SASI_Sector = (((DWORD)SASI_Cmd[1]&0x1f)<<16)|(((DWORD)SASI_Cmd[2])<<8)|((DWORD)SASI_Cmd[3]);
		SASI_Blocks = (DWORD)SASI_Cmd[4];
		SASI_Phase++;
		SASI_RW = 0;
		SASI_BufPtr = 0;
		SASI_Stat = 0;
		ZeroMemory(SASI_Buf, 256);
		result = SASI_Seek();
		if ( (result==0)||(result==-1) )
		{
//			SASI_Phase++;
			SASI_Error = 0x0f;
		}
		break;
	case 0x0b:					// Seek
		if (Config.HDImage[SASI_Device*2+SASI_Unit][0])
		{
			SASI_Stat = 0;
		}
		else
		{
			SASI_Stat = 0x02;
			SASI_Error = 0x7f;
		}
		SASI_Phase += 2;
//		SASI_Phase = 9;
		break;
	case 0xc2:
		SASI_Phase = 10;
		SASI_SenseStatPtr = 0;
		if (Config.HDImage[SASI_Device*2+SASI_Unit][0])
			SASI_Stat = 0;
		else
		{
			SASI_Stat = 0x02;
			SASI_Error = 0x7f;
		}
		break;
	default:
		SASI_Phase += 2;
	}
if (hddtrace) {
FILE *fp;
fp=fopen("_trace68.txt", "a");
fprintf(fp, "Com.  - %02X  Dev:%d Unit:%d\n", SASI_Cmd[0], SASI_Device, SASI_Unit);
fclose(fp);
}
}


// -----------------------------------------------------------------------
//   I/O Write
// -----------------------------------------------------------------------
void FASTCALL SASI_Write(DWORD adr, BYTE data)
{
	short result;
	int i;
	BYTE bit;

if (hddtrace&&((SASI_Phase!=3)||(adr!=0xe96001))) {
FILE *fp;
fp=fopen("_trace68.txt", "a");
fprintf(fp, "Write - Adr:%08X Data:%02X  Phase:%d  (Time:%08X)  @ $%08X\n", adr, data, SASI_Phase, timeGetTime(), regs.pc);
fclose(fp);
}
	if ( (adr==0xe96007)&&(SASI_Phase==0) )
	{
		SASI_Device = 0x7f;
		if (data)
		{
			for (i=0, bit=1; bit; i++, bit<<=1)
			{
				if (data&bit)
				{
					SASI_Device = i;
					break;
				}
			}
		}
		if ( (Config.HDImage[SASI_Device*2][0])||(Config.HDImage[SASI_Device*2+1][0]) )
		{
			SASI_Phase++;
			SASI_CmdPtr = 0;
		}
		else
		{
			SASI_Phase = 0;
		}
	}
	else if ( (adr==0xe96003)&&(SASI_Phase==1) )
	{
		SASI_Phase++;
	}
	else if (adr==0xe96005)						// SASI Reset
	{
		SASI_Phase = 0;
		SASI_Sector = 0;
		SASI_Blocks = 0;
		SASI_CmdPtr = 0;
		SASI_Device = 0;
		SASI_Unit = 0;
		SASI_BufPtr = 0;
		SASI_RW = 0;
		SASI_Stat = 0;
		SASI_Error = 0;
		SASI_SenseStatPtr = 0;
	}
	else if (adr==0xe96001)
	{
		if (SASI_Phase==2)
		{
			SASI_Cmd[SASI_CmdPtr++] = data;
			if (SASI_CmdPtr==6)			// コマンド発行終了
			{
//				SASI_Phase++;
				SASI_CheckCmd();
			}
		}
		else if ((SASI_Phase==3)&&(!SASI_RW))		// データライト中〜
		{
			SASI_Buf[SASI_BufPtr++] = data;
			if (SASI_BufPtr==256)
			{
				result = SASI_Flush();		// 現在のバッファを書き出す
				SASI_Blocks--;
				if (SASI_Blocks)		// まだ書くブロックがある？
				{
					SASI_Sector++;
					SASI_BufPtr = 0;
					result = SASI_Seek();	// 次のセクタをバッファに読む
					if (!result)		// result=0：イメージの最後（＝無効なセクタ）なら
					{
						SASI_Error = 0x0f;
						SASI_Phase++;
					}
				}
				else
					SASI_Phase++;		// 指定ブロックのライト完了
			}
		}
		else if (SASI_Phase==10)
		{
			SASI_SenseStatPtr++;
			if (SASI_SenseStatPtr==10)			// コマンド発行終了
			{
				SASI_Phase = 4;
			}
		}
		if (SASI_Phase==4)
		{
			IOC_IntStat|=0x10;
			if (IOC_IntStat&8) IRQH_Int(1, &SASI_Int);
		}
	}
	StatBar_HDD((SASI_Phase)?2:0);
}
