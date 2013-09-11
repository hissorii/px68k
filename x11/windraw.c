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

#if defined(ANDROID)
#include <GLES/gl.h>
#elif defined(PSP)
#include <pspdisplay.h>
#include <pspgu.h>
#endif
#include <SDL.h>
//#include <SDL_rotozoom.h>
#include "common.h"
#include "winx68k.h"
#include "winui.h"

#include "bg.h"
#include "crtc.h"
#include "gvram.h"
#include "joystick.h"
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

#ifdef PSP
WORD *ScrBufL = 0, *ScrBufR = 0;
#else
WORD *ScrBuf = 0;
#endif

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
GLuint background_tex = 0;
GLuint texid[VBTN_MAX] = {};
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
			// �������˶ᤤ���̤ʤ顢�Ȥ��Ƥ�����
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
0x0418c000+----------+---------+
          |���ۥ����ܡ�����    |
          |    �˻Ȥ������ΰ�  |
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

#define PSP_BUF_WIDTH (512)
#define PSP_SCR_WIDTH (480)
#define PSP_SCR_HEIGHT (272)

#endif // PSP

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

	glGenTextures(7, &background_tex);
	glBindTexture(GL_TEXTURE_2D, background_tex);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
//	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, ScrBuf);

	WORD BtnTex[vbtn_width*vbtn_height];
	int i;
	//�Ȥꤢ����������Фǡ�
	for (i = 0; i < vbtn_width*vbtn_height; i++) {
		BtnTex[i] = 0x03e0;
	}

	// �ܥ����ѥƥ������㡣�Ȥꤢ��������Ʊ������
	for (i = 0; i < VBTN_MAX; i++) {
		glBindTexture(GL_TEXTURE_2D, texid[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, vbtn_width, vbtn_height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, BtnTex);
	}
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
#else
	sdl_rgbsurface = SDL_CreateRGBSurface(SDL_SWSURFACE, 800, 600, 16, WinDraw_Pal16R, WinDraw_Pal16G, WinDraw_Pal16B, 0);

	if (sdl_rgbsurface == 0) {
		puts("ScrBuf allocate failed");
		exit(1);
	}
	ScrBuf = sdl_rgbsurface->pixels;

	printf("drawbuf: 0x%x, ScrBuf: 0x%x\n", sdl_surface->pixels, ScrBuf);
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
	int i;

	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glBindTexture(GL_TEXTURE_2D, background_tex);
	//ScrBuf����800x600���ΰ���ڤ�Ф��ƥƥ��������ž��
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 800, 600, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, ScrBuf);

	// magic number���䤿��¿�������ƥ�������Υ�������1024x1024
	// OpenGL�Ǥ������ΰ褬glOrthof()���������800x600

	// Texture ����ɬ�פ���ʬ��ȴ���Ф�
	// Texutre��ɸ��0.0f����1.0f�δ�
	SET_GLFLOATS(texture_coordinates,
		    0.0f, (GLfloat)TextDotY/1024,
		    0.0f, 0.0f,
		    (GLfloat)TextDotX/1024, (GLfloat)TextDotY/1024,
		    (GLfloat)TextDotX/1024, 0.0f);

	// �µ��β�����(realdisp_w x realdisp_h)�˴ط��ʤ���
	// ��ɸ��800x600
	w = (realdisp_h * 1.33333) / realdisp_w * 800;
	SET_GLFLOATS(vertices,
		     (800.0f - w)/2, (GLfloat)600,
		     (800.0f - w)/2, 0.0f,
		     (800.0f - w)/2 + w, (GLfloat)600,
		     (800.0f - w)/2 + w, 0.0f);
//�����ޤǲ�������
	draw_texture(texture_coordinates, vertices);

	// �ܥ���
	for (i = 0; i < VBTN_MAX; i++) {
		if (vbtn_state[i] == VBTN_NOUSE) continue;

		glBindTexture(GL_TEXTURE_2D, texid[i]);
		/* Texture ����ɬ�פ���ʬ��ȴ���Ф�(32x32�������Ȥ�) */
		SET_GLFLOATS(texture_coordinates, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f);
		SET_GLFLOATS(vertices,
			vbtn_rect[i].x, vbtn_rect[i].y2,
			vbtn_rect[i].x, vbtn_rect[i].y,
			vbtn_rect[i].x2, vbtn_rect[i].y2,
			vbtn_rect[i].x2, vbtn_rect[i].y);
		draw_texture(texture_coordinates, vertices);
	}

	//	glDeleteTextures(1, &texid);

	SDL_GL_SwapWindow(sdl_window);

