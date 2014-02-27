// ---------------------------------------------------------------------------------------
//  BG.C - BGとスプライト
//  ToDo：透明色の処理チェック（特に対Text間）
// ---------------------------------------------------------------------------------------

#include "common.h"
#include "windraw.h"
#include "winx68k.h"
#include "palette.h"
#include "tvram.h"
#include "crtc.h"
#include "bg.h"

#include "m68000.h"
#include "memory.h"

	BYTE	BG[0x8000];
	BYTE	Sprite_Regs[0x800];
	BYTE	BG_Regs[0x12];
	WORD	BG_CHREND = 0;
	WORD	BG_BG0TOP = 0;
	WORD	BG_BG0END = 0;
	WORD	BG_BG1TOP = 0;
	WORD	BG_BG1END = 0;
	BYTE	BG_CHRSIZE = 16;
	DWORD	BG_AdrMask = 511;
	DWORD	BG0ScrollX = 0, BG0ScrollY = 0;
	DWORD	BG1ScrollX = 0, BG1ScrollY = 0;

	long	BG_HAdjust = 0;
	long	BG_VLINE = 0;

	BYTE	BG_Dirty0[64*64];
	BYTE	BG_Dirty1[64*64];
	BYTE	BGCHR8[8*8*256];
	BYTE	BGCHR16[16*16*256];

	WORD	BG_LineBuf[1600];
	WORD	BG_PriBuf[1600];

	DWORD	VLINEBG = 0;


// -----------------------------------------------------------------------
//   初期化
// -----------------------------------------------------------------------
void BG_Init(void)
{
	DWORD i;
	ZeroMemory(Sprite_Regs, 0x800);
	ZeroMemory(BG, 0x8000);
	ZeroMemory(BGCHR8, 8*8*256);
	ZeroMemory(BGCHR16, 16*16*256);
	ZeroMemory(BG_LineBuf, 1600*2);
	for (i=0; i<0x12; i++)
		BG_Write(0xeb0800+i, 0);
	BG_CHREND = 0x8000;
}


// -----------------------------------------------------------------------
//   I/O Read
// -----------------------------------------------------------------------
BYTE FASTCALL BG_Read(DWORD adr)
{
	if ((adr>=0xeb0000)&&(adr<0xeb0400))
	{
		adr -= 0xeb0000;
		adr ^= 1;
		return Sprite_Regs[adr];
	}
	else if ((adr>=0xeb0800)&&(adr<0xeb0812))
		return BG_Regs[adr-0xeb0800];
	else if ((adr>=0xeb8000)&&(adr<0xec0000))
		return BG[adr-0xeb8000];
	else
		return 0xff;
}


