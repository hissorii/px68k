/*	$Id: winui.h,v 1.1.1.1 2003/04/28 18:06:56 nonaka Exp $	*/

#ifndef _winx68k_winui_h
#define _winx68k_winui_h

extern	BYTE	Debug_Text, Debug_Grp, Debug_Sp;
extern	DWORD	LastClock[4];

void WinUI_Init(void);

#ifndef _winx68k_gtkui_h
#define _winx68k_gtkui_h

#if 0
#include <gdk/gdk.h>
#include <gtk/gtk.h>

GtkWidget *create_menu(GtkWidget *w);
#endif

#endif //winx68k_gtkui_h

#endif //winx68k_winui_h
