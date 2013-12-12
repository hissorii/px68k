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

#include "common.h"
#ifdef USE_OGLES11
#if defined(ANDROID)
#include <GLES/gl.h>
#elif TARGET_OS_IPHONE
#include <OpenGLES/ES1/gl.h>
#endif
#endif
#ifdef PSP
#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspgu.h>
#endif
#include <SDL.h>
//#include <SDL_rotozoom.h>
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
#include "joystick.h"
#include "keyboard.h"

#if 0
#include "../icons/keropi.xpm"
#endif

extern BYTE Debug_Text, Debug_Grp, Debug_Sp;

#ifdef PSP
WORD *ScrBufL = 0, *ScrBufR = 0;
#else
WORD *ScrBuf = 0;
#endif

#if defined(PSP) || defined(USE_OGLES11)
WORD *menu_buffer;
WORD *kbd_buffer;
#endif

#ifdef  USE_SDLGFX
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

#ifdef USE_OGLES11
static GLuint texid[11];
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

#ifdef PSP

/*
          <----- 512byte ------>
0x04000000+--------------------+
          |                    |A
          | draw/disp 0 buffer ||272*2byte (16bit color)
          |                    |V
0x04044000+--------------------+
          |                    |A
          | draw/disp 1 buffer ||272*2byte
          |                    |V
0x04088000+--------------------+
          |                    |A
          | z bufer            ||272*2byte
          |                    |V
0x040cc000+--------------------+
          |                    |A
          | ScrBufL (left)     ||512*2byte (x68k screen size: 756x512)
          |                    |V
0x0414c000+----------+---------+
          |<- 256B ->|A
          |  ScrBufR ||512*2byte (x68k screen size: 756x512)
          |  (right) |V
0x0418c000+----------+---------+A
          |仮想キーボード用    || 512*256*2byte
          |    に使うかも領域  |V
0x041cc000+--------------------+
          |                    |
          | Virtexes           |
          |                    |
          +--------------------+
*/

static unsigned int __attribute__((aligned(16))) list[262144];

void *fbp0, *fbp1, *zbp;
struct Vertexes {
	unsigned short u, v;   // Texture (sx, sy)
	unsigned short color;
	short x, y, z;         // Screen (sx, sy, sz)
	unsigned short u2, v2; // Texture (ex, ey)
	unsigned short color2;
	short x2, y2, z2;      // Screen (ex, ey, ez)
};

struct Vertexes *vtxl = (struct Vertexes *)0x41cc000;
struct Vertexes *vtxr = (struct Vertexes *)(0x41cc000 + sizeof(struct Vertexes));
struct Vertexes *vtxm = (struct Vertexes *)(0x41cc000 + sizeof(struct Vertexes) * 2);
struct Vertexes *vtxk = (struct Vertexes *)(0x41cc000 + sizeof(struct Vertexes) * 3);

#define PSP_BUF_WIDTH (512)
#define PSP_SCR_WIDTH (480)
#define PSP_SCR_HEIGHT (272)

#endif // PSP

static void draw_kbd_to_tex(void);