// -----------------------------------------------------------------------
//   I/O Write
// -----------------------------------------------------------------------
void FASTCALL BG_Write(DWORD adr, BYTE data)
{
	DWORD bg16chr;
	int s1, s2, v = 0;
	s1 = (((BG_Regs[0x11]  &4)?2:1)-((BG_Regs[0x11]  &16)?1:0));
	s2 = (((CRTC_Regs[0x29]&4)?2:1)-((CRTC_Regs[0x29]&16)?1:0));
	if ( !(BG_Regs[0x11]&16) ) v = ((BG_Regs[0x0f]>>s1)-(CRTC_Regs[0x0d]>>s2));
	if ((adr>=0xeb0000)&&(adr<0xeb0400))
	{
		adr &= 0x3ff;
		adr ^= 1;
		if (Sprite_Regs[adr] != data)
		{
#ifdef USE_ASM
			_asm
			{
				mov	ebx, adr
				and	ebx, 3f8h
				mov	bx, word ptr Sprite_Regs[ebx+2]
				sub	bx, 16
				add	ebx, BG_VLINE
				sub	ebx, v
				and	ebx, 3ffh
				mov	al, 16
			spsetdirtylp1:
				mov	byte ptr TextDirtyLine[ebx], 1
				inc	bx
				and	bx, 3ffh
				dec	al
				jnz	spsetdirtylp1
			}
			Sprite_Regs[adr] = data;
			_asm
			{
				mov	ebx, adr
				and	ebx, 3f8h
				mov	bx, word ptr Sprite_Regs[ebx+2]
				sub	bx, 16
				add	ebx, BG_VLINE
				sub	ebx, v
				and	ebx, 3ffh
				mov	al, 16
			spsetdirtylp2:
				mov	byte ptr TextDirtyLine[ebx], 1
				inc	bx
				and	bx, 3ffh
				dec	al
				jnz	spsetdirtylp2
			}
#elif defined(USE_GAS) && defined(__i386__)
			asm (
				"mov	%0, %%ebx;"
				"and	$0x3f8, %%ebx;"
				"mov	Sprite_Regs + 2(%%ebx), %%bx;"
				"sub	$16, %%bx;"
				"add	(%1), %%ebx;"
				"sub	%2, %%ebx;"
				"and	$0x3ff, %%ebx;"
				"mov	$16, %%al;"
			"0:"
				"movb	$1, TextDirtyLine(%%ebx);"
				"inc	%%bx;"
				"and	$0x3ff, %%bx;"
				"dec	%%al;"
				"jnz	0b;"
			: /* output: nothing */
			: "m" (adr), "g" (BG_VLINE), "m" (v)
			: "ax", "bx", "memory");

			Sprite_Regs[adr] = data;

			asm (
				"mov	%0, %%ebx;"
				"and	$0x3f8, %%ebx;"
				"mov	Sprite_Regs + 2(%%ebx), %%bx;"
				"sub	$16, %%bx;"
				"add	(%1), %%ebx;"
				"sub	%2, %%ebx;"
				"and	$0x3ff, %%ebx;"
				"mov	$16, %%al;"
			"0:"
				"movb	$1, TextDirtyLine(%%ebx);"
				"inc	%%bx;"
				"and	$0x3ff, %%bx;"
				"dec	%%al;"
				"jnz	0b;"
			: /* output: nothing */
			: "m" (adr), "g" (BG_VLINE), "m" (v)
			: "ax", "bx", "memory");
#else /* !USE_ASM && !(USE_GAS && __i386__) */

			WORD t0, t, *pw;

			v = BG_VLINE - 16 - v;
			// get YPOS pointer (Sprite_Regs[] is little endian)
			pw = (WORD *)(Sprite_Regs + (adr & 0x3f8) + 2);

#define UPDATE_TDL(t)				\
{						\
	int i;					\
	for (i = 0; i < 16; i++) {		\
		TextDirtyLine[(t)] = 1;		\
		(t) = ((t) + 1) & 0x3ff;	\
	}					\
}

			t = t0 = (*pw + v) & 0x3ff;
			UPDATE_TDL(t);

			Sprite_Regs[adr] = data;

			t = (*pw + v) & 0x3ff;
			if (t != t0) {
				UPDATE_TDL(t);
			}

#endif /* USE_ASM */
		}
	}
	else if ((adr>=0xeb0800)&&(adr<0xeb0812))
	{
		adr -= 0xeb0800;
		if (BG_Regs[adr]==data) return;	// データに変化が無ければ帰る
		BG_Regs[adr] = data;
		switch(adr)
		{
		case 0x00:
		case 0x01:
			BG0ScrollX = (((DWORD)BG_Regs[0x00]<<8)+BG_Regs[0x01])&BG_AdrMask;
			TVRAM_SetAllDirty();
			break;
		case 0x02:
		case 0x03:
			BG0ScrollY = (((DWORD)BG_Regs[0x02]<<8)+BG_Regs[0x03])&BG_AdrMask;
			TVRAM_SetAllDirty();
			break;
		case 0x04:
		case 0x05:
			BG1ScrollX = (((DWORD)BG_Regs[0x04]<<8)+BG_Regs[0x05])&BG_AdrMask;
			TVRAM_SetAllDirty();
			break;
		case 0x06:
		case 0x07:
			BG1ScrollY = (((DWORD)BG_Regs[0x06]<<8)+BG_Regs[0x07])&BG_AdrMask;
			TVRAM_SetAllDirty();
			break;

		case 0x08:		// BG On/Off Changed
			TVRAM_SetAllDirty();
			break;

		case 0x0d:
			BG_HAdjust = ((long)BG_Regs[0x0d]-(CRTC_HSTART+4))*8;				// 水平方向は解像度による1/2はいらない？（Tetris）
			TVRAM_SetAllDirty();
			break;
		case 0x0f:
			BG_VLINE = ((long)BG_Regs[0x0f]-CRTC_VSTART)/((BG_Regs[0x11]&4)?1:2);	// BGとその他がずれてる時の差分
			TVRAM_SetAllDirty();
			break;

		case 0x11:		// BG ScreenRes Changed
			if (data&3)
			{
				if ((BG_BG0TOP==0x4000)||(BG_BG1TOP==0x4000))
					BG_CHREND = 0x4000;
				else if ((BG_BG0TOP==0x6000)||(BG_BG1TOP==0x6000))
					BG_CHREND = 0x6000;
				else
					BG_CHREND = 0x8000;
			}
			else
				BG_CHREND = 0x2000;
			BG_CHRSIZE = ((data&3)?16:8);
			BG_AdrMask = ((data&3)?1023:511);
			BG_HAdjust = ((long)BG_Regs[0x0d]-(CRTC_HSTART+4))*8;				// 水平方向は解像度による1/2はいらない？（Tetris）
			BG_VLINE = ((long)BG_Regs[0x0f]-CRTC_VSTART)/((BG_Regs[0x11]&4)?1:2);	// BGとその他がずれてる時の差分
			break;
		case 0x09:		// BG Plane Cfg Changed
			TVRAM_SetAllDirty();
			if (data&0x08)
			{
				if (data&0x30)
				{
					BG_BG1TOP = 0x6000;
					BG_BG1END = 0x8000;
				}
				else
				{
					BG_BG1TOP = 0x4000;
					BG_BG1END = 0x6000;
				}
			}
			else
				BG_BG1TOP = BG_BG1END = 0;
			if (data&0x01)
			{
				if (data&0x06)
				{
					BG_BG0TOP = 0x6000;
					BG_BG0END = 0x8000;
				}
				else
				{
					BG_BG0TOP = 0x4000;
					BG_BG0END = 0x6000;
				}
			}
			else
				BG_BG0TOP = BG_BG0END = 0;
			if (BG_Regs[0x11]&3)
			{
				if ((BG_BG0TOP==0x4000)||(BG_BG1TOP==0x4000))
					BG_CHREND = 0x4000;
				else if ((BG_BG0TOP==0x6000)||(BG_BG1TOP==0x6000))
					BG_CHREND = 0x6000;
				else
					BG_CHREND = 0x8000;
			}
			break;
		case 0x0b:
			break;
		}
		Draw_DrawFlag = 1;

	}
	else if ((adr>=0xeb8000)&&(adr<0xec0000))
	{
		adr -= 0xeb8000;
		if (BG[adr]==data) return;			// データに変化が無ければ帰る
		BG[adr] = data;
		if (adr<0x2000)
		{
			BGCHR8[adr*2]   = data>>4;
			BGCHR8[adr*2+1] = data&15;
		}
		bg16chr = ((adr&3)*2)+((adr&0x3c)*4)+((adr&0x40)>>3)+((adr&0x7f80)*2);
		BGCHR16[bg16chr]   = data>>4;
		BGCHR16[bg16chr+1] = data&15;

		if (adr<BG_CHREND)				// パターンエリア
		{
			TVRAM_SetAllDirty();
		}
		if ((adr>=BG_BG1TOP)&&(adr<BG_BG1END))	// BG1 MAPエリア
		{
			TVRAM_SetAllDirty();
		}
		if ((adr>=BG_BG0TOP)&&(adr<BG_BG0END))	// BG0 MAPエリア
		{
			TVRAM_SetAllDirty();
		}
	}
}

