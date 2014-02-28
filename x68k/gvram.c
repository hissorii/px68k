// ---------------------------------------------------------------------------------------
//  GVRAM.C - Graphic VRAM
// ---------------------------------------------------------------------------------------

#include	"common.h"
#include	"windraw.h"
#include	"winx68k.h"
#include	"crtc.h"
#include	"palette.h"
#include	"tvram.h"
#include	"gvram.h"
#include	"m68000.h"
#include	"memory.h"

	BYTE	GVRAM[0x80000];
	WORD	Grp_LineBuf[1024];
	WORD	Grp_LineBufSP[1024];		// 特殊プライオリティ／半透明用バッファ
	WORD	Grp_LineBufSP2[1024];		// 半透明ベースプレーン用バッファ（非半透明ビット格納）

	WORD	Pal16Adr[256];			// 16bit color パレットアドレス計算用

// xxx: for little endian only
#define GET_WORD_W8(src) (*(BYTE *)(src) | *((BYTE *)(src) + 1) << 8)


// -----------------------------------------------------------------------
//   初期化〜
// -----------------------------------------------------------------------
void GVRAM_Init(void)
{
	int i;

	ZeroMemory(GVRAM, 0x80000);
	for (i=0; i<128; i++)			// 16bit color パレットアドレス計算用
	{
		Pal16Adr[i*2] = i*4;
		Pal16Adr[i*2+1] = i*4+1;
	}
}


// -----------------------------------------------------------------------------------
//  高速クリア用ルーチン
// -----------------------------------------------------------------------------------

void FASTCALL GVRAM_FastClear(void)
{
	DWORD v, h;
	v = ((CRTC_Regs[0x29]&4)?512:256);
	h = ((CRTC_Regs[0x29]&3)?512:256);
	// やっぱちゃんと範囲指定しないと変になるものもある（ダイナマイトデュークとか）
#ifdef USE_ASM
	_asm
	{
		push	ebx
		push	ecx
		push	edx
		push	esi
		mov	ax, CRTC_FastClrMask
		mov	ecx, v
		mov	esi, GrphScrollY[0]
		and	esi, 511
		shl	esi, 10
	fclp2:
		mov	edx, GrphScrollX[0]
		and	edx, 511
		mov	ebx, h
	fclp:
		and	word ptr GVRAM[esi+edx*2], ax
		inc	edx
		and	edx, 511
		dec	ebx
		jne	fclp
		add	esi, 1024
		and	esi, 07fc00h
		dec	ecx
		jne	fclp2
		pop	esi
		pop	edx
		pop	ecx
		pop	ebx
	}
#elif defined(USE_GAS) && defined(__i386__)
	__asm__ __volatile__ (
		"pushl	%%ebx;"
		"pushl	%%ecx;"
		"pushl	%%edx;"
		"pushl	%%esi;"
		"movw	%0, %%ax;"
		"movl	%3, %%ecx;"
		"movl	%1, %%esi;"
		"andl	$511, %%esi;"
		"shll	$10, %%esi;"
	".fclp2:"
		"movl	%2, %%edx;"
		"andl	$511, %%edx;"
		"movl	%4, %%ebx;"
	".fclp:"
		"andw	%%ax, GVRAM(%%esi, %%edx, 2);"
		"incl	%%edx;"
		"andl	$511, %%edx;"
		"decl	%%ebx;"
		"jne	.fclp;"
		"addl	$1024, %%esi;"
		"andl	$0x7fc00, %%esi;"
		"decl	%%ecx;"
		"jne	.fclp2;"
		"popl	%%esi;"
		"popl	%%edx;"
		"popl	%%ecx;"
		"popl	%%ebx;"
	: /* output: nothing */
	: "m" (CRTC_FastClrMask), "m" (GrphScrollY[0]), "m" (GrphScrollX[0]),
	  "m" (v), "m" (h)
	: "eax");
#else /* !USE_ASM && !(USE_GAS && __i386__) */
{
	WORD *p;
	DWORD x, y;
	DWORD offx, offy;

	offy = (GrphScrollY[0] & 0x1ff) << 10;
	for (y = 0; y < v; ++y) {
		offx = GrphScrollX[0] & 0x1ff;
		p = (WORD *)(GVRAM + offy + offx * 2);

		for (x = 0; x < h; ++x) {
			*p++ &= CRTC_FastClrMask;
			offx = (offx + 1) & 0x1ff;
		}

		offy = (offy + 0x400) & 0x7fc00;
	}
}
#endif /* USE_ASM */
}


// -----------------------------------------------------------------------
//   VRAM Read
// -----------------------------------------------------------------------
BYTE FASTCALL GVRAM_Read(DWORD adr)
{
	BYTE ret=0;
	BYTE page;
	WORD *ram = (WORD*)(&GVRAM[adr&0x7fffe]);
	adr ^= 1;
	adr -= 0xc00000;

	if (CRTC_Regs[0x28]&8) {			// 読み込み側も65536モードのVRAM配置（苦胃頭捕物帳）
		if (adr<0x80000) ret = GVRAM[adr];
	} else {
		switch(CRTC_Regs[0x28]&3)
		{
		case 0:					// 16 colors
			if (!(adr&1))
			{
				if (CRTC_Regs[0x28]&4)		// 1024dot
				{
					ram = (WORD*)(&GVRAM[((adr&0xff800)>>1)+(adr&0x3fe)]);
					page = (BYTE)((adr>>17)&0x08);
					page += (BYTE)((adr>>8)&4);
					ret = (((*ram)>>page)&15);
				}
				else
				{
					page = (BYTE)((adr>>17)&0x0c);
					ret = (((*ram)>>page)&15);
				}
			}
			break;
		case 1:					// 256
		case 2:					// Unknown
			if ( adr<0x100000 )
			{
				if (!(adr&1))
				{
					page = (BYTE)((adr>>16)&0x08);
					ret = (BYTE)((*ram)>>page);
				}
			}
//			else
//				BusErrFlag = 1;
			break;
		case 3:					// 65536
			if (adr<0x80000)
				ret = GVRAM[adr];
//			else
//				BusErrFlag = 1;
			break;
		}
	}
	return ret;
}


// -----------------------------------------------------------------------
//   VRAM Write
// -----------------------------------------------------------------------
void FASTCALL GVRAM_Write(DWORD adr, BYTE data)
{
	BYTE page;
	int line = 1023, scr = 0;
	WORD *ram = (WORD*)(&GVRAM[adr&0x7fffe]);
	WORD temp;

	adr ^= 1;
	adr -= 0xc00000;


	if (CRTC_Regs[0x28]&8)				// 65536モードのVRAM配置？（Nemesis）
	{
		if ( adr<0x80000 )
		{
			GVRAM[adr] = data;
			line = (((adr&0x7ffff)/1024)-GrphScrollY[0])&511;
		}
	}
	else
	{
		switch(CRTC_Regs[0x28]&3)
		{
		case 0:					// 16 colors
			if (adr&1) break;
			if (CRTC_Regs[0x28]&4)		// 1024dot
			{
				ram = (WORD*)(&GVRAM[((adr&0xff800)>>1)+(adr&0x3fe)]);
				page = (BYTE)((adr>>17)&0x08);
				page += (BYTE)((adr>>8)&4);
				temp = ((WORD)data&15)<<page;
				*ram = ((*ram)&(~(0xf<<page)))|temp;
				line = ((adr/2048)-GrphScrollY[0])&1023;
			}
			else
			{
				page = (BYTE)((adr>>17)&0x0c);
				temp = ((WORD)data&15)<<page;
				*ram = ((*ram)&(~(0xf<<page)))|temp;
				switch(adr/0x80000)
				{
					case 0:	scr = GrphScrollY[0]; break;
					case 1: scr = GrphScrollY[1]; break;
					case 2: scr = GrphScrollY[2]; break;
					case 3: scr = GrphScrollY[3]; break;
				}
				line = (((adr&0x7ffff)/1024)-scr)&511;
			}
			break;
		case 1:					// 256 colors
		case 2:					// Unknown
			if ( adr<0x100000 )
			{
				if ( !(adr&1) )
				{
					scr = GrphScrollY[(adr>>18)&2];
					line = (((adr&0x7ffff)>>10)-scr)&511;
					TextDirtyLine[line] = 1;			// 32色4面みたいな使用方法時
					scr = GrphScrollY[((adr>>18)&2)+1];		//
					line = (((adr&0x7ffff)>>10)-scr)&511;		//
					if (adr&0x80000) adr+=1;
					adr &= 0x7ffff;
					GVRAM[adr] = data;
				}
			}
//			else
//			{
//				BusErrFlag = 1;
//				return;
//			}
			break;
		case 3:					// 65536 colors
			if ( adr<0x80000 )
			{
				GVRAM[adr] = data;
				line = (((adr&0x7ffff)>>10)-GrphScrollY[0])&511;
			}
//			else
//			{
//				BusErrFlag = 1;
//				return;
//			}
			break;
		}
		TextDirtyLine[line] = 1;
	}
}


// -----------------------------------------------------------------------
//   こっから後はライン単位での画面展開部
// -----------------------------------------------------------------------
LABEL void Grp_DrawLine16(void)
{
#ifdef USE_ASM
	__asm {
			pushf
			cld
			push	es
			mov	ax, ds
			mov	es, ax
			push	ebx
			push	esi
			push	edi
			mov	esi, GrphScrollY[0]
			add	esi, VLINE
			mov	al, CRTC_Regs[0x29]
			and	al, 1ch
			cmp	al, 1ch
			jne	gp16linenotspecial
			add	esi, VLINE
		gp16linenotspecial:
			and	esi, 511
			shl	esi, 10
			mov	ebx, GrphScrollX[0]
			and	ebx, 511
			lea	esi, GVRAM[esi+ebx*2]
			xor	ebx, 511
			inc	bx
			mov	ecx, TextDotX
			mov	edi, offset Grp_LineBuf
			xor	eax, eax
			xor	edx, edx
			cmp	ecx, ebx
			jbe	gp16linelp_b
			sub	ecx, ebx
		gp16linelp_a:
			lodsw
			or	ax, ax
			je	gp16linelp_a_skip
			mov	dl, ah
			mov	ah, 0
			mov	dh, 0
			mov	ax, word ptr Pal16Adr[eax*2]
			mov	dx, word ptr Pal16Adr[edx*2]
			mov	al, byte ptr Pal_Regs[eax]
			mov	ah, byte ptr Pal_Regs[edx+2]
			mov	ax, word ptr Pal16[eax*2]
;or ax, Ibit		; 20010120
		gp16linelp_a_skip:
			stosw
			dec	bx
			jnz	gp16linelp_a
			sub	esi, 400h
		gp16linelp_b:
			lodsw
			or	ax, ax
			je	gp16linelp_b_skip
			mov	dl, ah
			mov	ah, 0
			mov	dh, 0
			mov	ax, word ptr Pal16Adr[eax*2]
			mov	dx, word ptr Pal16Adr[edx*2]
			mov	al, byte ptr Pal_Regs[eax]
			mov	ah, byte ptr Pal_Regs[edx+2]
			mov	ax, word ptr Pal16[eax*2]
;or ax, Ibit		; 20010120
		gp16linelp_b_skip:
			stosw
			loop	gp16linelp_b
			pop	edi
			pop	esi
			pop	ebx
			pop	es
			popf
			ret
	}
#elif defined(USE_GAS) && defined(__i386__)
	__asm__ __volatile__ (
		"pushf;"
		"cld;"

#if 0	/* まあ、同じセグメント差してる筈だし… */
		"pushl	%%es;"

		"movw	%%ds, %%ax;"
		"movw	%%ax, %%es;"
#endif
		"pushl	%%ebx;"
		"pushl	%%esi;"
		"pushl	%%edi;"
		"movl	%0, %%esi;"
		"addl	%1, %%esi;"
		"movb	%2, %%al;"
		"andb	$0x1c, %%al;"
		"cmpb	$0x1c, %%al;"
		"jne	.gp16linenotspecial;"
		"addl	%1, %%esi;"
	".gp16linenotspecial:"
		"andl	$511, %%esi;"
		"shll	$10, %%esi;"
		"movl	%3, %%ebx;"
		"andl	$511, %%ebx;"
		"leal	GVRAM(%%esi, %%ebx, 2), %%esi;"
		"xorl	$511, %%ebx;"
		"incw	%%bx;"
		"movl	%4, %%ecx;"
		"movl	%5, %%edi;"
		"xorl	%%eax, %%eax;"
		"xorl	%%edx, %%edx;"
		"cmpl	%%ebx, %%ecx;"
		"jbe	.gp16linelp_b;"
		"subl	%%ebx, %%ecx;"
	".gp16linelp_a:"
		"lodsw;"
		"orw	%%ax, %%ax;"
		"je	.gp16linelp_a_skip;"
		"movb	%%ah, %%dl;"
		"movb	$0, %%ah;"
		"movb	$0, %%dh;"
		"movw	Pal16Adr(, %%eax, 2), %%ax;"
		"movw	Pal16Adr(, %%edx, 2), %%dx;"
		"movb	Pal_Regs(%%eax), %%al;"
		"movb	Pal_Regs + 2(%%edx), %%ah;"
		"movw	Pal16(, %%eax, 2), %%ax;"
	".gp16linelp_a_skip:"
		"stosw;"
		"decw	%%bx;"
		"jnz	.gp16linelp_a;"
		"subl	$0x400, %%esi;"
	".gp16linelp_b:"
		"lodsw;"
		"orw	%%ax, %%ax;"
		"je	.gp16linelp_b_skip;"
		"movb	%%ah, %%dl;"
		"movb	$0, %%ah;"
		"movb	$0, %%dh;"
		"movw	Pal16Adr(, %%eax, 2), %%ax;"
		"movw	Pal16Adr(, %%edx, 2), %%dx;"
		"movb	Pal_Regs(%%eax), %%al;"
		"movb	Pal_Regs + 2(%%edx), %%ah;"
		"movw	Pal16(, %%eax, 2), %%ax;"
	".gp16linelp_b_skip:"
		"stosw;"
		"loop	.gp16linelp_b;"

#if 0	/* まあ、同じセグメント差してる筈だし… */
		"popl	%%es;"
#endif
		"popl	%%edi;"
		"popl	%%esi;"
		"popl	%%ebx;"
		"popf;"
	: /* output: nothing */
	: "m" (GrphScrollY[0]), "m" (VLINE), "m" (CRTC_Regs[0x29]),
	  "m" (GrphScrollX[0]), "m" (TextDotX), "g" (Grp_LineBuf)
	: "ax", "cx", "dx");
#else /* !USE_ASM && !(USE_GAS && __i386__) */
	WORD *srcp, *destp;
	DWORD x, y;
	DWORD i;
	WORD v, v0;

	y = GrphScrollY[0] + VLINE;
	if ((CRTC_Regs[0x29] & 0x1c) == 0x1c)
		y += VLINE;
	y = (y & 0x1ff) << 10;

	x = GrphScrollX[0] & 0x1ff;
	srcp = (WORD *)(GVRAM + y + x * 2);
	destp = (WORD *)Grp_LineBuf;

	x = (x ^ 0x1ff) + 1;

	v = v0 = 0;
	i = 0;
	if (x < TextDotX) {
		for (; i < x; ++i) {
			v = *srcp++;
			if (v != 0) {
				v0 = (v >> 8) & 0xff;
				v &= 0x00ff;

				v = Pal_Regs[Pal16Adr[v]];
				v |= Pal_Regs[Pal16Adr[v0] + 2] << 8;
				v = Pal16[v];
			}
			*destp++ = v;
		}
		srcp -= 0x200;
	}

	for (; i < TextDotX; ++i) {
		v = *srcp++;
		if (v != 0) {
			v0 = (v >> 8) & 0xff;
			v &= 0x00ff;

			v = Pal_Regs[Pal16Adr[v]];
			v |= Pal_Regs[Pal16Adr[v0] + 2] << 8;
			v = Pal16[v];
		}
		*destp++ = v;
	}
#endif /* USE_ASM */
}


