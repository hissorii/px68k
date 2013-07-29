/*	$Id: windraw.c,v 1.3 2008/11/08 01:42:42 nonaka Exp $	*/

/* 
 * Copyright (c) 2003 NONAKA Kimihiro
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <SDL.h>
#include "common.h"
#include "winx68k.h"
#include "winui.h"

#include "bg.h"
#include "crtc.h"
#include "gvram.h"
#include "mouse.h"
#include "palette.h"
#include "prop.h"
#include "status.h"
#include "tvram.h"

#include "../icons/keropi.xpm"

extern BYTE Debug_Text, Debug_Grp, Debug_Sp;

WORD *ScrBuf = 0;

int Draw_Opaque;
int FullScreenFlag = 0;
extern BYTE Draw_RedrawAllFlag;
BYTE Draw_DrawFlag = 1;
BYTE Draw_ClrMenu = 0;

BYTE Draw_BitMask[800];
BYTE Draw_TextBitMask[800];

int winx = 0, winy = 0;
DWORD winh = 0, winw = 0;
DWORD root_width, root_height;
WORD FrameCount = 0;
int SplashFlag = 0;

WORD WinDraw_Pal16B, WinDraw_Pal16R, WinDraw_Pal16G;

DWORD WindowX = 0;
DWORD WindowY = 0;

GdkImage *surface;
GdkRectangle surface_rect = { 16, 16, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT };
GdkImage *scaled_screen;
GdkPixmap *pixmap;
GdkPixmap *splash_pixmap;
static int screen_mode;


GdkImage *gdk_scale_image(GdkImage *dest, GdkImage *src, GdkRectangle *dest_rect, GdkRectangle *src_rect);


void WinDraw_InitWindowSize(WORD width, WORD height)
{
	static BOOL inited = FALSE;

	if (!inited) {
		GdkWindow *t, *root = window->window;

		while ((t = gdk_window_get_parent(root)) != 0)
			root = t;
		gdk_window_get_size(root, &root_width, &root_height);
		inited = TRUE;
	}

	gdk_window_get_position(window->window, &winx, &winy);

	winw = width;
	winh = height;

	if (root_width < winw)
		winx = (root_width - winw) / 2;
	else if (winx < 0)
		winx = 0;
	else if ((winx + winw) > root_width)
		winx = root_width - winw;
	if (root_height < winh)
		winy = (root_height - winh) / 2;
	else if (winy < 0)
		winy = 0;
	else if ((winy + winh) > root_height)
		winy = root_height - winh;
}

void WinDraw_ChangeSize(void)
{
	DWORD oldx = WindowX, oldy = WindowY;
	int dif;

	Mouse_ChangePos();

	switch (Config.WinStrech) {
	case 0:
		WindowX = TextDotX;
		WindowY = TextDotY;
		break;

	case 1:
		WindowX = 768;
		WindowY = 512;
		break;

	case 2:
		if (TextDotX <= 384)
			WindowX = TextDotX * 2;
		else
			WindowX = TextDotX;
		if (TextDotY <= 256)
			WindowY = TextDotY * 2;
		else
			WindowY = TextDotY;
		break;

	case 3:
		if (TextDotX <= 384)
			WindowX = TextDotX * 2;
		else
			WindowX = TextDotX;
		if (TextDotY <= 256)
			WindowY = TextDotY * 2;
		else
			WindowY = TextDotY;
		dif = WindowX - WindowY;
		if ((dif > -32) && (dif < 32)) {
			// 正方形に近い画面なら、としておこう
			WindowX = (int)(WindowX * 1.25);
		}
		break;
	}

	if ((WindowX > 768) || (WindowX <= 0)) {
		if (oldx)
			WindowX = oldx;
		else
			WindowX = oldx = 768;
	}
	if ((WindowY > 512) || (WindowY <= 0)) {
		if (oldy)
			WindowY = oldy;
		else
			WindowY = oldy = 512;
	}

	surface_rect.width = TextDotX;
	surface_rect.height = TextDotY;

	if ((oldx == WindowX) && (oldy == WindowY))
		return;

	if ((TextDotX == WindowX) && (TextDotY == WindowY))
		screen_mode = 0;
	else {
		screen_mode = 1;
		if (scaled_screen)
			gdk_image_destroy(scaled_screen);
		scaled_screen = gdk_image_new(GDK_IMAGE_FASTEST,
		    surface->visual, WindowX, WindowY);
	}
	if (surface) {
		bzero(ScrBuf, FULLSCREEN_WIDTH * FULLSCREEN_HEIGHT * 2);
#if 1
		gdk_draw_rectangle(pixmap, window->style->black_gc, TRUE,
		    0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
#else
		gdk_draw_rectangle(pixmap, window->style->black_gc, TRUE,
		    oldx, 0, FULLSCREEN_WIDTH - oldx, FULLSCREEN_HEIGHT);
		gdk_draw_rectangle(pixmap, window->style->black_gc, TRUE,
		    0, oldy, FULLSCREEN_WIDTH - oldx, FULLSCREEN_HEIGHT - oldy);
#endif
	}

	WinDraw_InitWindowSize((WORD)WindowX, (WORD)WindowY);
	gtk_widget_set_usize(drawarea, winw, winh);
	gtk_widget_set_uposition(window, winx, winy);
	StatBar_Show(Config.WindowFDDStat);
	Mouse_ChangePos();
}

//static int dispflag = 0;
void WinDraw_StartupScreen(void)
{
}

void WinDraw_CleanupScreen(void)
{
}

void WinDraw_ChangeMode(int flag)
{

	/* full screen mode(TRUE) <-> window mode(FALSE) */
	(void)flag;
}

void WinDraw_ShowSplash(void)
{

	gdk_draw_pixmap(pixmap,
	    drawarea->style->fg_gc[GTK_WIDGET_STATE(drawarea)],
	    splash_pixmap, 0, 0,
	    768 - keropi_xpm_width, 512 - keropi_xpm_height,
	    keropi_xpm_width, keropi_xpm_height);
}