#elif defined(PSP)
	sceGuStart(GU_DIRECT, list);

	sceGuClear(0);

	// ��Ⱦʬ
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
		// ��Ⱦʬ
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

        sceGuFinish();
        sceGuSync(0, 0);

        sceGuSwapBuffers();

#else // Unix�� (��SDL_gfx)
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

#ifdef PSP
#ifdef FULLSCREEN_WIDTH
#undef FULLSCREEN_WIDTH
#endif
//PSP�Υƥ�������ϰ��դ�����512�ʤΤǡ�X68000��768x512���̤�
//512x512��256x512�κ�����Ĥ�ʬ���롣
//�ʲ���DrawLine�Ϥδؿ�������PSP�Ϻ����Υƥ���������̡���
//�������ʤ���Фʤ�ʤ���
//�Ȥ������Ȥǡ��ʲ���define�Ϻ�¦�ƥ�������β�����ɽ���Ƥ��롣
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
				Grp_DrawLine4SP((VCReg1[1]   )&3/*, 1*/);			// ȾƩ���β�����
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
		if ( (VCReg1[1]&3) <= ((VCReg1[1]>>4)&3) )	// Ʊ���ͤλ��ϡ�GRP0��ͥ��ʥɥ饹�ԡ�
		{
			if ( (VCReg2[0]&0x10)&&(VCReg2[1]&1) )
			{
				Grp_DrawLine8SP(0);			// ȾƩ���β�����
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
				Grp_DrawLine8SP(1);			// ȾƩ���β�����
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
//		gdrawed = 1;				// Grp���BG��������

	if ( ((VCReg1[0]&0x30)>>2) < (VCReg1[0]&0x0c) )
	{						// BG��������
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
	{						// Text��������
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
			} else {		// 20010120 �����ῧ��
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
					// Pri = 3�ʰ�ȿ�ˤ����ꤵ��Ƥ�����̤�ɽ��
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
					// Pri = 2 or 3�ʺǲ��̡ˤ����ꤵ��Ƥ�����̤�ɽ��
					// �ץ饤����ƥ���Ʊ�����ϡ�GRP<SP<TEXT���ʥɥ饹�ԡ�������YsIII����

					// Grp���Text����ˤ������Text�Ȥ�ȾƩ����Ԥ��ȡ�SP�Υץ饤����ƥ���
					// Text�˰��������롩�ʤĤޤꡢGrp��겼�ˤ��äƤ�SP��ɽ������롩��

					// KnightArms�Ȥ��򸫤�ȡ�ȾƩ���Υ١����ץ졼��ϰ��־�ˤʤ�ߤ����ġ�

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

					// Pri = 1��2���ܡˤ����ꤵ��Ƥ�����̤�ɽ��
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

					// Pri = 0�ʺ�ͥ��ˤ����ꤵ��Ƥ�����̤�ɽ��
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

					// �ü�ץ饤����ƥ����Υ���ե��å�
		if ( ((VCReg2[0]&0x5c)==0x14)&&(pron) )	// �ü�Pri���ϡ��оݥץ졼��ӥåȤϰ�̣��̵���餷���ʤĤ���ӡ���
		{
			WinDraw_DrawPriLine();
		}
		else if ( ((VCReg2[0]&0x5d)==0x1c)&&(tron) )	// ȾƩ���������Ƥ�Ʃ���ʥɥåȤ�ϡ��ե��顼������
		{						// ��AQUALES��

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
 * �������������
 */
unsigned int
gcd(unsigned int v0, unsigned int v1)
{
#if 0
/*
 * �桼����åɤθ߽�ˡ��
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
 * Brent �β��ɷ����르�ꥺ��
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