LABEL void FASTCALL Grp_DrawLine8(int page, int opaq)
{
#ifdef USE_ASM
	__asm {
			pushf
			cld
			push	ebx
			push	ecx
			push	esi
			push	edi

			and	ecx, 1		// ecx = page
			mov	esi, GrphScrollY[ecx*8]
			mov	edi, GrphScrollY[ecx*8+4]
			add	esi, VLINE
			add	edi, VLINE
			mov	al, CRTC_Regs[0x29]
			and	al, 1ch
			cmp	al, 1ch
			jne	gp8linenotspecial
			add	esi, VLINE
			add	edi, VLINE
		gp8linenotspecial:
			and	esi, 511
			shl	esi, 10
			add	esi, ecx
			and	edi, 511
			shl	edi, 10
			add	edi, ecx
			mov	ebx, GrphScrollX[ecx*8+4]
			and	ebx, 511
			add	edi, ebx
			add	edi, ebx
			mov	ebx, GrphScrollX[ecx*8]
			and	ebx, 511
			lea	esi, GVRAM[esi+ebx*2]
			xor	ebx, 511
			inc	bx

			xor	eax, eax
			mov	ecx, TextDotX

			or	edx, edx	// edx = opaq
			mov	edx, offset Grp_LineBuf
			je	gp8linelp

			cmp	ecx, ebx
			jbe	gp8olinelp_b
			sub	ecx, ebx

		gp8olinelp_a:
			lodsw
			mov ah, byte ptr GVRAM[edi]
			and ah, 0f0h
			and al, 0fh
			or al, ah
			mov	ah, 0
;or al,al
			mov	ax, word ptr GrphPal[eax*2]
;jz gp8noti_a
;or ax, Ibit		; 20010120
;gp8noti_a:
			mov	[edx], ax
			add	edx, 2
			add	edi, 2
			test	di, 03feh
			jnz	gp8onotxend1
			sub	edi, 0400h
		gp8onotxend1:
			dec	bx
			jnz	gp8olinelp_a
			sub	esi, 400h
		gp8olinelp_b:
			lodsw
			mov ah, byte ptr GVRAM[edi]
			and ah, 0f0h
			and al, 0fh
			or al, ah
			mov	ah, 0
;or al,al
			mov	ax, word ptr GrphPal[eax*2]
;jz gp8noti_b
;or ax, Ibit		; 20010120
;gp8noti_b:
			mov	[edx], ax
			add	edx, 2
			add	edi, 2
			test	di, 03feh
			jnz	gp8onotxend2
			sub	edi, 0400h
		gp8onotxend2:
			loop	gp8olinelp_b

			pop	edi
			pop	esi
			pop	ecx
			pop	ebx
			popf
			ret

		gp8linelp:
			cmp	ecx, ebx
			jbe	gp8linelp_b
			sub	ecx, ebx
		gp8linelp_a:
			lodsw
			mov ah, byte ptr GVRAM[edi]
			and ah, 0f0h
			and al, 0fh
			or al, ah
			and	ax, 00ffh
			jz	gp8lineskip_a
			mov	ax, word ptr GrphPal[eax*2]
;or ax, Ibit		; 20010120
			mov	[edx], ax
		gp8lineskip_a:
			add	edx, 2
			add	edi, 2
			test	di, 03feh
			jnz	gp8notxend1
			sub	edi, 0400h
		gp8notxend1:
			dec	bx
			jnz	gp8linelp_a
			sub	esi, 400h
		gp8linelp_b:
			lodsw
			mov ah, byte ptr GVRAM[edi]
			and ah, 0f0h
			and al, 0fh
			or al, ah
			and	ax, 00ffh
			jz	gp8lineskip_b
			mov	ax, word ptr GrphPal[eax*2]
;or ax, Ibit		; 20010120
			mov	[edx], ax
		gp8lineskip_b:
			add	edx, 2
			add	edi, 2
			test	di, 03feh
			jnz	gp8notxend2
			sub	edi, 0400h
		gp8notxend2:
			loop	gp8linelp_b

			pop	edi
			pop	esi
			pop	ecx
			pop	ebx
			popf
			ret
	}
#elif defined(USE_GAS) && defined(__i386__)
	__asm__ __volatile__ (
		"pushf;"
		"cld;"
		"pushl	%%ebx;"
		"pushl	%%ecx;"
		"pushl	%%esi;"
		"pushl	%%edi;"

		"movl	%0, %%ecx;"	// page
		"movl	%1, %%edx;"	// opaq

		"andl	$1, %%ecx;"	// ecx = page
		"movl	GrphScrollY(, %%ecx, 8), %%esi;"
		"movl	GrphScrollY + 4(, %%ecx, 8), %%edi;"
		"addl	%2, %%esi;"
		"addl	%2, %%edi;"
		"movb	%3, %%al;"
		"andb	$0x1c, %%al;"
		"cmpb	$0x1c, %%al;"
		"jne	.gp8linenotspecial;"
		"addl	%2, %%esi;"
		"addl	%2, %%edi;"
	".gp8linenotspecial:"
		"andl	$511, %%esi;"
		"shll	$10, %%esi;"
		"addl	%%ecx, %%esi;"
		"andl	$511, %%edi;"
		"shll	$10, %%edi;"
		"addl	%%ecx, %%edi;"
		"movl	GrphScrollX + 4(, %%ecx, 8), %%ebx;"
		"andl	$511, %%ebx;"
		"addl	%%ebx, %%edi;"
		"addl	%%ebx, %%edi;"
		"movl	GrphScrollX(, %%ecx, 8), %%ebx;"
		"andl	$511, %%ebx;"
		"leal	GVRAM(%%esi, %%ebx, 2), %%esi;"
		"xorl	$511, %%ebx;"
		"incw	%%bx;"

		"xorl	%%eax, %%eax;"
		"movl	%4, %%ecx;"

		"orl	%%edx, %%edx;"	// edx = opaq
		"movl	%5, %%edx;"
		"je	.gp8linelp;"

		"cmpl	%%ebx, %%ecx;"
		"jbe	.gp8olinelp_b;"
		"subl	%%ebx, %%ecx;"

	".gp8olinelp_a:"
		"lodsw;"
		"movb	GVRAM(%%edi), %%ah;"
		"andb	$0xf0, %%ah;"
		"andb	$0x0f, %%al;"
		"orb	%%ah, %%al;"
		"movb	$0, %%ah;"
		"movw	GrphPal(, %%eax, 2), %%ax;"
		"movw	%%ax, (%%edx);"
		"addl	$2, %%edx;"
		"addl	$2, %%edi;"
		"testw	$0x3fe, %%di;"
		"jnz	.gp8onotxend1;"
		"subl	$0x400, %%edi;"
	".gp8onotxend1:"
		"decw	%%bx;"
		"jnz	.gp8olinelp_a;"
		"subl	0x400, %%esi;"
	".gp8olinelp_b:"
		"lodsw;"
		"movb	GVRAM(%%edi), %%ah;"
		"andb	$0xf0, %%ah;"
		"andb	$0x0f, %%al;"
		"orb	%%ah, %%al;"
		"movb	$0, %%ah;"
		"movw	GrphPal(, %%eax, 2), %%ax;"
		"movw	%%ax, (%%edx);"
		"addl	$2, %%edx;"
		"addl	$2, %%edi;"
		"testw	$0x3fe, %%di;"
		"jnz	.gp8onotxend2;"
		"subl	$0x400, %%edi;"
	".gp8onotxend2:"
		"loop	.gp8olinelp_b;"
		"jmp	.gp8end;"

	".gp8linelp:"
		"cmpl	%%ebx, %%ecx;"
		"jbe	.gp8linelp_b;"
		"subl	%%ebx, %%ecx;"
	".gp8linelp_a:"
		"lodsw;"
		"movb	GVRAM(%%edi), %%ah;"
		"andb	$0xf0, %%ah;"
		"andb	$0x0f, %%al;"
		"orb	%%ah, %%al;"
		"andw	$0xff, %%ax;"
		"jz	.gp8lineskip_a;"
		"movw	GrphPal(, %%eax, 2), %%ax;"
		"movw	%%ax, (%%edx);"
	".gp8lineskip_a:"
		"addl	$2, %%edx;"
		"addl	$2, %%edi;"
		"testw	$0x3fe, %%di;"
		"jnz	.gp8notxend1;"
		"subl	$0x400, %%edi;"
	".gp8notxend1:"
		"decw	%%bx;"
		"jnz	.gp8linelp_a;"
		"subl	$0x400, %%esi;"
	".gp8linelp_b:"
		"lodsw;"
		"movb	GVRAM(%%edi), %%ah;"
		"andb	$0xf0, %%ah;"
		"andb	$0x0f, %%al;"
		"orb	%%ah, %%al;"
		"andw	$0xff, %%ax;"
		"jz	.gp8lineskip_b;"
		"movw	GrphPal(, %%eax, 2), %%ax;"
		"movw	%%ax, (%%edx);"
	".gp8lineskip_b:"
		"addl	$2, %%edx;"
		"addl	$2, %%edi;"
		"testw	$0x3fe, %%di;"
		"jnz	.gp8notxend2;"
		"subl	$0x400, %%edi;"
	".gp8notxend2:"
		"loop	.gp8linelp_b;"

	".gp8end:"
		"popl	%%edi;"
		"popl	%%esi;"
		"popl	%%ecx;"
		"popl	%%ebx;"
		"popf;"
	: /* output: nothing */
	: "m" (page), "m" (opaq), "m" (VLINE), "m" (CRTC_Regs[0x29]), 
	  "m" (TextDotX), "g" (Grp_LineBuf)
	: "ax", "dx");
#else /* !USE_ASM && !(USE_GAS && __i386__) */
	WORD *srcp, *destp;
	DWORD x, x0;
	DWORD y, y0;
	DWORD off;
	DWORD i;
	WORD v;

	page &= 1;

	y = GrphScrollY[page * 2] + VLINE;
	y0 = GrphScrollY[page * 2 + 1] + VLINE;
	if ((CRTC_Regs[0x29] & 0x1c) == 0x1c) {
		y += VLINE;
		y0 += VLINE;
	}
	y = ((y & 0x1ff) << 10) + page;
	y0 = ((y0 & 0x1ff) << 10) + page;

	x = GrphScrollX[page * 2] & 0x1ff;
	x0 = GrphScrollX[page * 2 + 1] & 0x1ff;

	off = y0 + x0 * 2;
	srcp = (WORD *)(GVRAM + y + x * 2);
	destp = (WORD *)Grp_LineBuf;

	x = (x ^ 0x1ff) + 1;

	v = 0;
	i = 0;

	if (opaq) {
		if (x < TextDotX) {
			for (; i < x; ++i) {
				v = GET_WORD_W8(srcp);
				srcp++;
				v = GrphPal[(GVRAM[off] & 0xf0) | (v & 0x0f)];
				*destp++ = v;

				off += 2;
				if ((off & 0x3fe) == 0x000)
					off -= 0x400;
			}
			srcp -= 0x200;
		}

		for (; i < TextDotX; ++i) {
			v = GET_WORD_W8(srcp);
			srcp++;
			v = GrphPal[(GVRAM[off] & 0xf0) | (v & 0x0f)];
			*destp++ = v;

			off += 2;
			if ((off & 0x3fe) == 0x000)
				off -= 0x400;
		}
	} else {
		if (x < TextDotX) {
			for (; i < x; ++i) {
				v = GET_WORD_W8(srcp);
				srcp++;
				v = (GVRAM[off] & 0xf0) | (v & 0x0f);
				if (v != 0x00)
					*destp = GrphPal[v];
				destp++;

				off += 2;
				if ((off & 0x3fe) == 0x000)
					off -= 0x400;
			}
			srcp -= 0x200;
		}

		for (; i < TextDotX; ++i) {
			v = GET_WORD_W8(srcp);
			srcp++;
			v = (GVRAM[off] & 0xf0) | (v & 0x0f);
			if (v != 0x00)
				*destp = GrphPal[v];
			destp++;

			off += 2;
			if ((off & 0x3fe) == 0x000)
				off -= 0x400;
		}
	}
#endif /* USE_ASM */
}

				// Manhattan Requiem Opening 7.0→7.5MHz
