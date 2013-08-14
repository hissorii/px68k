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

#if ANDROID
#include <GLES/gl.h>
#endif
#include <SDL.h>
//#include <SDL_rotozoom.h>
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

#ifdef ANDROID
#include <android/log.h>
#endif

#include "../icons/keropi.xpm"

extern BYTE Debug_Text, Debug_Grp, Debug_Sp;

WORD *ScrBuf = 0;
#if !SDL_VERSION_ATLEAST(2, 0, 0)
SDL_Surface *sdl_rgbsurface;
#endif

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

#ifdef ANDROID
GLuint texid[7] = {0, 0, 0, 0, 0, 0, 0};
#endif

#if SDL_VERSION_ATLEAST(2, 0, 0)
extern SDL_Window *sdl_window;
#endif

void WinDraw_InitWindowSize(WORD width, WORD height)
{
	static BOOL inited = FALSE;

	if (!inited) {
		inited = TRUE;
	}

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

	if ((oldx == WindowX) && (oldy == WindowY))
		return;

	WinDraw_InitWindowSize((WORD)WindowX, (WORD)WindowY);
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
}

void WinDraw_HideSplash(void)
{
}


int WinDraw_Init(void)
{
#if !SDL_VERSION_ATLEAST(2, 0, 0)
	SDL_Surface *sdl_surface;
#endif
	WindowX = 768;
	WindowY = 512;

#if !SDL_VERSION_ATLEAST(2, 0, 0)
	sdl_surface = SDL_GetVideoSurface();
	if (sdl_surface == NULL) {
		fprintf(stderr, "can't create surface.\n");
		return 1;
	}
#endif

#if SDL_VERSION_ATLEAST(2, 0, 0)
	WinDraw_Pal16R = 0xf800;
	WinDraw_Pal16G = 0x07e0;
	WinDraw_Pal16B = 0x001f;
#else
	WinDraw_Pal16R = sdl_surface->format->Rmask;
	WinDraw_Pal16G = sdl_surface->format->Gmask;
	WinDraw_Pal16B = sdl_surface->format->Bmask;
	printf("R: %x, G: %x, B: %x\n", WinDraw_Pal16R, WinDraw_Pal16G, WinDraw_Pal16B);
#endif

#ifdef ANDROID
	ScrBuf = malloc(1024*1024*2); // OpenGL ES 1.1 needs 2^x pixels

	glGenTextures(7, &texid[0]);
	glBindTexture(GL_TEXTURE_2D, texid[0]);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, ScrBuf);

	WORD BtnTex[32*32];
	int i;
	//とりあえず薄めの緑で。
	for (i = 0; i < 32*32; i++) {
		BtnTex[i] = 0x03e0;
	}

	// ボタン用テクスチャ。とりあえず全部同じ色。
	for (i = 1; i < 7; i++) {
		glBindTexture(GL_TEXTURE_2D, texid[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 32, 32, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, BtnTex);
	}
#else
	sdl_rgbsurface = SDL_CreateRGBSurface(SDL_SWSURFACE, 800, 600, 16, WinDraw_Pal16R, WinDraw_Pal16G, WinDraw_Pal16B, 0);

	if (sdl_rgbsurface == 0) {
		puts("ScrBuf allocate failed");
		exit(1);
	}
	ScrBuf = sdl_rgbsurface->pixels;

	printf("drawbuf: 0x%x, ScrBuf: 0x%x\n", sdl_surface->pixels, ScrBuf);
#endif

#if !SDL_VERSION_ATLEAST(2, 0, 0)
	SDL_LockSurface(sdl_rgbsurface);
#endif
	{
//		printf("hoge\n");
		int i;
		for (i = 0; i < 80; i++) {
			*(ScrBuf + i) = 0xffff;
		}
	}
#if !SDL_VERSION_ATLEAST(2, 0, 0)
	SDL_UnlockSurface(sdl_rgbsurface);
#endif

	return TRUE;
}

void
WinDraw_Cleanup(void)
{
}

void
WinDraw_Redraw(void)
{

	TVRAM_SetAllDirty();
}