int WinDraw_Init(void)
{
	int i, j;

#ifndef USE_OGLES11
	SDL_Surface *sdl_surface;

#if SDL_VERSION_ATLEAST(2, 0, 0)
	sdl_surface = SDL_GetWindowSurface(sdl_window);
#else
	sdl_surface = SDL_GetVideoSurface();
#endif
	if (sdl_surface == NULL) {
		fprintf(stderr, "can't create surface.\n");
		return 1;
	}

#endif
	WindowX = 768;
	WindowY = 512;

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

#if defined(USE_OGLES11)
	ScrBuf = malloc(1024*1024*2); // OpenGL ES 1.1 needs 2^x pixels
	if (ScrBuf == NULL) {
		return FALSE;
	}

	kbd_buffer = malloc(1024*1024*2); // OpenGL ES 1.1 needs 2^x pixels
	if (kbd_buffer == NULL) {
		return FALSE;
	}

	p6logd("kbd_buffer 0x%x", kbd_buffer);

	memset(texid, 0, sizeof(texid));
	glGenTextures(11, &texid[0]);

	// texid[0] for the main screen
	glBindTexture(GL_TEXTURE_2D, texid[0]);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, ScrBuf);

	WORD BtnTex[32*32];
	//とりあえず薄めの緑で。
	for (i = 0; i < 32*32; i++) {
		BtnTex[i] = 0x03e0;
	}

	// ボタン用テクスチャ。とりあえず全部同じ色。
	for (i = 1; i < 9; i++) {
		glBindTexture(GL_TEXTURE_2D, texid[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		if (i == 7) {
			// とりあえずキーボードonボタンは薄めの黄色で。
			for (j = 0; j < 32*32; j++) {
				BtnTex[j] = (0x7800 | 0x03e0);
			}
		}
		if (i == 8) {
			// とりあえずmenu onボタンは薄めの白色で。
			for (j = 0; j < 32*32; j++) {
				BtnTex[j] = (0x7800 | 0x03e0 | 0x0f);
			}
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 32, 32, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, BtnTex);
	}

	// メニュー描画用テクスチャ。
	glBindTexture(GL_TEXTURE_2D, texid[9]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, menu_buffer);

	draw_kbd_to_tex();

	// ソフトウェアキーボード描画用テクスチャ。
	glBindTexture(GL_TEXTURE_2D, texid[10]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, kbd_buffer);

#elif defined(PSP)

	fbp0 = 0; // offset 0
	fbp1 = (void *)((unsigned int)fbp0 + PSP_BUF_WIDTH * PSP_SCR_HEIGHT * 2);
	zbp = (void *)((unsigned int)fbp1 + PSP_BUF_WIDTH * PSP_SCR_HEIGHT * 2);

	sceGuInit();

	sceGuStart(GU_DIRECT,list);
	sceGuDrawBuffer(GU_PSM_5650, fbp0, PSP_BUF_WIDTH);
	sceGuDispBuffer(PSP_SCR_WIDTH, PSP_SCR_HEIGHT, fbp1, PSP_BUF_WIDTH);
	sceGuDepthBuffer(zbp, PSP_BUF_WIDTH);
	sceGuOffset(2048 - (PSP_SCR_WIDTH/2), 2048 - (PSP_SCR_HEIGHT/2));
	sceGuViewport(2048, 2048, PSP_SCR_WIDTH, PSP_SCR_HEIGHT);
	sceGuDepthRange(65535, 0);
	sceGuScissor(0, 0, PSP_SCR_WIDTH, PSP_SCR_HEIGHT);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuFrontFace(GU_CW);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
	sceGuFinish();
	sceGuSync(0, 0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);

	ScrBufL = (WORD *)0x040cc000;
	ScrBufR = (WORD *)0x0414c000;
	kbd_buffer = (WORD *)0x418c000;

	draw_kbd_to_tex();
#else

#ifdef USE_SDLGFX
	sdl_rgbsurface = SDL_CreateRGBSurface(SDL_SWSURFACE, 800, 600, 16, WinDraw_Pal16R, WinDraw_Pal16G, WinDraw_Pal16B, 0);

	if (sdl_rgbsurface == 0) {
		puts("ScrBuf allocate failed");
		exit(1);
	}
	ScrBuf = sdl_rgbsurface->pixels;

	printf("drawbuf: 0x%x, ScrBuf: 0x%x\n", sdl_surface->pixels, ScrBuf);
#else
	ScrBuf = malloc(800 * 600 * 2);
#endif

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

#ifdef USE_OGLES11
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

void draw_button(GLuint texid, GLfloat x, GLfloat y, GLfloat s, GLfloat *tex, GLfloat *ver)
{
	glBindTexture(GL_TEXTURE_2D, texid);
	// Texture から必要な部分を抜き出す(32x32を全部使う)
	SET_GLFLOATS(tex, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f);
	// s倍にして貼り付ける
	SET_GLFLOATS(ver, x, y + 32.0f * s, x, y, x + 32.0f * s, y + 32.0f * s, x + 32.0f * s, y + 0.0f);
	draw_texture(tex, ver);
}

void draw_all_buttons(GLfloat *tex, GLfloat *ver, GLfloat scale)
{
	int i;
	VBTN_POINTS *p;

	p = Joystick_get_btn_points(scale);

	// 仮想キーはtexid: 1から6まで、キーボードonボタンが7、menuボタンが8
	for (i = 1; i < 9; i++) {
		draw_button(texid[i], p->x, p->y, scale, tex, ver);
		p++;
	}
}
#endif // USE_OGLES11

void FASTCALL
WinDraw_Draw(void)
{
	SDL_Surface *sdl_surface;
	static int oldtextx = -1, oldtexty = -1;

	if (oldtextx != TextDotX) {
		oldtextx = TextDotX;
		p6logd("TextDotX: %d\n", TextDotX);
	}
	if (oldtexty != TextDotY) {
		oldtexty = TextDotY;
		p6logd("TextDotY: %d\n", TextDotY);
	}

#if defined(USE_OGLES11)
	GLfloat texture_coordinates[8];
	GLfloat vertices[8];
	GLfloat w;

	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);

	// アルファブレンドしない(上のテクスチャが下のテクスチャを隠す)
	glBlendFunc(GL_ONE, GL_ZERO);

	glBindTexture(GL_TEXTURE_2D, texid[0]);
	//ScrBufから800x600の領域を切り出してテクスチャに転送
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 800, 600, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, ScrBuf);

	// magic numberがやたら多いが、テクスチャのサイズが1024x1024
	// OpenGLでの描画領域がglOrthof()で定義した800x600

	// X68K 画面描画

	// Texture から必要な部分を抜き出す
	// Texutre座標は0.0fから1.0fの間
	SET_GLFLOATS(texture_coordinates,
		     0.0f, (GLfloat)TextDotY/1024,
		     0.0f, 0.0f,
		     (GLfloat)TextDotX/1024, (GLfloat)TextDotY/1024,
		     (GLfloat)TextDotX/1024, 0.0f);

	// 実機の解像度(realdisp_w x realdisp_h)に関係なく、
	// 座標は800x600
	w = (realdisp_h * 1.33333) / realdisp_w * 800;
	SET_GLFLOATS(vertices,
		     (800.0f - w)/2, (GLfloat)600,
		     (800.0f - w)/2, 0.0f,
		     (800.0f - w)/2 + w, (GLfloat)600,
		     (800.0f - w)/2 + w, 0.0f);

	draw_texture(texture_coordinates, vertices);

	// ソフトウェアキーボード描画

	if (Keyboard_IsSwKeyboard()) {
		glBindTexture(GL_TEXTURE_2D, texid[10]);
		//kbd_bufferから800x600の領域を切り出してテクスチャに転送
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 800, 600, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, kbd_buffer);

		// Texture から必要な部分を抜き出す
		// Texutre座標は0.0fから1.0fの間
		SET_GLFLOATS(texture_coordinates,
			     0.0f, (GLfloat)kbd_h/1024,
			     0.0f, 0.0f,
			     (GLfloat)kbd_w/1024, (GLfloat)kbd_h/1024,
			     (GLfloat)kbd_w/1024, 0.0f);

		// 実機の解像度(realdisp_w x realdisp_h)に関係なく、
		// 座標は800x600

		float kbd_scale = 0.8;
		SET_GLFLOATS(vertices,
			     (GLfloat)kbd_x, (GLfloat)(kbd_h * kbd_scale + kbd_y),
			     (GLfloat)kbd_x, (GLfloat)kbd_y,
			     (GLfloat)(kbd_w * kbd_scale + kbd_x), (GLfloat)(kbd_h * kbd_scale + kbd_y),
			     (GLfloat)(kbd_w * kbd_scale + kbd_x), (GLfloat)kbd_y);

		draw_texture(texture_coordinates, vertices);
	}

	// 仮想パッド/ボタン描画

	// アルファブレンドする(スケスケいやん)
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	draw_all_buttons(texture_coordinates, vertices, (GLfloat)WinUI_get_vkscale());

	//	glDeleteTextures(1, &texid);

	SDL_GL_SwapWindow(sdl_window);

