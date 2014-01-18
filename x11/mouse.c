/* 
 * Copyright (c) 2003,2008 NONAKA Kimihiro
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
#include "winx68k.h"
#include "prop.h"
#include "scc.h"
#include "crtc.h"
#include "mouse.h"

float	MouseDX = 0;
float	MouseDY = 0;
BYTE	MouseStat = 0;
BYTE	MouseSW = 0;

POINT	CursorPos;
int	mousex = 0, mousey = 0;

#if 0
static GdkPixmap *cursor_pixmap;
static GdkCursor *cursor;

static void getmaincenter(GtkWidget *w, POINT *p);
void gdk_window_set_pointer(GdkWindow *window, gint x, gint y);
#endif

void Mouse_Init(void)
{
	if (Config.JoyOrMouse) {
		Mouse_StartCapture(1);
	}
}


// ----------------------------------
//	Mouse Event Occured
// ----------------------------------
void Mouse_Event(int param, float dx, float dy)
{
	printf("ME(): %d %d\n", dx, dy);

	if (MouseSW) {
		switch (param) {
		case 0:	// mouse move
			MouseDX += dx;
			MouseDY += dy;
			break;
		case 1:	// left button
			if (dx != 0)
				MouseStat |= 1;
			else
				MouseStat &= 0xfe;
			break;
		case 2:	// right button
			if (dx != 0)
				MouseStat |= 2;
			else
				MouseStat &= 0xfd;
			break;
		default:
			break;
		}
	}
}


// ----------------------------------
//	Mouse Data send to SCC
// ----------------------------------
void Mouse_SetData(void)
{
	POINT pt;
	int x, y;

	if (MouseSW) {

		x = (int)MouseDX;
		y = (int)MouseDY;

		MouseDX = MouseDY = 0;

		MouseSt = MouseStat;

		if (x > 127) {
			MouseSt |= 0x10;
			MouseX = 127;
		} else if (x < -128) {
			MouseSt |= 0x20;
			MouseX = -128;
		} else {
			MouseX = (signed char)x;
		}

		if (y > 127) {
			MouseSt |= 0x40;
			MouseY = 127;
		} else if (y < -128) {
			MouseSt |= 0x80;
			MouseY = -128;
		} else {
			MouseY = (signed char)y;
		}

	} else {
		MouseSt = 0;
		MouseX = 0;
		MouseY = 0;
	}
}


// ----------------------------------
//	Start Capture
// ----------------------------------
void Mouse_StartCapture(int flag)
{
	if (flag && !MouseSW) {
		MouseSW = 1;
	} else 	if (!flag && MouseSW) {
		MouseSW = 0;
	}
}

void Mouse_ChangePos(void)
{
#if 0
	if (MouseSW) {
		POINT pt;

		getmaincenter(window, &pt);
		gdk_window_set_pointer(window->window, pt.x, pt.y);
	}
#endif
}

#if 0
static void
getmaincenter(GtkWidget *w, POINT *p)
{

	p->x = w->allocation.x + w->allocation.width / 2;
	p->y = w->allocation.y + w->allocation.height / 2;
}

#if GTK_MAJOR_VERSION == 1
#include <gdk/gdkprivate.h>

void
gdk_window_set_pointer(GdkWindow *window, gint x, gint y)
{ 
	GdkWindowPrivate *private;

	if (!window)
		window = (GdkWindow *)&gdk_root_parent;

	private = (GdkWindowPrivate *)window;
	if (private->destroyed)
		return;

	XWarpPointer(private->xdisplay, None, private->xwindow,
	    0, 0, 0, 0, x, y);
}
#else	/* GTK_MAJOR_VERSION != 1 */
#include <gdk/gdkx.h>

void
gdk_window_set_pointer(GdkWindow *window, gint x, gint y)
{
	GdkScreen *screen;

	if (window == NULL) {
		screen = gdk_screen_get_default();
		window = gdk_screen_get_root_window(screen);
	}
	if (GDK_WINDOW_DESTROYED(window))
		return;

	XWarpPointer(GDK_WINDOW_XDISPLAY(window), None, GDK_WINDOW_XID(window),
	    0, 0, 0, 0, x, y);
}
#endif	/* GTK_MAJOR_VERSION == 1 */
#endif