#ifdef ANDROID
#define SET_GLFLOATS(dst, a, b, c, d, e, f, g, h)		\
{								\
	dst[0] = (a); dst[1] = (b); dst[2] = (c); dst[3] = (d);	\
	dst[4] = (e); dst[5] = (f); dst[6] = (g); dst[7] = (h);	\
}

static void draw_texture(GLfloat *coor, GLfloat *vert)
{
	glTexCoordPointer(2, GL_FLOAT, 0, coor);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vert);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

#define draw_button(texid, x, y)					\
{									\
	glBindTexture(GL_TEXTURE_2D, texid);				\
	/* Texture から必要な部分を抜き出す(32x32を全部使う) */		\
	SET_GLFLOATS(texture_coordinates, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f); \
	SET_GLFLOATS(vertices, (GLfloat)(x), (y)+32.0f, (GLfloat)(x), (GLfloat)(y), (x)+32.0f, (y)+32.0f, (x)+32.0f, (y)+0.0f); \
	draw_texture(texture_coordinates, vertices);			\
}
#endif

void FASTCALL
WinDraw_Draw(void)
{
	SDL_Surface *sdl_surface, *roto_surface = NULL;
	int ret;
	int x, y, Bpp;
	WORD c, *p, *p2, dummy, *dst16;
	DWORD *dst32, dat32;
	static int oldtextx = -1, oldtexty = -1;

	if (oldtextx != TextDotX) {
		oldtextx = TextDotX;
#ifdef ANDROID
		__android_log_print(ANDROID_LOG_DEBUG,"Tag","TextDotX: %d", TextDotX);
#else
		printf("TextDotX: %d\n", TextDotX);
#endif
	}
	if (oldtexty != TextDotY) {
		oldtexty = TextDotY;
#ifdef ANDROID
		__android_log_print(ANDROID_LOG_DEBUG,"Tag","TextDotY: %d", TextDotY);
#else
		printf("TextDotY: %d\n", TextDotY);
#endif
	}

#if defined(ANDROID)
	GLfloat texture_coordinates[8];
	GLfloat vertices[8];
	GLfloat w;

	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glBindTexture(GL_TEXTURE_2D, texid[0]);
glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 800, 600, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, ScrBuf);

	// Texture から必要な部分を抜き出す
	// Texutre座標は0.0fから1.0fの間
	SET_GLFLOATS(texture_coordinates,
		    0.0f, (GLfloat)TextDotY/1024,
		    0.0f, 0.0f,
		    (GLfloat)TextDotX/1024, (GLfloat)TextDotY/1024,
		    (GLfloat)TextDotX/1024, 0.0f);

	// 実機の解像度(1280x736)に関係なく、座標は800x600
	w = (736*1.33333)/1280*800;
	SET_GLFLOATS(vertices,
		     (800.0f - w)/2, (GLfloat)600,
		     (800.0f - w)/2, 0.0f,
		     (800.0f - w)/2 + w, (GLfloat)600,
		     (800.0f - w)/2 + w, 0.0f);

	draw_texture(texture_coordinates, vertices);

	// 左右上下 上上下下左右左右BA
	draw_button(texid[1], 20, 450);
	draw_button(texid[2], 100, 450);
	draw_button(texid[3], 60, 400);
	draw_button(texid[4], 60, 500);

	// ボタン
	draw_button(texid[5], 680, 450);
	draw_button(texid[6], 750, 450);

	//	glDeleteTextures(1, &texid);

	SDL_GL_SwapWindow(sdl_window);
#elif defined(PSP)
	sdl_surface = SDL_GetVideoSurface();

	// とりあえず指定範囲内のみblit
	// PSPのsdl_surfaceのバッファは幅512
	p = ScrBuf;
	dst16 = sdl_surface->pixels;
	for (y = 0; y < 272; y++) {
		p = ScrBuf + 800 * y;
		// surface->pixelsはvoid *
		dst16 = sdl_surface->pixels + 512 * 2 * y;
		for (x = 0; x < 480; x++) {
			*dst16++ = *p++;
		}
	}
	SDL_UpdateRect(sdl_surface, 0, 0, 480, 272);