#elif defined(PSP)
	sceGuStart(GU_DIRECT, list);

	sceGuClearColor(0);
	sceGuClearDepth(0);
	sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);

	// 左半分
	vtxl->u = 0;
	vtxl->v = 0;
	vtxl->color = 0;
	vtxl->x = (480 - (272 * 1.33333)) / 2; // 1.3333 = 4 : 3
	vtxl->y = 0;
	vtxl->z = 0;
	vtxl->u2 = (TextDotX >= 512)? 512 : TextDotX;
	vtxl->v2 = TextDotY;
	vtxl->color2 = 0;
	vtxl->x2 = (TextDotX >= 512)?
		vtxl->x + 272 * 1.33333 * (512.0 / TextDotX) :
		vtxl->x + 272 * 1.33333;
	vtxl->y2 = 272;
	vtxl->z2 = 0;

	sceGuTexMode(GU_PSM_5650, 0, 0, 0);
	sceGuTexImage(0, 512, 512, 512, ScrBufL);
	sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGB);
	sceGuTexFilter(GU_LINEAR, GU_LINEAR);

	sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT|GU_COLOR_5650|GU_VERTEX_16BIT|GU_TRANSFORM_2D, 2, 0, vtxl);

	if (TextDotX > 512) {
		// 右半分
		vtxr->u = 0;
		vtxr->v = 0;
		vtxr->color = 0;
		vtxr->x = vtxl->x2;
		vtxr->y = 0;
		vtxr->z = 0;
		vtxr->u2 = TextDotX - 512;
		vtxr->v2 = TextDotY;
		vtxr->color2 = 0;
		vtxr->x2 = vtxl->x + 272 * 1.33333;
		vtxr->y2 = 272;
		vtxr->z2 = 0;

		sceGuTexMode(GU_PSM_5650, 0, 0, 0);
		sceGuTexImage(0, 256, 512, 256, ScrBufR);
		sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGB);
		sceGuTexFilter(GU_LINEAR, GU_LINEAR);

		sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT|GU_COLOR_5650|GU_VERTEX_16BIT|GU_TRANSFORM_2D, 2, 0, vtxr);
	}

	if (Keyboard_IsSwKeyboard()) {
		vtxk->u = 0;
		vtxk->v = 0;
		vtxk->color = 0;
		vtxk->x = kbd_x;
		vtxk->y = kbd_y;
		vtxk->z = 0;
		vtxk->u2 = kbd_w;
		vtxk->v2 = kbd_h;
		vtxk->color2 = 0;
		vtxk->x2 = kbd_x + kbd_w;
		vtxk->y2 = kbd_y + kbd_h;
		vtxk->z2 = 0;

		sceGuTexImage(0, 512, 256, 512, kbd_buffer);
		sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGB);
		sceGuTexFilter(GU_LINEAR, GU_LINEAR);

		sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT|GU_COLOR_5650|GU_VERTEX_16BIT|GU_TRANSFORM_2D, 2, 0, vtxk);
	}

	sceGuFinish();
	sceGuSync(0, 0);

	sceGuSwapBuffers();

#else // OpenGL ES 未使用

#if SDL_VERSION_ATLEAST(2, 0, 0)
	sdl_surface = SDL_GetWindowSurface(sdl_window);
#else
	sdl_surface = SDL_GetVideoSurface();
#endif

#ifdef USE_SDLGFX
	SDL_Surface *roto_surface = NULL;
	int ret;

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
#else
	int x, y, Bpp;
//	WORD c, *p, *p2, dummy, *dst16;
	WORD *p, *dst16;
	DWORD *dst32, dat32;

	Bpp = sdl_surface->format->BytesPerPixel;
	// 2倍に拡大する
	if (TextDotX <= 256 && TextDotY <= 256) {
		for (y = 0; y < 256; y++) {
			p = ScrBuf + 800 * y;
			// surface->pixelsはvoid *
			dst16 = sdl_surface->pixels + sdl_surface->w * Bpp * y * 2;
			dst32 = (DWORD *)dst16;
			for (x = 0; x < 256; x++) {
				if  (Bpp == 4) {
					dat32 = (DWORD)(*p & 0xf800) << 8 | (*p & 0x07e0) << 5 | (*p & 0x001f) << 3;
					*dst32++ = dat32;
					*dst32 = dat32;
					dst32 += sdl_surface->w;
					*dst32-- = dat32;
					*dst32 = dat32;
					p++;
					dst32 -= sdl_surface->w;
					dst32 += 2;
				} else if (Bpp == 2) {
					*dst16++ = *p;
					*dst16 = *p;
					dst16 += sdl_surface->w;
					*dst16-- = *p;
					*dst16 = *p;
					p++;
					dst16 -= sdl_surface->w;
					dst16 += 2;
				} else {
					// xxx 未サポート
				}
			}
		}
	} else {
		for (y = 0; y < 512; y++) {
			p = ScrBuf + 800 * y;
			// surface->pixelsはvoid *
			dst16 = sdl_surface->pixels + sdl_surface->w * Bpp * y;
			dst32 = (DWORD *)dst16;
			for (x = 0; x < 800; x++) {
				if (Bpp == 4) {
					*dst32++ = (DWORD)(*p & 0xf800) << 8 | (*p & 0x07e0) << 5 | (*p & 0x001f) << 3;
				} else if (Bpp == 2) {
					*dst16++ = *p;
				}
				p++;
			}
		}
	}
#endif

#if SDL_VERSION_ATLEAST(2, 0, 0)
	SDL_UpdateWindowSurface(sdl_window);
#else
	SDL_UpdateRect(sdl_surface, 0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
#endif

#endif

	FrameCount++;
	if (!Draw_DrawFlag/* && is_installed_idle_process()*/)
		return;
	Draw_DrawFlag = 0;

	if (SplashFlag)
		WinDraw_ShowSplash();
}

#ifdef PSP
#ifdef FULLSCREEN_WIDTH
#undef FULLSCREEN_WIDTH
#endif
//PSPのテクスチャは一辺が最大512なので、X68000の768x512画面を
//512x512と256x512の左右二つに分ける。
//以下のDrawLine系の関数処理もPSPは左右のテクスチャを別々に
//処理しなければならない。
//ということで、以下のdefineは左側テクスチャの横幅を表している。
#define FULLSCREEN_WIDTH 512
#endif

