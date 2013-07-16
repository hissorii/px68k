// ---------------------------------------------------------------------------------------
//  TVRAM.C - Text VRAM
//  ToDo : 透明色処理とか色々
// ---------------------------------------------------------------------------------------

#include	"common.h"
#include	"winx68k.h"
#include	"windraw.h"
#include	"bg.h"
#include	"crtc.h"
#include	"palette.h"
#include	"m68000.h"
#include	"tvram.h"

	BYTE	TVRAM[0x80000];
	BYTE	TextDrawWork[1024*1024];
	BYTE	TextDirtyLine[1024];

	BYTE	TextDrawPattern[2048*4];

//	WORD	Text_LineBuf[1024];	// →BGのを使うように変更
	BYTE	Text_TrFlag[1024];

INLINE void TVRAM_WriteByteMask(DWORD adr, BYTE data);

// -----------------------------------------------------------------------
//   全部書き換え〜
// -----------------------------------------------------------------------
void TVRAM_SetAllDirty(void)
{
	memset(TextDirtyLine, 1, 1024);
}


// -----------------------------------------------------------------------
//   初期化
// -----------------------------------------------------------------------
void TVRAM_Init(void)
{
	int i, j, bit;
	ZeroMemory(TVRAM, 0x80000);
	ZeroMemory(TextDrawWork, 1024*1024);
	TVRAM_SetAllDirty();

	ZeroMemory(TextDrawPattern, 2048*4);		// パターンテーブル初期化
	for (i=0; i<256; i++)
	{
		for (j=0, bit=0x80; j<8; j++, bit>>=1)
		{
			if (i&bit) {
				TextDrawPattern[i*8+j     ] = 1;
				TextDrawPattern[i*8+j+2048] = 2;
				TextDrawPattern[i*8+j+4096] = 4;
				TextDrawPattern[i*8+j+6144] = 8;
			}
		}
	}
}


// -----------------------------------------------------------------------
//   撤収
// -----------------------------------------------------------------------
void TVRAM_Cleanup(void)
{
}


// -----------------------------------------------------------------------
//   読むなり
// -----------------------------------------------------------------------
BYTE FASTCALL TVRAM_Read(DWORD adr)
{
	adr &= 0x7ffff;
	adr ^= 1;
	return TVRAM[adr];
}


// -----------------------------------------------------------------------
//   1ばいと書くなり
// -----------------------------------------------------------------------
INLINE void TVRAM_WriteByte(DWORD adr, BYTE data)
{
	if (TVRAM[adr]!=data)
	{
		TextDirtyLine[(((adr&0x1ffff)/128)-TextScrollY)&1023] = 1;
		TVRAM[adr] = data;
	}
}


// -----------------------------------------------------------------------
//   ますく付きで書くなり
// -----------------------------------------------------------------------
INLINE void TVRAM_WriteByteMask(DWORD adr, BYTE data)
{
	data = (TVRAM[adr] & CRTC_Regs[0x2e + ((adr^1) & 1)]) | (data & (~CRTC_Regs[0x2e + ((adr ^ 1) & 1)]));
	if (TVRAM[adr] != data)
	{
		TextDirtyLine[(((adr&0x1ffff)/128)-TextScrollY)&1023] = 1;
		TVRAM[adr] = data;
	}
}