LABEL void FASTCALL Grp_DrawLine4(DWORD page, int opaq)
{
#ifdef USE_ASM
	__asm {
			pushf
			cld
			push	ebx
			push	ecx
			push	esi
			and	ecx, 3
			mov	esi, GrphScrollY[ecx*4]
			add	esi, VLINE
			mov	al, CRTC_Regs[0x29]
			and	al, 1ch
			cmp	al, 1ch
			jne	gp4linenotspecial
			add	esi, VLINE
		gp4linenotspecial:
			and	esi, 511
			shl	esi, 10
			mov	ebx, GrphScrollX[ecx*4]
			and	ebx, 511
			lea	esi, [esi+ebx*2]
			xor	ebx, 511
			inc	bx
			xor	eax, eax

			shr	cl, 1
			lea	esi, GVRAM[esi+ecx]	// ecx = page/2
			jnc	gp4olinepage0		// (shr cl,1) page0or2

			mov	ecx, TextDotX
			or	edx, edx
			mov	edx, offset Grp_LineBuf
			jz	gp4linelp2		// opaq == 0

			cmp	ecx, ebx
			jbe	gp4olinelp2_b
			sub	ecx, ebx
		gp4olinelp2_a:
			lodsw
			mov	ah, 0
			shr	al, 4
			mov	ax, word ptr GrphPal[eax*2]
			mov	[edx], ax
			add	edx, 2
			dec	bx
			jnz	gp4olinelp2_a
			sub	esi, 400h
		gp4olinelp2_b:
			lodsw
			mov	ah, 0
			shr	al, 4
			mov	ax, word ptr GrphPal[eax*2]
			mov	[edx], ax
			add	edx, 2
			loop	gp4olinelp2_b
			pop	esi
			pop	ecx
			pop	ebx
			popf
			ret

		gp4linelp2:
			cmp	ecx, ebx
			jbe	gp4linelp2_b
			sub	ecx, ebx
		gp4linelp2_a:
			lodsw
			mov	ah, 0
			shr	al, 4			// shrのZFは確認済
			jz	gp4lineskip2_a
			mov	ax, word ptr GrphPal[eax*2]
			mov	[edx], ax
		gp4lineskip2_a:
			add	edx, 2
			dec	bx
			jnz	gp4linelp2_a
			sub	esi, 400h
		gp4linelp2_b:
			lodsw
			mov	ah, 0
			shr	al, 4			// shrのZFは確認済
			jz	gp4lineskip2_b
			mov	ax, word ptr GrphPal[eax*2]
			mov	[edx], ax
		gp4lineskip2_b:
			add	edx, 2
			loop	gp4linelp2_b
			pop	esi
			pop	ecx
			pop	ebx
			popf
			ret

		gp4olinepage0:
			mov	ecx, TextDotX
			or	edx, edx
			mov	edx, offset Grp_LineBuf
			jz	gp4linelp		// opaq == 0

			cmp	ecx, ebx
			jbe	gp4olinelp_b
			sub	ecx, ebx
		gp4olinelp_a:
			lodsw
			and	ax, 15
			mov	ax, word ptr GrphPal[eax*2]
			mov	[edx], ax
			add	edx, 2
			dec	bx
			jnz	gp4olinelp_a
			sub	esi, 400h
		gp4olinelp_b:
			lodsw
			and	ax, 15
			mov	ax, word ptr GrphPal[eax*2]
			mov	[edx], ax
			add	edx, 2
			loop	gp4olinelp_b
			pop	esi
			pop	ecx
			pop	ebx
			popf
			ret

		gp4linelp:
			cmp	ecx, ebx
			jbe	gp4linelp_b
			sub	ecx, ebx
		gp4linelp_a:
			lodsw
			and	ax, 15
			jz	gp4lineskip_a
			mov	ax, word ptr GrphPal[eax*2]
			mov	[edx], ax
		gp4lineskip_a:
			add	edx, 2
			dec	bx
			jnz	gp4linelp_a
			sub	esi, 400h
		gp4linelp_b:
			lodsw
			and	ax, 15
			jz	gp4lineskip_b
			mov	ax, word ptr GrphPal[eax*2]
			mov	[edx], ax
		gp4lineskip_b:
			add	edx, 2
			loop	gp4linelp_b
			pop	esi
			pop	ecx
			pop	ebx
			popf
			ret
	}
#elif defined(USE_GAS) && defined(__i386__)
	__asm__ __volatile__ (
		"pushf;"
		"cld;"
		"pushl	%%ebx;"
		"pushl	%%ecx;"
		"pushl	%%esi;"
		"pushl	%%edi;"
	
		"movl	%0, %%ecx;"	// page
		"movl	%1, %%edx;"	// opaq

		"andl	$3, %%ecx;"	// ecx = page
		"movl	GrphScrollY(, %%ecx, 4), %%esi;"
		"addl	%2, %%esi;"
		"movb	%3, %%al;"
		"andb	$0x1c, %%al;"
		"cmpb	$0x1c, %%al;"
		"jne	.gp4linenotspecial;"
		"addl	%2, %%esi;"
	".gp4linenotspecial:"
		"andl	$511, %%esi;"
		"shll	$10, %%esi;"
		"movl	GrphScrollX(, %%ecx, 4), %%ebx;"
		"andl	$511, %%ebx;"
		"leal	(%%esi, %%ebx, 2), %%esi;"
		"xorl	$511, %%ebx;"
		"incw	%%bx;"
		"xorl	%%eax, %%eax;"

		"shrb	$1, %%cl;"
		"leal	GVRAM(%%esi, %%ecx), %%esi;"	// ecx = page/2
		"jnc	.gp4olinepage0;"		// (shr cl,1) page0or2

		"movl	%4, %%ecx;"
		"orl	%%edx, %%edx;"
		"movl	%5, %%edx;"
		"jz	.gp4linelp2;"			// opaq == 0

		"cmpl	%%ebx, %%ecx;"
		"jbe	.gp4olinelp2_b;"
		"subl	%%ebx, %%ecx;"
	".gp4olinelp2_a:"
		"lodsw;"
		"movb	$0, %%ah;"
		"shrb	$4, %%al;"
		"movw	GrphPal(, %%eax, 2), %%ax;"
		"movw	%%ax, (%%edx);"
		"addl	$2, %%edx;"
		"decw	%%bx;"
		"jnz	.gp4olinelp2_a;"
		"subl	$0x400, %%esi;"
	".gp4olinelp2_b:"
		"lodsw;"
		"movb	$0, %%ah;"
		"shrb	$4, %%al;"
		"movw	GrphPal(, %%eax, 2), %%ax;"
		"movw	%%ax, (%%edx);"
		"addl	$2, %%edx;"
		"loop	.gp4olinelp2_b;"
		"jmp	.gp4end;"

	".gp4linelp2:"
		"cmpl	%%ebx, %%ecx;"
		"jbe	.gp4linelp2_b;"
		"subl	%%ebx, %%ecx;"
	".gp4linelp2_a:"
		"lodsw;"
		"movb	$0, %%ah;"
		"shrb	$4, %%al;"			// shrのZFは確認済
		"jz	.gp4lineskip2_a;"
		"movw	GrphPal(, %%eax, 2), %%ax;"
		"movw	%%ax, (%%edx);"
	".gp4lineskip2_a:"
		"addl	$2, %%edx;"
		"decw	%%bx;"
		"jnz	.gp4linelp2_a;"
		"subl	$0x400, %%esi;"
	".gp4linelp2_b:"
		"lodsw;"
		"movb	$0,%%ah;"
		"shrb	$4, %%al;"			// shrのZFは確認済
		"jz	.gp4lineskip2_b;"
		"movw	GrphPal(, %%eax, 2), %%ax;"
		"movw	%%ax, (%%edx);"
	".gp4lineskip2_b:"
		"addl	$2, %%edx;"
		"loop	.gp4linelp2_b;"
		"jmp	.gp4end;"

	".gp4olinepage0:"
		"movl	(%4), %%ecx;"
		"orl	%%edx, %%edx;"
		"movl	%5, %%edx;"
		"jz	.gp4linelp;"		// opaq == 0

		"cmpl	%%ebx, %%ecx;"
		"jbe	.gp4olinelp_b;"
		"subl	%%ebx, %%ecx;"
	".gp4olinelp_a:"
		"lodsw;"
		"andw	$15, %%ax;"
		"movw	GrphPal(, %%eax, 2), %%ax;"
		"movw	%%ax, (%%edx);"
		"addl	$2, %%edx;"
		"decw	%%bx;"
		"jnz	.gp4olinelp_a;"
		"subl	$0x400, %%esi;"
	".gp4olinelp_b:"
		"lodsw;"
		"andw	$15, %%ax;"
		"movw	GrphPal(, %%eax, 2), %%ax;"
		"movw	%%ax, (%%edx);"
		"addl	$2, %%edx;"
		"loop	.gp4olinelp_b;"
		"jmp	.gp4end;"

	".gp4linelp:"
		"cmpl	%%ebx, %%ecx;"
		"jbe	.gp4linelp_b;"
		"subl	%%ebx, %%ecx;"
	".gp4linelp_a:"
		"lodsw;"
		"andw	$15, %%ax;"
		"jz	.gp4lineskip_a;"
		"movw	GrphPal(, %%eax, 2), %%ax;"
		"movw	%%ax, (%%edx);"
	".gp4lineskip_a:"
		"addl	$2, %%edx;"
		"decw	%%bx;"
		"jnz	.gp4linelp_a;"
		"subl	$0x400, %%esi;"
	".gp4linelp_b:"
		"lodsw;"
		"andw	$15, %%ax;"
		"jz	.gp4lineskip_b;"
		"movw	GrphPal(, %%eax, 2), %%ax;"
		"movw	%%ax, (%%edx);"
	".gp4lineskip_b:"
		"addl	$2, %%edx;"
		"loop	.gp4linelp_b;"

	".gp4end:"
		"popl	%%edi;"
		"popl	%%esi;"
		"popl	%%ecx;"
		"popl	%%ebx;"
		"popf;"
	: /* output: nothing */
	: "m" (page), "m" (opaq), "m" (VLINE), "m" (CRTC_Regs[0x29]), 
	  "m" (TextDotX), "g" (Grp_LineBuf)
	: "ax", "dx");
#else /* !USE_ASM && !(USE_GAS && __i386__) */
	WORD *srcp, *destp;	// XXX: ALIGN
	DWORD x, y;
	DWORD off;
	DWORD i;
	WORD v;

	page &= 3;

	y = GrphScrollY[page] + VLINE;
	if ((CRTC_Regs[0x29] & 0x1c) == 0x1c)
		y += VLINE;
	y = (y & 0x1ff) << 10;

	x = GrphScrollX[page] & 0x1ff;
	off = y + x * 2;

	x ^= 0x1ff;

	srcp = (WORD *)(GVRAM + off + (page >> 1));
	destp = (WORD *)Grp_LineBuf;

	v = 0;
	i = 0;

	if (page & 1) {
		if (opaq) {
			if (x < TextDotX) {
				for (; i < x; ++i) {
					v = GET_WORD_W8(srcp);
					srcp++;
					v = GrphPal[(v >> 4) & 0xf];
					*destp++ = v;
				}
				srcp -= 0x200;
			}
			for (; i < TextDotX; ++i) {
				v = GET_WORD_W8(srcp);
				srcp++;
				v = GrphPal[(v >> 4) & 0xf];
				*destp++ = v;
			}
		} else {
			if (x < TextDotX) {
				for (; i < x; ++i) {
					v = GET_WORD_W8(srcp);
					srcp++;
					v = (v >> 4) & 0x0f;
					if (v != 0x00)
						*destp = GrphPal[v];
					destp++;
				}
				srcp -= 0x200;
			}
			for (; i < TextDotX; ++i) {
				v = GET_WORD_W8(srcp);
				srcp++;
				v = (v >> 4) & 0x0f;
				if (v != 0x00)
					*destp = GrphPal[v];
				destp++;
			}
		}
	} else {
		if (opaq) {
			if (x < TextDotX) {
				for (; i < x; ++i) {
					v = GET_WORD_W8(srcp);
					srcp++;
					v = GrphPal[v & 0x0f];
					*destp++ = v;
				}
				srcp -= 0x200;
			}
			for (; i < TextDotX; ++i) {
				v = GET_WORD_W8(srcp);
				srcp++;
				v = GrphPal[v & 0x0f];
				*destp++ = v;
			}
		} else {
			if (x < TextDotX) {
				for (; i < x; ++i) {
					v = GET_WORD_W8(srcp);
					srcp++;
					v &= 0x0f;
					if (v != 0x00)
						*destp = GrphPal[v];
					destp++;
				}
				srcp -= 0x200;
			}
			for (; i < TextDotX; ++i) {
				v = GET_WORD_W8(srcp);
				srcp++;
				v &= 0x0f;
				if (v != 0x00)
					*destp = GrphPal[v];
				destp++;
			}
		}
	}
#endif /* USE_ASM */
}

					// この画面モードは勘弁して下さい…
