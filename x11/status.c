/*	$Id: status.c,v 1.3 2008/11/08 01:42:42 nonaka Exp $	*/

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

#if 0
/* -------------------------------------------------------------------------- *
 *   FDD状態表示用ステータスバー                                              *
 * -------------------------------------------------------------------------- */

#include "common.h"
#include "windraw.h"
#include "winx68k.h"
#include "prop.h"
#include "status.h"
#include "fileio.h"
#include "fdd.h"

#if 0
#include "../icons/fd_inserted.xpm"
#include "../icons/fd_inuse.xpm"
#include "../icons/fd_normal.xpm"
#include "../icons/fd_protected.xpm"
#include "../icons/hd_inserted.xpm"
#include "../icons/hd_inuse.xpm"
#include "../icons/hd_normal.xpm"
#include "../icons/nulldev.xpm"

static char **pixmaps[] = {
	fd_normal,
	fd_inuse,
	fd_inserted,
	fd_protected,
	hd_normal,
	hd_inuse,
	hd_inserted,
	nulldev,
};
#endif

//static GtkWidget *statbar;
//static GtkWidget *devstat[2 /* FDD */ + 1 /* SASI */];
//static GtkWidget *devlabel[2 /* FDD */ + 1 /* SASI */];
//static GdkPixmap *devstat_pixmap[2 /* FDD */ + 1 /* SASI */];
//static GdkPixmap *dev_pixmap[NELEMENTS(pixmaps)];

typedef struct {
	int insert;
	int access;
	int blink;
	int timer;
	char file[MAX_PATH];
} FDDLED;

static FDDLED FddLed[2];
//static int HddLed = 0, HddLedTmp = 0;

#if 0
/* ----- GTK signal handler */
static gint
expose(GtkWidget *w, GdkEventExpose *ev, gpointer d)
{

        if (ev->type == GDK_EXPOSE) {
		if (ev->count == 0) {
			gdk_draw_pixmap(w->window,
			    w->style->fg_gc[GTK_WIDGET_STATE(w)],
			    devstat_pixmap[(int)d], 0, 0, 0, 0, 16, 16);
		}
		return TRUE;
	}
	return FALSE;
}
/* ----- */
#endif

void
StatBar_Redraw(void)
{

	if (Config.FullScrFDDStat) {
		StatBar_FDD(0, 0, FddLed[0].access);
		StatBar_FDD(0, 1, FddLed[0].insert);
		StatBar_FDD(1, 0, FddLed[1].access);
		StatBar_FDD(1, 1, FddLed[1].insert);
		StatBar_FDName(0, FddLed[0].file);
		StatBar_FDName(1, FddLed[1].file);
	}
}


void
StatBar_Show(int sw)
{
#if 1
	(void)sw;
#else
	if (sw) {
		if (statbar == 0) {
			GdkVisual *visual;
			GdkColormap *colormap;
			GdkBitmap *mask;
			int pos = 0;
			int i;

			statbar = gtk_table_new(1, 18, FALSE);
			gtk_box_pack_start(GTK_BOX(main_vbox), statbar,
			    FALSE, TRUE, 0);

			colormap = gtk_widget_get_colormap(window);
			for (i = 0; i < NELEMENTS(dev_pixmap); ++i) {
				dev_pixmap[i] =
				    gdk_pixmap_colormap_create_from_xpm_d(NULL,
				    colormap, &mask, NULL, pixmaps[i]);
			}

			for (i = 0; i < NELEMENTS(devstat); ++i) {
				GtkWidget *w;
				int idx = 0;

				if (i == NELEMENTS(devstat) - 1) {
					w = gtk_vseparator_new();
					gtk_table_attach(GTK_TABLE(statbar), w,
					    pos, pos + 1, 0, 1, 0, 0, 0, 0);
					pos++;
					gtk_widget_show(w);
					idx += 4;
				}

				w = devstat[i] = gtk_drawing_area_new();
				gtk_drawing_area_size(GTK_DRAWING_AREA(w),
				    16, 16);
				gtk_table_attach(GTK_TABLE(statbar), w,
				    pos, pos + 1, 0, 1, 0, 0, 0, 0);
				pos++;
				gtk_signal_connect(GTK_OBJECT(w),"expose_event",
				    GTK_SIGNAL_FUNC(expose), (gpointer)i);
				gtk_widget_show(w);

				devlabel[i] = gtk_label_new(
				    (i == NELEMENTS(devstat) - 1) ?
				        "HDD"
					  :
					"                    "
					"                    "
					"                    "
					"                    ");
				gtk_table_attach_defaults(GTK_TABLE(statbar),
				    devlabel[i], pos, pos + 3, 0, 1);
				pos += 3;
				gtk_widget_show(devlabel[i]);

				visual = gtk_widget_get_visual(w);
				devstat_pixmap[i] = gdk_pixmap_new(w->window,
				    16, 16, visual->depth);
				gdk_draw_pixmap(devstat_pixmap[i],
				    w->style->fg_gc[GTK_WIDGET_STATE(w)],
				    dev_pixmap[idx], 0, 0, 0, 0, 16, 16);
			}

		}
		gtk_widget_show(statbar);
	} else {
		if (statbar)
			gtk_widget_hide(statbar);
	}
#endif
}