#ifndef USE_GAS
//#define USE_GAS
#endif

// -----------------------------------------------------------------------
//   1ライン分の描画
// -----------------------------------------------------------------------
#ifdef USE_ASM
#include	"bg.x86"
LABEL void FASTCALL BG_DrawLine(int opaq, int gd) {
	__asm {
			pushf
			push	ebx
			push	esi
			push	edi
			push	edx
			push	ebp

			//xor	eax, eax
			mov	ax, TextPal[0]
			shl	eax, 16
			mov	ax, TextPal[0]
			mov	ebx, 0xffffffff
			mov	edi, 16*2
			or	ecx, ecx			// ecx = opaq
			jz	noclrloop
			mov	ecx, TextDotX
			shr	ecx, 1
		BGLineClr_lp:
			mov	dword ptr BG_LineBuf[edi], eax
			mov	dword ptr BG_PriBuf[edi], ebx	// SP間のプライオリティ情報初期化
			add	edi, 4
			loop	BGLineClr_lp
			jmp	bgclrloopend

		noclrloop:
			mov	ecx, TextDotX
			shr	ecx, 1
		BGLineClr_lp2:
			mov	dword ptr BG_PriBuf[edi], ebx	// SP間のプライオリティ情報初期化
			add	edi, 4
			loop	BGLineClr_lp2

		bgclrloopend:
			or	edx, edx			// edx = gd
			je	BG_NOGRP

			cmp	BG_CHRSIZE, 8
			jne	BG16

			Sprite_DrawLineMcr(81, 1)
;			test	BG_Regs[9], 8
;			je	BG8_1skiped
;			BG_DrawLineMcr8(1, BG_BG1TOP, BG1ScrollX, BG1ScrollY)
;		BG8_1skiped:
			Sprite_DrawLineMcr(82, 2)
			test	BG_Regs[9], 1
			je	BG_0skiped
			BG_DrawLineMcr8(0, BG_BG0TOP, BG0ScrollX, BG0ScrollY)
			jmp	BG_0skiped

		BG16:
			Sprite_DrawLineMcr(161, 1)
;			test	BG_Regs[9], 8
;			je	BG16_1skiped
;			BG_DrawLineMcr16(1, BG_BG1TOP, BG1ScrollX, BG1ScrollY)
;		BG16_1skiped:
			Sprite_DrawLineMcr(162, 2)
			test	BG_Regs[9], 1
			je	BG_0skiped
			BG_DrawLineMcr16(0, BG_BG0TOP, BG0ScrollX, BG0ScrollY)
			jmp	BG_0skiped

		BG_NOGRP:
			cmp	BG_CHRSIZE, 8
			jne	BG16_ng

			Sprite_DrawLineMcr(ng81, 1)
			test	BG_Regs[9], 8
			je	BG8_ng_1skiped
			BG_DrawLineMcr8_ng(1, BG_BG1TOP, BG1ScrollX, BG1ScrollY)
		BG8_ng_1skiped:
			Sprite_DrawLineMcr(ng82, 2)
			test	BG_Regs[9], 1
			je	BG_0skiped
			BG_DrawLineMcr8_ng(0, BG_BG0TOP, BG0ScrollX, BG0ScrollY)
			jmp	BG_0skiped

		BG16_ng:
			Sprite_DrawLineMcr(ng161, 1)
			test	BG_Regs[9], 8
			je	BG16_ng_1skiped
			BG_DrawLineMcr16_ng(1, BG_BG1TOP, BG1ScrollX, BG1ScrollY)
		BG16_ng_1skiped:
			Sprite_DrawLineMcr(ng162, 2)
			test	BG_Regs[9], 1
			je	BG_0skiped
			BG_DrawLineMcr16_ng(0, BG_BG0TOP, BG0ScrollX, BG0ScrollY)
		BG_0skiped:
			Sprite_DrawLineMcr(163, 3)
			pop	ebp
			pop	edx
			pop	edi
			pop	esi
			pop	ebx
			popf
			ret
	}
}
#elif defined(USE_GAS) && defined(__i386__)
#if 0
LABEL void FASTCALL BG_DrawLine(int opaq, int gd) {
	extern LABEL void FASTCALL __BG_DrawLine(int opaq, int gd);
	__BG_DrawLine(opaq, gd);
}
#endif
#else /* !USE_ASM && !(USE_GAS && __i386__) */
struct SPRITECTRLTBL {
	WORD	sprite_posx;
	WORD	sprite_posy;
	WORD	sprite_ctrl;
	BYTE	sprite_ply;
	BYTE	dummy;
} __attribute__ ((packed));
typedef struct SPRITECTRLTBL SPRITECTRLTBL_T;