void FASTCALL Grp_DrawLine4h(void)
{
#ifdef USE_ASM
	__asm
	{
		push	esi
		push	edi
		mov	esi, GrphScrollY[0]
		add	esi, VLINE
		mov	al, CRTC_Regs[0x29]
		and	al, 1ch
		cmp	al, 1ch
		jne	gp4hlinenotspecial
		add	esi, VLINE
	gp4hlinenotspecial:
		and	esi, 1023
		test	esi, 512
		jnz	gp4h_plane23
		shl	esi, 10
		mov	edx, GrphScrollX[0]
		mov	edi, edx
		and	edx, 511
		add	esi, edx
		add	esi, edx
		mov	cl, 00h
		test	edi, 512
		jz	gp4h_main
		add	cl, 4
		jmp	gp4h_main
	gp4h_plane23:
		and	esi, 511
		shl	esi, 10
		mov	edx, GrphScrollX[0]
		mov	edi, edx
		and	edx, 511
		add	esi, edx
		add	esi, edx
		mov	cl, 08h
		test	edi, 512
		jz	gp4h_main
		add	cl, 4
	gp4h_main:
		and	edi, 511
		xor	di, 511
		inc	di
		//and	di, 512
		mov	ebx, TextDotX
		xor	edx, edx
	gp4hlinelp:
		mov	ax, word ptr GVRAM[esi]
		shr	ax, cl
		and	eax, 15
		mov	ax, word ptr GrphPal[eax*2]
		mov	Grp_LineBuf[edx], ax
		add	esi, 2
		add	edx, 2
		dec	di
		jnz	gp4hline_cnt
		sub	esi, 0400h
		xor	cl, 4
		mov	di, 512
	gp4hline_cnt:
		dec	bx
		jnz	gp4hlinelp
//		loop	gp4hlinelp
		pop	edi
		pop	esi
	}
#elif defined(USE_GAS) && defined(__i386__)
	__asm__ __volatile__ (
		"pushl	%%esi;"
		"pushl	%%edi;"
		"movl	%0, %%esi;"
		"addl	%1, %%esi;"
		"movb	%2, %%al;"
		"andb	$0x1c, %%al;"
		"cmpb	$0x1c, %%al;"
		"jne	.gp4hlinenotspecial;"
		"addl	%1, %%esi;"
	".gp4hlinenotspecial:"
		"andl	$1023, %%esi;"
		"testl	$512, %%esi;"
		"jnz	.gp4h_plane23;"
		"shll	$10, %%esi;"
		"movl	%3, %%edx;"
		"movl	%%edx, %%edi;"
		"andl	$511, %%edx;"
		"addl	%%edx, %%esi;"
		"addl	%%edx, %%esi;"
		"movb	$0, %%cl;"
		"testl	$512, %%edi;"
		"jz	.gp4h_main;"
		"addb	$4, %%cl;"
		"jmp	.gp4h_main;"
	".gp4h_plane23:"
		"andl	$511, %%esi;"
		"shll	$10, %%esi;"
		"movl	%3, %%edx;"
		"movl	%%edx, %%edi;"
		"andl	$511, %%edx;"
		"addl	%%edx, %%esi;"
		"addl	%%edx, %%esi;"
		"movb	$0x08, %%cl;"
		"testl	$512, %%edi;"
		"jz	.gp4h_main;"
		"addb	$4, %%cl;"
	".gp4h_main:"
		"andl	$511, %%edi;"
		"xorw	$511, %%di;"
		"incw	%%di;"
		"movl	%4, %%ebx;"
		"xorl	%%edx, %%edx;"
	".gp4hlinelp:"
		"movw	GVRAM(%%esi), %%ax;"
		"shrw	%%cl, %%ax;"
		"andl	$15, %%eax;"
		"movw	GrphPal(, %%eax, 2), %%ax;"
		"movw	%%ax, Grp_LineBuf(%%edx);"
		"addl	$2, %%esi;"
		"addl	$2, %%edx;"
		"decw	%%di;"
		"jnz	.gp4hline_cnt;"
		"subl	$0x400, %%esi;"
		"xorb	$4, %%cl;"
		"movw	$512, %%di;"
	".gp4hline_cnt:"
		"decw	%%bx;"
		"jnz	.gp4hlinelp;"
		"popl	%%edi;"
		"popl	%%esi;"
	: /* output: nothing */
	: "m" (GrphScrollY[0]), "m" (VLINE), "m" (CRTC_Regs[0x29]), 
	  "m" (GrphScrollX[0]), "m" (TextDotX)
	: "ax", "bx", "cx", "dx");
#else /* !USE_ASM && !(USE_GAS && __i386__) */
	WORD *srcp, *destp;
	DWORD x, y;
	DWORD i;
	WORD v;
	int bits;

	y = GrphScrollY[0] + VLINE;
	if ((CRTC_Regs[0x29] & 0x1c) == 0x1c)
		y += VLINE;
	y &= 0x3ff;

	if ((y & 0x200) == 0x000) {
		y <<= 10;
		bits = (GrphScrollX[0] & 0x200) ? 4 : 0;
	} else {
		y = (y & 0x1ff) << 10;
		bits = (GrphScrollX[0] & 0x200) ? 12 : 8;
	}

	x = GrphScrollX[0] & 0x1ff;
	srcp = (WORD *)(GVRAM + y + x * 2);
	destp = (WORD *)Grp_LineBuf;

	x = ((x & 0x1ff) ^ 0x1ff) + 1;

	for (i = 0; i < TextDotX; ++i) {
		v = *srcp++;
		*destp++ = GrphPal[(v >> bits) & 0x0f];

		if (--x == 0) {
			srcp -= 0x200;
			bits ^= 4;
			x = 512;
		}
	}
#endif /* !USE_ASM */
}


// -------------------------------------------------
// --- 半透明／特殊Priのベースとなるページの描画 ---
// -------------------------------------------------
void FASTCALL Grp_DrawLine16SP(void)
{
#ifdef USE_ASM
	__asm
	{
		push	edi
		push	esi
		mov	edi, 0
		mov	esi, GrphScrollY[0]
		add	esi, VLINE
		mov	al, CRTC_Regs[0x29]
		and	al, 1ch
		cmp	al, 1ch
		jne	gp16splinenotspecial
		add	esi, VLINE
	gp16splinenotspecial:
		and	esi, 511
		shl	esi, 10
		mov	ebx, GrphScrollX[0]
		and	ebx, 511
		add	esi, ebx
		add	esi, ebx
		xor	ebx, 511
		inc	ebx
		mov	ecx, TextDotX
		xor	edx, edx
	gp16splinelp:
		movzx	eax, byte ptr GVRAM[esi+1]
		mov	dh, byte ptr Pal_Regs[eax*2]
		movzx	eax, byte ptr GVRAM[esi]
		mov	dl, byte ptr Pal_Regs[eax*2+1]
		test	al, 1
		jnz	gp16splinesp
		and	dx, 0fffeh
		mov	ax, word ptr Pal16[edx*2]
		mov	word ptr Grp_LineBufSP[edi], 0
		mov	Grp_LineBufSP2[edi], ax
		jmp	gp16splineskip
	gp16splinesp:
		and	dx, 0fffeh
		mov	ax, word ptr Pal16[edx*2]
		mov	word ptr Grp_LineBufSP2[edi], 0
		mov	word ptr Grp_LineBufSP[edi], ax
	gp16splineskip:
		add	esi, 2
		add	edi, 2
		dec	ebx
		jnz	gp16spline_cnt
		sub	esi, 0400h
	gp16spline_cnt:
//		dec	cx
//		jnz	gp16splinelp
		loop	gp16splinelp
		pop	esi
		pop	edi
	}
#elif defined(USE_GAS) && defined(__i386__)
	__asm__ __volatile__ (
		"pushl	%%edi;"
		"pushl	%%esi;"
		"movl	$0, %%edi;"
		"movl	%0, %%esi;"
		"addl	%1, %%esi;"
		"movb	%2, %%al;"
		"andb	$0x1c, %%al;"
		"cmpb	$0x1c, %%al;"
		"jne	.gp16splinenotspecial;"
		"addl	%1, %%esi;"
	".gp16splinenotspecial:"
		"andl	$511, %%esi;"
		"shll	$10, %%esi;"
		"movl	%3, %%ebx;"
		"andl	$511, %%ebx;"
		"addl	%%ebx, %%esi;"
		"addl	%%ebx, %%esi;"
		"xorl	$511, %%ebx;"
		"incl	%%ebx;"
		"movl	%4, %%ecx;"
		"xorl	%%edx, %%edx;"
	".gp16splinelp:"
		"movzbl	GVRAM + 1(%%esi), %%eax;"
		"movb	Pal_Regs(, %%eax, 2), %%dh;"
		"movzbl	GVRAM(%%esi), %%eax;"
		"movb	Pal_Regs + 1(, %%eax, 2), %%dl;"
		"testb	$1, %%al;"
		"jnz	.gp16splinesp;"
		"andw	$0xfffe, %%dx;"
		"movw	Pal16(, %%edx, 2), %%ax;"
		"movw	$0, Grp_LineBufSP(%%edi);"
		"movw	%%ax, Grp_LineBufSP2(%%edi);"
		"jmp	.gp16splineskip;"
	".gp16splinesp:"
		"andw	$0xfffe, %%dx;"
		"movw	Pal16(, %%edx, 2), %%ax;"
		"movw	$0, Grp_LineBufSP2(%%edi);"
		"movw	%%ax, Grp_LineBufSP(%%edi);"
	".gp16splineskip:"
		"addl	$2, %%esi;"
		"addl	$2, %%edi;"
		"decl	%%ebx;"
		"jnz	.gp16spline_cnt;"
		"subl	$0x0400, %%esi;"
	".gp16spline_cnt:"
		"loop	.gp16splinelp;"
		"popl	%%esi;"
		"popl	%%edi;"
	: /* output: nothing */
	: "m" (GrphScrollY[0]), "m" (VLINE), "m" (CRTC_Regs[0x29]), 
	  "m" (GrphScrollX[0]), "m" (TextDotX)
	: "ax", "bx", "cx", "dx");
#else /* !USE_ASM && !(USE_GAS && __i386__) */
	DWORD x, y;
	DWORD off;
	DWORD i;
	WORD v;

	y = GrphScrollY[0] + VLINE;
	if ((CRTC_Regs[0x29] & 0x1c) == 0x1c)
		y += VLINE;
	y = (y & 0x1ff) << 10;

	x = GrphScrollX[0] & 0x1ff;
	off = y + x * 2;
	x = (x ^ 0x1ff) + 1;

	for (i = 0; i < TextDotX; ++i) {
		v = (Pal_Regs[GVRAM[off+1]*2] << 8) | Pal_Regs[GVRAM[off]*2+1];
		if ((GVRAM[off] & 1) == 0) {
			Grp_LineBufSP[i] = 0;
			Grp_LineBufSP2[i] = Pal16[v & 0xfffe];
		} else {
			Grp_LineBufSP[i] = Pal16[v & 0xfffe];
			Grp_LineBufSP2[i] = 0;
		}

		off += 2;
		if (--x == 0)
			off -= 0x400;
	}
#endif /* USE_ASM */
}