void
StatBar_UpdateTimer(void)
{
#if 0
	FddLed[0].timer ^= 1;
	FddLed[1].timer ^= 1;

	if (FddLed[0].blink && (!FddLed[0].file[0]) && statbar) {
		gdk_draw_pixmap(devstat_pixmap[0],
		    devstat[0]->style->fg_gc[GTK_WIDGET_STATE(devstat[0])],
		    dev_pixmap[FddLed[0].timer], 0, 0, 0, 0, 16, 16);
	}
	if (FddLed[1].blink && (!FddLed[1].file[0]) && statbar) {
		gdk_draw_pixmap(devstat_pixmap[1],
		    devstat[1]->style->fg_gc[GTK_WIDGET_STATE(devstat[1])],
		    dev_pixmap[FddLed[1].timer], 0, 0, 0, 0, 16, 16);
	}
	if (HddLed != HddLedTmp) {
		HddLed = HddLedTmp;
		if (statbar) {
			gdk_draw_pixmap(devstat_pixmap[2],
			    devstat[2]->style->fg_gc[
			      GTK_WIDGET_STATE(devstat[2])],
			    dev_pixmap[HddLed ? 5 : 4], 0, 0, 0, 0, 16, 16);
		}
	}
	StatBar_Redraw();
#endif
}


void
StatBar_HDD(int hd)
{
#if 1
	(void)hd;
#else
	HddLedTmp = hd;
	if ((HddLed != hd) && (hd)) {
		gdk_draw_pixmap(devstat_pixmap[2],
		    devstat[2]->style->fg_gc[GTK_WIDGET_STATE(devstat[2])],
		    dev_pixmap[HddLed ? 5 : 4], 0, 0, 0, 0, 16, 16);
		HddLed = hd;
	}
	StatBar_Redraw();
#endif
}


void
StatBar_SetFDD(int drv, char* file)
{
#if 1
	(void)drv;
	(void)file;
#else
	char name[81];
	char *f;
	int i;

	if ((drv < 0)||(drv > 1))
		return;

	f = (char *)getFileName(file);
	strcpy(FddLed[drv].file, f);
	g_snprintf(name, sizeof(name), "%s", f);

	if (f[0]) {
		FddLed[drv].access = 1;
		FddLed[drv].insert = 1;
	} else {
		FddLed[drv].access = 0;
		FddLed[drv].insert = 0;
	}

	if (statbar) {
		int len = strlen(name);
		int c = ((FddLed[drv].blink) && (!FddLed[drv].file[0])) ? FddLed[drv].timer : FddLed[drv].access;
		gdk_draw_pixmap(devstat_pixmap[drv],
		    devstat[drv]->style->fg_gc[
		      GTK_WIDGET_STATE(devstat[drv])],
		    dev_pixmap[c], 0, 0, 0, 0, 16, 16);

		memset(name + len, ' ', sizeof(name) - len);
		name[80] = '\0';
		gtk_label_set_text(devlabel[drv], name);
	}
	StatBar_Redraw();
#endif
}


void
StatBar_ParamFDD(int drv, int access, int insert, int blink)
{
#if 1
	(void)drv;
	(void)access;
	(void)insert;
	(void)blink;
#else
	int update;

	if ((drv < 0) || (drv > 1))
		return;

	update = 0;
	if (FddLed[drv].access != access) {
		FddLed[drv].access = access;
		update = 1;
	}
	if (FddLed[drv].insert != insert) {
		FddLed[drv].insert = insert;
		update = 1;
	}
	if (FddLed[drv].blink != blink) {
		FddLed[drv].blink = blink;
		update = 1;
	}

	if (update) {
		if (statbar) {
			int c = ((FddLed[drv].blink) && (!FddLed[drv].file[0])) ? FddLed[drv].timer : FddLed[drv].access;
			gdk_draw_pixmap(devstat_pixmap[drv],
			    devstat[drv]->style->fg_gc[
			      GTK_WIDGET_STATE(devstat[drv])],
			    dev_pixmap[c], 0, 0, 0, 0, 16, 16);
		}
		StatBar_Redraw();
	}
#endif
}


void
StatBar_FDName(int drv, char* name)
{
	/* Full screen 専用 */
	(void)drv;
	(void)name;
}


void
StatBar_FDD(int drv, int led, int col)
{
	/* Full screen 専用 */
	(void)drv;
	(void)led;
	(void)col;
}
#endif