INLINE void
Sprite_DrawLineMcr(int pri)
{
	SPRITECTRLTBL_T *sct = (SPRITECTRLTBL_T *)Sprite_Regs;
	DWORD y;
	DWORD t;
	int n;

	for (n = 127; n >= 0; --n) {
		if ((sct[n].sprite_ply & 3) == pri) {
			SPRITECTRLTBL_T *sctp = &sct[n];

			t = (sctp->sprite_posx + BG_HAdjust) & 0x3ff;
			if (t >= TextDotX + 16)
				continue;

			y = sctp->sprite_posy & 0x3ff;
			y -= VLINEBG;
			y += BG_VLINE;
			y = -y;
			y += 16;

			// add y, 16; jnc .spline_lpcnt
			if (y <= 15) {
				BYTE *p;
				DWORD pal;
				int i, d;
				BYTE bh, dat;
				
				if (sctp->sprite_ctrl < 0x4000) {
					p = &BGCHR16[((sctp->sprite_ctrl * 256) & 0xffff)  + (y * 16)];
					d = 1;
				} else  if ((sctp->sprite_ctrl - 0x4000) & 0x8000) {
					p = &BGCHR16[((sctp->sprite_ctrl * 256) & 0xffff) + (((y * 16) & 0xff) ^ 0xf0) + 15];
					d = -1;
				} else if ((signed short)(sctp->sprite_ctrl) >= 0x4000) {
					p = &BGCHR16[((sctp->sprite_ctrl * 256) & 0xffff) + (y * 16) + 15];
					d = -1;
				}  else {
					p = &BGCHR16[((sctp->sprite_ctrl << 8) & 0xffff)  + (((y * 16) & 0xff) ^ 0xf0)];
					d = 1;
				}

				for (i = 0; i < 16; i++, t++, p += d) {
					pal = *p & 0xf;
					if (pal) {
						pal |= (sctp->sprite_ctrl >> 4) & 0xf0;
						if (BG_PriBuf[t] >= n * 8) {
							BG_LineBuf[t] = TextPal[pal];
							Text_TrFlag[t] |= 2;
							BG_PriBuf[t] = n * 8;
						}
					}
				}
			}
		}
	}
}