#ifdef PSP
#define WD_MEMCPY(src)						\
{								\
	if (TextDotX > 512) {					\
		memcpy(&ScrBufL[adr], (src), 512 * 2);		\
		adr = VLINE * 256;				\
		memcpy(&ScrBufR[adr], (WORD *)(src) + 512, TextDotX * 2 - 512 * 2); \
	} else {						\
		memcpy(&ScrBufL[adr], (src), TextDotX * 2);	\
	}							\
}
#else
#define WD_MEMCPY(src) memcpy(&ScrBuf[adr], (src), TextDotX * 2)
#endif

#ifdef PSP
#define WD_LOOP(start, end, sub)				\
{								\
	if (TextDotX > 512) {					\
		for (i = (start); i < 512 + (start); i++, adr++) {	\
			sub(L);						\
		}							\
		adr = VLINE * 256;					\
		for (i = 512 + (start); i < (end); i++, adr++) {	\
			sub(R);						\
		}							\
	} else {							\
		for (i = (start); i < (end); i++, adr++) {		\
			sub(L);						\
		}							\
	}								\
}
#else
#define WD_LOOP(start, end, sub)			\
{ 							\
	for (i = (start); i < (end); i++, adr++) {	\
		sub();					\
	}						\
}
#endif

#define WD_SUB(SUFFIX, src)			\
{						\
	w = (src);				\
	if (w != 0)				\
		ScrBuf##SUFFIX[adr] = w;	\
}


INLINE void WinDraw_DrawGrpLine(int opaq)
{
#define _DGL_SUB(SUFFIX) WD_SUB(SUFFIX, Grp_LineBuf[i])

	DWORD adr = VLINE*FULLSCREEN_WIDTH;
	WORD w;
	int i;

	if (opaq) {
		WD_MEMCPY(Grp_LineBuf);
	} else {
		WD_LOOP(0,  TextDotX, _DGL_SUB);
	}
}

INLINE void WinDraw_DrawGrpLineNonSP(int opaq)
{
#define _DGL_NSP_SUB(SUFFIX) WD_SUB(SUFFIX, Grp_LineBufSP2[i])

	DWORD adr = VLINE*FULLSCREEN_WIDTH;
	WORD w;
	int i;

	if (opaq) {
		WD_MEMCPY(Grp_LineBufSP2);
	} else {
		WD_LOOP(0,  TextDotX, _DGL_NSP_SUB);
	}
}

INLINE void WinDraw_DrawTextLine(int opaq, int td)
{
#define _DTL_SUB2(SUFFIX) WD_SUB(SUFFIX, BG_LineBuf[i])
#define _DTL_SUB(SUFFIX)		\
{					\
	if (Text_TrFlag[i] & 1) {	\
		_DTL_SUB2(SUFFIX);	\
	}				\
}	

	DWORD adr = VLINE*FULLSCREEN_WIDTH;
	WORD w;
	int i;

	if (opaq) {
		WD_MEMCPY(&BG_LineBuf[16]);
	} else {
		if (td) {
			WD_LOOP(16, TextDotX + 16, _DTL_SUB);
		} else {
			WD_LOOP(16, TextDotX + 16, _DTL_SUB2);
		}
	}
}

INLINE void WinDraw_DrawTextLineTR(int opaq)
{
#define _DTL_TR_SUB(SUFFIX)			   \
{						   \
	w = Grp_LineBufSP[i - 16];		   \
	if (w != 0) {				   \
		w &= Pal_HalfMask;		   \
		v = BG_LineBuf[i];		   \
		if (v & Ibit)			   \
			w += Pal_Ix2;		   \
		v &= Pal_HalfMask;		   \
		v += w;				   \
		v >>= 1;			   \
	} else {				   \
		if (Text_TrFlag[i] & 1)		   \
			v = BG_LineBuf[i];	   \
		else				   \
			v = 0;			   \
	}					   \
	ScrBuf##SUFFIX[adr] = (WORD)v;		   \
}

#define _DTL_TR_SUB2(SUFFIX)			   \
{						   \
	if (Text_TrFlag[i] & 1) {		   \
		w = Grp_LineBufSP[i - 16];	   \
		v = BG_LineBuf[i];		   \
						   \
		if (v != 0) {			   \
			if (w != 0) {			\
				w &= Pal_HalfMask;	\
				if (v & Ibit)		\
					w += Pal_Ix2;	\
				v &= Pal_HalfMask;	\
				v += w;			\
				v >>= 1;		\
			}				\
			ScrBuf##SUFFIX[adr] = (WORD)v;	\
		}					\
	}						\
}

	DWORD adr = VLINE*FULLSCREEN_WIDTH;
	DWORD v;
	WORD w;
	int i;

	if (opaq) {
		WD_LOOP(16, TextDotX + 16, _DTL_TR_SUB);
	} else {
		WD_LOOP(16, TextDotX + 16, _DTL_TR_SUB2);
	}
}

INLINE void WinDraw_DrawBGLine(int opaq, int td)
{
#define _DBL_SUB2(SUFFIX) WD_SUB(SUFFIX, BG_LineBuf[i])
#define _DBL_SUB(SUFFIX)			 \
{						 \
	if (Text_TrFlag[i] & 2) {		 \
		_DBL_SUB2(SUFFIX); \
	} \
}

	DWORD adr = VLINE*FULLSCREEN_WIDTH;
	WORD w;
	int i;

#if 0 // debug for segv
	static int log_start = 0;

	if (TextDotX == 128 && TextDotY == 128) {
		log_start = 1;
	}
	if (log_start) {
		printf("opaq/td: %d/%d VLINE: %d, TextDotX: %d\n", opaq, td, VLINE, TextDotX);
	}
#endif

	if (opaq) {
		WD_MEMCPY(&BG_LineBuf[16]);
	} else {
		if (td) {
			WD_LOOP(16, TextDotX + 16, _DBL_SUB);
		} else {
			WD_LOOP(16, TextDotX + 16, _DBL_SUB2);
		}
	}
}

