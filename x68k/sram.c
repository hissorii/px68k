// ---------------------------------------------------------------------------------------
//  SRAM.C - SRAM (16kb) 領域
// ---------------------------------------------------------------------------------------

#include	"common.h"
#include	"fileio.h"
#include	"prop.h"
#include	"winx68k.h"
#include	"sysport.h"
#include	"memory.h"
#include	"sram.h"

	BYTE	SRAM[0x4000];
	BYTE	SRAMFILE[] = "sram.dat";


// -----------------------------------------------------------------------
//   役に立たないうぃるすチェック
// -----------------------------------------------------------------------
void SRAM_VirusCheck(void)
{
	//int i, ret;

	if (!Config.SRAMWarning) return;				// Warning発生モードでなければ帰る

	if ( (cpu_readmem24_dword(0xed3f60)==0x60000002)
	   &&(cpu_readmem24_dword(0xed0010)==0x00ed3f60) )		// 特定うぃるすにしか効かないよ〜
	{
#if 0 /* XXX */
		ret = MessageBox(hWndMain,
			"このSRAMデータはウィルスに感染している可能性があります。\n該当個所のクリーンアップを行いますか？",
			"けろぴーからの警告", MB_ICONWARNING | MB_YESNO);
		if (ret == IDYES)
		{
			for (i=0x3c00; i<0x4000; i++)
				SRAM[i] = 0;
			SRAM[0x11] = 0x00;
			SRAM[0x10] = 0xed;
			SRAM[0x13] = 0x01;
			SRAM[0x12] = 0x00;
			SRAM[0x19] = 0x00;
		}
#endif /* XXX */
		SRAM_Cleanup();
		SRAM_Init();			// Virusクリーンアップ後のデータを書き込んでおく
	}
}


// -----------------------------------------------------------------------
//   初期化
// -----------------------------------------------------------------------
void SRAM_Init(void)
{
	int i;
	BYTE tmp;
	FILEH fp;

	for (i=0; i<0x4000; i++)
		SRAM[i] = 0;

	fp = File_OpenCurDir(SRAMFILE);
	if (fp)
	{
		File_Read(fp, SRAM, 0x4000);
		File_Close(fp);
		for (i=0; i<0x4000; i+=2)
		{
			tmp = SRAM[i];
			SRAM[i] = SRAM[i+1];
			SRAM[i+1] = tmp;
		}
	}
}


// -----------------------------------------------------------------------
//   撤収〜
// -----------------------------------------------------------------------
void SRAM_Cleanup(void)
{
	int i;
	BYTE tmp;
	FILEH fp;

	for (i=0; i<0x4000; i+=2)
	{
		tmp = SRAM[i];
		SRAM[i] = SRAM[i+1];
		SRAM[i+1] = tmp;
	}

	fp = File_OpenCurDir(SRAMFILE);
	if (!fp)
		fp = File_CreateCurDir(SRAMFILE, FTYPE_SRAM);
	if (fp)
	{
		File_Write(fp, SRAM, 0x4000);
		File_Close(fp);
	}
}


// -----------------------------------------------------------------------
//   りーど
// -----------------------------------------------------------------------
BYTE FASTCALL SRAM_Read(DWORD adr)
{
	adr &= 0xffff;
	adr ^= 1;
	if (adr<0x4000)
		return SRAM[adr];
	else
		return 0xff;
}


// -----------------------------------------------------------------------
//   らいと
// -----------------------------------------------------------------------
void FASTCALL SRAM_Write(DWORD adr, BYTE data)
{
	//int ret;

	if ( (SysPort[5]==0x31)&&(adr<0xed4000) )
	{
		if ((adr==0xed0018)&&(data==0xb0))	// SRAM起動への切り替え（簡単なウィルス対策）
		{
			if (Config.SRAMWarning)		// Warning発生モード（デフォルト）
			{
#if 0 /* XXX */
				ret = MessageBox(hWndMain,
					"SRAMブートに切り替えようとしています。\nウィルスの危険がない事を確認してください。\nSRAMブートに切り替え、継続しますか？",
					"けろぴーからの警告", MB_ICONWARNING | MB_YESNO);
				if (ret != IDYES)
				{
					data = 0;	// STDブートにする
				}
#endif /* XXX */
			}
		}
		adr &= 0xffff;
		adr ^= 1;
		SRAM[adr] = data;
	}
}