// -----------------------------------------------------------------------
//   書くなり
// -----------------------------------------------------------------------
void FASTCALL TVRAM_Write(DWORD adr, BYTE data)
{
	adr &= 0x7ffff;
	adr ^= 1;
	if (CRTC_Regs[0x2a]&1)			// 同時アクセス
	{
		adr &= 0x1ffff;
		if (CRTC_Regs[0x2a]&2)		// Text Mask
		{
			if (CRTC_Regs[0x2b]&0x10) TVRAM_WriteByteMask(adr        , data);
			if (CRTC_Regs[0x2b]&0x20) TVRAM_WriteByteMask(adr+0x20000, data);
			if (CRTC_Regs[0x2b]&0x40) TVRAM_WriteByteMask(adr+0x40000, data);
			if (CRTC_Regs[0x2b]&0x80) TVRAM_WriteByteMask(adr+0x60000, data);
		}
		else
		{
			if (CRTC_Regs[0x2b]&0x10) TVRAM_WriteByte(adr        , data);
			if (CRTC_Regs[0x2b]&0x20) TVRAM_WriteByte(adr+0x20000, data);
			if (CRTC_Regs[0x2b]&0x40) TVRAM_WriteByte(adr+0x40000, data);
			if (CRTC_Regs[0x2b]&0x80) TVRAM_WriteByte(adr+0x60000, data);
		}
	}
	else					// シングルアクセス
	{
		if (CRTC_Regs[0x2a]&2)		// Text Mask
		{
			TVRAM_WriteByteMask(adr, data);
		}
		else
		{
			TVRAM_WriteByte(adr, data);
		}
	}
#ifdef USE_ASM
	_asm {
		push	edi
		push	esi

		mov	eax, adr
		mov	esi, eax
		and	esi, 01ffffh		; TVRAM Adr
		mov	edi, eax
		and	edi, 01ff80h		; 下位7bitマスク
		shl	edi, 3
		and	eax, 07fh
		xor	al, 1
		shl	eax, 3
		add	edi, eax		; edi = workadr

		xor	eax, eax

		mov	al, byte ptr TVRAM[esi+60000h]
		mov	ecx, dword ptr (TextDrawPattern+6144)[eax*8]
		mov	edx, dword ptr (TextDrawPattern+6144)[eax*8+4]
		mov	al, byte ptr TVRAM[esi+40000h]
		or	ecx, dword ptr (TextDrawPattern+4096)[eax*8]
		or	edx, dword ptr (TextDrawPattern+4096)[eax*8+4]
		mov	al, byte ptr TVRAM[esi+20000h]
		or	ecx, dword ptr (TextDrawPattern+2048)[eax*8]
		or	edx, dword ptr (TextDrawPattern+2048)[eax*8+4]
		mov	al, byte ptr TVRAM[esi]
		or	ecx, dword ptr TextDrawPattern[eax*8]
		or	edx, dword ptr TextDrawPattern[eax*8+4]
		mov	dword ptr TextDrawWork[edi], ecx
		mov	dword ptr (TextDrawWork+4)[edi], edx

		pop	esi
		pop	edi
	}
#elif defined(USE_GAS) && defined(__i386__)
	asm (
		"mov	%0, %%eax;"
		"mov	%%eax, %%esi;"
		"and	$0x1ffff, %%esi;"	/* TVRAM Adr */
		"mov	%%eax, %%edi;"
		"and	$0x1ff80, %%edi;"	/* 下位7bitマスク */
		"shl	$3, %%edi;"
		"and	$0x7f, %%eax;"
		"xor	$1, %%al;"
		"shl	$3, %%eax;"
		"add	%%eax, %%edi;"		/* edi = workadr */

		"xor	%%eax, %%eax;"

		"mov	TVRAM + 0x60000(%%esi), %%al;"
		"mov	TextDrawPattern + 6144(, %%eax, 8), %%ecx;"
		"mov	TextDrawPattern + 6144 + 4(, %%eax, 8), %%edx;"
		"mov	TVRAM + 0x40000(%%esi), %%al;"
		"or	TextDrawPattern + 4096(, %%eax, 8), %%ecx;"
		"or	TextDrawPattern + 4096 + 4(, %%eax, 8), %%edx;"
		"mov	TVRAM + 0x20000(%%esi), %%al;"
		"or	TextDrawPattern + 2048(, %%eax, 8), %%ecx;"
		"or	TextDrawPattern + 2048 + 4(, %%eax, 8), %%edx;"
		"mov	TVRAM(%%esi), %%al;"
		"or	TextDrawPattern(, %%eax, 8), %%ecx;"
		"or	TextDrawPattern + 4(, %%eax, 8), %%edx;"
		"mov	%%ecx, TextDrawWork(%%edi);"
		"mov	%%edx, TextDrawWork + 4(%%edi);"
	: /* output: nothing */
	: "m" (adr)
	: "ax", "cx", "dx", "si", "di", "memory");
#else /* !USE_ASM && !(USE_GAS && __i386__) */
	{
		DWORD *ptr = (DWORD *)TextDrawPattern;
		DWORD tvram_addr = adr & 0x1ffff;
		DWORD workadr = ((adr & 0x1ff80) + ((adr ^ 1) & 0x7f)) << 3;
		DWORD t0, t1;
		BYTE pat;

		pat = TVRAM[tvram_addr + 0x60000];
		t0 = ptr[(pat * 2) + 1536];
		t1 = ptr[(pat * 2 + 1) + 1536];

		pat = TVRAM[tvram_addr + 0x40000];
		t0 |= ptr[(pat * 2) + 1024];
		t1 |= ptr[(pat * 2 + 1) + 1024];

		pat = TVRAM[tvram_addr + 0x20000];
		t0 |= ptr[(pat * 2) + 512];
		t1 |= ptr[(pat * 2 + 1) + 512];

		pat = TVRAM[tvram_addr];
		t0 |= ptr[(pat * 2)];
		t1 |= ptr[(pat * 2 + 1)];

		*((DWORD *)&TextDrawWork[workadr]) = t0;
		*(((DWORD *)(&TextDrawWork[workadr])) + 1) = t1;
	}
#endif	/* USE_ASM */
}