void FASTCALL Grp_DrawLine8SP(int page)
{
#ifdef USE_ASM
		__asm
		{
			push	esi
push	edi
and	ecx, 1
mov	esi, GrphScrollY[ecx*8]
mov	edi, GrphScrollY[ecx*8+4]
			add	esi, VLINE
add	edi, VLINE
			mov	al, CRTC_Regs[0x29]
			and	al, 1ch
			cmp	al, 1ch
			jne	gp8splinenotspecial
			add	esi, VLINE
add	edi, VLINE
		gp8splinenotspecial:
			and	esi, 511
			shl	esi, 10
and	edi, 511
shl	edi, 10
mov	eax, GrphScrollX[ecx*8+4]
and	eax, 511
add	edi, eax
add	edi, eax
			mov	ebx, GrphScrollX[ecx*8]
			and	ebx, 511
			add	esi, ebx
			add	esi, ebx
add	esi, ecx
add	edi, ecx

			xor	bx, 511
			inc	bx
			mov	ecx, TextDotX
			xor	edx, edx
			xor	eax, eax
		gp8osplinelp:
			mov	al, byte ptr GVRAM[esi]
mov ah, byte ptr GVRAM[edi]
and ah, 0f0h
and al, 0fh
or al, ah
			//xor	ah, ah
			test	al, 1
			jnz	gp8osplinesp
			and	eax, 0feh
			jz	gp8onotzero2
			mov	ax, word ptr GrphPal[eax*2]
		gp8onotzero2:
			mov	word ptr Grp_LineBufSP[edx], 0
			mov	word ptr Grp_LineBufSP2[edx], ax
			jmp	gp8osplineskip
		gp8osplinesp:
			and	eax, 0feh
			jz	gp8onotzero			// ついんびー。Palette0以外の$0000は特殊Priでは透明じゃなく黒扱いらしい
			mov	ax, word ptr GrphPal[eax*2]
			or	ax, Ibit			// Palette0以外はIbit立ててごまかそー
		gp8onotzero:					// 半透明が変になる時は、半透明と特殊Priを別ルーチンにしなきゃ…
			mov	word ptr Grp_LineBufSP[edx], ax
			mov	word ptr Grp_LineBufSP2[edx], 0
		gp8osplineskip:
			add	esi, 2
add	edi, 2
test	di, 03feh
jnz	gp8spnotxend
sub	edi, 0400h
gp8spnotxend:
			add	edx, 2
			dec	bx
			jnz	gp8ospline_cnt
			sub	esi, 0400h
		gp8ospline_cnt:
			loop	gp8osplinelp
pop	edi
			pop	esi
		}
#elif defined(USE_GAS) && defined(__i386__)
	__asm__ __volatile__ (
		"pushl	%%esi;"
		"pushl	%%edi;"

		"movl	%0, %%ecx;"	// page

		"andl	$1, %%ecx;"
		"movl	GrphScrollY(, %%ecx, 8), %%esi;"
		"movl	GrphScrollY + 4(, %%ecx, 8), %%edi;"
		"addl	%1, %%esi;"
		"addl	%1, %%edi;"
		"movb	%2, %%al;"
		"andb	$0x1c, %%al;"
		"cmpb	$0x1c, %%al;"
		"jne	.gp8splinenotspecial;"
		"addl	%1, %%esi;"
		"addl	%1, %%edi;"
	".gp8splinenotspecial:"
		"andl	$511, %%esi;"
		"shll	$10, %%esi;"
		"andl	$511, %%edi;"
		"shll	$10, %%edi;"
		"movl	GrphScrollX + 4(, %%ecx, 8), %%eax;"
		"andl	$511, %%eax;"
		"addl	%%eax, %%edi;"
		"addl	%%eax, %%edi;"
		"movl	GrphScrollX(, %%ecx, 8), %%ebx;"
		"andl	$511, %%ebx;"
		"addl	%%ebx, %%esi;"
		"addl	%%ebx, %%esi;"
		"addl	%%ecx, %%esi;"
		"addl	%%ecx, %%edi;"

		"xorw	$511, %%bx;"
		"incw	%%bx;"
		"movl	%2, %%ecx;"
		"xorl	%%edx, %%edx;"
		"xorl	%%eax, %%eax;"
	".gp8osplinelp:"
		"movb	GVRAM(%%esi), %%al;"
		"movb	GVRAM(%%edi), %%ah;"
		"andb	$0x0f0, %%ah;"
		"andb	$0x0f, %%al;"
		"orb	%%ah, %%al;"
		"testb	$1, %%al;"
		"jnz	.gp8osplinesp;"
		"andl	$0x0fe, %%eax;"
		"jz	.gp8onotzero2;"
		"movw	GrphPal(, %%eax, 2), %%ax;"
	".gp8onotzero2:"
		"movw	$0, Grp_LineBufSP(%%edx);"
		"movw	%%ax, Grp_LineBufSP2(%%edx);"
		"jmp	.gp8osplineskip;"
	".gp8osplinesp:"
		"andl	$0x0fe, %%eax;"
		"jz	.gp8onotzero;"
		"movw	GrphPal(, %%eax, 2), %%ax;"
		"or	%4, %%ax;"
	".gp8onotzero:"
		"movw	%%ax, Grp_LineBufSP(%%edx);"
		"movw	$0, Grp_LineBufSP2(%%edx);"
	".gp8osplineskip:"
		"addl	$2, %%esi;"
		"addl	$2, %%edi;"
		"testw	$0x03fe, %%di;"
		"jnz	.gp8spnotxend;"
		"subl	$0x0400, %%edi;"
	".gp8spnotxend:"
		"addl	$2, %%edx;"
		"decw	%%bx;"
		"jnz	.gp8ospline_cnt;"
		"subl	$0x0400, %%esi;"
	".gp8ospline_cnt:"
		"loop	.gp8osplinelp;"

		"popl	%%edi;"
		"popl	%%esi;"
	: /* output: nothing */
	: "m" (page), "m" (VLINE), "m" (CRTC_Regs[0x29]), "m" (TextDotX),
	  "m" (Ibit)
	: "ax", "bx", "cx", "dx");
#else /* !USE_ASM && !(USE_GAS && __i386__) */
	DWORD x, x0;
	DWORD y, y0;
	DWORD off, off0;
	DWORD i;
	WORD v;

	page &= 1;

	y = GrphScrollY[page * 2] + VLINE;
	y0 = GrphScrollY[page * 2 + 1] + VLINE;
	if ((CRTC_Regs[0x29] & 0x1c) == 0x1c) {
		y += VLINE;
		y0 += VLINE;
	}
	y = (y & 0x1ff) << 10;
	y0 = (y0 & 0x1ff) << 10;

	x = GrphScrollX[page * 2] & 0x1ff;
	x0 = GrphScrollX[page * 2 + 1] & 0x1ff;

	off = y + x * 2 + page;
	off0 = y0 + x0 * 2 + page;

	x = (x ^ 0x1ff) + 1;

	for (i = 0; i < TextDotX; ++i) {
		v = (GVRAM[off] & 0x0f) | (GVRAM[off0] & 0xf0);
		if ((v & 1) == 0) {
			v &= 0xfe;
			if (v != 0x00)
				v = GrphPal[v];
			Grp_LineBufSP[i] = 0;
			Grp_LineBufSP2[i] = v;
		} else {
			v &= 0xfe;
			if (v != 0x00)
				v = GrphPal[v] | Ibit;
			Grp_LineBufSP[i] = v;
			Grp_LineBufSP2[i] = 0;
		}

		off += 2;
		off0 += 2;
		if ((off0 & 0x3fe) == 0)
			off0 -= 0x400;
		if (--x == 0)
			off -= 0x400;
	}
#endif /* USE_ASM */
}


