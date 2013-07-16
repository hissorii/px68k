/*	$Id: about.c,v 1.4 2008/11/08 02:24:18 nonaka Exp $	*/

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

#include "windows.h"

#include "winx68k.h"
#include "about.h"
#include "version.h"

#include "../icons/adpcm.xpm"
#include "../icons/keropi_icon.xpm"
#include "../icons/motorola.xpm"
#include "../icons/nekolgc.xpm"
#include "../icons/opm.xpm"

static void
dialog_destroy(GtkWidget *w, GtkWidget **wp)
{

        UNUSED(wp);
	gtk_widget_destroy(w);
}

static GtkWidget *
create_pixmap_from_xpm_d(GtkWidget *w, char **xpm)
{
	GdkColormap *colormap;
	GdkPixmap *pixmap;
	GdkBitmap *mask;
	GtkWidget *newpixmap;

	colormap = gtk_widget_get_colormap(w);
	pixmap = gdk_pixmap_colormap_create_from_xpm_d(NULL, colormap,
	    &mask, NULL, xpm);
	newpixmap = gtk_pixmap_new(pixmap, mask);
	gdk_pixmap_unref(pixmap);
	gdk_bitmap_unref(mask);

	gtk_widget_ref(newpixmap);
	gtk_object_set_data_full(GTK_OBJECT(w), "pixmap",
	    newpixmap, (GtkDestroyNotify)gtk_widget_unref);

	return newpixmap;
}

void
create_about_dialog(void)
{
	GtkWidget *dialog;
	GtkWidget *table;
	GtkWidget *button;
	GtkWidget *frame;
	GtkWidget *frame_table;
	GtkWidget *label;
	GtkWidget *adpcm_pixmap;
	GtkWidget *keropi_pixmap;
	GtkWidget *motorola_pixmap;
	GtkWidget *nekolgc_pixmap;
	GtkWidget *opm_pixmap;

	dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(dialog), "About Keropi");
	gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
	gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
#if GTK_MAJOR_VERSION > 1
	gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
#endif
	gtk_signal_connect(GTK_OBJECT(dialog), "destroy",
	    GTK_SIGNAL_FUNC(dialog_destroy), NULL);

	table = gtk_table_new(6, 2, FALSE);
	gtk_container_add(GTK_CONTAINER(dialog), table);

	button = gtk_button_new_with_label("OK");
	gtk_table_attach(GTK_TABLE(table), button, 4, 5, 1, 2,
	    GTK_FILL|GTK_EXPAND, 0, 0, 0);
	gtk_signal_connect_object(GTK_OBJECT(button), "clicked",
	    GTK_SIGNAL_FUNC(gtk_widget_destroy), GTK_OBJECT(dialog));
	GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
	GTK_WIDGET_SET_FLAGS(button, GTK_HAS_DEFAULT);
	gtk_widget_grab_default(button);
	gtk_widget_show(button);

	motorola_pixmap = create_pixmap_from_xpm_d(dialog, motorola);
	gtk_table_attach_defaults(GTK_TABLE(table),motorola_pixmap, 0, 1, 1, 2);
	gtk_widget_show(motorola_pixmap);

	adpcm_pixmap = create_pixmap_from_xpm_d(dialog, adpcm);
	gtk_table_attach_defaults(GTK_TABLE(table), adpcm_pixmap, 1, 2, 1, 2);
	gtk_widget_show(adpcm_pixmap);

	opm_pixmap = create_pixmap_from_xpm_d(dialog, opm);
	gtk_table_attach_defaults(GTK_TABLE(table), opm_pixmap, 2, 3, 1, 2);
	gtk_widget_show(opm_pixmap);

	nekolgc_pixmap = create_pixmap_from_xpm_d(dialog, nekolgc);
	gtk_table_attach_defaults(GTK_TABLE(table), nekolgc_pixmap, 3, 4, 1, 2);
	gtk_widget_show(nekolgc_pixmap);

	frame = gtk_frame_new(NULL);
	gtk_container_set_border_width(GTK_CONTAINER(frame), 5);
	gtk_table_attach_defaults(GTK_TABLE(table), frame, 0, 6, 0, 1);
	gtk_widget_show(frame);

	frame_table = gtk_table_new(2, 1, FALSE);
	gtk_container_add(GTK_CONTAINER(frame), frame_table);
	gtk_widget_show(frame_table);

	keropi_pixmap = create_pixmap_from_xpm_d(dialog, keropi_icon);
	gtk_misc_set_padding(GTK_MISC(keropi_pixmap), 5, 0);
	gtk_table_attach_defaults(GTK_TABLE(frame_table),
	    keropi_pixmap, 0, 1, 0, 1);
	gtk_widget_show(keropi_pixmap);

	label = gtk_label_new("Keropi for X - " SOURCE_VERSION "\n"
			      "Modified by NONAKA Kimihiro\n"
	                      "\nKeropi (WinX68k)\n"
	                      "SHARP X680x0 series emulator\n"
	                      "Version " APP_VER_STRING "\n"
			      "Copyright (C) 2000-02 Kenjo\n"
			      "\nUsing \"FM Sound Generator\" (C) cisc");
	gtk_misc_set_padding(GTK_MISC(label), 5, 0);
	gtk_table_attach_defaults(GTK_TABLE(frame_table), label, 1, 2, 0, 1);
	gtk_widget_show(label);

	gtk_widget_show(table);
	gtk_widget_show(dialog);
}