INLINE void WinDraw_DrawBGLineTR(int opaq)
{

#define _DBL_TR_SUB3()			\
{					\
	if (w != 0) {			\
		w &= Pal_HalfMask;	\
		if (v & Ibit)		\
			w += Pal_Ix2;	\
		v &= Pal_HalfMask;	\
		v += w;			\
		v >>= 1;		\
	}				\
}

#define _DBL_TR_SUB(SUFFIX) \
{					\
	w = Grp_LineBufSP[i - 16];	\
	v = BG_LineBuf[i];		\
					\
	_DBL_TR_SUB3()			\
	ScrBuf##SUFFIX[adr] = (WORD)v;	\
}

#define _DBL_TR_SUB2(SUFFIX) \
{							\
	if (Text_TrFlag[i] & 2) {  			\
		w = Grp_LineBufSP[i - 16];		\
		v = BG_LineBuf[i];			\
							\
		if (v != 0) {				\
			_DBL_TR_SUB3()			\
			ScrBuf##SUFFIX[adr] = (WORD)v;	\
		}					\
	}						\
}

	DWORD adr = VLINE*FULLSCREEN_WIDTH;
	DWORD v;
	WORD w;
	int i;

	if (opaq) {
		WD_LOOP(16, TextDotX + 16, _DBL_TR_SUB);
	} else {
		WD_LOOP(16, TextDotX + 16, _DBL_TR_SUB2);
	}

}