void FASTCALL Grp_DrawLine4SP(DWORD page/*, int opaq*/)
{
	DWORD scrx, scry;
	page &= 3;
	switch(page)		// 美しくなさすぎる（笑）
	{
	case 0:	scrx = GrphScrollX[0]; scry = GrphScrollY[0]; break;
	case 1: scrx = GrphScrollX[1]; scry = GrphScrollY[1]; break;
	case 2: scrx = GrphScrollX[2]; scry = GrphScrollY[2]; break;
	case 3: scrx = GrphScrollX[3]; scry = GrphScrollY[3]; break;
	}

#ifdef USE_ASM
	if (page&1)
	{
//	if (opaq)
//	{
		__asm
		{
			push	esi
			mov	esi, scry
			add	esi, VLINE
			mov	al, CRTC_Regs[0x29]
			and	al, 1ch
			cmp	al, 1ch
			jne	gp4splinenotspecial
			add	esi, VLINE
		gp4splinenotspecial:
			and	esi, 511
			shl	esi, 10
			mov	ebx, scrx
			and	ebx, 511
			add	esi, ebx
			add	esi, ebx
//			add	esi, vram
			test	page, 2
			jz	gp4ospline2page0
			inc	esi
		gp4ospline2page0:
			xor	bx, 511
			inc	bx
			mov	ecx, TextDotX
			xor	edx, edx
		gp4osplinelp2:
			mov	al, byte ptr GVRAM[esi]
			shr	al, 4
			test	al, 1
			jnz	gp4o2splinesp
			and	eax, 14
			mov	ax, word ptr GrphPal[eax*2]
			mov	word ptr Grp_LineBufSP[edx], 0
			mov	Grp_LineBufSP2[edx], ax
			jmp	gp4o2splineskip
		gp4o2splinesp:
			and	eax, 14
			mov	ax, word ptr GrphPal[eax*2]
			mov	word ptr Grp_LineBufSP[edx], ax
			mov	word ptr Grp_LineBufSP2[edx], 0
		gp4o2splineskip:
			add	esi, 2
			add	edx, 2
			dec	bx
			jnz	gp4ospline_cnt2
			sub	esi, 0400h
		gp4ospline_cnt2:
//			dec	cx
//			jnz	gp4osplinelp2
			loop	gp4osplinelp2
			pop	esi
		}
/*	}
	else
	{
		__asm
		{
			push	esi
			mov	esi, scry
			add	esi, VLINE
			and	esi, 511
			shl	esi, 10
			mov	ebx, scrx
			and	ebx, 511
			add	esi, ebx
			add	esi, ebx
//			add	esi, vram
			test	page, 2
			jz	gp4spline2page0
			inc	esi
		gp4spline2page0:
			xor	bx, 511
			inc	bx
			mov	ecx, TextDotX
			xor	edx, edx
		gp4splinelp2:
			mov	al, byte ptr GVRAM[esi]
			shr	al, 4
			test	al, 1
			jnz	gp4splinesp2
			and	eax, 14
			jz	gp4splineskip2_2
			mov	ax, word ptr GrphPal[eax*2]
			mov	Grp_LineBuf[edx], ax
		gp4splineskip2_2:
			mov	word ptr Grp_LineBufSP[edx], 0
			jmp	gp4splineskip2
		gp4splinesp2:
			and	eax, 14
			jz	gp4splineskip2_2
			mov	ax, word ptr GrphPal[eax*2]
//			mov	Grp_LineBuf[edx], ax
			mov	word ptr Grp_LineBufSP[edx], ax
		gp4splineskip2:
			add	esi, 2
			add	edx, 2
			dec	bx
			jnz	gp4spline_cnt2
			sub	esi, 0400h
		gp4spline_cnt2:
//			dec	cx
//			jnz	gp4splinelp2
			loop	gp4splinelp2
			pop	esi
		}
	}
*/
	}
	else
	{
//	if (opaq)
//	{
		__asm
		{
			push	esi
			mov	esi, scry
			add	esi, VLINE
			mov	al, CRTC_Regs[0x29]
			and	al, 1ch
			cmp	al, 1ch
			jne	gp4osplinenotspecial
			add	esi, VLINE
		gp4osplinenotspecial:
			and	esi, 511
			shl	esi, 10
			mov	ebx, scrx
			and	ebx, 511
			add	esi, ebx
			add	esi, ebx
//			add	esi, vram
			test	page, 2
			jz	gp4osplinepage0
			inc	esi
		gp4osplinepage0:
			xor	bx, 511
			inc	bx
			mov	ecx, TextDotX
			xor	edx, edx
		gp4osplinelp:
			mov	al, byte ptr GVRAM[esi]
			test	al, 1
			jnz	gp4osplinesp
			and	eax, 14
			mov	ax, word ptr GrphPal[eax*2]
			mov	word ptr Grp_LineBufSP[edx], 0
			mov	Grp_LineBufSP2[edx], ax
			jmp	gp4osplineskip
		gp4osplinesp:
			and	eax, 14
			mov	ax, word ptr GrphPal[eax*2]
			mov	word ptr Grp_LineBufSP[edx], ax
			mov	word ptr Grp_LineBufSP2[edx], 0
		gp4osplineskip:
			add	esi, 2
			add	edx, 2
			dec	bx
			jnz	gp4ospline_cnt
			sub	esi, 0400h
		gp4ospline_cnt:
//			dec	cx
//			jnz	gp4osplinelp
			loop	gp4osplinelp
			pop	esi
		}
/*	}
	else
	{
		__asm
		{
			push	esi
			mov	esi, scry
			add	esi, VLINE
			and	esi, 511
			shl	esi, 10
			mov	ebx, scrx
			and	ebx, 511
			add	esi, ebx
			add	esi, ebx
//			add	esi, vram
			test	page, 2
			jz	gp4splinepage0
			inc	esi
		gp4splinepage0:
			xor	bx, 511
			inc	bx
			mov	ecx, TextDotX
			xor	edx, edx
		gp4splinelp:
			mov	al, byte ptr GVRAM[esi]
			test	al, 1
			jnz	gp4splinesp
			and	eax, 14
			jz	gp4splineskip_2
			mov	ax, word ptr GrphPal[eax*2]
			mov	Grp_LineBuf[edx], ax
		gp4splineskip_2:
			mov	word ptr Grp_LineBufSP[edx], 0
			jmp	gp4splineskip
		gp4splinesp:
			and	eax, 14
			jz	gp4splineskip_2
			mov	ax, word ptr GrphPal[eax*2]
//			mov	Grp_LineBuf[edx], ax
			mov	word ptr Grp_LineBufSP[edx], ax
		gp4splineskip:
			add	esi, 2
			add	edx, 2
			dec	bx
			jnz	gp4spline_cnt
			sub	esi, 0400h
		gp4spline_cnt:
//			dec	cx
//			jnz	gp4splinelp
			loop	gp4splinelp
			pop	esi
		}
	}
*/	}
#elif defined(USE_GAS) && defined(__i386__)
	if (page & 1) {
		__asm__ __volatile__ (
			"pushl	%%esi;"
			"movl	%1, %%esi;"
			"addl	%2, %%esi;"
			"movb	%3, %%al;"
			"andb	$0x1c, %%al;"
			"cmpb	$0x1c, %%al;"
			"jne	.gp4splinenotspecial;"
			"addl	%2, %%esi;"
		".gp4splinenotspecial:"
			"andl	$511, %%esi;"
			"shll	$10, %%esi;"
			"movl	%4, %%ebx;"
			"andl	$511, %%ebx;"
			"addl	%%ebx, %%esi;"
			"addl	%%ebx, %%esi;"
			"testl	$2, %0;"
			"jz	.gp4ospline2page0;"
			"incl	%%esi;"
		".gp4ospline2page0:"
			"xorw	$511, %%bx;"
			"incw	%%bx;"
			"movl	%5, %%ecx;"
			"xorl	%%edx, %%edx;"
		".gp4osplinelp2:"
			"movb	GVRAM(%%esi), %%al;"
			"shrb	$4, %%al;"
			"testb	$1, %%al;"
			"jnz	.gp4o2splinesp;"
			"andl	$14, %%eax;"
			"movw	GrphPal(, %%eax, 2), %%ax;"
			"movw	$0, Grp_LineBufSP(%%edx);"
			"movw	%%ax, Grp_LineBufSP2(%%edx);"
			"jmp	.gp4o2splineskip;"
		".gp4o2splinesp:"
			"andl	$14, %%eax;"
			"movw	GrphPal(, %%eax, 2), %%ax;"
			"movw	%%ax, Grp_LineBufSP(%%edx);"
			"movw	$0, Grp_LineBufSP2(%%edx);"
		".gp4o2splineskip:"
			"addl	$2, %%esi;"
			"addl	$2, %%edx;"
			"decw	%%bx;"
			"jnz	.gp4ospline_cnt2;"
			"subl	$0x0400, %%esi;"
		".gp4ospline_cnt2:"
			"loop	.gp4osplinelp2;"
			"popl	%%esi;"
		: /* output: nothing */
		: "m" (page), "m" (scry), "m" (VLINE), "m" (CRTC_Regs[0x29]),
		  "m" (scrx), "m" (TextDotX)
		: "ax", "bx", "cx", "dx");
	} else {
		__asm__ __volatile__ (
			"pushl	%%esi;"
			"movl	%1, %%esi;"
			"addl	%2, %%esi;"
			"movb	%3, %%al;"
			"andb	$0x1c, %%al;"
			"cmpb	$0x1c, %%al;"
			"jne	.gp4osplinenotspecial;"
			"addl	%2, %%esi;"
		".gp4osplinenotspecial:"
			"andl	$511, %%esi;"
			"shll	$10, %%esi;"
			"movl	%4, %%ebx;"
			"andl	$511, %%ebx;"
			"addl	%%ebx, %%esi;"
			"addl	%%ebx, %%esi;"
			"testl	$2, %0;"
			"jz	.gp4osplinepage0;"
			"incl	%%esi;"
		".gp4osplinepage0:"
			"xorw	$511, %%bx;"
			"incw	%%bx;"
			"movl	%5, %%ecx;"
			"xorl	%%edx, %%edx;"
		".gp4osplinelp:"
			"movb	GVRAM(%%esi), %%al;"
			"testb	$1, %%al;"
			"jnz	.gp4osplinesp;"
			"andl	$14, %%eax;"
			"movw	GrphPal(, %%eax, 2), %%ax;"
			"movw	$0, Grp_LineBufSP(%%edx);"
			"movw	%%ax, Grp_LineBufSP2(%%edx);"
			"jmp	.gp4osplineskip;"
		".gp4osplinesp:"
			"andl	$14, %%eax;"
			"movw	GrphPal(, %%eax, 2), %%ax;"
			"movw	%%ax, Grp_LineBufSP(%%edx);"
			"movw	$0, Grp_LineBufSP2(%%edx);"
		".gp4osplineskip:"
			"addl	$2, %%esi;"
			"addl	$2, %%edx;"
			"decw	%%bx;"
			"jnz	.gp4ospline_cnt;"
			"subl	$0x0400, %%esi;"
		".gp4ospline_cnt:"
			"loop	.gp4osplinelp;"
			"popl	%%esi;"
		: /* output: nothing */
		: "m" (page), "m" (scry), "m" (VLINE), "m" (CRTC_Regs[0x29]),
		  "m" (scrx), "m" (TextDotX)
		: "ax", "bx", "cx", "dx");
	}
#else /* !USE_ASM && !(USE_GAS && __i386__) */
{
	DWORD x, y;
	DWORD off;
	DWORD i;
	WORD v;

	if (page & 1) {
		y = scry + VLINE;
		if ((CRTC_Regs[0x29] & 0x1c) == 0x1c)
			y += VLINE;
		y = (y & 0x1ff) << 10;

		x = scrx & 0x1ff;
		off = y + x * 2;
		if (page & 2)
			off++;
		x = (x ^ 0x1ff) + 1;

		for (i = 0; i < TextDotX; ++i) {
			v = GVRAM[off] >> 4;
			if ((v & 1) == 0) {
				v &= 0x0e;
				Grp_LineBufSP[i] = 0;
				Grp_LineBufSP2[i] = GrphPal[v];
			} else {
				v &= 0x0e;
				Grp_LineBufSP[i] = GrphPal[v];
				Grp_LineBufSP2[i] = 0;
			}

			off += 2;
			if (--x == 0)
				off -= 0x400;
		}
	} else {
		y = scry + VLINE;
		if ((CRTC_Regs[0x29] & 0x1c) == 0x1c)
			y += VLINE;
		y = (y & 0x1ff) << 10;

		x = scrx & 0x1ff;
		off = y + x * 2;
		if (page & 2)
			off++;
		x = (x ^ 0x1ff) + 1;

		for (i = 0; i < TextDotX; ++i) {
			v = GVRAM[off];
			if ((v & 1) == 0) {
				v &= 0x0e;
				Grp_LineBufSP[i] = 0;
				Grp_LineBufSP2[i] = GrphPal[v];
			} else {
				v &= 0x0e;
				Grp_LineBufSP[i] = GrphPal[v];
				Grp_LineBufSP2[i] = 0;
			}

			off += 2;
			if (--x == 0)
				off -= 0x400;
		}
	}
}
#endif /* USE_ASM */
}


void FASTCALL Grp_DrawLine4hSP(void)
{
#ifdef USE_ASM
	__asm
	{
		push	esi
		push	edi
		mov	esi, GrphScrollY[0]
		add	esi, VLINE
		mov	al, CRTC_Regs[0x29]
		and	al, 1ch
		cmp	al, 1ch
		jne	gp4hsplinenotspecial
		add	esi, VLINE
	gp4hsplinenotspecial:
		and	esi, 1023
		test	esi, 512
		jnz	gp4hsp_plane23
		shl	esi, 10
		mov	edx, GrphScrollX[0]
		mov	edi, edx
		and	edx, 511
		add	esi, edx
		add	esi, edx
		mov	cl, 00h
		test	edi, 512
		jz	gp4hsp_main
		add	cl, 4
		jmp	gp4hsp_main
	gp4hsp_plane23:
		and	esi, 511
		shl	esi, 10
		mov	edx, GrphScrollX[0]
		mov	edi, edx
		and	edx, 511
		add	esi, edx
		add	esi, edx
		mov	cl, 08h
		test	edi, 512
		jz	gp4hsp_main
		add	cl, 4
	gp4hsp_main:
		and	edi, 511
		xor	di, 511
		inc	di
		mov	ebx, TextDotX
		xor	edx, edx
	gp4hsplinelp:
		mov	ax, word ptr GVRAM[esi]
		shr	ax, cl
		test	ax, 1
		jnz	gp4hsplinesp
		and	eax, 14
		mov	ax, word ptr GrphPal[eax*2]
		mov	word ptr Grp_LineBufSP[edx], 0
		mov	Grp_LineBufSP2[edx], ax
		jmp	gp4hsplineskip
	gp4hsplinesp:
		and	eax, 14
		mov	ax, word ptr GrphPal[eax*2]
		mov	word ptr Grp_LineBufSP[edx], ax
		mov	word ptr Grp_LineBufSP2[edx], 0
	gp4hsplineskip:
		add	esi, 2
		add	edx, 2
		dec	di
		jnz	gp4hspline_cnt
		sub	esi, 0800h
	gp4hspline_cnt:
		dec	bx
		jnz	gp4hsplinelp
//		loop	gp4hsplinelp
		pop	edi
		pop	esi
	}
#elif defined(USE_GAS) && defined(__i386__)
	__asm__ __volatile__ (
		"pushl	%%esi;"
		"pushl	%%edi;"
		"movl	%0, %%esi;"
		"addl	%1, %%esi;"
		"movb	%2, %%al;"
		"andb	$0x1c, %%al;"
		"cmpb	$0x1c, %%al;"
		"jne	.gp4hsplinenotspecial;"
		"addl	%1, %%esi;"
	".gp4hsplinenotspecial:"
		"andl	$1023, %%esi;"
		"testl	$512, %%esi;"
		"jnz	.gp4hsp_plane23;"
		"shll	$10, %%esi;"
		"movl	%3, %%edx;"
		"movl	%%edx, %%edi;"
		"andl	$511, %%edx;"
		"addl	%%edx, %%esi;"
		"addl	%%edx, %%esi;"
		"movb	$0x00, %%cl;"
		"testl	$512, %%edi;"
		"jz	.gp4hsp_main;"
		"addb	$4, %%cl;"
		"jmp	.gp4hsp_main;"
	".gp4hsp_plane23:"
		"andl	$511, %%esi;"
		"shll	$10, %%esi;"
		"movl	%3, %%edx;"
		"movl	%%edx, %%edi;"
		"andl	$511, %%edx;"
		"addl	%%edx, %%esi;"
		"addl	%%edx, %%esi;"
		"movb	$0x08, %%cl;"
		"testl	$512, %%edi;"
		"jz	.gp4hsp_main;"
		"addb	$4, %%cl;"
	".gp4hsp_main:"
		"andl	$511, %%edi;"
		"xorw	$511, %%di;"
		"incw	%%di;"
		"movl	%4, %%ebx;"
		"xorl	%%edx, %%edx;"
	".gp4hsplinelp:"
		"movw	GVRAM(%%esi), %%ax;"
		"shrw	%%cl, %%ax;"
		"testw	$1, %%ax;"
		"jnz	.gp4hsplinesp;"
		"andl	$14, %%eax;"
		"movw	GrphPal(, %%eax, 2), %%ax;"
		"movw	$0, Grp_LineBufSP(%%edx);"
		"movw	%%ax, Grp_LineBufSP2(%%edx);"
		"jmp	.gp4hsplineskip;"
	".gp4hsplinesp:"
		"andl	$14, %%eax;"
		"movw	GrphPal(, %%eax, 2), %%ax;"
		"movw	%%ax, Grp_LineBufSP(%%edx);"
		"movw	$0, Grp_LineBufSP2(%%edx);"
	".gp4hsplineskip:"
		"addl	$2, %%esi;"
		"addl	$2, %%edx;"
		"decw	%%di;"
		"jnz	.gp4hspline_cnt;"
		"subl	$0x0800, %%esi;"
	".gp4hspline_cnt:"
		"decw	%%bx;"
		"jnz	.gp4hsplinelp;"
		"popl	%%edi;"
		"popl	%%esi;"
	: /* output: nothing */
	: "m" (GrphScrollY[0]), "m" (VLINE), "m" (CRTC_Regs[0x29]),
	  "m" (GrphScrollX[0]), "m" (TextDotX)
	: "ax", "bx", "cx", "dx");
#else /* !USE_ASM && !(USE_GAS && __i386__) */
	WORD *srcp;
	DWORD x, y;
	DWORD i;
	int bits;
	WORD v;

	y = GrphScrollY[0] + VLINE;
	if ((CRTC_Regs[0x29] & 0x1c) == 0x1c)
		y += VLINE;
	y &= 0x3ff;

	if ((y & 0x200) == 0x000) {
		y <<= 10;
		bits = (GrphScrollX[0] & 0x200) ? 4 : 0;
	} else {
		y = (y & 0x1ff) << 10;
		bits = (GrphScrollX[0] & 0x200) ? 12 : 8;
	}

	x = GrphScrollX[0] & 0x1ff;
	srcp = (WORD *)(GVRAM + y + x * 2);
	x = ((x & 0x1ff) ^ 0x1ff) + 1;

	for (i = 0; i < TextDotX; ++i) {
		v = *srcp++ >> bits;
		if ((v & 1) == 0) {
			Grp_LineBufSP[i] = 0;
			Grp_LineBufSP2[i] = GrphPal[v & 0x0e];
		} else {
			Grp_LineBufSP[i] = GrphPal[v & 0x0e];
			Grp_LineBufSP2[i] = 0;
		}

		if (--x == 0)
			srcp -= 0x400;
	}
#endif /* USE_ASM */
}