void WinDraw_HideSplash(void)
{

	gdk_draw_rectangle(pixmap, window->style->black_gc, TRUE, 0, 0,
	    FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
	Draw_DrawFlag = 1;
}

int WinDraw_Init(void)
{
	GdkVisual *visual;
	GdkColormap *colormap;
	GdkBitmap *mask;

	SDL_Surface *sdl_surface;

	WindowX = 768;
	WindowY = 512;

	if ((root_width < WindowX) || (root_height < WindowY)) {
		fprintf(stderr, "No support resolution.\n");
		return FALSE;
	}

	visual = gtk_widget_get_visual(drawarea);

	switch (visual->type) {
	case GDK_VISUAL_TRUE_COLOR:
		break;
	case GDK_VISUAL_DIRECT_COLOR:
		if (visual->depth >= 15)
			break;
		/* FALLTHROUGH */
	default:
		fprintf(stderr, "No support visual class.\n");
		return FALSE;
	}

	sdl_surface = SDL_GetVideoSurface();
	if (sdl_surface == NULL) {
		g_message("can't create surface.");
		return 1;
	}

	WinDraw_Pal16R = sdl_surface->format->Rmask;
	WinDraw_Pal16G = sdl_surface->format->Gmask;
	WinDraw_Pal16B = sdl_surface->format->Bmask;

	ScrBuf = (WORD *)sdl_surface->pixels;

	SDL_LockSurface(sdl_surface);
	{
//		printf("hoge\n");
		int i;
		for (i = 0; i < 80*100; i++) {
			*(ScrBuf + i) = 0xffff;
		}
	}
	SDL_UnlockSurface(sdl_surface);

	pixmap = gdk_pixmap_new(drawarea->window,
	    FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT, visual->depth);
	if (pixmap == NULL) {
		g_message("can't create pixmap.");
		return FALSE;
	}
	gdk_draw_rectangle(pixmap, window->style->black_gc, TRUE, 0, 0,
	    FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);

	/* けろぴーすぷらっしゅ用意 */
	colormap = gtk_widget_get_colormap(window);
	splash_pixmap = gdk_pixmap_colormap_create_from_xpm_d(NULL, colormap,
	    &mask, NULL, keropi_xpm);
	if (splash_pixmap == NULL)
		g_error("Couldn't create replacement pixmap.");

	return TRUE;
}

void
WinDraw_Cleanup(void)
{

	if (splash_pixmap) {
		gdk_pixmap_unref(splash_pixmap);
		splash_pixmap = 0;
	}
	if (pixmap) {
		gdk_pixmap_unref(pixmap);
		pixmap = 0;
	}
	if (scaled_screen) {
		gdk_image_destroy(scaled_screen);
		scaled_screen = 0;
	}
	if (surface) {
		gdk_image_destroy(surface);
		surface = 0;
		ScrBuf = 0;
	}
	gdk_window_get_position(window->window, &winx, &winy);
}

void
WinDraw_Redraw(void)
{

	TVRAM_SetAllDirty();
}

void FASTCALL
WinDraw_Draw(void)
{
	SDL_Surface *sdl_surface;
	sdl_surface = SDL_GetVideoSurface();
	SDL_UpdateRect(sdl_surface, 0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);

	GtkWidget *w = (GtkWidget *)drawarea;
	GdkDrawable *d = (GdkDrawable *)drawarea->window;

	FrameCount++;
	if (!Draw_DrawFlag && is_installed_idle_process())
		return;
	Draw_DrawFlag = 0;

	if (SplashFlag)
		WinDraw_ShowSplash();

	if (screen_mode == 0) {
		gdk_draw_pixmap(d, w->style->fg_gc[GTK_WIDGET_STATE(w)],
		    pixmap, 0, 0, 0, 0, TextDotX, TextDotY);
	} else {
		gdk_scale_image(scaled_screen, surface, NULL, &surface_rect);
#if 0
		gdk_draw_image(d, w->style->fg_gc[GTK_WIDGET_STATE(w)],
		    scaled_screen, 0, 0, 0, 0, WindowX, WindowY);
#endif
	}
}

INLINE void WinDraw_DrawGrpLine(int opaq)
{
#ifdef USE_ASM
	DWORD adr = ((VLINE+16)*1600+32);
	if (opaq)
	{
	__asm {
			mov	ebx, adr
			add	ebx, offset ScrBuf
			mov	edx, offset Grp_LineBuf
			mov	ecx, TextDotX
			shr	cx, 1
		drawgrplineolp:
			mov	eax, [edx]
			mov	[ebx], eax
			add	edx, 4
			add	ebx, 4
			loop	drawgrplineolp
		}
	}
	else
	{
	__asm {
			mov	ebx, adr
			add	ebx, offset ScrBuf
			mov	edx, offset Grp_LineBuf
			mov	ecx, TextDotX
		drawgrplinelp:
			mov	ax, [edx]
			or	ax, ax
			jz	drawgrplineskip
			mov	[ebx], ax
		drawgrplineskip:
			add	edx, 2
			add	ebx, 2
			loop	drawgrplinelp
		}
	}
#elif defined(USE_GAS) && defined(__i386__)
	DWORD adr = ((VLINE+16)*1600+32);
	if (opaq) {
		asm (
			"mov	%0, %%ebx;"
			"add	%1, %%ebx;"
			"mov	%2, %%edx;"
			"mov	(%3), %%ecx;"
			"shr	$1, %%cx;"
		"0:"
			"mov	(%%edx), %%eax;"
			"mov	%%eax, (%%ebx);"
			"add	$4, %%edx;"
			"add	$4, %%ebx;"
			"loop	0b;"
		: /* output: nothing */
		: "m" (adr), "g" (ScrBuf), "g" (Grp_LineBuf), "g" (TextDotX)
		: "ax", "bx", "cx", "dx", "memory");
	} else {
		asm (
			"mov	%0, %%ebx;"
			"add	%1, %%ebx;"
			"mov	%2, %%edx;"
			"mov	(%3), %%ecx;"
			"shr	$1, %%cx;"
		"0:"
			"mov	(%%edx), %%ax;"
			"or	%%ax, %%ax;"
			"jz	1f;"
			"mov	%%ax, (%%ebx);"
		"1:"
			"add	$2, %%edx;"
			"add	$2, %%ebx;"
			"loop	0b;"
		: /* output: nothing */
		: "m" (adr), "g" (ScrBuf), "g" (Grp_LineBuf), "g" (TextDotX)
		: "ax", "bx", "cx", "dx", "memory");
	}
#else /* !USE_ASM && !(USE_GAS && __i386__) */
	DWORD adr = ((VLINE+16)*FULLSCREEN_WIDTH+16);
	WORD w;
	int i;

	if (opaq) {
		memcpy(&ScrBuf[adr], Grp_LineBuf, TextDotX * 2);
	} else {
		for (i = 0; i < TextDotX; i++, adr++) {
			w = Grp_LineBuf[i];
			if (w != 0)
				ScrBuf[adr] = w;
		}
	}
#endif /* USE_ASM */
}

INLINE void WinDraw_DrawGrpLineNonSP(int opaq)
{
#ifdef USE_ASM
	DWORD adr = ((VLINE+16)*1600+32);
	if (opaq)
	{
	__asm {
			mov	ebx, adr
			add	ebx, offset ScrBuf
			mov	edx, offset Grp_LineBufSP2
			mov	ecx, TextDotX
		drawgrpnslinelpo:
			mov	ax, [edx]
			mov	[ebx], ax
			add	edx, 2
			add	ebx, 2
			loop	drawgrpnslinelpo
		}
	}
	else
	{
	__asm {
			mov	ebx, adr
			add	ebx, offset ScrBuf
			mov	edx, offset Grp_LineBufSP2
			mov	ecx, TextDotX
		drawgrpnslinelp:
			mov	ax, [edx]
			or	ax, ax
			jz	drawgrpnslineskip
			mov	[ebx], ax
		drawgrpnslineskip:
			add	edx, 2
			add	ebx, 2
			loop	drawgrpnslinelp
		}
	}
#elif defined(USE_GAS) && defined(__i386__)
	DWORD adr = ((VLINE+16)*1600+32);
	if (opaq) {
		asm (
			"mov	%0, %%ebx;"
			"add	%1, %%ebx;"
			"mov	%2, %%edx;"
			"mov	(%3), %%ecx;"
		"0:"
			"mov	(%%edx), %%ax;"
			"mov	%%ax, (%%ebx);"
			"add	$2, %%edx;"
			"add	$2, %%ebx;"
			"loop	0b;"
		: /* output: nothing */
		: "m" (adr), "g" (ScrBuf), "g" (Grp_LineBufSP2), "g" (TextDotX)
		: "ax", "bx", "cx", "dx", "memory");
	} else {
		asm (
			"mov	%0, %%ebx;"
			"add	%1, %%ebx;"
			"mov	%2, %%edx;"
			"mov	(%3), %%ecx;"
		"0:"
			"mov	(%%edx), %%ax;"
			"or	%%ax, %%ax;"
			"jz	1f;"
			"mov	%%ax, (%%ebx);"
		"1:"
			"add	$2, %%edx;"
			"add	$2, %%ebx;"
			"loop	0b;"
		: /* output: nothing */
		: "m" (adr), "g" (ScrBuf), "g" (Grp_LineBufSP2), "g" (TextDotX)
		: "ax", "bx", "cx", "dx", "memory");
	}
#else /* !USE_ASM && !(USE_GAS && __i386__) */
	DWORD adr = ((VLINE+16)*FULLSCREEN_WIDTH+16);
	WORD w;
	int i;

	if (opaq) {
		memcpy(&ScrBuf[adr], Grp_LineBufSP2, TextDotX * 2);
	} else {
		for (i = 0; i < TextDotX; i++, adr++) {
			w = Grp_LineBufSP2[i];
			if (w != 0)
				ScrBuf[adr] = w;
		}
	}
#endif /* USE_ASM */
}


INLINE void WinDraw_DrawTextLine(int opaq, int td)
{
#ifdef USE_ASM
	DWORD adr = ((VLINE+16)*1600+32);
	if (opaq)
	{
		__asm {
			mov	ebx, adr
			add	ebx, offset ScrBuf
			xor	edx, edx
			mov	ecx, TextDotX
		drawtextlineolp:
			mov	ax, word ptr (BG_LineBuf+32)[edx*2]
		//	test	byte ptr (Text_TrFlag+16)[edx], 1
		//	jnz	drawtextlineoskip
		//	mov	ax, TextPal[0]
		//drawtextlineoskip:
			mov	[ebx], ax
			inc	edx
			add	ebx, 2
			loop	drawtextlineolp
		}
	}
	else
	{
		if (td)
		{
		__asm {
			mov	ebx, adr
			add	ebx, offset ScrBuf
			xor	edx, edx
			mov	ecx, TextDotX
		drawtextlinetdlp:
			test	byte ptr (Text_TrFlag+16)[edx], 1
			jz	drawtextlinetdskip
			mov	ax, word ptr (BG_LineBuf+32)[edx*2]
			or	ax, ax
			jz	drawtextlinetdskip
//mov	[ebx], 0xffff
			mov	[ebx], ax
		drawtextlinetdskip:
			inc	edx
			add	ebx, 2
			loop	drawtextlinetdlp
		}
		}
		else
		{
		__asm {
			mov	ebx, adr
			add	ebx, offset ScrBuf
			xor	edx, edx
			mov	ecx, TextDotX
		drawtextlinelp:
		//	test	byte ptr (Text_TrFlag+16)[edx], 3
		//	jnz	drawtextlinenonskip
		//	mov	ax, TextPal[0]
		//	jmp	drawtextlinenonskip2
		//drawtextlinenonskip:
		//	test	byte ptr (Text_TrFlag+16)[edx], 1
		//	jz	drawtextlineskip
			mov	ax, word ptr (BG_LineBuf+32)[edx*2]
		//drawtextlinenonskip2:
			or	ax, ax
			jz	drawtextlineskip
			mov	[ebx], ax
		drawtextlineskip:
			inc	edx
			add	ebx, 2
			loop	drawtextlinelp
		}
		}
	}
#elif defined(USE_GAS) && defined(__i386__)
	DWORD adr = ((VLINE+16)*1600+32);
	if (opaq) {
		asm (
			"mov	%0, %%ebx;"
			"add	%1, %%ebx;"
			"xor	%%edx, %%edx;"
			"mov	(%2), %%ecx;"
		"0:"
			"mov	BG_LineBuf + 32(, %%edx, 2), %%ax;"
			"mov	%%ax, (%%ebx);"
			"inc	%%edx;"
			"add	$2, %%ebx;"
			"loop	0b;"
		: /* output: nothing */
		: "m" (adr), "g" (ScrBuf), "g" (TextDotX)
		: "ax", "bx", "cx", "dx", "memory");
	} else {
		if (td) {
			asm (
				"mov	%0, %%ebx;"
				"add	%1, %%ebx;"
				"xor	%%edx, %%edx;"
				"mov	(%2), %%ecx;"
			"0:"
				"testb	$1, Text_TrFlag + 16(%%edx);"
				"jz	1f;"
				"mov	BG_LineBuf + 32(, %%edx, 2), %%ax;"
				"or	%%ax, %%ax;"
				"jz	1f;"
				"mov	%%ax, (%%ebx);"
			"1:"
				"inc	%%edx;"
				"add	$2, %%ebx;"
				"loop	0b;"
			: /* output: nothing */
			: "m" (adr), "g" (ScrBuf), "g" (TextDotX)
			: "ax", "bx", "cx", "dx", "memory");
		} else {
			asm (
				"mov	%0, %%ebx;"
				"add	%1, %%ebx;"
				"xor	%%edx, %%edx;"
				"mov	(%2), %%ecx;"
			"0:"
				"mov	BG_LineBuf + 32(, %%edx, 2), %%ax;"
				"or	%%ax, %%ax;"
				"jz	1f;"
				"mov	%%ax, (%%ebx);"
			"1:"
				"inc	%%edx;"
				"add	$2, %%ebx;"
				"loop	0b;"
			: /* output: nothing */
			: "m" (adr), "g" (ScrBuf), "g" (TextDotX)
			: "ax", "bx", "cx", "dx", "memory");
		}
	}
#else /* !USE_ASM && !(USE_GAS && __i386__) */
	DWORD adr = ((VLINE+16)*FULLSCREEN_WIDTH+16);
	WORD w;
	int i;

	if (opaq) {
		memcpy(&ScrBuf[adr], &BG_LineBuf[16], TextDotX * 2);
	} else {
		if (td) {
			for (i = 16; i < TextDotX + 16; i++, adr++) {
				if (Text_TrFlag[i] & 1) {
					w = BG_LineBuf[i];
					if (w != 0)
						ScrBuf[adr] = w;
				}
			}
		} else {
			for (i = 16; i < TextDotX + 16; i++, adr++) {
				w = BG_LineBuf[i];
				if (w != 0)
					ScrBuf[adr] = w;
			}
		}
	}
#endif /* USE_ASM */
}


INLINE void WinDraw_DrawTextLineTR(int opaq)
{
#ifdef USE_ASM
	DWORD adr = ((VLINE+16)*1600+32);
	if (opaq)
	{
		__asm {
			push	edi
			mov	ebx, adr
			add	ebx, offset ScrBuf
			xor	edx, edx
			xor	edi, edi
			xor	eax, eax
			mov	ecx, TextDotX
		drawtexttrlineolp:
			mov	di, word ptr Grp_LineBufSP[edx*2]
			or	di, di
			jnz	drawtexttrlineotr

			test	byte ptr (Text_TrFlag+16)[edx], 1
			jz	drawtexttrlineopal0

			mov	ax, word ptr (BG_LineBuf+32)[edx*2]
			jmp	drawtexttrlineonorm

		drawtexttrlineopal0:
			xor	ax, ax
			jmp	drawtexttrlineonorm
		drawtexttrlineotr:
						// ねこーねこー
			mov	ax, word ptr (BG_LineBuf+32)[edx*2]
			and	di, Pal_HalfMask
			test	ax, Ibit
			jz	drawtexttrlineotrI
			add	di, Pal_Ix2
		drawtexttrlineotrI:
			and	ax, Pal_HalfMask
			add	ax, di		// 17bit計算中
			rcr	ax, 1		// 17bit計算中
		drawtexttrlineonorm:
			mov	[ebx], ax
			inc	edx
			add	ebx, 2
			dec	cx
			jnz	drawtexttrlineolp
//			loop	drawtexttrlineolp
			pop	edi
		}
	}
	else
	{
		__asm {
			push	edi
			mov	ebx, adr
			add	ebx, offset ScrBuf
			xor	edx, edx
			mov	ecx, TextDotX
		drawtexttrlinelp:
			test	byte ptr (Text_TrFlag+16)[edx], 1
			jz	drawtexttrlineskip

			mov	di, word ptr Grp_LineBufSP[edx*2]
			or	di, di
			jnz	drawtexttrlinetr

			mov	ax, word ptr (BG_LineBuf+32)[edx*2]
			or	ax, ax
			jz	drawtexttrlineskip
			jmp	drawtexttrlinenorm

		drawtexttrlinetr:
						// ねこーねこー
			mov	ax, word ptr (BG_LineBuf+32)[edx*2]
			or	ax, ax
			jz	drawtexttrlineskip
			and	di, Pal_HalfMask
			test	ax, Ibit
			jz	drawtexttrlinetrI
			add	di, Pal_Ix2
		drawtexttrlinetrI:
			and	ax, Pal_HalfMask
			add	ax, di		// 17bit計算中
			rcr	ax, 1		// 17bit計算中
		drawtexttrlinenorm:
			mov	[ebx], ax
		drawtexttrlineskip:
			inc	edx
			add	ebx, 2
			dec	cx
			jnz	drawtexttrlinelp
//			loop	drawtexttrlinelp
			pop	edi
		}
	}
#elif defined(USE_GAS) && defined(__i386__)
	DWORD adr = ((VLINE+16)*1600+32);
	if (opaq) {
		asm (
			"mov	%0, %%ebx;"
			"add	%1, %%ebx;"
			"xor	%%edx, %%edx;"
			"xor	%%edi, %%edi;"
			"xor	%%eax, %%eax;"
			"mov	(%2), %%ecx;"
		"0:"
			"mov	Grp_LineBufSP(, %%edx, 2), %%di;"
			"or	%%di, %%di;"
			"jnz	2f;"

			"testb	$1, Text_TrFlag + 16(%%edx);"
			"jz	1f;"

			"mov	BG_LineBuf + 32(, %%edx, 2), %%ax;"
			"jmp	4f;"

		"1:"
			"xor	%%ax, %%ax;"
			"jmp	4f;"
		"2:"
						// ねこーねこー
			"mov	BG_LineBuf + 32(, %%edx, 2), %%ax;"
			"and	(%3), %%di;"
			"test	(%4), %%ax;"
			"jz	3f;"
			"add	(%5), %%di;"
		"3:"
			"and	(%3), %%ax;"
			"add	%%di, %%ax;"	// 17bit計算中
			"rcr	$1, %%ax;"	// 17bit計算中
		"4:"
			"mov	%%ax, (%%ebx);"
			"inc	%%edx;"
			"add	$2, %%ebx;"
			"dec	%%cx;"
			"jnz	0b;"
		: /* output: nothing */
		: "m" (adr), "g" (ScrBuf), "g" (TextDotX), "g" (Pal_HalfMask),
		  "g" (Ibit), "g" (Pal_Ix2)
		: "ax", "bx", "cx", "dx", "di", "memory");
	} else {
		asm (
			"mov	%0, %%ebx;"
			"add	%1, %%ebx;"
			"xor	%%edx, %%edx;"
			"mov	(%2), %%ecx;"
		"0:"
			"testb	$1, Text_TrFlag + 16(%%edx);"
			"jz	4f;"

			"mov	Grp_LineBufSP(, %%edx, 2), %%di;"
			"or	%%di, %%di;"
			"jnz	1f;"

			"mov	BG_LineBuf + 32(, %%edx, 2), %%ax;"
			"or	%%ax, %%ax;"
			"jz	4f;"
			"jmp	3f;"

		"1:"
						// ねこーねこー
			"mov	BG_LineBuf + 32(, %%edx, 2), %%ax;"
			"or	%%ax, %%ax;"
			"jz	4f;"
			"and	(%3), %%di;"
			"test	(%4), %%ax;"
			"jz	2f;"
			"add	(%5), %%di;"
		"2:"
			"and	(%3), %%ax;"
			"add	%%di, %%ax;"	// 17bit計算中
			"rcr	$1, %%ax;"	// 17bit計算中
		"3:"
			"mov	%%ax, (%%ebx);"
		"4:"
			"inc	%%edx;"
			"add	$2, %%ebx;"
			"dec	%%cx;"
			"jnz	0b;"
		: /* output: nothing */
		: "m" (adr), "g" (ScrBuf), "g" (TextDotX), "g" (Pal_HalfMask),
		  "g" (Ibit), "g" (Pal_Ix2)
		: "ax", "bx", "cx", "dx", "di", "memory");
	}
#else /* !USE_ASM && !(USE_GAS && __i386__) */
	DWORD adr = ((VLINE+16)*FULLSCREEN_WIDTH+16);
	DWORD v;
	WORD w;
	int i;

	if (opaq) {
		for (i = 16; i < TextDotX + 16; i++, adr++) {
			w = Grp_LineBufSP[i - 16];
			if (w != 0) {
				w &= Pal_HalfMask;
				v = BG_LineBuf[i];
				if (v & Ibit)
					w += Pal_Ix2;
				v &= Pal_HalfMask;
				v += w;
				v >>= 1;
			} else {
				if (Text_TrFlag[i] & 1)
					v = BG_LineBuf[i];
				else
					v = 0;
			}
			ScrBuf[adr] = (WORD)v;
		}
	} else {
		for (i = 16; i < TextDotX + 16; i++, adr++) {
			if (Text_TrFlag[i] & 1) {
				w = Grp_LineBufSP[i - 16];
				v = BG_LineBuf[i];

				if (v != 0) {
					if (w != 0) {
						w &= Pal_HalfMask;
						if (v & Ibit)
							w += Pal_Ix2;
						v &= Pal_HalfMask;
						v += w;
						v >>= 1;
					}
					ScrBuf[adr] = (WORD)v;
				}
			}
		}
	}
#endif /* USE_ASM */
}

INLINE void WinDraw_DrawTextLineTR2(int opaq)
{
#ifdef USE_ASM
	DWORD adr = ((VLINE+16)*1600+32);
	if (opaq)
	{
		__asm {
			push	edi
			mov	ebx, adr
			add	ebx, offset ScrBuf
			xor	edx, edx
			xor	edi, edi
			xor	eax, eax
			mov	ecx, TextDotX
		drawtexttrline2olp:
			mov	di, word ptr Grp_LineBufSP[edx*2]
			or	di, di
			jnz	drawtexttrline2otr

			test	byte ptr (Text_TrFlag+16)[edx], 1
			jz	drawtexttrline2opal0

			mov	ax, word ptr (BG_LineBuf+32)[edx*2]
			jmp	drawtexttrline2onorm

		drawtexttrline2opal0:
			xor	ax, ax
			jmp	drawtexttrline2onorm
		drawtexttrline2otr:
						// ねこーねこー
			mov	ax, word ptr (BG_LineBuf+32)[edx*2]
			and	di, Pal_HalfMask
			test	ax, Ibit
			jz	drawtexttrline2otrI
			add	di, Pal_Ix2
		drawtexttrline2otrI:
			and	ax, Pal_HalfMask
			add	ax, di		// 17bit計算中
			rcr	ax, 1		// 17bit計算中
		drawtexttrline2onorm:
			mov	[ebx], ax
			inc	edx
			add	ebx, 2
			dec	cx
			jnz	drawtexttrline2olp
//			loop	drawtexttrline2olp
			pop	edi
		}
	}
	else
	{
		__asm {
			push	edi
			mov	ebx, adr
			add	ebx, offset ScrBuf
			xor	edx, edx
			mov	ecx, TextDotX
		drawtexttrline2lp:
			test	byte ptr (Text_TrFlag+16)[edx], 1
			jz	drawtexttrline2skip

			mov	di, word ptr Grp_LineBufSP[edx*2]
			or	di, di
			jnz	drawtexttrline2tr

			mov	ax, word ptr (BG_LineBuf+32)[edx*2]
			or	ax, ax
			jz	drawtexttrline2skip
			jmp	drawtexttrline2norm

		drawtexttrline2tr:
						// ねこーねこー
			mov	ax, word ptr (BG_LineBuf+32)[edx*2]
			or	ax, ax
			jz	drawtexttrline2skip
			and	di, Pal_HalfMask
			test	ax, Ibit
			jz	drawtexttrline2trI
			add	di, Pal_Ix2
		drawtexttrline2trI:
			and	ax, Pal_HalfMask
			add	ax, di		// 17bit計算中
			rcr	ax, 1		// 17bit計算中
		drawtexttrline2norm:
			mov	[ebx], ax
		drawtexttrline2skip:
			inc	edx
			add	ebx, 2
			dec	cx
			jnz	drawtexttrline2lp
//			loop	drawtexttrline2lp
			pop	edi
		}
	}
#elif defined(USE_GAS) && defined(__i386__)
	DWORD adr = ((VLINE+16)*1600+32);
	if (opaq) {
		asm (
			"mov	%0, %%ebx;"
			"add	%1, %%ebx;"
			"xor	%%edx, %%edx;"
			"xor	%%edi, %%edi;"
			"xor	%%eax, %%eax;"
			"mov	(%2), %%ecx;"
		"0:"
			"mov	Grp_LineBufSP(, %%edx, 2), %%di;"
			"or	%%di, %%di;"
			"jnz	2f;"

			"testb	$1, Text_TrFlag + 16(%%edx);"
			"jz	1f;"

			"mov	BG_LineBuf + 32(, %%edx, 2), %%ax;"
			"jmp	4f;"

		"1:"
			"xor	%%ax, %%ax;"
			"jmp	4f;"
		"2:"
						// ねこーねこー
			"mov	BG_LineBuf + 32 (, %%edx, 2), %%ax;"
			"and	(%3), %%di;"
			"test	(%4), %%ax;"
			"jz	3f;"
			"add	(%5), %%di;"
		"3:"
			"and	(%3), %%ax;"
			"add	%%di, %%ax;"		// 17bit計算中
			"rcr	$1, %%ax;"		// 17bit計算中
		"4:"
			"mov	%%ax, (%%ebx);"
			"inc	%%edx;"
			"add	$2, %%ebx;"
			"dec	%%cx;"
			"jnz	0b;"
		: /* output: nothing */
		: "m" (adr), "g" (ScrBuf), "g" (TextDotX),
		  "g" (Pal_HalfMask), "g" (Ibit), "g" (Pal_Ix2)
		: "ax", "bx", "cx", "dx", "di", "memory");
	} else {
		asm (
			"mov	%0, %%ebx;"
			"add	%1, %%ebx;"
			"xor	%%edx, %%edx;"
			"mov	(%2), %%ecx;"
		".drawtexttrline2lp:"
			"testb	$1, Text_TrFlag + 16(%%edx);"
			"jz	.drawtexttrline2skip;"

			"mov	Grp_LineBufSP(, %%edx, 2), %%di;"
			"or	%%di, %%di;"
			"jnz	.drawtexttrline2tr;"

			"mov	BG_LineBuf + 32(, %%edx, 2), %%ax;"
			"or	%%ax, %%ax;"
			"jz	.drawtexttrline2skip;"
			"jmp	.drawtexttrline2norm;"

		".drawtexttrline2tr:"
						// ねこーねこー
			"mov	BG_LineBuf + 32(, %%edx, 2), %%ax;"
			"or	%%ax, %%ax;"
			"jz	.drawtexttrline2skip;"
			"and	(%3), %%di;"
			"test	(%4), %%ax;"
			"jz	.drawtexttrline2trI;"
			"add	(%5), %%di;"
		".drawtexttrline2trI:"
			"and	(%3), %%ax;"
			"add	%%di, %%ax;"	// 17bit計算中
			"rcr	$1, %%ax;"	// 17bit計算中
		".drawtexttrline2norm:"
			"mov	%%ax, (%%ebx);"
		".drawtexttrline2skip:"
			"inc	%%edx;"
			"add	$2, %%ebx;"
			"dec	%%cx;"
			"jnz	.drawtexttrline2lp;"
		: /* output: nothing */
		: "m" (adr), "g" (ScrBuf), "g" (TextDotX),
		  "g" (Pal_HalfMask), "g" (Ibit), "g" (Pal_Ix2)
		: "ax", "bx", "cx", "dx", "di", "memory");
	}
#else /* !USE_ASM && !(USE_GAS && __i386__) */
	DWORD adr = ((VLINE+16)*FULLSCREEN_WIDTH+16);
	DWORD v;
	WORD w;
	int i;

	if (opaq) {
		for (i = 16; i < TextDotX + 16; i++, adr++) {
			w = Grp_LineBufSP[i - 16];
			if (w != 0) {
				w &= Pal_HalfMask;
				v = BG_LineBuf[i];
				if (v & Ibit)
					w += Pal_Ix2;
				v &= Pal_HalfMask;
				v += w;
				v >>= 1;
			} else {
				if (Text_TrFlag[i] & 1)
					v = BG_LineBuf[i];
				else
					v = 0;
			}
			ScrBuf[adr] = (WORD)v;
		}
	} else {
		for (i = 16; i < TextDotX + 16; i++, adr++) {
			if (Text_TrFlag[i] & 1) {
				w = Grp_LineBufSP[i - 16];
				v = BG_LineBuf[i];

				if (v != 0) {
					if (w != 0) {
						w &= Pal_HalfMask;
						if (v & Ibit)
							w += Pal_Ix2;
						v &= Pal_HalfMask;
						v += w;
						v >>= 1;
					}
					ScrBuf[adr] = (WORD)v;
				}
			}
		}
	}
#endif /* USE_ASM */
}


INLINE void WinDraw_DrawBGLine(int opaq, int td)
{
#ifdef USE_ASM
	DWORD adr = ((VLINE+16)*1600+32);
	if (opaq)
	{
		__asm {
			mov	ebx, adr
			add	ebx, offset ScrBuf
			xor	edx, edx
			mov	ecx, TextDotX
		drawbglineolp:
			mov	ax, word ptr (BG_LineBuf+32)[edx*2]
		//	test	byte ptr (Text_TrFlag+16)[edx], 3
		//	jnz	drawbglineoskip
		//	mov	ax, TextPal[0]
		//drawbglineoskip:
			mov	[ebx], ax
			inc	edx
			add	ebx, 2
			loop	drawbglineolp
		}
	}
	else
	{
		if (td)
		{
		__asm {
			mov	ebx, adr
			add	ebx, offset ScrBuf
			xor	edx, edx
			mov	ecx, TextDotX
		drawbglinetdlp:
			test	byte ptr (Text_TrFlag+16)[edx], 2
			jz	drawbglinetdskip
			mov	ax, word ptr (BG_LineBuf+32)[edx*2]
			or	ax, ax
			jz	drawbglinetdskip
			mov	[ebx], ax
		drawbglinetdskip:
			inc	edx
			add	ebx, 2
			loop	drawbglinetdlp
		}
		}
		else
		{
		__asm {
			mov	ebx, adr
			add	ebx, offset ScrBuf
			xor	edx, edx
			mov	ecx, TextDotX
		drawbglinelp:
		//	test	byte ptr (Text_TrFlag+16)[edx], 3
		//	jnz	drawbglinenonskip
		//	mov	ax, TextPal[0]
		//	jmp	drawbglinenonskip2
		//drawbglinenonskip:
		//	test	byte ptr (Text_TrFlag+16)[edx], 2
		//	jz	drawbglineskip
			mov	ax, word ptr (BG_LineBuf+32)[edx*2]
		//drawbglinenonskip2:
			or	ax, ax
			jz	drawbglineskip
			mov	[ebx], ax
		drawbglineskip:
			inc	edx
			add	ebx, 2
			loop	drawbglinelp
		}
		}
	}
#elif defined(USE_GAS) && defined(__i386__)
	DWORD adr = ((VLINE+16)*1600+32);
	if (opaq) {
		asm (
			"mov	%0, %%ebx;"
			"add	%1, %%ebx;"
			"xor	%%edx, %%edx;"
			"mov	(%2), %%ecx;"
		"0:"
			"mov	BG_LineBuf + 32(, %%edx, 2), %%ax;"
			"mov	%%ax, (%%ebx);"
			"inc	%%edx;"
			"add	$2, %%ebx;"
			"loop	0b;"
		: /* output: nothing */
		: "m" (adr), "g" (ScrBuf), "g" (TextDotX)
		: "ax", "bx", "cx", "dx", "memory");
	} else {
		if (td) {
			asm (
				"mov	%0, %%ebx;"
				"add	%1, %%ebx;"
				"xor	%%edx, %%edx;"
				"mov	(%2), %%ecx;"
			"0:"
				"testb	$2, Text_TrFlag + 16(%%edx);"
				"jz	1f;"
				"mov	BG_LineBuf + 32(, %%edx, 2), %%ax;"
				"or	%%ax, %%ax;"
				"jz	1f;"
				"mov	%%ax, (%%ebx);"
			"1:"
				"inc	%%edx;"
				"add	$2, %%ebx;"
				"loop	0b;"
			: /* output: nothing */
			: "m" (adr), "g" (ScrBuf), "g" (TextDotX)
			: "ax", "bx", "cx", "dx", "memory");
		} else {
			asm (
				"mov	%0, %%ebx;"
				"add	%1, %%ebx;"
				"xor	%%edx, %%edx;"
				"mov	(%2), %%ecx;"
			"0:"
				"mov	BG_LineBuf + 32(, %%edx, 2), %%ax;"
				"or	%%ax, %%ax;"
				"jz	1;"
				"mov	%%ax, (%%ebx);"
			"1:"
				"inc	%%edx;"
				"add	$2, %%ebx;"
				"loop	0b;"
			: /* output: nothing */
			: "m" (adr), "g" (ScrBuf), "g" (TextDotX)
			: "ax", "bx", "cx", "dx", "memory");
		}
	}
#else /* !USE_ASM && !(USE_GAS && __i386__) */
	DWORD adr = ((VLINE+16)*FULLSCREEN_WIDTH+16);
	WORD w;
	int i;

	if (opaq) {
		memcpy(&ScrBuf[adr], &BG_LineBuf[16], TextDotX * 2);
	} else {
		if (td) {
			for (i = 16; i < TextDotX + 16; i++, adr++) {
				if (Text_TrFlag[i] & 2) {
					w = BG_LineBuf[i];
					if (w != 0)
						ScrBuf[adr] = w;
				}
			}
		} else {
			for (i = 16; i < TextDotX + 16; i++, adr++) {
				w = BG_LineBuf[i];
				if (w != 0)
					ScrBuf[adr] = w;
			}
		}
	}
#endif /* USE_ASM */
}


INLINE void WinDraw_DrawBGLineTR(int opaq)
{
#ifdef USE_ASM
	DWORD adr = ((VLINE+16)*1600+32);
	if (opaq)
	{
		__asm {
			push	edi
			mov	ebx, adr
			add	ebx, offset ScrBuf
			xor	edx, edx
			xor	edi, edi
			xor	eax, eax
			mov	ecx, TextDotX
		drawbgtrlineolp:
			mov	di, word ptr Grp_LineBufSP[edx*2]
			or	di, di
			jnz	drawbgtrlineotr

		//	test	byte ptr (Text_TrFlag+16)[edx], 2
		//	jz	drawbgtrlineopal0

			mov	ax, word ptr (BG_LineBuf+32)[edx*2]
			jmp	drawbgtrlineonorm

		//drawbgtrlineopal0:
		//	xor	ax, ax
		//	jmp	drawbgtrlineonorm

		drawbgtrlineotr:

			mov	ax, word ptr (BG_LineBuf+32)[edx*2]
			and	di, Pal_HalfMask
			test	ax, Ibit
			jz	drawbgtrlineotrI
			add	di, Pal_Ix2
		drawbgtrlineotrI:
			and	ax, Pal_HalfMask
			add	ax, di		// 17bit計算中
			rcr	ax, 1		// 17bit計算中

		drawbgtrlineonorm:
			mov	[ebx], ax
			inc	edx
			add	ebx, 2
			dec	cx
			jnz	drawbgtrlineolp
//			loop	drawbgtrlineolp
			pop	edi
		}
	}
	else
	{
		__asm {
			push	edi
			mov	ebx, adr
			add	ebx, offset ScrBuf
			xor	edx, edx
			mov	ecx, TextDotX
		drawbgtrlinelp:
			test	byte ptr (Text_TrFlag+16)[edx], 2
			jz	drawbgtrlineskip

			mov	di, word ptr Grp_LineBufSP[edx*2]
			or	di, di
			jnz	drawbgtrlinetr

			mov	ax, word ptr (BG_LineBuf+32)[edx*2]
			or	ax, ax
			jz	drawbgtrlineskip
			jmp	drawbgtrlinenorm

		drawbgtrlinetr:
						// ねこーねこー
			mov	ax, word ptr (BG_LineBuf+32)[edx*2]
			or	ax, ax
			jz	drawbgtrlineskip
			and	di, Pal_HalfMask
			test	ax, Ibit
			jz	drawbgtrlinetrI
			add	di, Pal_Ix2
		drawbgtrlinetrI:
			and	ax, Pal_HalfMask
			add	ax, di		// 17bit計算中
			rcr	ax, 1		// 17bit計算中

		drawbgtrlinenorm:
			mov	[ebx], ax
		drawbgtrlineskip:
			inc	edx
			add	ebx, 2
			dec	cx
			jnz	drawbgtrlinelp
//			loop	drawbgtrlinelp
			pop	edi
		}
	}
#elif defined(USE_GAS) && defined(__i386__)
	DWORD adr = ((VLINE+16)*1600+32);
	if (opaq) {
		asm (
			"mov	%0, %%ebx;"
			"add	%1, %%ebx;"
			"xor	%%edx, %%edx;"
			"xor	%%edi, %%edi;"
			"xor	%%eax, %%eax;"
			"mov	(%2), %%ecx;"
		"0:"
			"mov	Grp_LineBufSP(, %%edx, 2), %%di;"
			"or	%%di, %%di;"
			"jnz	1f;"

			"mov	BG_LineBuf + 32(, %%edx, 2), %%ax;"
			"jmp	3f;"

		"1:"
			"mov	BG_LineBuf + 32(, %%edx, 2), %%ax;"
			"and	(%3), %%di;"
			"test	(%4), %%ax;"
			"jz	2f;"
			"add	(%5), %%di;"
		"2:"
			"and	(%3), %%ax;"
			"add	%%di, %%ax;"	// 17bit計算中
			"rcr	$1, %%ax;"	// 17bit計算中

		"3:"
			"mov	%%ax, (%%ebx);"
			"inc	%%edx;"
			"add	$2, %%ebx;"
			"dec	%%cx;"
			"jnz	0b;"
		: /* output: nothing */
		: "m" (adr), "g" (ScrBuf), "g" (TextDotX),
		  "g" (Pal_HalfMask), "g" (Ibit), "g" (Pal_Ix2)
		: "ax", "bx", "cx", "dx", "di", "memory");
	} else {
		asm (
			"mov	%0, %%ebx;"
			"add	%1, %%ebx;"
			"xor	%%edx, %%edx;"
			"mov	(%2), %%ecx;"
		"0:"
			"testb	$2, Text_TrFlag + 16(%%edx);"
			"jz	4f;"

			"mov	Grp_LineBufSP(, %%edx, 2), %%di;"
			"or	%%di, %%di;"
			"jnz	1f;"

			"mov	BG_LineBuf + 32(, %%edx, 2), %%ax;"
			"orw	%%ax, %%ax;"
			"jz	4f;"
			"jmp	3f;"

		"1:"
						// ねこーねこー
			"mov	BG_LineBuf + 32(, %%edx, 2), %%ax;"
			"or	%%ax, %%ax;"
			"jz	4f;"
			"and	(%3), %%di;"
			"test	(%4), %%ax;"
			"jz	2f;"
			"add	(%5), %%di;"
		"2:"
			"and	(%3), %%ax;"
			"add	%%di, %%ax;"	// 17bit計算中
			"rcr	$1, %%ax;"	// 17bit計算中

		"3:"
			"mov	%%ax, (%%ebx);"
		"4:"
			"inc	%%edx;"
			"add	$2, %%ebx;"
			"dec	%%cx;"
			"jnz	0b;"
		: /* output: nothing */
		: "m" (adr), "g" (ScrBuf), "g" (TextDotX),
		  "g" (Pal_HalfMask), "g" (Ibit), "g" (Pal_Ix2)
		: "ax", "bx", "cx", "dx", "di", "memory");
	}
#else /* !USE_ASM && !(USE_GAS && __i386__) */
	DWORD adr = ((VLINE+16)*FULLSCREEN_WIDTH+16);
	DWORD v;
	WORD w;
	int i;

	if (opaq) {
		for (i = 16; i < TextDotX + 16; i++, adr++) {
			w = Grp_LineBufSP[i - 16];
			v = BG_LineBuf[i];

			if (w != 0) {
				w &= Pal_HalfMask;
				if (v & Ibit)
					w += Pal_Ix2;
				v &= Pal_HalfMask;
				v += w;
				v >>= 1;
			}
			ScrBuf[adr] = (WORD)v;
		}
	} else {
		for (i = 16; i < TextDotX + 16; i++, adr++) {
			if (Text_TrFlag[i] & 2) {
				w = Grp_LineBufSP[i - 16];
				v = BG_LineBuf[i];

				if (v != 0) {
					if (w != 0) {
						w &= Pal_HalfMask;
						if (v & Ibit)
							w += Pal_Ix2;
						v &= Pal_HalfMask;
						v += w;
						v >>= 1;
					}
					ScrBuf[adr] = (WORD)v;
				}
			}
		}
	}
#endif /* USE_ASM */
}


INLINE void WinDraw_DrawPriLine(void)
{
#ifdef USE_ASM
	DWORD adr = ((VLINE+16)*1600+32);
	{
		__asm {
			mov	ebx, adr
			add	ebx, offset ScrBuf
			mov	edx, offset Grp_LineBufSP
			mov	ecx, TextDotX
		drawprilinelp:
			mov	ax, [edx]
			or	ax, ax
			jz	drawprilineskip
			mov	[ebx], ax
		drawprilineskip:
			add	edx, 2
			add	ebx, 2
//			dec	cx
//			jnz	drawprilinelp
			loop	drawprilinelp
		}
	}
#elif defined(USE_GAS) && defined(__i386__)
	DWORD adr = ((VLINE+16)*1600+32);

	asm (
		"mov	%0, %%ebx;"
		"add	%1, %%ebx;"
		"add	%2, %%edx;"
		"mov	(%3), %%ecx;"
	"0:"
		"mov	(%%edx), %%ax;"
		"or	%%ax, %%ax;"
		"jz	1f;"
		"mov	%%ax, (%%ebx);"
	"1:"
		"add	$2, %%edx;"
		"add	$2, %%ebx;"
		"loop	0b;"
	: /* output: nothing */
	: "m" (adr), "g" (ScrBuf), "g" (Grp_LineBufSP), "g" (TextDotX)
	: "ax", "bx", "cx", "dx", "memory");
#else /* !USE_ASM && !(USE_GAS && __i386__) */
	DWORD adr = ((VLINE+16)*FULLSCREEN_WIDTH+16);
	WORD w;
	int i;

	for (i = 0; i < TextDotX; i++, adr++) {
		w = Grp_LineBufSP[i];
		if (w != 0)
			ScrBuf[adr] = w;
	}
#endif /* USE_ASM */
}


INLINE void WinDraw_DrawTRLine(void)
{
#ifdef USE_ASM
	DWORD adr = ((VLINE+16)*1600+32);
		__asm {
			push	edi
			mov	ebx, adr
			add	ebx, offset ScrBuf
			xor	edx, edx
			xor	edi, edi
			xor	eax, eax
			mov	ecx, TextDotX
		drawtrlinelp:
			mov	di, word ptr Grp_LineBufSP[edx]
			or	di, di
			jz	drawtrlineskip

//		drawtrlineo:
#if 1						// ねこーねこー
			mov	ax, [ebx]
			and	di, Pal_HalfMask
			test	ax, Ibit
			jz	drawtrlineoI
			add	di, Pal_Ix2
		drawtrlineoI:
			and	ax, Pal_HalfMask
			add	ax, di		// 17bit計算中
			rcr	ax, 1		// 17bit計算中
#else
			push	ecx
			mov	cx, [ebx]
			and	cx, WinDraw_Pal16R
			and	di, WinDraw_Pal16R
			add	ecx, edi
			shr	ecx, 1
			and	cx, WinDraw_Pal16R
			mov	ax, cx
			mov	di, word ptr Grp_LineBufSP[edx]
			mov	cx, [ebx]
			and	cx, WinDraw_Pal16G
			and	di, WinDraw_Pal16G
			add	ecx, edi
			shr	ecx, 1
			and	cx, WinDraw_Pal16G
			or	ax, cx
			mov	di, word ptr Grp_LineBufSP[edx]
			mov	cx, [ebx]
			and	cx, WinDraw_Pal16B
			and	di, WinDraw_Pal16B
			add	ecx, edi
			shr	ecx, 1
			and	cx, WinDraw_Pal16B
			or	ax, cx
			mov	cx, [ebx]
			and	cx, Ibit
			or	ax, cx
			pop	ecx
#endif
			mov	[ebx], ax
		drawtrlineskip:
			add	edx, 2
			add	ebx, 2
			dec	cx
			jnz	drawtrlinelp
//			loop	drawtrlinelp
			pop	edi
		}
#elif defined(USE_GAS) && defined(__i386__)
	DWORD adr = ((VLINE+16)*1600+32);
		asm (
			"mov	%0, %%ebx;"
			"add	%1, %%ebx;"
			"xor	%%edx, %%edx;"
			"xor	%%edi, %%edi;"
			"xor	%%eax, %%eax;"
			"mov	(%2), %%ecx;"
		"0:"
			"mov	Grp_LineBufSP(%%edx), %%di;"
			"or	%%di, %%di;"
			"jz	2f;"

						// ねこーねこー
			"mov	(%%ebx), %%ax;"
			"and	(%3), %%di;"
			"test	(%4), %%ax;"
			"jz	1f;"
			"add	(%5), %%di;"
		"1:"
			"and	(%3), %%ax;"
			"add	%%di, %%ax;"
			"rcr	$1, %%ax;"
			"mov	%%ax, (%%ebx);"
		"2:"
			"add	$2, %%edx;"
			"add	$2, %%ebx;"
			"dec	%%cx;"
			"jnz	0b;"
		: /* output: nothing */
		: "m" (adr), "g" (ScrBuf), "g" (TextDotX),
		  "g" (Pal_HalfMask), "g" (Ibit), "g" (Pal_Ix2)
		: "ax", "bx", "cx", "dx", "di", "memory");
#else /* !USE_ASM && !(USE_GAS && __i386__) */
	DWORD adr = ((VLINE+16)*FULLSCREEN_WIDTH+16);
	DWORD v;
	WORD w;
	int i;

	for (i = 0; i < TextDotX; i++, adr++) {
		w = Grp_LineBufSP[i];
		if (w != 0) {
			v = ScrBuf[adr];
			w &= Pal_HalfMask;
			if (v & Ibit)
				w += Pal_Ix2;
			v &= Pal_HalfMask;
			v += w;
			v >>= 1;
			ScrBuf[adr] = (WORD)v;
		}
	}
#endif /* USE_ASM */
}


void WinDraw_DrawLine(void)
{
	int opaq, ton=0, gon=0, bgon=0, tron=0, pron=0, tdrawed=0;

	if (!TextDirtyLine[VLINE]) return;
	TextDirtyLine[VLINE] = 0;
	Draw_DrawFlag = 1;


	if (Debug_Grp)
	{
	switch(VCReg0[1]&3)
	{
	case 0:					// 16 colors
		if (VCReg0[1]&4)		// 1024dot
		{
			if (VCReg2[1]&0x10)
			{
				if ( (VCReg2[0]&0x14)==0x14 )
				{
					Grp_DrawLine4hSP();
					pron = tron = 1;
				}
				else
				{
					Grp_DrawLine4h();
					gon=1;
				}
			}
		}
		else				// 512dot
		{
			if ( (VCReg2[0]&0x10)&&(VCReg2[1]&1) )
			{
				Grp_DrawLine4SP((VCReg1[1]   )&3/*, 1*/);			// 半透明の下準備
				pron = tron = 1;
			}
			opaq = 1;
			if (VCReg2[1]&8)
			{
				Grp_DrawLine4((VCReg1[1]>>6)&3, 1);
				opaq = 0;
				gon=1;
			}
			if (VCReg2[1]&4)
			{
				Grp_DrawLine4((VCReg1[1]>>4)&3, opaq);
				opaq = 0;
				gon=1;
			}
			if (VCReg2[1]&2)
			{
				if ( ((VCReg2[0]&0x1e)==0x1e)&&(tron) )
					Grp_DrawLine4TR((VCReg1[1]>>2)&3, opaq);
				else
					Grp_DrawLine4((VCReg1[1]>>2)&3, opaq);
				opaq = 0;
				gon=1;
			}
			if (VCReg2[1]&1)
			{
//				if ( (VCReg2[0]&0x1e)==0x1e )
//				{
//					Grp_DrawLine4SP((VCReg1[1]   )&3, opaq);
//					tron = pron = 1;
//				}
//				else
				if ( (VCReg2[0]&0x14)!=0x14 )
				{
					Grp_DrawLine4((VCReg1[1]   )&3, opaq);
					gon=1;
				}
			}
		}
		break;
	case 1:	
	case 2:	
		opaq = 1;		// 256 colors
		if ( (VCReg1[1]&3) <= ((VCReg1[1]>>4)&3) )	// 同じ値の時は、GRP0が優先（ドラスピ）
		{
			if ( (VCReg2[0]&0x10)&&(VCReg2[1]&1) )
			{
				Grp_DrawLine8SP(0);			// 半透明の下準備
				tron = pron = 1;
			}
			if (VCReg2[1]&4)
			{
				if ( ((VCReg2[0]&0x1e)==0x1e)&&(tron) )
					Grp_DrawLine8TR(1, 1);
				else
					Grp_DrawLine8(1, 1);
				opaq = 0;
				gon=1;
			}
			if (VCReg2[1]&1)
			{
				if ( (VCReg2[0]&0x14)!=0x14 )
				{
					Grp_DrawLine8(0, opaq);
					gon=1;
				}
			}
		}
		else
		{
			if ( (VCReg2[0]&0x10)&&(VCReg2[1]&1) )
			{
				Grp_DrawLine8SP(1);			// 半透明の下準備
				tron = pron = 1;
			}
			if (VCReg2[1]&4)
			{
				if ( ((VCReg2[0]&0x1e)==0x1e)&&(tron) )
					Grp_DrawLine8TR(0, 1);
				else
					Grp_DrawLine8(0, 1);
				opaq = 0;
				gon=1;
			}
			if (VCReg2[1]&1)
			{
				if ( (VCReg2[0]&0x14)!=0x14 )
				{
					Grp_DrawLine8(1, opaq);
					gon=1;
				}
			}
		}
		break;
	case 3:					// 65536 colors
		if (VCReg2[1]&15)
		{
			if ( (VCReg2[0]&0x14)==0x14 )
			{
				Grp_DrawLine16SP();
				tron = pron = 1;
			}
			else
			{
				Grp_DrawLine16();
				gon=1;
			}
		}
		break;
	}
	}


//	if ( ( ((VCReg1[0]&0x30)>>4) < (VCReg1[0]&0x03) ) && (gon) )
//		gdrawed = 1;				// GrpよりBGの方が上

	if ( ((VCReg1[0]&0x30)>>2) < (VCReg1[0]&0x0c) )
	{						// BGの方が上
		if ((VCReg2[1]&0x20)&&(Debug_Text))
		{
			Text_DrawLine(1);
			ton = 1;
		}
		else
			ZeroMemory(Text_TrFlag, TextDotX+16);

		if ((VCReg2[1]&0x40)&&(BG_Regs[8]&2)&&(!(BG_Regs[0x11]&2))&&(Debug_Sp))
		{
			int s1, s2;
			s1 = (((BG_Regs[0x11]  &4)?2:1)-((BG_Regs[0x11]  &16)?1:0));
			s2 = (((CRTC_Regs[0x29]&4)?2:1)-((CRTC_Regs[0x29]&16)?1:0));
			VLINEBG = VLINE;
			VLINEBG <<= s1;
			VLINEBG >>= s2;
			if ( !(BG_Regs[0x11]&16) ) VLINEBG -= ((BG_Regs[0x0f]>>s1)-(CRTC_Regs[0x0d]>>s2));
			BG_DrawLine(!ton, 0);
			bgon = 1;
		}
	}
	else
	{						// Textの方が上
		if ((VCReg2[1]&0x40)&&(BG_Regs[8]&2)&&(!(BG_Regs[0x11]&2))&&(Debug_Sp))
		{
			int s1, s2;
			s1 = (((BG_Regs[0x11]  &4)?2:1)-((BG_Regs[0x11]  &16)?1:0));
			s2 = (((CRTC_Regs[0x29]&4)?2:1)-((CRTC_Regs[0x29]&16)?1:0));
			VLINEBG = VLINE;
			VLINEBG <<= s1;
			VLINEBG >>= s2;
			if ( !(BG_Regs[0x11]&16) ) VLINEBG -= ((BG_Regs[0x0f]>>s1)-(CRTC_Regs[0x0d]>>s2));
			ZeroMemory(Text_TrFlag, TextDotX+16);
			BG_DrawLine(1, 1);
			bgon = 1;
		}
		else
		{
			if ((VCReg2[1]&0x20)&&(Debug_Text))
			{
#ifdef USE_ASM
				_asm{
					mov	ax, TextPal[0]
					shl	eax, 16
					mov	ax, TextPal[0]
					mov	edx, 16*2
					mov	ecx, TextDotX
					shr	ecx, 1
				BGLineClr_lp:
					mov	dword ptr BG_LineBuf[edx], eax
					add	edx, 4
					loop	BGLineClr_lp
				}
#elif defined(USE_GAS) && defined(__i386__)
				asm (
					"mov	(%0), %%ax;"
					"shl	$16, %%eax;"
					"mov	(%0), %%ax;"
					"mov	$16*2, %%edx;"
					"mov	(%1), %%ecx;"
					"shr	$1, %%ecx;"
				"0:"
					"mov	%%eax, BG_LineBuf(%%edx);"
					"add	$4, %%edx;"
					"loop	0b;"
				: /* output: nothing */
				: "g" (TextPal[0]), "g" (TextDotX)
				: "ax", "cx", "dx", "memory");
#else /* !USE_ASM && !(USE_GAS && __i386__) */
				int i;
				for (i = 16; i < TextDotX + 16; ++i)
					BG_LineBuf[i] = TextPal[0];
#endif /* USE_ASM */
			} else {		// 20010120 （琥珀色）
#ifdef USE_ASM
				_asm{
					xor	eax, eax
					mov	edx, 16*2
					mov	ecx, TextDotX
					shr	ecx, 1
				BGLineClr_lp2:
					mov	dword ptr BG_LineBuf[edx], eax
					add	edx, 4
					loop	BGLineClr_lp2
				}
#elif defined(USE_GAS) && defined(__i386__)
				asm (
					"xor	%%eax, %%eax;"
					"mov	$16*2, %%edx;"
					"mov	(%0), %%ecx;"
					"shr	$1, %%ecx;"
				"0:"
					"mov	%%eax, BG_LineBuf(%%edx);"
					"add	$4, %%edx;"
					"loop	0b;"
				: /* output: nothing */
				: "g" (TextDotX)
				: "ax", "cx", "dx", "memory");
#else /* !USE_ASM && !(USE_GAS && __i386__) */
				bzero(&BG_LineBuf[16], TextDotX * 2);
#endif /* !USE_ASM */
			}
			ZeroMemory(Text_TrFlag, TextDotX+16);
			bgon = 1;
		}

		if ((VCReg2[1]&0x20)&&(Debug_Text))
		{
			Text_DrawLine(!bgon);
			ton = 1;
		}
	}


	opaq = 1;


#if 0
					// Pri = 3（違反）に設定されている画面を表示
		if ( ((VCReg1[0]&0x30)==0x30)&&(bgon) )
		{
			if ( ((VCReg2[0]&0x5d)==0x1d)&&((VCReg1[0]&0x03)!=0x03)&&(tron) )
			{
				if ( (VCReg1[0]&3)<((VCReg1[0]>>2)&3) )
				{
					WinDraw_DrawBGLineTR(opaq);
					tdrawed = 1;
					opaq = 0;
				}
			}
			else
			{
				WinDraw_DrawBGLine(opaq, /*tdrawed*/0);
				tdrawed = 1;
				opaq = 0;
			}
		}
		if ( ((VCReg1[0]&0x0c)==0x0c)&&(ton) )
		{
			if ( ((VCReg2[0]&0x5d)==0x1d)&&((VCReg1[0]&0x03)!=0x0c)&&(tron) )
				WinDraw_DrawTextLineTR(opaq);
			else
				WinDraw_DrawTextLine(opaq, /*tdrawed*/((VCReg1[0]&0x30)==0x30));
			opaq = 0;
			tdrawed = 1;
		}
#endif
					// Pri = 2 or 3（最下位）に設定されている画面を表示
					// プライオリティが同じ場合は、GRP<SP<TEXT？（ドラスピ、桃伝、YsIII等）

					// GrpよりTextが上にある場合にTextとの半透明を行うと、SPのプライオリティも
					// Textに引きずられる？（つまり、Grpより下にあってもSPが表示される？）

					// KnightArmsとかを見ると、半透明のベースプレーンは一番上になるみたい…。

		if ( (VCReg1[0]&0x02) )
		{
			if (gon)
			{
				WinDraw_DrawGrpLine(opaq);
				opaq = 0;
			}
			if (tron)
			{
				WinDraw_DrawGrpLineNonSP(opaq);
				opaq = 0;
			}
		}
		if ( (VCReg1[0]&0x20)&&(bgon) )
		{
			if ( ((VCReg2[0]&0x5d)==0x1d)&&((VCReg1[0]&0x03)!=0x02)&&(tron) )
			{
				if ( (VCReg1[0]&3)<((VCReg1[0]>>2)&3) )
				{
					WinDraw_DrawBGLineTR(opaq);
					tdrawed = 1;
					opaq = 0;
				}
			}
			else
			{
				WinDraw_DrawBGLine(opaq, /*0*/tdrawed);
				tdrawed = 1;
				opaq = 0;
			}
		}
		if ( (VCReg1[0]&0x08)&&(ton) )
		{
			if ( ((VCReg2[0]&0x5d)==0x1d)&&((VCReg1[0]&0x03)!=0x02)&&(tron) )
				WinDraw_DrawTextLineTR(opaq);
			else
				WinDraw_DrawTextLine(opaq, tdrawed/*((VCReg1[0]&0x30)>=0x20)*/);
			opaq = 0;
			tdrawed = 1;
		}

					// Pri = 1（2番目）に設定されている画面を表示
		if ( ((VCReg1[0]&0x03)==0x01)&&(gon) )
		{
			WinDraw_DrawGrpLine(opaq);
			opaq = 0;
		}
		if ( ((VCReg1[0]&0x30)==0x10)&&(bgon) )
		{
			if ( ((VCReg2[0]&0x5d)==0x1d)&&(!(VCReg1[0]&0x03))&&(tron) )
			{
				if ( (VCReg1[0]&3)<((VCReg1[0]>>2)&3) )
				{
					WinDraw_DrawBGLineTR(opaq);
					tdrawed = 1;
					opaq = 0;
				}
			}
			else
			{
				WinDraw_DrawBGLine(opaq, ((VCReg1[0]&0xc)==0x8));
				tdrawed = 1;
				opaq = 0;
			}
		}
		if ( ((VCReg1[0]&0x0c)==0x04) && ((VCReg2[0]&0x5d)==0x1d) && (VCReg1[0]&0x03) && (((VCReg1[0]>>4)&3)>(VCReg1[0]&3)) && (bgon) && (tron) )
		{
			WinDraw_DrawBGLineTR(opaq);
			tdrawed = 1;
			opaq = 0;
			if (tron)
			{
				WinDraw_DrawGrpLineNonSP(opaq);
			}
		}
		else if ( ((VCReg1[0]&0x03)==0x01)&&(tron)&&(gon)&&(VCReg2[0]&0x10) )
		{
			WinDraw_DrawGrpLineNonSP(opaq);
			opaq = 0;
		}
		if ( ((VCReg1[0]&0x0c)==0x04)&&(ton) )
		{
			if ( ((VCReg2[0]&0x5d)==0x1d)&&(!(VCReg1[0]&0x03))&&(tron) )
				WinDraw_DrawTextLineTR(opaq);
			else
				WinDraw_DrawTextLine(opaq, ((VCReg1[0]&0x30)>=0x10));
			opaq = 0;
			tdrawed = 1;
		}

					// Pri = 0（最優先）に設定されている画面を表示
		if ( (!(VCReg1[0]&0x03))&&(gon) )
		{
			WinDraw_DrawGrpLine(opaq);
			opaq = 0;
		}
		if ( (!(VCReg1[0]&0x30))&&(bgon) )
		{
			WinDraw_DrawBGLine(opaq, /*tdrawed*/((VCReg1[0]&0xc)>=0x4));
			tdrawed = 1;
			opaq = 0;
		}
		if ( (!(VCReg1[0]&0x0c)) && ((VCReg2[0]&0x5d)==0x1d) && (((VCReg1[0]>>4)&3)>(VCReg1[0]&3)) && (bgon) && (tron) )
		{
			WinDraw_DrawBGLineTR(opaq);
			tdrawed = 1;
			opaq = 0;
			if (tron)
			{
				WinDraw_DrawGrpLineNonSP(opaq);
			}
		}
		else if ( (!(VCReg1[0]&0x03))&&(tron)&&(VCReg2[0]&0x10) )
		{
			WinDraw_DrawGrpLineNonSP(opaq);
			opaq = 0;
		}
		if ( (!(VCReg1[0]&0x0c))&&(ton) )
		{
			WinDraw_DrawTextLine(opaq, 1);
			tdrawed = 1;
			opaq = 0;
		}

					// 特殊プライオリティ時のグラフィック
		if ( ((VCReg2[0]&0x5c)==0x14)&&(pron) )	// 特殊Pri時は、対象プレーンビットは意味が無いらしい（ついんびー）
		{
			WinDraw_DrawPriLine();
		}
		else if ( ((VCReg2[0]&0x5d)==0x1c)&&(tron) )	// 半透明時に全てが透明なドットをハーフカラーで埋める
		{						// （AQUALES）
#ifdef USE_ASM
			DWORD adr = ((VLINE+16)*1600+32);
			__asm {
				mov	ebx, adr
				add	ebx, offset ScrBuf
				xor	edx, edx
				mov	ecx, TextDotX
			trsublinelp:
				mov	ax, word ptr Grp_LineBufSP[edx]
				or	ax, ax
				jz	trsublineskip
				test	word ptr [ebx], 0ffffh
				jnz	trsublineskip
				and	ax, Pal_HalfMask
				shr	ax, 1		// 17bit計算中
				mov	[ebx], ax
			trsublineskip:
				add	edx, 2
				add	ebx, 2
				loop	trsublinelp
			}
#elif defined(USE_GAS) && defined(__i386__)
			DWORD adr = ((VLINE+16)*1600+32);
			asm (
				"mov	%0, %%ebx;"
				"add	%1, %%ebx;"
				"xor	%%edx, %%edx;"
				"mov	(%2), %%ecx;"
			"0:"
				"mov	Grp_LineBufSP(%%edx), %%ax;"
				"or	%%ax, %%ax;"
				"jz	1f;"
				"testw	$0xffff, (%%ebx);"
				"jnz	1f;"
				"and	(%3), %%ax;"
				"shr	$1, %%ax;"	// 17bit計算中
				"mov	%%ax, (%%ebx);"
			"1:"
				"add	$2, %%edx;"
				"add	$2, %%ebx;"
				"loop	0b;"
			: /* output: nothing */
			: "m" (adr), "g" (ScrBuf), "g" (TextDotX),
			  "g" (Pal_HalfMask)
			: "ax", "bx", "cx", "dx", "memory");
#else /* !USE_ASM && !(USE_GAS && __i386__) */
			DWORD adr = ((VLINE+16)*FULLSCREEN_WIDTH+16);
			WORD w;
			int i;

			for (i = 0; i < TextDotX; ++i, ++adr) {
				w = Grp_LineBufSP[i];
				if (w != 0 && (ScrBuf[adr] & 0xffff) == 0)
					ScrBuf[adr] = (w & Pal_HalfMask) >> 1;
			}
#endif /* USE_ASM */
		}


	if (opaq)
	{
#ifdef USE_ASM
		DWORD adr = ((VLINE+16)*1600+32);
		__asm {
			mov	ebx, adr
			add	ebx, offset ScrBuf
			mov	ecx, TextDotX
			shr	cx, 1
		clrscrlp:
			mov	dword ptr [ebx], 0
			add	ebx, 4
//			dec	cx
//			jnz	clrscrlp
			loop	clrscrlp
		}
#elif defined(USE_GAS) && defined(__i386__)
		DWORD adr = ((VLINE+16)*1600+32);
		asm (
			"mov	%0, %%ebx;"
			"add	%1, %%ebx;"
			"mov	(%2), %%ecx;"
			"shr	$1, %%cx;"
		"0:"
			"movl	$0, (%%ebx);"
			"add	$4, %%ebx;"
			"loop	0b;"
		: /* output: nothing */
		: "m" (adr), "g" (ScrBuf), "g" (TextDotX)
		: "bx", "cx", "memory");
#else /* !USE_ASM && !(USE_GAS && __i386__) */
		DWORD adr = ((VLINE+16)*FULLSCREEN_WIDTH+16);
		bzero(&ScrBuf[adr], TextDotX * 2);
#endif /* USE_ASM */
	}

	switch (screen_mode) {
	case 0:
#if 0
		gdk_draw_image(pixmap,
		    drawarea->style->fg_gc[GTK_WIDGET_STATE(drawarea)],
		    surface, 16, 16 + VLINE, 0, VLINE, WindowX, 1);
#endif
		break;
	}
}


/* ----- */

/**
 * 最大公約数を求める
 */
unsigned int
gcd(unsigned int v0, unsigned int v1)
{
#if 0
/*
 * ユークリッドの互除法版
 */
	unsigned int t;

	if (v0 == 0 || v1 == 0)
		return 0;
	if (v0 == 1 || v1 == 1)
		return 1;

	if (v0 < v1)
		t = v0, v0 = v1, v1 = t;

	for (; t = v0 % v1; v0 = v1, v1 = t)
		continue;
	return v1;
#else
/*
 * Brent の改良型アルゴリズム
 */
	unsigned int c;
	unsigned int d;
	unsigned int t;

	if (v0 == 0 || v1 == 0)
		return 0;
	if (v0 == 1 || v1 == 1)
		return 1;
	if (v0 == v1)
		return v0;

	for (c = 0; !(v0 & 1) && !(v1 & 1); v0 >>= 1, v1 >>= 1, ++c)
		continue;

	while (!(v0 & 1))
		v0 >>= 1;
	while (!(v1 & 1))
		v1 >>= 1;
	if (v0 < v1)
		t = v0, v0 = v1, v1 = t;

	for (; (d = v0 - v1) != 0; v0 = v1, v1 = d) {
		while ((d & 1) == 0)
			d >>= 1;
		if (v1 < d)
			t = v1, v1 = d, d = t;
	}
	return v1 << c;
#endif
}

static void expand16_fast(GdkImage *dest, GdkImage *src, GdkRectangle *dr, GdkRectangle *sr, int ratio[4]);

GdkImage *
gdk_scale_image(GdkImage *dest, GdkImage *src, GdkRectangle *dest_rect, GdkRectangle *src_rect)
{
	GdkRectangle dr, sr;
	GdkImage *new = dest;
	int ratio[4];
	int x_gcd, y_gcd;

	g_return_val_if_fail(src, 0);
	g_return_val_if_fail((src->visual->depth == 15 || src->visual->depth == 16), 0);

	if (dest_rect == 0) {
		GdkImage *p = dest ? dest : src;
		dr.x = dr.y = 0;
		dr.width = p->width;
		dr.height = p->height;
	} else 
		dr = *dest_rect;

	if (src_rect == 0) {
		sr.x = sr.y = 0;
		sr.width = src->width;
		sr.height = src->height;
	} else {
		sr = *src_rect;
		g_return_val_if_fail((sr.x >= 0 && sr.y >= 0), 0);
	}

	if (new == 0) {
		new = gdk_image_new(GDK_IMAGE_FASTEST, src->visual, dr.width + (dr.x > 0 ? dr.x : 0), dr.height + (dr.y > 0 ? dr.y : 0));
		g_return_val_if_fail(new, 0);
	}

	x_gcd = gcd(sr.width, dr.width);
	y_gcd = gcd(sr.height, dr.height);

	if (x_gcd >= 2 && y_gcd >= 2)
		/* continue */;
	else {
		if (dest == 0)
			gdk_image_destroy(new);
		g_return_val_if_fail((x_gcd >= 2 && y_gcd >= 2), 0);
	}

	ratio[0] /* dx_ratio */ = dr.width / x_gcd;
	ratio[1] /* sx_ratio */ = sr.width / x_gcd;
	ratio[2] /* dy_ratio */ = dr.height / y_gcd;
	ratio[3] /* sy_ratio */ = sr.height / y_gcd;
#if 0
	if (ratio[0] >= ratio[1] && ratio[2] >= ratio[3])
		/* continue */;
	else {
		printf("sr(width, height) = (%d, %d), dr(width, height) = (%d, %d)\n", sr.width, sr.height, dr.width, dr.height);
		printf("gcd(x, y) = (%d, %d)\n", x_gcd, y_gcd);
		printf("ratio(0, 1, 2, 3) = (%d, %d, %d, %d)\n", ratio[0], ratio[1], ratio[2], ratio[3]);
		if (dest == 0)
			gdk_image_destroy(new);
		g_return_val_if_fail((ratio[0] >= ratio[1] && ratio[2] >= ratio[3]), NULL);
	}
#endif
	expand16_fast(new, src, &dr, &sr, ratio);

	return new;
}

static void
expand16_fast(GdkImage *dest, GdkImage *src, GdkRectangle *dr, GdkRectangle *sr, int ratio[4])
{
	guint16 *dp, *sp;
	int dest_right, dest_bottom;
	int sx, sy;
	int dx, dy;
	int x_ratio = 0, y_ratio = 0;

	dest_right = dr->x + dr->width;
	dest_bottom = dr->y + dr->height;
	if (dest_right < 0 && dest_bottom < 0)
		return;

	for (sy = sr->y, dy = dr->y; dy < dest_bottom; ++dy) {
		dp = (guint16 *)(dest->mem + dest->bpl * dy);
		sp = (guint16 *)(src->mem + src->bpl * sy);
		for (sx = sr->x, dx = dr->x; dx < dest_right; ++dx) {
			if (dx >= 0 && dy >= 0)
				dp[dx] = sp[sx];

			x_ratio += ratio[1] /* sx_ratio */;
			if (ratio[0] /* dx_ratio */ <= x_ratio) {
				++sx;
				x_ratio -= ratio[0] /* dx_ratio */;
			}
		}

		y_ratio += ratio[3] /* sy_ratio */;
		if (ratio[2] /* dy_ratio */ <= y_ratio) {
			++sy;
			y_ratio -= ratio[2] /* dy_ratio */;
		}
	}
}