// -----------------------------------------------------------------------
//   らすたこぴー時のあっぷでーと
// -----------------------------------------------------------------------
void FASTCALL TVRAM_RCUpdate(void)
{
	DWORD adr = ((DWORD)CRTC_Regs[0x2d]<<9);

#ifdef USE_ASM
	_asm
	{
		push	edi
		push	esi
		mov	esi, adr
		mov	edi, esi
		shl	edi, 3
		mov	esi, adr
		mov	cx, 512
		xor	eax, eax
	rcu_mainloop:
		xor	esi, 1
		mov	al, byte ptr TVRAM[esi+60000h]
		mov	ebx, dword ptr (TextDrawPattern+6144)[eax*8]
		mov	edx, dword ptr (TextDrawPattern+6144)[eax*8+4]
		mov	al, byte ptr TVRAM[esi+40000h]
		or	ebx, dword ptr (TextDrawPattern+4096)[eax*8]
		or	edx, dword ptr (TextDrawPattern+4096)[eax*8+4]
		mov	al, byte ptr TVRAM[esi+20000h]
		or	ebx, dword ptr (TextDrawPattern+2048)[eax*8]
		or	edx, dword ptr (TextDrawPattern+2048)[eax*8+4]
		mov	al, byte ptr TVRAM[esi]
		or	ebx, dword ptr TextDrawPattern[eax*8]
		or	edx, dword ptr TextDrawPattern[eax*8+4]
		mov	dword ptr TextDrawWork[edi], ebx
		add	edi, 4
		mov	dword ptr TextDrawWork[edi], edx
		add	edi, 4
		xor	esi, 1
		inc	esi
		dec	cx
		jnz	rcu_mainloop
//		loop	rcu_mainloop
		pop	esi
		pop	edi
	}
#elif defined(USE_GAS) && defined(__i386__)
	asm (
		"mov	%0, %%esi;"
		"mov	%%esi, %%edi;"
		"shl	$3, %%edi;"
		"mov	%0, %%esi;"
		"mov	$512, %%cx;"
		"xor	%%eax, %%eax;"
	".rcu_mainloop:"
		"xor	$1, %%esi;"
		"mov	TVRAM + 0x60000(%%esi), %%al;"
		"mov	TextDrawPattern + 6144(, %%eax, 8), %%ebx;"
		"mov	TextDrawPattern + 6144 + 4(, %%eax, 8), %%edx;"
		"mov	TVRAM + 0x40000(%%esi), %%al;"
		"or	TextDrawPattern + 4096(, %%eax, 8), %%ebx;"
		"or	TextDrawPattern + 4096 + 4(, %%eax, 8), %%edx;"
		"mov	TVRAM + 0x20000(%%esi), %%al;"
		"or	TextDrawPattern + 2048(, %%eax, 8), %%ebx;"
		"or	TextDrawPattern + 2048 + 4(, %%eax, 8), %%edx;"
		"mov	TVRAM(%%esi), %%al;"
		"or	TextDrawPattern(, %%eax, 8), %%ebx;"
		"or	TextDrawPattern + 4(, %%eax, 8), %%edx;"
		"mov	%%ebx, TextDrawWork(%%edi);"
		"add	$4, %%edi;"
		"mov	%%edx, TextDrawWork(%%edi);"
		"add	$4, %%edi;"
		"xor	$1, %%esi;"
		"inc	%%esi;"
		"loop	.rcu_mainloop;"
	: /* output: nothing */
	: "m" (adr)
	: "ax", "bx", "cx", "dx", "si", "di", "memory");
#else /* !USE_ASM && !(USE_GAS && __i386__) */
	/* XXX: BUG */
	DWORD *ptr = (DWORD *)TextDrawPattern;
	DWORD *wptr = (DWORD *)(TextDrawWork + (adr << 3));
	DWORD t0, t1;
	DWORD tadr;
	BYTE pat;
	int i;

	for (i = 0; i < 512; i++, adr++) {
		tadr = adr ^ 1;

		pat = TVRAM[tadr + 0x60000];
		t0 = ptr[(pat * 2) + 1536];
		t1 = ptr[(pat * 2 + 1) + 1536];

		pat = TVRAM[tadr + 0x40000];
		t0 |= ptr[(pat * 2) + 1024];
		t1 |= ptr[(pat * 2 + 1) + 1024];

		pat = TVRAM[tadr + 0x20000];
		t0 |= ptr[(pat * 2) + 512];
		t1 |= ptr[(pat * 2 + 1) + 512];

		pat = TVRAM[tadr];
		t0 |= ptr[(pat * 2)];
		t1 |= ptr[(pat * 2 + 1)];

		*wptr++ = t0;
		*wptr++ = t1;
	}
#endif	/* USE_ASM */
}