// -------------------------------------------------
// --- 半透明の対象となるページの描画 --------------
// 2ページ以上あるグラフィックモードのみなので、
// 256色2面 or 16色4面のモードのみ。
// 256色時は、Opaqueでない方のモードはいらないかも…
// （必ずOpaqueモードの筈）
// -------------------------------------------------
// ここはまだ32色x4面モードの実装をしてないれす…
// （れじすた足りないよぅ…）
// -------------------------------------------------
							// やけにすっきり
LABEL void FASTCALL
Grp_DrawLine8TR(int page, int opaq)
{
#ifdef USE_ASM
	__asm {
			or	edx, edx
			jz	opaq_zero
			push	ebx
			push	ecx
			push	esi
			push	edi
			and	ecx, 1
			mov	esi, GrphScrollY[ecx*8]
			add	esi, VLINE
			mov	al, CRTC_Regs[0x29]
			and	al, 1ch
			cmp	al, 1ch
			jne	gp8trlinenotspecial
			add	esi, VLINE
		gp8trlinenotspecial:
			and	esi, 511
			shl	esi, 10
			mov	ebx, GrphScrollX[ecx*8]
			and	ebx, 511
			add	esi, ecx
			mov	edx, TextDotX
			xor	edi, edi
		gp8otrlinelp:
			movzx	eax, word ptr Grp_LineBufSP[edi]
			movzx	ecx, byte ptr GVRAM[esi+ebx*2]
			or	ax, ax
			jnz	gp8otrlinetr
			mov	cx, word ptr GrphPal[ecx*2]
			jmp	gp8otrlinenorm
		gp8otrlinetr:
			jcxz	gp8otrlinenorm		// けろぴー…
			mov	cx, word ptr GrphPal[ecx*2]
			jcxz	gp8otrlinenorm		// けろぴー…
			and	ax, Pal_HalfMask
			test	cx, Ibit
			jz	gp8otrlinetrI
			or	ax, Pal_Ix2
		gp8otrlinetrI:
			and	cx, Pal_HalfMask
			add	cx, ax			// 17bit計算中
			rcr	cx, 1			// 17bit計算中
		gp8otrlinenorm:
			mov	Grp_LineBuf[edi], cx
			inc	bx
			and	bh, 1			// and	bx, 511
			add	edi, 2
			dec	dx
			jnz	gp8otrlinelp
			pop	edi
			pop	esi
			pop	ecx
			pop	ebx
		opaq_zero:
			ret
	}
#elif defined(USE_GAS) && defined(__i386__)
	__asm__ __volatile__ (
		"movl	%0, %%ecx;"	// ecx = page
		"movl	%1, %%edx;"	// edx = opaq

		"orl	%%edx, %%edx;"
		"jz	.opaq_zero;"

		"andl	$1, %%ecx;"
		"movl	GrphScrollY(, %%ecx, 8), %%esi;"
		"addl	%2, %%esi;"
		"movb	%3, %%al;"
		"andb	$0x1c, %%al;"
		"cmpb	$0x1c, %%al;"
		"jne	.gp8trlinenotspecial;"
		"addl	%2, %%esi;"
	".gp8trlinenotspecial:"
		"andl	$511, %%esi;"
		"shll	$10, %%esi;"
		"movl	GrphScrollX(, %%ecx, 8), %%ebx;"
		"andl	$511, %%ebx;"
		"addl	%%ecx, %%esi;"
		"movl	%4, %%edx;"
		"xorl	%%edi, %%edi;"
	".gp8otrlinelp:"
		"movzwl	Grp_LineBufSP(%%edi), %%eax;"
		"movzbl	GVRAM(%%esi, %%ebx, 2), %%ecx;"
		"orw	%%ax, %%ax;"
		"jnz	.gp8otrlinetr;"
		"movw	GrphPal(, %%ecx, 2), %%cx;"
		"jmp	.gp8otrlinenorm;"
	".gp8otrlinetr:"
		"jcxz	.gp8otrlinenorm;"	// けろぴー…
		"movw	GrphPal(, %%ecx, 2), %%cx;"
		"jcxz	.gp8otrlinenorm;"	// けろぴー…
		"andw	%5, %%ax;"
		"testw	%6, %%cx;"
		"jz	.gp8otrlinetrI;"
		"orw	%7, %%ax;"
	".gp8otrlinetrI:"
		"andw	%5, %%cx;"
		"addw	%%ax, %%cx;"		// 17bit計算中
		"rcrw	$1, %%cx;"		// 17bit計算中
	".gp8otrlinenorm:"
		"movw	%%cx, Grp_LineBuf(%%edi);"
		"incw	%%bx;"
		"andb	$1, %%bh;"		// and	bx, 511
		"addl	$2, %%edi;"
		"decw	%%dx;"
		"jnz	.gp8otrlinelp;"

	".opaq_zero:"
	: /* output: nothing */
	: "m" (page), "m" (opaq), "m" (VLINE), "m" (CRTC_Regs[0x29]),
	  "m" (TextDotX), "m" (Pal_HalfMask), "m" (Ibit), "m" (Pal_Ix2)
	: "ax", "bx", "cx", "dx", "si", "di");
#else /* !USE_ASM && !(USE_GAS && __i386__) */
	if (opaq) {
		DWORD x, y;
		DWORD v, v0;
		DWORD i;

		page &= 1;

		y = GrphScrollY[page * 2] + VLINE;
		if ((CRTC_Regs[0x29] & 0x1c) == 0x1c)
			y += VLINE;
		y = ((y & 0x1ff) << 10) + page;
		x = GrphScrollX[page * 2] & 0x1ff;

		for (i = 0; i < TextDotX; ++i, x = (x + 1) & 0x1ff) {
			v0 = Grp_LineBufSP[i];
			v = GVRAM[y + x * 2];

			if (v0 != 0) {
				if (v != 0) {
					v = GrphPal[v];
					if (v != 0) {
						v0 &= Pal_HalfMask;
						if (v & Ibit)
							v0 |= Pal_Ix2;
						v &= Pal_HalfMask;
						v += v0;
						v >>= 1;
					}
				}
			} else
				v = GrphPal[v];
			Grp_LineBuf[i] = (WORD)v;
		}
	}
#endif /* USE_ASM */
}

