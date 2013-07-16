// ---------------------------------------------------------------------------------------
//  PALETTE.C - Text/BG/Graphic Palette
// ---------------------------------------------------------------------------------------

#include	"common.h"
#include	"windraw.h"
#include	"tvram.h"
#include	"bg.h"
#include	"crtc.h"
#include	"memory.h"
#include	"m68000.h"
#include	"palette.h"

	BYTE	Pal_Regs[1024];
	WORD	TextPal[256];
	WORD	GrphPal[256];
	WORD	Pal16[65536];
	WORD	Ibit;				// 半透明処理とかで使うかも〜

	WORD	Pal_HalfMask, Pal_Ix2;
	WORD	Pal_R, Pal_G, Pal_B;		// 画面輝度変更時用

// ----- DDrawの16ビットモードの色マスクからX68k→Win用の変換テーブルを作る -----
// X68kは「GGGGGRRRRRBBBBBI」の構造。Winは「RRRRRGGGGGGBBBBB」の形が多いみたい。が、
// 違う場合もあるみたいなので計算してみやう。
void Pal_SetColor(void)
{
	WORD TempMask, bit;
	WORD R[5] = {0, 0, 0, 0, 0};
	WORD G[5] = {0, 0, 0, 0, 0};
	WORD B[5] = {0, 0, 0, 0, 0};
	int r, g, b, i;

	r = g = b = 5;
	Pal_R = Pal_G = Pal_B = 0;
	TempMask = 0;				// 使われているビットをチェック（Iビット用）
	for (bit=0x8000; bit; bit>>=1)
	{					// 各色毎に左（上位）から5ビットずつ拾う
		if ( (WinDraw_Pal16R&bit)&&(r) )
		{
			R[--r] = bit;
			TempMask |= bit;
			Pal_R |= bit;
		}
		if ( (WinDraw_Pal16G&bit)&&(g) )
		{
			G[--g] = bit;
			TempMask |= bit;
			Pal_G |= bit;
		}
		if ( (WinDraw_Pal16B&bit)&&(b) )
		{
			B[--b] = bit;
			TempMask |= bit;
			Pal_B |= bit;
		}
	}

	Ibit = 1;
	for (bit=1; bit; bit<<=1)
	{					// 使われていないビット（通常は6ビット以上ある色の
		if (!(TempMask&bit))		// 最下位ビット）をIビットとして扱う
		{				// 0のビットが複数だったときを考えて、Ibit=~TempMask; にはしてないです
			Ibit = bit;
			break;
		}
	}

	// ねこーねこー
	// Pal_Ix2 = 0 になったらどうしよう… その時は32bit拡張…

	// → Riva128なんかでは見事になるみたい（笑） でもそれでも特に問題無いかも…

	Pal_HalfMask = ~(B[0] | R[0] | G[0] | Ibit);
	Pal_Ix2 = Ibit << 1;

/*
	// どーしても変そうなら、これを入れるか…32bit拡張めんどくさいし（汗
	if (Ibit==0x8000)			// Ibitが最上位ビットだったら、青の最下位と入れ替え
	{
		Ibit = B[0];
		B[0] = 0;			// 青は4bitになっちゃうけど我慢して ^^;
		Pal_HalfMask = ~(B[1] | R[0] | G[0] | Ibit | 0x8000);
		Pal_Ix2 = Ibit << 1;
	}
*/
						// X68kのビット配置に合わせてテーブル作成
						// すっげ〜手際が悪いね（汗
	for (i=0; i<65536; i++)
	{
		bit = 0;
		if (i&0x8000) bit |= G[4];
		if (i&0x4000) bit |= G[3];
		if (i&0x2000) bit |= G[2];
		if (i&0x1000) bit |= G[1];
		if (i&0x0800) bit |= G[0];
		if (i&0x0400) bit |= R[4];
		if (i&0x0200) bit |= R[3];
		if (i&0x0100) bit |= R[2];
		if (i&0x0080) bit |= R[1];
		if (i&0x0040) bit |= R[0];
		if (i&0x0020) bit |= B[4];
		if (i&0x0010) bit |= B[3];
		if (i&0x0008) bit |= B[2];
		if (i&0x0004) bit |= B[1];
		if (i&0x0002) bit |= B[0];
		if (i&0x0001) bit |= Ibit;
		Pal16[i] = bit;
	}
}