INLINE void WinDraw_DrawPriLine(void)
{
#define _DPL_SUB(SUFFIX) WD_SUB(SUFFIX, Grp_LineBufSP[i])

	DWORD adr = VLINE*FULLSCREEN_WIDTH;
	WORD w;
	int i;

	WD_LOOP(0, TextDotX, _DPL_SUB);
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

#define _DL_SUB(SUFFIX) \
{								\
	w = Grp_LineBufSP[i];					\
	if (w != 0 && (ScrBuf##SUFFIX[adr] & 0xffff) == 0)	\
		ScrBuf##SUFFIX[adr] = (w & Pal_HalfMask) >> 1;	\
}

			DWORD adr = VLINE*FULLSCREEN_WIDTH;
			WORD w;
			int i;

			WD_LOOP(0, TextDotX, _DL_SUB);
		}


	if (opaq)
	{
		DWORD adr = VLINE*FULLSCREEN_WIDTH;
#ifdef PSP
		if (TextDotX > 512) {
			bzero(&ScrBufL[adr], TextDotX * 2);
			adr = VLINE * 256;
			bzero(&ScrBufR[adr], (TextDotX - 512) * 2);
		} else {
			bzero(&ScrBufL[adr], TextDotX * 2);
		}
#else
		bzero(&ScrBuf[adr], TextDotX * 2);
#endif
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

/********** menu 関連ルーチン **********/

struct _px68k_menu {
	WORD *sbp;  // surface buffer ptr
	WORD *mlp; // menu locate ptr
	WORD mcolor; // color of chars to write
	WORD mbcolor; // back ground color of chars to write
	int ml_x;
	int ml_y;
	int mfs; // menu font size;
} p6m;

#if !defined(PSP) && !defined(USE_OGLES11)
SDL_Surface *menu_surface;
#endif

// 画面タイプを変更する
enum ScrType {x68k, pc98};
int scr_type = x68k;

/* sjis→jisコード変換 */
static WORD sjis2jis(WORD w)
{
	BYTE wh, wl;

	wh = w / 256, wl = w % 256;

	wh <<= 1;
	if (wl < 0x9f) {
		wh += (wh < 0x3f)? 0x1f : -0x61;
		wl -= (wl > 0x7e)? 0x20 : 0x1f;
	} else {
		wh += (wh < 0x3f)? 0x20 : -0x60;
		wl -= 0x7e;
	}

	return (wh * 256 + wl);
}

/* JISコードから0 originのindexに変換する */
/* ただし0x2921-0x2f7eはX68KのROM上にないので飛ばす */
static WORD jis2idx(WORD jc)
{
	if (jc >= 0x3000) {
		jc -= 0x3021;
	} else {
		jc -= 0x2121;
	}
	jc = jc % 256 + (jc / 256) * 0x5e;

	return jc;
}

#define isHankaku(s) ((s) >= 0x20 && (s) <= 0x7e || (s) >= 0xa0 && (s) <= 0xdf)
#if defined(PSP)
// display width 480, buffer width 512
#define MENU_WIDTH 512
#elif defined(ANDROID)
// display width 800, buffer width 1024 だけれど 800 にしないとだめ
#define MENU_WIDTH 800
#else
#define MENU_WIDTH 800
#endif

// fs : font size : 16 or 24
// 半角文字の場合は16bitの上位8bitにデータを入れておくこと
// (半角or全角の判断ができるように)
static DWORD get_font_addr(WORD sjis, int fs)
{
	WORD jis, j_idx;
	BYTE jhi;
	int fsb; // file size in bytes

	// 半角文字
	if (isHankaku(sjis >> 8)) {
		switch (fs) {
		case 8:
			return (0x3a000 + (sjis >> 8) * (1 * 8));
		case 16:
			return (0x3a800 + (sjis >> 8) * (1 * 16));
		case 24:
			return (0x3d000 + (sjis >> 8) * (2 * 24));
		default:
			return -1;
		}
	}

	// 全角文字
	if (fs == 16) {
		fsb = 2 * 16;
	} else if (fs == 24) {
		fsb = 3 * 24;
	} else {
		return -1;
	}

	jis = sjis2jis(sjis);
	j_idx = (DWORD)jis2idx(jis);
	jhi = (BYTE)(jis >> 8);

#if 0
	printf("sjis code = 0x%x\n", sjis);
	printf("jis code = 0x%x\n", jis);
	printf("jhi 0x%x j_idx 0x%x\n", jhi, j_idx);
#endif

	if (jhi >= 0x21 && jhi <= 0x28) {
		// 非漢字
		return  ((fs == 16)? 0x0 : 0x40000) + j_idx * fsb;
	} else if (jhi >= 0x30 && jhi <= 0x74) {
		// 第一水準/第二水準
		return  ((fs == 16)? 0x5e00 : 0x4d380) + j_idx * fsb;
	} else {
		// ここにくることはないはず
		return -1;
	}
}

// RGB565
static void set_mcolor(WORD c)
{
	p6m.mcolor = c;
}

// mbcolor = 0 なら透明色とする
static void set_mbcolor(WORD c)
{
	p6m.mbcolor = c;
}

// グラフィック座標
static void set_mlocate(int x, int y)
{
	p6m.ml_x = x, p6m.ml_y = y;
}

// キャラクタ文字の座標 (横軸は1座標が半角文字幅になる)
static void set_mlocateC(int x, int y)
{
	p6m.ml_x = x * p6m.mfs / 2, p6m.ml_y = y * p6m.mfs;
}

static void set_sbp(WORD *p)
{
	p6m.sbp = p;
}

// menu font size (16 or 24)
static void set_mfs(int fs)
{
	p6m.mfs = fs;
}

static WORD *get_ml_ptr()
{
	p6m.mlp = p6m.sbp + MENU_WIDTH * p6m.ml_y + p6m.ml_x;
	return p6m.mlp;
}

// ・半角文字の場合は16bitの上位8bitにデータを入れておくこと
//   (半角or全角の判断ができるように)
// ・表示した分cursorは先に移動する
static void draw_char(WORD sjis)
{
	DWORD f;
	WORD *p;
	int i, j, k, wc, w;
	BYTE c;
	WORD bc;

	int h = p6m.mfs;

	p = get_ml_ptr();

	f = get_font_addr(sjis, h);

	if (f < 0)
		return;

	// h=8は半角のみ
	w = (h == 8)? 8 : (isHankaku(sjis >> 8)? h / 2 : h);

	for (i = 0; i < h; i++) {
		wc = w;
		for (j = 0; j < ((w % 8 == 0)? w / 8 : w / 8 + 1); j++) {
			c = FONT[f++];
			for (k = 0; k < 8 ; k++) {
				bc = p6m.mbcolor? p6m.mbcolor : *p;
				*p = (c & 0x80)? p6m.mcolor : bc;
				p++;
				c = c << 1;
				wc--;
				if (wc == 0)
					break;
			}
		}
		p = p + MENU_WIDTH - w;
	}

	p6m.ml_x += w;
}

static void draw_str(char *cp)
{
	int i, len;
	BYTE *s;
	WORD wc;

	len = strlen(cp);
	s = (BYTE *)cp;

	for (i = 0; i < len; i++) {
		if (isHankaku(*s)) {
			// 最初の8bitで半全角を判断するので半角の場合は
			// あらかじめ8bit左シフトしておく
			draw_char((WORD)*s << 8);
			s++;
		} else {
			wc = (WORD)(*s << 8) + *(s + 1);
			draw_char(wc);
			s += 2;
			i++;
		}
		// 8x8描画(ソフトキーボードのFUNCキーは文字幅を縮める)
		if (p6m.mfs == 8) {
			p6m.ml_x -= 3;
		}
	}
}

int WinDraw_MenuInit(void)
{
#if defined(PSP)
	// menuは速度遅くて良いのでメインメモリからmalloc()
	menu_buffer = malloc(512 * 512 * 2);
	if (menu_buffer == NULL) {
		return FALSE;
	}
	set_sbp(menu_buffer);
	// 使用フォントの変更 24 or 16
	set_mfs(16);
#elif defined(USE_OGLES11)
	//
	menu_buffer = malloc(1024 * 1024 * 2);
	if (menu_buffer == NULL) {
		return FALSE;
	}
	set_sbp(menu_buffer);
	set_mfs(24);
#else

#if SDL_VERSION_ATLEAST(2, 0, 0)
	menu_surface = SDL_GetWindowSurface(sdl_window);
#else
	menu_surface = SDL_GetVideoSurface();
#endif

	if (!menu_surface)
		return FALSE;
	set_sbp((WORD *)(menu_surface->pixels));
	set_mfs(24);
#endif
	set_mcolor(0xffff);
	set_mbcolor(0);

	return TRUE;
}

#include "menu_str_sjis.txt"

#ifdef PSP
static void psp_draw_menu(void)
{
	sceKernelDcacheWritebackAll();

	sceGuStart(GU_DIRECT, list);

	sceGuClearColor(0);
	sceGuClearDepth(0);
	sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);

	// 左半分
	vtxm->u = 0;
	vtxm->v = 0;
	vtxm->color = 0;
	vtxm->x = 0;
	vtxm->y = 0;
	vtxm->z = 0;
	vtxm->u2 = 480;
	vtxm->v2 = 272;
	vtxm->color2 = 0;
	vtxm->x2 = 480;
	vtxm->y2 = 272;
	vtxm->z2 = 0;

	sceGuTexMode(GU_PSM_5650, 0, 0, 0);
	sceGuTexImage(0, 512, 512, 512, menu_buffer);
	sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGB);
	//sceGuTexFilter(GU_LINEAR, GU_LINEAR);

	sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT|GU_COLOR_5650|GU_VERTEX_16BIT|GU_TRANSFORM_2D, 2, 0, vtxm);

	sceGuFinish();
	sceGuSync(0, 0);

	sceGuSwapBuffers();
}
#endif

#ifdef USE_OGLES11
static void ogles11_draw_menu(void)
{
	GLfloat texture_coordinates[8];
	GLfloat vertices[8];

	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glBindTexture(GL_TEXTURE_2D, texid[9]);
	//ScrBufから800x600の領域を切り出してテクスチャに転送
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 800, 600, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, menu_buffer);

	// Texture から必要な部分を抜き出す
	// Texutre座標は0.0fから1.0fの間
	SET_GLFLOATS(texture_coordinates,
		     0.0f, (GLfloat)600/1024,
		     0.0f, 0.0f,
		     (GLfloat)800/1024, (GLfloat)600/1024,
		     (GLfloat)800/1024, 0.0f);

	// 実機の解像度(realdisp_w x realdisp_h)に関係なく、
	// 座標は800x600
	SET_GLFLOATS(vertices,
		     40.0f, (GLfloat)600,
		     40.0f, 0.0f,
		     (GLfloat)800, (GLfloat)600,
		     (GLfloat)800, 0.0f);

	draw_texture(texture_coordinates, vertices);

	draw_all_buttons(texture_coordinates, vertices, (GLfloat)WinUI_get_vkscale());

	SDL_GL_SwapWindow(sdl_window);
}
#endif