LABEL void FASTCALL
Grp_DrawLine4TR(DWORD page, int opaq)
{
#ifdef USE_ASM
	__asm {
			push	ebx
			push	ecx
			push	esi
			push	edi

			and	ecx, 3
			mov	esi, GrphScrollY[ecx*4]
			add	esi, VLINE
			mov	al, CRTC_Regs[0x29]
			and	al, 1ch
			cmp	al, 1ch
			jne	gp4trlinenotspecial
			add	esi, VLINE
		gp4trlinenotspecial:
			and	esi, 511
			shl	esi, 10
			mov	ebx, GrphScrollX[ecx*4]
			and	ebx, 511
			xor	edi, edi

			shr	cl, 1
			jnc	pagebit0eq0		// jmp (page 0 or 2)

			add	esi, ecx		// ecx = page/2
			or	edx, edx
			je	gp4trline2page0

			mov	edx, TextDotX
		gp4otrlinelp2:
			movzx	eax, word ptr Grp_LineBufSP[edi]
			movzx	ecx, byte ptr GVRAM[esi+ebx*2]
			shr	cl, 4
			or	ax, ax
			jnz	gp4otrlinetr2
			mov	cx, word ptr GrphPal[ecx*2]
			jmp	gp4otrlinenorm2
		gp4otrlinetr2:
			jcxz	gp4otrlinenorm2		// けろぴー
			mov	cx, word ptr GrphPal[ecx*2]
			jcxz	gp4otrlinenorm2		// けろぴー
			and	ax, Pal_HalfMask
			test	cx, Ibit
			jz	gp4otrlinetr2I
			or	ax, Pal_Ix2
		gp4otrlinetr2I:
			and	cx, Pal_HalfMask
			add	cx, ax			// 17bit計算中
			rcr	cx, 1			// 17bit計算中
		gp4otrlinenorm2:
			mov	Grp_LineBuf[edi], cx
			inc	bx
			and	bh, 1			// and	bx, 511
			add	edi, 2
			dec	dx
			jnz	gp4otrlinelp2
			pop	edi
			pop	esi
			pop	ecx
			pop	ebx
			ret

		gp4trline2page0:
			mov	edx, TextDotX
		gp4trlinelp2:
			movzx	eax, word ptr Grp_LineBufSP[edi]
			or	ax, ax
			jnz	gp4trlinetr2
			movzx	ecx, byte ptr GVRAM[esi+ebx*2]
			shr	cl, 4
			jcxz	gp4trlineskip2
			mov	cx, word ptr GrphPal[ecx*2]
			jmp	gp4trlinenorm2
		gp4trlinetr2:
			movzx	ecx, byte ptr GVRAM[esi+ebx*2]
			shr	cl, 4
			jcxz	gp4trlinenorm2		// けろぴー
			mov	cx, word ptr GrphPal[ecx*2]
			jcxz	gp4trlineskip2		// けろぴー
			and	ax, Pal_HalfMask
			test	cx, Ibit
			jz	gp4trlinetr2I
			or	ax, Pal_Ix2
		gp4trlinetr2I:
			and	cx, Pal_HalfMask
			add	cx, ax			// 17bit計算中
			rcr	cx, 1			// 17bit計算中
		gp4trlinenorm2:
			mov	Grp_LineBuf[edi], cx
		gp4trlineskip2:
			inc	bx
			and	bh, 1			// and	bx, 511
			add	edi, 2
			dec	dx
			jnz	gp4trlinelp2
			pop	edi
			pop	esi
			pop	ecx
			pop	ebx
			ret

		pagebit0eq0:
			add	esi, ecx		// ecx = page/2
			or	edx, edx
			je	gp4trlinepage0

			mov	edx, TextDotX
		gp4otrlinelp:
			mov	cl, byte ptr GVRAM[esi+ebx*2]
			and	ecx, 15
			movzx	eax, word ptr Grp_LineBufSP[edi]
			or	ax, ax
			jnz	gp4otrlinetr
			mov	cx, word ptr GrphPal[ecx*2]
			jmp	gp4otrlinenorm
		gp4otrlinetr:
			jcxz	gp4otrlinenorm		// けろぴー
			mov	cx, word ptr GrphPal[ecx*2]
			jcxz	gp4otrlinenorm		// けろぴー
			and	ax, Pal_HalfMask
			test	cx, Ibit
			jz	gp4otrlinetrI
			or	ax, Pal_Ix2
		gp4otrlinetrI:
			and	cx, Pal_HalfMask
			add	cx, ax			// 17bit計算中
			rcr	cx, 1			// 17bit計算中
		gp4otrlinenorm:
			mov	Grp_LineBuf[edi], cx
			inc	bx
			and	bh, 1			// and	bx, 511
			add	edi, 2
			dec	dx
			jnz	gp4otrlinelp
			pop	edi
			pop	esi
			pop	ecx
			pop	ebx
			ret

		gp4trlinepage0:
			mov	edx, TextDotX
		gp4trlinelp:
			mov	cl, byte ptr GVRAM[esi+ebx*2]
			and	ecx, 15
			movzx	eax, word ptr Grp_LineBufSP[edi]
			or	ax, ax
			jnz	gp4trlinetr

			jcxz	gp4trlineskip
			mov	cx, word ptr GrphPal[ecx*2]
			jmp	gp4trlinenorm

		gp4trlinetr:
			jcxz	gp4trlinenorm		// けろぴー
			mov	cx, word ptr GrphPal[ecx*2]
			jcxz	gp4trlineskip		// けろぴー
			and	ax, Pal_HalfMask
			test	cx, Ibit
			jz	gp4trlinetrI
			or	ax, Pal_Ix2
		gp4trlinetrI:
			and	cx, Pal_HalfMask
			add	cx, ax			// 17bit計算中
			rcr	cx, 1			// 17bit計算中
		gp4trlinenorm:
			mov	Grp_LineBuf[edi], cx
		gp4trlineskip:
			inc	bx
			and	bh, 1			// and	bx, 511
			add	edi, 2
			dec	dx
			jnz	gp4trlinelp
			pop	edi
			pop	esi
			pop	ecx
			pop	ebx
			ret
	}
#elif defined(USE_GAS) && defined(__i386__)
	__asm__ __volatile__ (
		"pushl	%%ebx;"
		"pushl	%%ecx;"
		"pushl	%%esi;"
		"pushl	%%edi;"

		"movl	%0, %%ecx;"	// ecx = page
		"movl	%1, %%edx;"	// edx = opaq

		"andl	$3, %%ecx;"
		"movl	GrphScrollY(, %%ecx, 4), %%esi;"
		"addl	%2, %%esi;"
		"movb	%3, %%al;"
		"andb	$0x1c, %%al;"
		"cmpb	$0x1c, %%al;"
		"jne	.gp4trlinenotspecial;"
		"addl	%2, %%esi;"
	".gp4trlinenotspecial:"
		"andl	$511, %%esi;"
		"shll	$10, %%esi;"
		"movl	GrphScrollX(, %%ecx, 4), %%ebx;"
		"andl	$511, %%ebx;"
		"xorl	%%edi, %%edi;"

		"shrb	$1, %%cl;"
		"jnc	.pagebit0eq0;"

		"addl	%%ecx, %%esi;"
		"orl	%%edx, %%edx;"
		"je	.gp4trline2page0;"

		"movl	%4, %%edx;"
	".gp4otrlinelp2:"
		"movzwl	Grp_LineBufSP(%%edi), %%eax;"
		"movzbl	GVRAM(%%esi, %%ebx, 2), %%ecx;"
		"shrb	$4, %%cl;"
		"orw	%%ax, %%ax;"
		"jnz	.gp4otrlinetr2;"
		"movw	GrphPal(, %%ecx, 2), %%cx;"
		"jmp	.gp4otrlinenorm2;"
	".gp4otrlinetr2:"
		"jcxz	.gp4otrlinenorm2;"
		"movw	GrphPal(, %%ecx, 2), %%cx;"
		"jcxz	.gp4otrlinenorm2;"
		"andw	%5, %%ax;"
		"testw	%6, %%cx;"
		"jz	.gp4otrlinetr2I;"
		"orw	%7, %%ax;"
	".gp4otrlinetr2I:"
		"andw	%5, %%cx;"
		"addw	%%ax, %%cx;"
		"rcrw	$1, %%cx;"
	".gp4otrlinenorm2:"
		"movw	%%cx, Grp_LineBuf(%%edi);"
		"incw	%%bx;"
		"andb	$1, %%bh;"
		"addl	$2, %%edi;"
		"decw	%%dx;"
		"jnz	.gp4otrlinelp2;"
		"jmp	.gp4tr_end;"

	".gp4trline2page0:"
		"movl	%4, %%edx;"
	".gp4trlinelp2:"
		"movzwl	Grp_LineBufSP(%%edi), %%eax;"
		"orw	%%ax, %%ax;"
		"jnz	.gp4trlinetr2;"
		"movzbl	GVRAM(%%esi, %%ebx, 2), %%ecx;"
		"shrb	$4, %%cl;"
		"jcxz	.gp4trlineskip2;"
		"movw	GrphPal(, %%ecx, 2), %%cx;"
		"jmp	.gp4trlinenorm2;"
	".gp4trlinetr2:"
		"movzbl	GVRAM(%%esi, %%ebx, 2), %%ecx;"
		"shrb	$4, %%cl;"
		"jcxz	.gp4trlinenorm2;"
		"movw	GrphPal(, %%ecx, 2), %%cx;"
		"jcxz	.gp4trlineskip2;"
		"andw	%5, %%ax;"
		"testw	%6, %%cx;"
		"jz	.gp4trlinetr2I;"
		"orw	%7, %%ax;"
	".gp4trlinetr2I:"
		"andw	%5, %%cx;"
		"addw	%%ax, %%cx;"
		"rcrw	$1, %%cx;"
	".gp4trlinenorm2:"
		"movw	%%cx, Grp_LineBuf(%%edi);"
	".gp4trlineskip2:"
		"incw	%%bx;"
		"andb	$1, %%bh;"
		"addl	$2, %%edi;"
		"decw	%%dx;"
		"jnz	.gp4trlinelp2;"
		"jmp	.gp4tr_end;"

	".pagebit0eq0:"
		"addl	%%ecx, %%esi;"
		"orl	%%edx, %%edx;"
		"je	.gp4trlinepage0;"

		"movl	%4, %%edx;"
	".gp4otrlinelp:"
		"movb	GVRAM(%%esi, %%ebx, 2), %%cl;"
		"andl	$15, %%ecx;"
		"movzx	Grp_LineBufSP(%%edi), %%eax;"
		"orw	%%ax, %%ax;"
		"jnz	.gp4otrlinetr;"
		"movw	GrphPal(, %%ecx, 2), %%cx;"
		"jmp	.gp4otrlinenorm;"
	".gp4otrlinetr:"
		"jcxz	.gp4otrlinenorm;"
		"movw	GrphPal(, %%ecx, 2), %%cx;"
		"jcxz	.gp4otrlinenorm;"
		"andw	%5, %%ax;"
		"testw	%6, %%cx;"
		"jz	.gp4otrlinetrI;"
		"orw	%7, %%ax;"
	".gp4otrlinetrI:"
		"andw	%5, %%cx;"
		"addw	%%ax, %%cx;"
		"rcrw	$1, %%cx;"
	".gp4otrlinenorm:"
		"movw	%%cx, Grp_LineBuf(%%edi);"
		"incw	%%bx;"
		"andb	$1, %%bh;"
		"addl	$2, %%edi;"
		"decw	%%dx;"
		"jnz	.gp4otrlinelp;"
		"jmp	.gp4tr_end;"

	".gp4trlinepage0:"
		"movl	%4, %%edx;"
	".gp4trlinelp:"
		"movb	GVRAM(%%esi, %%ebx, 2), %%cl;"
		"andl	$15, %%ecx;"
		"movzx	Grp_LineBufSP(%%edi), %%eax;"
		"orw	%%ax, %%ax;"
		"jnz	.gp4trlinetr;"

		"jcxz	.gp4trlineskip;"
		"movw	GrphPal(, %%ecx, 2), %%cx;"
		"jmp	.gp4trlinenorm;"

	".gp4trlinetr:"
		"jcxz	.gp4trlinenorm;"
		"movw	GrphPal(, %%ecx, 2), %%cx;"
		"jcxz	.gp4trlineskip;"
		"andw	%5, %%ax;"
		"testw	%6, %%cx;"
		"jz	.gp4trlinetrI;"
		"orw	%7, %%ax;"
	".gp4trlinetrI:"
		"andw	%5, %%cx;"
		"addw	%%ax, %%cx;"
		"rcrw	$1, %%cx;"
	".gp4trlinenorm:"
		"mov	%%cx, Grp_LineBuf(%%edi);"
	".gp4trlineskip:"
		"incw	%%bx;"
		"andb	$1, %%bh;"
		"addl	$2, %%edi;"
		"decw	%%dx;"
		"jnz	.gp4trlinelp;"

	".gp4tr_end:"
		"popl	%%edi;"
		"popl	%%esi;"
		"popl	%%ecx;"
		"popl	%%ebx;"
	: /* output: nothing */
	: "m" (page), "m" (opaq), "m" (VLINE), "m" (CRTC_Regs[0x29]),
	  "m" (TextDotX), "m" (Pal_HalfMask), "m" (Ibit), "m" (Pal_Ix2)
	: "ax", "bx", "cx", "dx");
#else /* !USE_ASM && !(USE_GAS && __i386__) */
	DWORD x, y;
	DWORD v, v0;
	DWORD i;

	page &= 3;

	y = GrphScrollY[page] + VLINE;
	if ((CRTC_Regs[0x29] & 0x1c) == 0x1c)
		y += VLINE;
	y = (y & 0x1ff) << 10;
	x = GrphScrollX[page] & 0x1ff;

	if (page & 1) {
		page >>= 1;
		y += page;

		if (opaq) {
			for (i = 0; i < TextDotX; ++i, x = (x + 1) & 0x1ff) {
				v0 = Grp_LineBufSP[i];
				v = GVRAM[y + x * 2] >> 4;

				if (v0 != 0) {
					if (v != 0) {
						v = GrphPal[v];
						if (v != 0) {
							v0 &= Pal_HalfMask;
							if (v & Ibit)
								v0 |= Pal_Ix2;
							v &= Pal_HalfMask;
							v += v0;
							v >>= 1;
						}
					}
				} else
					v = GrphPal[v];
				Grp_LineBuf[i] = (WORD)v;
			}
		} else {
			for (i = 0; i < TextDotX; ++i, x = (x + 1) & 0x1ff) {
				v0 = Grp_LineBufSP[i];

				if (v0 == 0) {
					v = GVRAM[y + x * 2] >> 4;
					if (v != 0)
						Grp_LineBuf[i] = GrphPal[v];
				} else {
					v = GVRAM[y + x * 2] >> 4;
					if (v != 0) {
						v = GrphPal[v];
						if (v != 0) {
							v0 &= Pal_HalfMask;
							if (v & Ibit)
								v0 |= Pal_Ix2;
							v &= Pal_HalfMask;
							v += v0;
							v = GrphPal[v >> 1];
							Grp_LineBuf[i]=(WORD)v;
						}
					} else
						Grp_LineBuf[i] = (WORD)v;
				}
			}
		}
	} else {
		page >>= 1;
		y += page;

		if (opaq) {
			for (i = 0; i < TextDotX; ++i, x = (x + 1) & 0x1ff) {
				v = GVRAM[y + x * 2] & 0x0f;
				v0 = Grp_LineBufSP[i];

				if (v0 != 0) {
					if (v != 0) {
						v = GrphPal[v];
						if (v != 0) {
							v0 &= Pal_HalfMask;
							if (v & Ibit)
								v0 |= Pal_Ix2;
							v &= Pal_HalfMask;
							v += v0;
							v >>= 1;
						}
					}
				} else
					v = GrphPal[v];
				Grp_LineBuf[i] = (WORD)v;
			}
		} else {
			for (i = 0; i < TextDotX; ++i, x = (x + 1) & 0x1ff) {
				v = GVRAM[y + x * 2] & 0x0f;
				v0 = Grp_LineBufSP[i];

				if (v0 != 0) {
					if (v != 0) {
						v = GrphPal[v];
						if (v != 0) {
							v0 &= Pal_HalfMask;
							if (v & Ibit)
								v0 |= Pal_Ix2;
							v &= Pal_HalfMask;
							v += v0;
							v >>= 1;
							Grp_LineBuf[i]=(WORD)v;
						}
					} else
						Grp_LineBuf[i] = (WORD)v;
				} else if (v != 0)
					Grp_LineBuf[i] = GrphPal[v];
			}
		}
	}
#endif /* USE_ASM */
}

/*
MS-C のを gas のインラインアセンブリにてけとーに変換

s/\<.*:/.&/
s/[^ \t]/"&/
s/[^ \t:]$/&;"/
s/[^ \t]:$/&"/
s/\<e*[abcd][xlh]\>/%%&/g
s/\<e*[ds]i\>/%%&/g
s/\<[0-9]*[^h]\>/$&/g
s/\([^%]\)\(\<0*[0-9a-fA-F]*\)h/\1$0x\2/g
s/"\([^ \t]*\)[ \t][ \t]*\(.*\), \(.*\);/"\1 \3, \2;/
                                            + ここはタブ
s/\[/(/g
s/]/)/g
*/