// -----------------------------------------------------------------------
//   初期化
// -----------------------------------------------------------------------
void Pal_Init(void)
{
	ZeroMemory(Pal_Regs, 1024);
	ZeroMemory(TextPal, 512);
	ZeroMemory(GrphPal, 512);
	Pal_SetColor();
}


// -----------------------------------------------------------------------
//   I/O Read
// -----------------------------------------------------------------------
BYTE FASTCALL Pal_Read(DWORD adr)
{
	if (adr<0xe82400)
		return Pal_Regs[adr-0xe82000];
	else return 0xff;
}


// -----------------------------------------------------------------------
//   I/O Write
// -----------------------------------------------------------------------
void FASTCALL Pal_Write(DWORD adr, BYTE data)
{
	WORD pal;

	if (adr>=0xe82400) return;

	adr -= 0xe82000;
	if (Pal_Regs[adr] == data) return;

	if (adr<0x200)
	{
		Pal_Regs[adr] = data;
		TVRAM_SetAllDirty();
		pal = Pal_Regs[adr&0xfffe];
		pal = (pal<<8)+Pal_Regs[adr|1];
		GrphPal[adr/2] = Pal16[pal];
	}
	else if (adr<0x400)
	{
		if (MemByteAccess) return;		// TextPalはバイトアクセスは出来ないらしい（神戸恋愛物語）
		Pal_Regs[adr] = data;
		TVRAM_SetAllDirty();
		pal = Pal_Regs[adr&0xfffe];
		pal = (pal<<8)+Pal_Regs[adr|1];
		TextPal[(adr-0x200)/2] = Pal16[pal];
	}
}


// -----------------------------------------------------------------------
//   こんとらすと変更（パレットに対するWin側の表示色で実現してます ^^;）
// -----------------------------------------------------------------------
void Pal_ChangeContrast(int num)
{
	WORD bit;
	WORD R[5] = {0, 0, 0, 0, 0};
	WORD G[5] = {0, 0, 0, 0, 0};
	WORD B[5] = {0, 0, 0, 0, 0};
	int r, g, b, i;
	int palr, palg, palb;
	WORD pal;

	TVRAM_SetAllDirty();

	r = g = b = 5;

	for (bit=0x8000; bit; bit>>=1)
	{					// 各色毎に左（上位）から5ビットずつ拾う
		if ( (WinDraw_Pal16R&bit)&&(r) ) R[--r] = bit;
		if ( (WinDraw_Pal16G&bit)&&(g) ) G[--g] = bit;
		if ( (WinDraw_Pal16B&bit)&&(b) ) B[--b] = bit;
	}

	for (i=0; i<65536; i++)
	{
		palr = palg = palb = 0;
		if (i&0x8000) palg |= G[4];
		if (i&0x4000) palg |= G[3];
		if (i&0x2000) palg |= G[2];
		if (i&0x1000) palg |= G[1];
		if (i&0x0800) palg |= G[0];
		if (i&0x0400) palr |= R[4];
		if (i&0x0200) palr |= R[3];
		if (i&0x0100) palr |= R[2];
		if (i&0x0080) palr |= R[1];
		if (i&0x0040) palr |= R[0];
		if (i&0x0020) palb |= B[4];
		if (i&0x0010) palb |= B[3];
		if (i&0x0008) palb |= B[2];
		if (i&0x0004) palb |= B[1];
		if (i&0x0002) palb |= B[0];
		pal = palr | palb | palg;
		palg = (WORD)((palg*num)/15)&Pal_G;
		palr = (WORD)((palr*num)/15)&Pal_R;
		palb = (WORD)((palb*num)/15)&Pal_B;
		Pal16[i] = palr | palb | palg;
		if ((pal)&&(!Pal16[i])) Pal16[i] = B[0];
		if (i&0x0001) Pal16[i] |= Ibit;
	}

	for (i=0; i<256; i++)
	{
		pal = Pal_Regs[i*2];
		pal = (pal<<8)+Pal_Regs[i*2+1];
		GrphPal[i] = Pal16[pal];

		pal = Pal_Regs[i*2+512];
		pal = (pal<<8)+Pal_Regs[i*2+513];
		TextPal[i] = Pal16[pal];
	}
}