#define BG_DRAWLINE_LOOPY(cnt) \
{ \
	bl = bl << 4;							\
	for (j = 0; j < cnt; j++, esi += d, edi++) {			\
		dat = *esi | bl;					\
		if (dat == 0)						\
			continue;					\
		if ((dat & 0xf) || !(Text_TrFlag[edi + 1] & 2)) {	\
			BG_LineBuf[1 + edi] = TextPal[dat];		\
			Text_TrFlag[edi + 1] |= 2;			\
		}							\
	}								\
}

#define BG_DRAWLINE_LOOPY_NG(cnt) \
{  \
	bl = bl << 4;					    \
        for (j = 0; j < cnt; j++, esi += d, edi++) {	    \
                dat = *esi & 0xf;			    \
		if (dat) {				    \
			dat |= bl;			    \
                        BG_LineBuf[1 + edi] = TextPal[dat]; \
			Text_TrFlag[edi + 1] |= 2;	    \
                }					    \
        }						    \
}

void bg_drawline_loopx8(WORD BGTOP, DWORD BGScrollX, DWORD BGScrollY, long adjust, int ng)
{
       unsigned char dat, bl;
       int i, j, d;
       DWORD ebp, edx, edi, ecx;
       WORD si;
       BYTE *esi;

       ebp = ((BGScrollY + VLINEBG - BG_VLINE) & 7) << 3;
       edx = BGTOP + (((BGScrollY + VLINEBG - BG_VLINE) & 0x1f8) << 4);
       edi = ((BGScrollX - adjust) & 7) ^ 15;
       ecx = ((BGScrollX - adjust) & 0x1f8) >> 2;

       for (i = TextDotX >> 3; i >= 0; i--) {
               bl = BG[ecx + edx];
               si = (WORD)BG[ecx + edx + 1] << 6;

               if (bl < 0x40) {
                       esi = &BGCHR8[si + ebp];
                       d = +1;
               } else if ((bl - 0x40) & 0x80) {
                       esi = &BGCHR8[si + 0x3f - ebp];
                       d = -1;
               } else if ((signed char)bl >= 0x40) {
                       esi = &BGCHR8[si + ebp + 7];
                       d = -1;
               } else {
                       esi = &BGCHR8[si + 0x38 - ebp];
                       d = +1;
               }
	       if (ng) {
		       BG_DRAWLINE_LOOPY_NG(8);
	       } else {
		       BG_DRAWLINE_LOOPY(8);
	       }
               ecx += 2;
               ecx &= 0x7f;
       }
}