#else // Unix系 (要SDL_gfx)
	sdl_surface = SDL_GetVideoSurface();
	if (sdl_rgbsurface == NULL) {
		puts("xxx sdl_rgbsurface not allocated yet");
		return;
	}
	if (TextDotX <= 512) {
		roto_surface = rotozoomSurfaceXY(sdl_rgbsurface, 0.0, 512.0*1.33333/TextDotX, 512.0/TextDotY, 0);
	}
	if (roto_surface) {
		ret = SDL_BlitSurface(roto_surface, NULL, sdl_surface, NULL);
		SDL_FreeSurface(roto_surface);
	} else {
		ret = SDL_BlitSurface(sdl_rgbsurface, NULL, sdl_surface, NULL);
	}
	if (ret < 0) {
		printf("SDL_BlitSurface() failed %d\n", ret);
	}

	SDL_UpdateRect(sdl_surface, 0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
#endif

	FrameCount++;
	if (!Draw_DrawFlag/* && is_installed_idle_process()*/)
		return;
	Draw_DrawFlag = 0;

	if (SplashFlag)
		WinDraw_ShowSplash();
}

INLINE void WinDraw_DrawGrpLine(int opaq)
{
	DWORD adr = VLINE*FULLSCREEN_WIDTH;
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
}

INLINE void WinDraw_DrawGrpLineNonSP(int opaq)
{
	DWORD adr = VLINE*FULLSCREEN_WIDTH;
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
}


INLINE void WinDraw_DrawTextLine(int opaq, int td)
{
	DWORD adr = VLINE*FULLSCREEN_WIDTH;
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
}


INLINE void WinDraw_DrawTextLineTR(int opaq)
{
	DWORD adr = VLINE*FULLSCREEN_WIDTH;
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
}

INLINE void WinDraw_DrawTextLineTR2(int opaq)
{
	DWORD adr = VLINE*FULLSCREEN_WIDTH;
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
}


INLINE void WinDraw_DrawBGLine(int opaq, int td)
{
	DWORD adr = VLINE*FULLSCREEN_WIDTH;
	WORD w;
	int i;

	static int log_start = 0;

#if 0 // debug for segv
	if (TextDotX == 128 && TextDotY == 128) {
		log_start = 1;
	}
	if (log_start) {
		printf("opaq/td: %d/%d VLINE: %d, TextDotX: %d\n", opaq, td, VLINE, TextDotX);
	}
#endif

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
}


INLINE void WinDraw_DrawBGLineTR(int opaq)
{
	DWORD adr = VLINE*FULLSCREEN_WIDTH;
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
}

INLINE void WinDraw_DrawPriLine(void)
{
	DWORD adr = VLINE*FULLSCREEN_WIDTH;
	WORD w;
	int i;

	for (i = 0; i < TextDotX; i++, adr++) {
		w = Grp_LineBufSP[i];
		if (w != 0)
			ScrBuf[adr] = w;
	}
}

INLINE void WinDraw_DrawTRLine(void)
{
	DWORD adr = VLINE*FULLSCREEN_WIDTH;
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
				int i;
				for (i = 16; i < TextDotX + 16; ++i)
					BG_LineBuf[i] = TextPal[0];
			} else {		// 20010120 （琥珀色）
				bzero(&BG_LineBuf[16], TextDotX * 2);
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
			DWORD adr = VLINE*FULLSCREEN_WIDTH;
			WORD w;
			int i;

			for (i = 0; i < TextDotX; ++i, ++adr) {
				w = Grp_LineBufSP[i];
				if (w != 0 && (ScrBuf[adr] & 0xffff) == 0)
					ScrBuf[adr] = (w & Pal_HalfMask) >> 1;
			}
		}


	if (opaq)
	{
		DWORD adr = VLINE*FULLSCREEN_WIDTH;
		bzero(&ScrBuf[adr], TextDotX * 2);
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