// -----------------------------------------------------------------------
//   1ライン描画
// -----------------------------------------------------------------------
void FASTCALL Text_DrawLine(int opaq)
{
#ifdef USE_ASM
	__asm {
		push	edi
		or	ecx, ecx		//ecx = opaq
		jz	tdlnotopaq
		mov	edi, 0
		mov	edx, VLINE
		mov	al, CRTC_Regs[0x29]
		and	al, 1ch
		cmp	al, 1ch
		jne	textlinenotspecial
		shl	edx, 1
	textlinenotspecial:
		add	edx, TextScrollY
		and	edx, 1023
		shl	edx, 10
		mov	ebx, TextScrollX
		and	ebx, 1023
		add	edx, ebx
		xor	bx, 1023
		inc	bx
		mov	ecx, TextDotX
	looptextline:
		movzx	eax, byte ptr TextDrawWork[edx]
		mov	byte ptr (Text_TrFlag+16)[edi], 0
		and	al, 15
		jz	textline_skip
		mov	byte ptr (Text_TrFlag+16)[edi], 1
	textline_skip:
		mov	ax, word ptr TextPal[eax*2]
		mov	word ptr (BG_LineBuf+32)[edi*2], ax
		inc	edi
		inc	edx
		dec	bx
		jz	endtextline
		loop	looptextline
		jmp	finishtextline
	endtextline:
		dec	cx
		jz	finishtextline
		mov	ax, word ptr TextPal[0]
	endtextlineloop:
		mov	word ptr (BG_LineBuf+32)[edi*2], ax
		mov	byte ptr (Text_TrFlag+16)[edi], 0
		inc	edi
		loop	endtextlineloop
		jmp	finishtextline


	tdlnotopaq:
		mov	edi, 0
		mov	edx, VLINE
		mov	al, CRTC_Regs[0x29]
		and	al, 1ch
		cmp	al, 1ch
		jne	notextlinenotspecial
		shl	edx, 1
	notextlinenotspecial:
		add	edx, TextScrollY
		and	edx, 1023
		shl	edx, 10
		mov	ebx, TextScrollX
		and	ebx, 1023
		add	edx, ebx
		xor	bx, 1023
		inc	bx
		mov	ecx, TextDotX
	nolooptextline:
		movzx	eax, byte ptr TextDrawWork[edx]
		and	al, 15
		jz	notextline_skip
		or	byte ptr (Text_TrFlag+16)[edi], 1
		mov	ax, word ptr TextPal[eax*2]
		mov	word ptr (BG_LineBuf+32)[edi*2], ax
	notextline_skip:
		inc	edi
		inc	edx
		dec	bx
		jz	finishtextline
		loop	nolooptextline

	finishtextline:
		pop	edi
	}
#elif defined(USE_GAS) && defined(__i386__)
	asm (
		"mov	%0, %%ecx;"
		"or	%%ecx, %%ecx;"
		"jz	.tdlnotopaq;"
		"mov	$0, %%edi;"
		"mov	(%1), %%edx;"
		"mov	(%2), %%al;"
		"and	$0x1c, %%al;"
		"cmp	$0x1c, %%al;"
		"jne	.textlinenotspecial;"
		"shl	$1, %%edx;"
	".textlinenotspecial:"
		"add	(%3), %%edx;"
		"and	$1023, %%edx;"
		"shl	$10, %%edx;"
		"mov	(%4), %%ebx;"
		"and	$1023, %%ebx;"
		"add	%%ebx, %%edx;"
		"xor	$1023, %%bx;"
		"inc	%%bx;"
		"mov	(%6), %%ecx;"
	".looptextline:"
		"movzbl	TextDrawWork(%%edx), %%eax;"
		"movb	$0, Text_TrFlag + 16(%%edi);"
		"and	$15, %%al;"
		"jz	.textline_skip;"
		"movb	$1, Text_TrFlag + 16(%%edi);"
	".textline_skip:"
		"mov	TextPal(, %%eax, 2), %%ax;"
		"mov	%%ax, BG_LineBuf + 32(, %%edi, 2);"
		"inc	%%edi;"
		"inc	%%edx;"
		"dec	%%bx;"
		"jz	.endtextline;"
		"loop	.looptextline;"
		"jmp	.finishtextline;"
	".endtextline:"
		"dec	%%cx;"
		"jz	.finishtextline;"
		"mov	(%5), %%ax;"
	".endtextlineloop:"
		"mov	%%ax, BG_LineBuf + 32(, %%edi, 2);"
		"movb	$0, Text_TrFlag + 16(%%edi);"
		"inc	%%edi;"
		"loop	.endtextlineloop;"
		"jmp	.finishtextline;"

	".tdlnotopaq:"
		"mov	$0, %%edi;"
		"mov	(%1), %%edx;"
		"mov	(%2), %%al;"
		"and	$0x1c, %%al;"
		"cmp	$0x1c, %%al;"
		"jne	.notextlinenotspecial;"
		"shl	$1, %%edx;"
	".notextlinenotspecial:"
		"add	(%3), %%edx;"
		"and	$1023, %%edx;"
		"shl	$10, %%edx;"
		"mov	(%4), %%ebx;"
		"and	$1023, %%ebx;"
		"add	%%ebx, %%edx;"
		"xor	$1023, %%bx;"
		"inc	%%bx;"
		"mov	(%6), %%ecx;"
	".nolooptextline:"
		"movzbl	TextDrawWork(%%edx), %%eax;"
		"and	$15, %%al;"
		"jz	.notextline_skip;"
		"orb	$1, Text_TrFlag + 16(%%edi);"
		"mov	TextPal(, %%eax, 2), %%ax;"
		"mov	%%ax, BG_LineBuf + 32(, %%edi, 2);"
	".notextline_skip:"
		"inc	%%edi;"
		"inc	%%edx;"
		"dec	%%bx;"
		"jz	.finishtextline;"
		"loop	.nolooptextline;"

	".finishtextline:"
	: /* output: nothing */
	: "m" (opaq), "g" (VLINE), "g" (CRTC_Regs[0x29]),
	  "g" (TextScrollY), "g" (TextScrollX), "g" (TextPal[0]), "g" (TextDotX)
	: "ax", "bx", "cx", "dx", "si", "di", "memory");
#else /* !USE_ASM && !(USE_GAS && __i386__) */
	DWORD addr;
	DWORD x, y;
	DWORD off = 16;
	DWORD i;
	BYTE t;

	y = TextScrollY + VLINE;
	if ((CRTC_Regs[0x29] & 0x1c) == 0x1c)
		y += VLINE;
	y = (y & 0x3ff) << 10;

	x = TextScrollX & 0x3ff;
	addr = x + y;
	x = (x ^ 0x3ff) + 1;

	if (opaq) {
		for (i = 0; (i < TextDotX) && (x > 0); i++, x--, off++) {
			t = TextDrawWork[addr++] & 0xf;
			Text_TrFlag[off] = t ? 1 : 0;
			BG_LineBuf[off] = TextPal[t];
		}
		if (i++ != TextDotX) {
			for (; i < TextDotX; i++, off++) {
				BG_LineBuf[off] = TextPal[0];
				Text_TrFlag[off] = 0;
			}
		}
	} else {
		for (i = 0; (i < TextDotX) && (x > 0); i++, x--, off++) {
			t = TextDrawWork[addr++] & 0xf;
			if (t) {
				Text_TrFlag[off] |= 1;
				BG_LineBuf[off] = TextPal[t];
			}
		}
	}
#endif	/* USE_ASM */
}