void bg_drawline_loopx16(WORD BGTOP, DWORD BGScrollX, DWORD BGScrollY, long adjust, int ng)
{
       unsigned char dat, bl;
       int i, j, d;
       DWORD ebp, edx, edi, ecx;
       WORD si;
       BYTE *esi;

       ebp = ((BGScrollY + VLINEBG - BG_VLINE) & 15) << 4;
       edx = BGTOP + (((BGScrollY + VLINEBG - BG_VLINE) & 0x3f0) << 3);
       edi = ((BGScrollX - adjust) & 15) ^ 15;
       ecx = ((BGScrollX - adjust) & 0x3f0) >> 3;

       for (i = TextDotX >> 4; i >= 0; i--) {
		bl = BG[ecx + edx];
		si = BG[ecx + edx + 1] << 8;

		if (bl < 0x40) {
			esi = &BGCHR16[si + ebp];
			d = +1;
		} else if ((bl - 0x40) & 0x80) {
			esi = &BGCHR16[si + 0xff - ebp];
			d = -1;
		} else if ((signed char)bl >= 0x40) {
			esi = &BGCHR16[si + ebp + 15];
			d = -1;
		} else {
			esi = &BGCHR16[si + 0xf0 - ebp];
			d = +1;
		}
		if (ng) {
			BG_DRAWLINE_LOOPY_NG(16);
		} else {
			BG_DRAWLINE_LOOPY(16);
		}
		ecx += 2;
		ecx &= 0x7f;
	}
}

INLINE void
BG_DrawLineMcr8(WORD BGTOP, DWORD BGScrollX, DWORD BGScrollY)
{
       bg_drawline_loopx8(BGTOP, BGScrollX, BGScrollY, BG_HAdjust, 0);
}

INLINE void
BG_DrawLineMcr16(WORD BGTOP, DWORD BGScrollX, DWORD BGScrollY)
{
	bg_drawline_loopx16(BGTOP, BGScrollX, BGScrollY, BG_HAdjust, 0);
}

INLINE void
BG_DrawLineMcr8_ng(WORD BGTOP, DWORD BGScrollX, DWORD BGScrollY)
{
       bg_drawline_loopx8(BGTOP, BGScrollX, BGScrollY, BG_HAdjust, 1);
}

INLINE void
BG_DrawLineMcr16_ng(WORD BGTOP, DWORD BGScrollX, DWORD BGScrollY)
{
       bg_drawline_loopx16(BGTOP, BGScrollX, BGScrollY, 0, 1);
}

LABEL void FASTCALL
BG_DrawLine(int opaq, int gd)
{
	int i;
	void (*func8)(WORD, DWORD, DWORD), (*func16)(WORD, DWORD, DWORD);

	if (opaq) {
		for (i = 16; i < TextDotX + 16; ++i) {
			BG_LineBuf[i] = TextPal[0];
			BG_PriBuf[i] = 0xffff;
		}
	} else {
		for (i = 16; i < TextDotX + 16; ++i) {
			BG_PriBuf[i] = 0xffff;
		}
	}

	func8 = (gd)? BG_DrawLineMcr8 : BG_DrawLineMcr8_ng;
	func16 = (gd)? BG_DrawLineMcr16 : BG_DrawLineMcr16_ng;

	Sprite_DrawLineMcr(1);
	if ((BG_Regs[9] & 8) && (BG_CHRSIZE == 8)) { // BG1 on
		(*func8)(BG_BG1TOP, BG1ScrollX, BG1ScrollY);
	}
	Sprite_DrawLineMcr(2);
	if (BG_Regs[9] & 1) { // BG0 on
		if (BG_CHRSIZE == 8) {
			(*func8)(BG_BG0TOP, BG0ScrollX, BG0ScrollY);
		} else {
			(*func16)(BG_BG0TOP, BG0ScrollX, BG0ScrollY);
		}
	}
	Sprite_DrawLineMcr(3);
}
#endif /* USE_ASM */