void WinDraw_DrawMenu(int menu_state, int mkey_y, int *mval_y)
{
	int i;

// ソフトウェアキーボード描画時にset_sbp(kbd_buffer)されているので戻す
#if defined(PSP) || defined(USE_OGLES11)
	set_sbp(menu_buffer);
#endif
// ソフトウェアキーボード描画時にset_mfs(16)されているので戻す
#if defined(ANDROID) || TARGET_OS_IPHONE
	set_mfs(24);
#endif

	// タイトル
	if (scr_type == x68k) {
		set_mcolor(0x07ff); // cyan
		set_mlocateC(0, 0);
		draw_str(twaku_str);
		set_mlocateC(0, 1);
		draw_str(twaku2_str);
		set_mlocateC(0, 2);
		draw_str(twaku3_str);

		set_mcolor(0xffff);
		set_mlocateC(1, 1);
		draw_str(title_str);
	} else {
		set_mcolor(0xffff);
		set_mlocateC(0, 0);
		draw_str(pc98_title1_str);
		set_mlocateC(0, 2);
		draw_str(pc98_title3_str);
		set_mcolor(0x07ff);
		set_mlocateC(0, 1);
		draw_str(pc98_title2_str);
	}

	// 真ん中
	if (scr_type == x68k) {
		set_mcolor(0xffff);
		set_mlocate(3 * p6m.mfs / 2, 3.5 * p6m.mfs);
		draw_str(waku_val_str[0]);
		set_mlocate(17 * p6m.mfs / 2, 3.5 * p6m.mfs);
		draw_str(waku_val_str[1]);

		// 真ん中枠
		set_mcolor(0xffe0); // yellow
		set_mlocateC(1, 4);
		draw_str(waku_str);
		for (i = 5; i < 12; i++) {
			set_mlocateC(1, i);
			draw_str(waku2_str);
		}
		set_mlocateC(1, 12);
		draw_str(waku3_str);
	}

	// アイテム/キーワード
	set_mcolor(0xffff);
	for (i = 0; i < 7; i++) {
		set_mlocateC(3, 5 + i);
		if (menu_state == ms_key && i == mkey_y) {
			set_mcolor(0x0);
			set_mbcolor(0xffe0); // yellow);
		} else {
			set_mcolor(0xffff);
			set_mbcolor(0x0);
		}
		draw_str(menu_item_key[i]);
	}

	// アイテム/現在値
	set_mcolor(0xffff);
	set_mbcolor(0x0);
	for (i = 0; i < 7; i++) {
		if (menu_state == ms_value && i == mkey_y) {
			set_mcolor(0x0);
			set_mbcolor(0xffe0); // yellow);
		} else {
			set_mcolor(0xffff);
			set_mbcolor(0x0);
		}
		if (scr_type == x68k) {
			set_mlocateC(17, 5 + i);
		} else {
			set_mlocateC(25, 5 + i);
		}
		if ((i == 1 || i == 2) && mval_y[i] == 0) {
			if (Config.FDDImage[i - 1][0] == '\0') {
				draw_str(" -- no disk --");
			} else {
				char *p;
				p = Config.FDDImage[i - 1];
				// 先頭のカレントディレクトリ名を表示しない
				if (!strncmp(CUR_DIR_STR, p, CUR_DIR_SLEN)) {
					draw_str(p + CUR_DIR_SLEN);
				} else {
					draw_str(p);
				}
			}
		} else {
			draw_str(menu_items[i][mval_y[i]]);
		}
	}

	if (scr_type == x68k) {
		// 下枠
		set_mcolor(0x07ff); // cyan
		set_mbcolor(0x0);
		set_mlocateC(0, 13);
		draw_str(swaku_str);
		set_mlocateC(0, 14);
		draw_str(swaku2_str);
		set_mlocateC(0, 15);
		draw_str(swaku2_str);
		set_mlocateC(0, 16);
		draw_str(swaku3_str);
	}

	// キャプション
	set_mcolor(0xffff);
	set_mbcolor(0x0);
	set_mlocateC(2, 14);
	draw_str(item_cap[mkey_y]);
	if (menu_state == ms_value) {
		set_mlocateC(2, 15);
		draw_str(item_cap2[mkey_y]);
	}
#if defined(PSP)
	psp_draw_menu();
#elif defined(USE_OGLES11)
	ogles11_draw_menu();
#else

#if SDL_VERSION_ATLEAST(2, 0, 0)
	SDL_UpdateWindowSurface(sdl_window);
#else
	SDL_UpdateRect(menu_surface, 0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
#endif
	
#endif
}

void WinDraw_DrawMenufile(struct menu_flist *mfl)
{
	int i;

	// 下枠
	//set_mcolor(0xf800); // red
	//set_mcolor(0xf81f); // magenta
	set_mcolor(0xffff);
	set_mbcolor(0x1); // 0x0だと透過モード
	set_mlocateC(1, 1);
	draw_str(swaku_str);
	for (i = 2; i < 16; i++) {
		set_mlocateC(1, i);
		draw_str(swaku2_str);
	}
	set_mlocateC(1, 16);
	draw_str(swaku3_str);

	for (i = 0; i < 14; i++) {
		if (i + 1 > mfl->num) {
			break;
		}
		if (i == mfl->y) {
			set_mcolor(0x0);
			set_mbcolor(0xffff);
		} else {
			set_mcolor(0xffff);
			set_mbcolor(0x1);
		}
		// ディレクトリだったらファイル名を[]で囲う
		set_mlocateC(3, i + 2);
		if (mfl->type[i + mfl->ptr]) draw_str("[");
		draw_str(mfl->name[i + mfl->ptr]);
		if (mfl->type[i + mfl->ptr]) draw_str("]");
	}

	set_mbcolor(0x0); // 透過モードに戻しておく

#if defined(PSP)
	psp_draw_menu();
#elif defined(USE_OGLES11)
	ogles11_draw_menu();
#else

#if SDL_VERSION_ATLEAST(2, 0, 0)
	SDL_UpdateWindowSurface(sdl_window);
#else
	SDL_UpdateRect(menu_surface, 0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
#endif

#endif
}

void WinDraw_ClearMenuBuffer(void)
{
#if defined(PSP)
	memset(menu_buffer, 0, 512*272*2);
#elif defined(USE_OGLES11)
	memset(menu_buffer, 0, 800*600*2);
#else
	SDL_FillRect(menu_surface, NULL, 0);
#endif

}

/********** ソフトウェアキーボード描画 **********/

#if defined(PSP) || defined(USE_OGLES11)

#if defined(PSP)
// display width 480, buffer width 512
#define KBDBUF_WIDTH 512
#elif defined(USE_OGLES11)
// display width 800, buffer width 1024 だけれど 800 にしないとだめ
#define KBDBUF_WIDTH 800
#endif

#define KBD_FS 16 // keyboard font size : 16

// キーを反転する
void WinDraw_reverse_key(int x, int y)
{
	WORD *p;
	int kp;
	int i, j;
	
	kp = Keyboard_get_key_ptr(kbd_kx, kbd_ky);

	p = kbd_buffer + KBDBUF_WIDTH * kbd_key[kp].y + kbd_key[kp].x;

	for (i = 0; i < kbd_key[kp].h; i++) {
		for (j = 0; j < kbd_key[kp].w; j++) {
			*p = ~(*p);
			p++;
		}
		p = p + KBDBUF_WIDTH - kbd_key[kp].w;
	}
}

static void draw_kbd_to_tex()
{
	int i, x, y;
	WORD *p;

	// SJIS 漢字コード
	char zen[] = {0x91, 0x53, 0x00};
	char larw[] = {0x81, 0xa9, 0x00};
	char rarw[] = {0x81, 0xa8, 0x00};
	char uarw[] = {0x81, 0xaa, 0x00};
	char darw[] = {0x81, 0xab, 0x00};
	char ka[] = {0x82, 0xa9, 0x00};
	char ro[] = {0x83, 0x8d, 0x00};
	char ko[] = {0x83, 0x52, 0x00};
	char ki[] = {0x8b, 0x4c, 0x00};
	char to[] = {0x93, 0x6f, 0x00};
	char hi[] = {0x82, 0xd0, 0x00};

	kbd_key[12].s = ka;
	kbd_key[13].s = ro;
	kbd_key[14].s = ko;
	kbd_key[16].s = ki;
	kbd_key[17].s = to;
	kbd_key[76].s = uarw;
	kbd_key[94].s = larw;
	kbd_key[95].s = darw;
	kbd_key[96].s = rarw;
	kbd_key[101].s = hi;
	kbd_key[108].s = zen;
#ifdef PSP
	kbd_key[0].s = "BK";
	kbd_key[1].s = "CP";
	kbd_key[15].s = "CA";
	kbd_key[18].s = "HP";
	kbd_key[19].s = "EC";
	kbd_key[34].s = "HM";
	kbd_key[35].s = "IN";
	kbd_key[36].s = "DL";
	kbd_key[37].s = "CL";
	kbd_key[55].s = "";
	kbd_key[56].s = "RU";
	kbd_key[57].s = "RD";
	kbd_key[58].s = "UD";
	kbd_key[63].s = "CTR";
	kbd_key[81].s = "";
	kbd_key[93].s = "";
	kbd_key[100].s = "";
	kbd_key[102].s = "X1";
	kbd_key[103].s = "X2";
	kbd_key[105].s = "X3";
	kbd_key[106].s = "X4";
	kbd_key[107].s = "X5";
	kbd_key[109].s = "OP1";
	kbd_key[110].s = "OP2";
#endif

	set_sbp(kbd_buffer);
	set_mfs(KBD_FS);
	set_mbcolor(0);
	set_mcolor(0);

	// キーボードの背景
	p = kbd_buffer;
	for (y = 0; y < kbd_h; y++) {
		for (x = 0; x < kbd_w; x++) {
			*p++ = (0x7800 | 0x03e0 | 0x000f);
		}
		p = p + KBDBUF_WIDTH - kbd_w;
	}

	// キーの描画
	for (i = 0; kbd_key[i].x != -1; i++) {
		p = kbd_buffer + kbd_key[i].y * KBDBUF_WIDTH + kbd_key[i].x;
		for (y = 0; y < kbd_key[i].h; y++) {
			for (x = 0; x < kbd_key[i].w; x++) {
				if (x == (kbd_key[i].w - 1)
				    || y == (kbd_key[i].h - 1)) {
					// キーに影をつけ立体的に見せる
					*p++ = 0x0000;
				} else {
					*p++ = 0xffff;
				}
			}
			p = p + KBDBUF_WIDTH - kbd_key[i].w;
		}
		if (strlen(kbd_key[i].s) == 3 && *(kbd_key[i].s) == 'F') {
			// FUNCキー刻印描画
			set_mlocate(kbd_key[i].x + kbd_key[i].w / 2
				    - strlen(kbd_key[i].s) * (8 / 2)
				    + (strlen(kbd_key[i].s) - 1) * 3 / 2,
				    kbd_key[i].y + kbd_key[i].h / 2 - 8 / 2);
			set_mfs(8);
			draw_str(kbd_key[i].s);
			set_mfs(KBD_FS);
		} else {
			// 刻印は上下左右ともセンタリングする
			set_mlocate(kbd_key[i].x + kbd_key[i].w / 2
				    - strlen(kbd_key[i].s) * (KBD_FS / 2 / 2),
				    kbd_key[i].y
				    + kbd_key[i].h / 2 - KBD_FS / 2);
			draw_str(kbd_key[i].s);
		}
	}

	WinDraw_reverse_key(kbd_kx, kbd_ky);
}

#endif
