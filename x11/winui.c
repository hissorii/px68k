/*	$Id: winui.c,v 1.8 2008/11/11 02:46:30 nonaka Exp $	*/

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

/* -------------------------------------------------------------------------- *
 *  WINUI.C - UI関連                                                          *
 * -------------------------------------------------------------------------- */

#include <sys/stat.h>
#include <errno.h>

#include "common.h"
#include "about.h"
#include "keyboard.h"
#include "windraw.h"
#include "dswin.h"
#include "fileio.h"
#include "prop.h"
#include "status.h"
#include "joystick.h"
#include "mouse.h"
#include "winx68k.h"
#include "version.h"
#include "juliet.h"
#include "fdd.h"
#include "irqh.h"
#include "../m68000/m68000.h"
#include "crtc.h"
#include "mfp.h"
#include "fdc.h"
#include "disk_d88.h"
#include "dmac.h"
#include "ioc.h"
#include "rtc.h"
#include "sasi.h"
#include "bg.h"
#include "palette.h"
#include "crtc.h"
#include "pia.h"
#include "scc.h"
#include "midi.h"
#include "adpcm.h"
#include "mercury.h"
#include "tvram.h"

#include "fmg_wrap.h"

extern	BYTE		fdctrace;
extern	BYTE		traceflag;
extern	WORD		FrameCount;
extern	DWORD		TimerICount;
extern	unsigned int	hTimerID;
	DWORD		timertick=0;
extern	int		FullScreenFlag;
	int		UI_MouseFlag = 0;
	int		UI_MouseX = -1, UI_MouseY = -1;
extern	int		NoWaitMode;
extern	short		timertrace;

	BYTE		MenuClearFlag = 0;

	BYTE		Debug_Text=1, Debug_Grp=1, Debug_Sp=1;
	BYTE		FrameRate = 2;

	char		filepath[MAX_PATH] = ".";
	int		fddblink = 0;
	int		fddblinkcount = 0;
	int		hddtrace = 0;
extern  int		dmatrace;

	DWORD		LastClock[4] = {0, 0, 0, 0};

#if 0
#define	EVENT_MASK \
			(GDK_BUTTON1_MOTION_MASK	\
			 | GDK_BUTTON2_MOTION_MASK	\
			 | GDK_BUTTON3_MOTION_MASK	\
			 | GDK_POINTER_MOTION_MASK	\
			 | GDK_POINTER_MOTION_HINT_MASK	\
			 | GDK_KEY_PRESS_MASK		\
			 | GDK_KEY_RELEASE_MASK		\
			 | GDK_BUTTON_PRESS_MASK	\
			 | GDK_BUTTON_RELEASE_MASK	\
			 | GDK_EXPOSURE_MASK)


static gint key_press(GtkWidget *w, GdkEventKey *ev);
static gint key_release(GtkWidget *w, GdkEventKey *ev);
static gint button_press(GtkWidget *w, GdkEventButton *ev);
static gint button_release(GtkWidget *w, GdkEventButton *ev);
static gint motion_notify(GtkWidget *w, GdkEventMotion *ev);
static gint expose(GtkWidget *w, GdkEventExpose *ev);

static void xmenu_toggle_item(char *name, int onoff, int emitp);

static gint wm_timer(gpointer data);

static void file_selection(int type, char *title, char *defstr, void *arg);

typedef struct {
	GtkWidget *fs;
	int type;
	void *arg;
} file_selection_t;
#endif

/******************************************************************************
 * init
 ******************************************************************************/
void
WinUI_Init(void)
{
#if 0
	/*
	 * ハンドラ設定
	 */
	gtk_signal_connect(GTK_OBJECT(window), "destroy", 
	    GTK_SIGNAL_FUNC(gtk_main_quit), (void *)"WM destroy");
	gtk_signal_connect(GTK_OBJECT(window), "key_press_event",
	    GTK_SIGNAL_FUNC(key_press), NULL);
	gtk_signal_connect(GTK_OBJECT(window), "key_release_event",
	    GTK_SIGNAL_FUNC(key_release), NULL);
	gtk_signal_connect(GTK_OBJECT(window), "button_press_event",
	    GTK_SIGNAL_FUNC(button_press), NULL);
	gtk_signal_connect(GTK_OBJECT(window), "button_release_event",
	    GTK_SIGNAL_FUNC(button_release), NULL);
	gtk_signal_connect(GTK_OBJECT(window), "motion_notify_event",
	    GTK_SIGNAL_FUNC(motion_notify), NULL);

	gtk_signal_connect(GTK_OBJECT(drawarea), "expose_event",
	    GTK_SIGNAL_FUNC(expose), NULL);

	/*
	 * イベント設定
	 */
	gtk_widget_add_events(GTK_WIDGET(window), EVENT_MASK);

	/*
	 * タイトルバー変更用タイマ
	 */
	hTimerID = gtk_timeout_add(500, wm_timer, (gpointer)window);
#endif
}

#if 0
/******************************************************************************
 * イベントハンドラ
 ******************************************************************************/

#include <gdk/gdkkeysyms.h>

/*
 - Signal: gint GtkWidget::expose_event (GtkWidget *WIDGET,
          GdkEventExpose *EVENT)
*/
static gint
expose(GtkWidget *w, GdkEventExpose *ev)
{

	UNUSED(w);

	if (ev->type == GDK_EXPOSE) {
		if (ev->count == 0) {
			WinDraw_Redraw();
			WinDraw_Draw();
		}
		return TRUE;
	}
	return FALSE;
}

/*
 - Signal: gint GtkWidget::key_press_event (GtkWidget *WIDGET,
          GdkEventKey *EVENT)
*/
static gint
key_press(GtkWidget *w, GdkEventKey *ev)
{

	UNUSED(w);

	if (ev->type == GDK_KEY_PRESS) {
		if (ev->keyval != GDK_F12)
			Keyboard_KeyDown(ev->keyval);
		return TRUE;
	}
	return FALSE;
}

/*
 - Signal: gint GtkWidget::key_release_event (GtkWidget *WIDGET,
          GdkEventKey *EVENT)
*/
static gint
key_release(GtkWidget *w, GdkEventKey *ev)
{

	UNUSED(w);

	if (ev->type == GDK_KEY_RELEASE) {
		if (ev->keyval == GDK_F12) {
			xmenu_toggle_item("mouse", UI_MouseFlag ^ 1, 1);
		} else
			Keyboard_KeyUp(ev->keyval);
		return TRUE;
	}
	return FALSE;
}

/*
 - Signal: gint GtkWidget::button_press_event (GtkWidget *WIDGET,
          GdkEventButton *EVENT)
*/
static gint
button_press(GtkWidget *w, GdkEventButton *ev)
{

	UNUSED(w);

	if (ev->type == GDK_BUTTON_PRESS) {
		switch (ev->button) {
		case 1:
			Mouse_Event(1, TRUE);
			break;

		case 2:
			xmenu_toggle_item("mouse", UI_MouseFlag ^ 1, 1);
			break;

		case 3:
			Mouse_Event(2, TRUE);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

/*
 - Signal: gint GtkWidget::button_release_event (GtkWidget *WIDGET,
          GdkEventButton *EVENT)
*/
static gint
button_release(GtkWidget *w, GdkEventButton *ev)
{

	UNUSED(w);

	if (ev->type == GDK_BUTTON_RELEASE) {
		switch (ev->button) {
		case 1:
			Mouse_Event(1, FALSE);
			break;

		case 2:
			break;

		case 3:
			Mouse_Event(2, FALSE);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

/*
- Signal: gint GtkWidget::motion_notify_event (GtkWidget *WIDGET,
         GdkEventMotion *EVENT)
*/
static gint
motion_notify(GtkWidget *w, GdkEventMotion *ev)
{
	GdkModifierType state;
	int x, y;

	UNUSED(w);

	if (ev->type == GDK_MOTION_NOTIFY) {
		if (ev->is_hint) {
			gdk_window_get_pointer (ev->window, &x, &y, &state);
		} else {
			x = ev->x;
			y = ev->y;
			state = ev->state;
		}
		Mouse_Event(0, (x << 16) | y);
		return TRUE;
	}
	return FALSE;
}

/*-
 * タイマ
 */
gint
wm_timer(gpointer data)
{
	DWORD timernowtick = timeGetTime();
	unsigned int freq = TimerICount / (timernowtick - timertick);
	int fddflag = 0;

	timertick = timernowtick;
	LastClock[3] = LastClock[2];
	LastClock[2] = LastClock[1];
	LastClock[1] = LastClock[0];
	LastClock[0] = freq;
	TimerICount = 0;


	if (fddblink) {
		char buf[256];
#ifdef WIN68DEBUG
		g_snprintf(buf, sizeof(buf),
		    "%s - %2d fps / %2d.%03d MHz  PC:%08X",
		    PrgTitle, FrameCount, (freq/1000), (freq%1000), C68k_Get_Reg(&C68K, C68K_PC));
#else
		if ( juliet_YM2151IsEnable() ) {
			g_snprintf(buf, sizeof(buf),
			    "%s v%s w/ ROMEO - %2d fps / %2d.%03d MHz",
			    PrgTitle, APP_VER_STRING,
			    FrameCount, (freq/1000), (freq%1000));
		} else {
			g_snprintf(buf, sizeof(buf),
			    "%s v%s - %2d fps / %2d.%03d MHz",
			    PrgTitle, APP_VER_STRING,
			    FrameCount, (freq/1000), (freq%1000));
		}
#endif
		FrameCount = 0;
		gtk_window_set_title(GTK_WINDOW(data), buf);
	}
	fddblink ^= 1;

	StatBar_UpdateTimer();

	if (fddflag) {
		if (fddblink) {
			fddblinkcount++;
		}
	} else {
		fddblinkcount = 0;
	}

	return TRUE;
}

/******************************************************************************
 * メニュー
 ******************************************************************************/

static void reset(gpointer, guint, GtkWidget *);
static void nmi_reset(gpointer, guint, GtkWidget *);
static void exit_from_menu(gpointer, guint, GtkWidget *);
static void framerate(gpointer, guint, GtkWidget *);
static void stretch(gpointer, guint, GtkWidget *);
static void xvimode(gpointer, guint, GtkWidget *);
static void toggle(gpointer, guint, GtkWidget *);
static void videoreg_save(gpointer, guint, GtkWidget *);
static void fdd_open(gpointer, guint, GtkWidget *);
static void fdd_eject(gpointer, guint, GtkWidget *);

#define	f(f)	((GtkItemFactoryCallback)(f))
#define	s(s)	((guint)(s))
static GtkItemFactoryEntry menu_items[] = {
{ "/_System", 			NULL, NULL, 0, "<Branch>" },
{ "/System/_Reset", 		NULL, f(reset), 0, NULL },
{ "/System/_NMI Reset",		NULL, f(nmi_reset), 0, NULL },
{ "/System/sep",		NULL, NULL, 0, "<Separator>" },
{ "/System/_Quit",		NULL, f(exit_from_menu), 0, NULL },
{ "/FDD_0",			NULL, NULL, 0, "<Branch>" },
{ "/FDD0/_Open",		NULL, f(fdd_open), 0, NULL },
{ "/FDD0/_Eject",		NULL, f(fdd_eject), 0, NULL },
{ "/FDD_1",			NULL, NULL, 0, "<Branch>" },
{ "/FDD1/_Open",		NULL, f(fdd_open), 1, NULL },
{ "/FDD1/_Eject",		NULL, f(fdd_eject), 1, NULL },
{ "/_Display",			NULL, NULL, 0, "<Branch>" },
{ "/Display/_Full Screen",	NULL, f(toggle),s("fullscreen"),"<ToggleItem>"},
{ "/Display/sep0",		NULL, NULL, 0, "<Separator>" },
{ "/Display/Auto Frame Skip",	NULL, f(framerate), 7, "<RadioItem>" },
{ "/Display/_60 fps",		NULL,f(framerate),1,"/Display/Auto Frame Skip"},
{ "/Display/_30 fps",		NULL, f(framerate), 2, "/Display/60 fps" },
{ "/Display/_20 fps",		NULL, f(framerate), 3, "/Display/30 fps" },
{ "/Display/1_5 fps",		NULL, f(framerate), 4, "/Display/20 fps" },
{ "/Display/_12 fps",		NULL, f(framerate), 5, "/Display/15 fps" },
{ "/Display/1_0 fps",		NULL, f(framerate), 6, "/Display/12 fps" },
{ "/Display/_7.5 fps",		NULL, f(framerate), 8, "/Display/10 fps" },
{ "/Display/sep1",		NULL, NULL, 0, "<Separator>" },
{ "/Display/Fixed Stretch",	NULL, f(stretch), 1, "<RadioItem>" },
{ "/Display/Auto Stretch",	NULL, f(stretch), 2, "/Display/Fixed Stretch" },
{ "/Display/Auto with X68 Size", NULL, f(stretch), 3, "/Display/Auto Stretch" },
{ "/Display/No Stretch",       NULL,f(stretch),0,"/Display/Auto with X68 Size"},
{ "/_Option",			NULL, NULL, 0, "<Branch>" },
{ "/Option/Mouse Mode [F12]",	NULL, f(toggle), s("mouse"), "<ToggleItem>" },
{ "/Option/sep0",		NULL, NULL, 0, "<Separator>" },
{ "/Option/_JoyKey Mode",	NULL, f(toggle), s("joykey"), "<ToggleItem>" },
{"/Option/Reverse JoyKey _Button",NULL,f(toggle),s("joykeyrev"),"<ToggleItem>"},
{"/Option/_Use JoyKey as Joy2",	NULL, f(toggle),s("joykeyjoy2"),"<ToggleItem>"},
{ "/Option/sep1",		NULL, NULL, 0, "<Separator>" },
{ "/Option/10MHz (Normal)",	NULL, f(xvimode), 0, "<RadioItem>" },
{ "/Option/16MHz (XVI)",	NULL, f(xvimode), 1, "/Option/10MHz (Normal)" },
{ "/Option/24MHz (RED ZONE)",	NULL, f(xvimode), 2, "/Option/16MHz (XVI)" },
{ "/Option/sep2",		NULL, NULL, 0, "<Separator>" },
{ "/Option/No-_Wait Mode",	NULL, f(toggle), s("nowait"), "<ToggleItem>" },
{ "/Option/sep3",		NULL, NULL, 0, "<Separator>" },
{ "/Option/Config",		NULL, f(PropPage_Init), 0, NULL },
{ "/_Help",			NULL, NULL, 0, "<Branch>" },
{ "/Help/About",		NULL, f(create_about_dialog), 0, NULL },
{ "/Debug",			NULL, NULL, 0, "<Branch>" },
{ "/Debug/Trace",		NULL, f(toggle), s("db_trace"),"<ToggleItem>"},
{ "/Debug/sep0",		NULL, NULL, 0, "<Separator>" },
{ "/Debug/HDD Trace",		NULL,f(toggle),s("db_hddtrace"),"<ToggleItem>"},
{ "/Debug/DMA Trace",		NULL,f(toggle),s("db_dmatrace"),"<ToggleItem>"},
{ "/Debug/MFP Timer Trace",	NULL,f(toggle),s("db_mfptrace"),"<ToggleItem>"},
{ "/Debug/sep1",		NULL, NULL, 0, "<Separator>" },
{ "/Debug/Text On|Off",		NULL, f(toggle), s("db_text"), "<ToggleItem>"},
{ "/Debug/Grp On|Off",		NULL, f(toggle), s("db_grp"), "<ToggleItem>" },
{ "/Debug/SP|BG On|Off",	NULL, f(toggle), s("db_spgb"), "<ToggleItem>"},
{ "/Debug/VideoReg Save",	NULL, f(videoreg_save), 0, NULL },
};
#undef	s
#undef	f

typedef struct toggle_tag {
	int cookie;
	char *name;
	char *menu_item;
	int stat;
} toggle_t;

static toggle_t toggle_items[] = {
	{  0, "fullscreen",	"/Display/Full Screen",			0 },
	{  1, "mouse",		"/Option/Mouse Mode [F12]",		0 },
	{  2, "joykey",		"/Option/JoyKey Mode",			0 },
	{  3, "joykeyrev",	"/Option/Reverse JoyKey Button",	0 },
	{  4, "joykeyjoy2",	"/Option/Use JoyKey as Joy2",		0 },
	{  5, "nowait",		"/Option/No-Wait Mode",			0 },
	{  6, "db_trace",	"/Debug/Trace",				0 },
	{  7, "db_hddtrace",	"/Debug/HDD Trace",			0 },
	{  8, "db_dmatrace",	"/Debug/DMA Trace",			0 },
	{  9, "db_mfptrace",	"/Debug/MFP Timer Trace",		0 },
	{ 10, "db_text",	"/Debug/Text On|Off",			0 },
	{ 11, "db_grp",		"/Debug/Grp On|Off",			0 },
	{ 12, "db_spbg",	"/Debug/SP|BG On|Off",			0 },
};

static GtkItemFactory *item_factory;
static int menu_inited = 0;

static void disable_unused_items(void);
static void enable_item(char *name);
static void disable_item(char *name);
static void select_item(char *name);
static void xmenu_select_framerate(int kind);
static void xmenu_select_stretch(int kind);
//static void xmenu_select_xvimode(int kind);
#endif

#if 0
GtkWidget *
create_menu(GtkWidget *w)
{
	GtkAccelGroup *accel_group;

	accel_group = gtk_accel_group_new();
	item_factory = gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<main>", accel_group);
	gtk_item_factory_create_items(item_factory, NELEMENTS(menu_items), menu_items, NULL);
	gtk_window_add_accel_group(GTK_WINDOW(w), accel_group);

	xmenu_toggle_item("fullscreen", FullScreenFlag, 1);
	xmenu_toggle_item("mouse", MouseSW, 1);
	xmenu_toggle_item("joykey", Config.JoyKey, 1);
	xmenu_toggle_item("joykeyrev", Config.JoyKeyReverse, 1);
	xmenu_toggle_item("joykeyjoy2", Config.JoyKeyJoy2, 1);
	xmenu_toggle_item("nowait", NoWaitMode, 1);
#ifdef	DEBUG
	xmenu_toggle_item("db_trace", traceflag, 1);
	xmenu_toggle_item("db_hddtrace", hddtrace, 1);
	xmenu_toggle_item("db_dmatrace", dmatrace, 1);
	xmenu_toggle_item("db_mfptrace", timertrace, 1);
	xmenu_toggle_item("db_text", Debug_Text, 1);
	xmenu_toggle_item("db_grp", Debug_Grp, 1);
	xmenu_toggle_item("db_spbg", Debug_Sp, 1);
#endif

	xmenu_select_framerate(FrameRate);
	xmenu_select_stretch(Config.WinStrech);

	disable_unused_items();

	menu_inited = 1;

	return gtk_item_factory_get_widget(item_factory, "<main>");
}
#endif

#if 0
static void
disable_unused_items(void)
{
	static char *items[] = {
		"/Display/Full Screen",
#ifndef	DEBUG
		"/Debug/Trace",
		"/Debug/HDD Trace",
		"/Debug/DMA Trace",
		"/Debug/MFP Timer Trace",
		"/Debug/Text On|Off",
		"/Debug/Grp On|Off",
		"/Debug/SP|BG On|Off",
		"/Debug/VideoReg Save",
#endif
		NULL
	};
	int i;

	for (i = 0; i < NELEMENTS(items); i++) {
		if (items[i])
			disable_item(items[i]);
	}

	if (!Config.JoyKey) {
		disable_item("/Option/Reverse JoyKey Button");
		disable_item("/Option/Use JoyKey as Joy2");
	}
}

static void
enable_item(char *name)
{
	GtkWidget *menu_item;

	menu_item = gtk_item_factory_get_widget(item_factory, name);
	gtk_widget_set_sensitive(menu_item, TRUE);
}

static void
disable_item(char *name)
{
	GtkWidget *menu_item;

	menu_item = gtk_item_factory_get_widget(item_factory, name);
	gtk_widget_set_sensitive(menu_item, FALSE);
}

static void
select_item(char *name)
{
	GtkWidget *menu_item;

	menu_item = gtk_item_factory_get_widget(item_factory, name);
	gtk_signal_emit_by_name(GTK_OBJECT(menu_item), "activate-item");
}

void
xmenu_toggle_item(char *name, int onoff, int emitp)
{
	int i;

	for (i = 0; i < NELEMENTS(toggle_items); ++i) {
		if (strcmp(toggle_items[i].name, name) == 0)
			break;
	}
	if (i == NELEMENTS(toggle_items))
		return;

	if (onoff != toggle_items[i].stat) {
		toggle_items[i].stat = onoff;
		if (emitp)
			select_item(toggle_items[i].menu_item);
	}
}

static void
xmenu_select_framerate(int kind)
{
	static char *name[] = {
		NULL,				/* 0 */
		"/Display/60 fps",		/* 1 */
		"/Display/30 fps",		/* 2 */
		"/Display/20 fps",		/* 3 */
		"/Display/15 fps",		/* 4 */
		"/Display/12 fps",		/* 5 */
		"/Display/10 fps",		/* 6 */
		"/Display/Auto Frame Skip",	/* 7 */
		"/Display/7.5 fps",		/* 8 */
	};

	if (kind > 0 && kind < NELEMENTS(name)) {
		if (name[kind])
			select_item(name[kind]);
	}
}

static void
xmenu_select_stretch(int kind)
{
	static char *name[] = {
		"/Display/No Stretch",
		"/Display/Fixed Stretch",
		"/Display/Auto Stretch",
		"/Display/Auto with X68 Size",
	};

	if (kind >= 0 && kind < NELEMENTS(name)) {
		if (name[kind])
			select_item(name[kind]);
	}
}

#if 0
static void
xmenu_select_xvimode(int kind)
{
	static char *name[] = {
		"/Option/10MHz (Normal)",
		"/Option/16MHz (XVI)",
		"/Option/24MHz (RED ZONE)",
	};

	if (kind >= 0 && kind < NELEMENTS(name)) {
		if (name[kind])
			select_item(name[kind]);
	}
}
#endif

/*
 * item function
 */
static void
exit_from_menu(gpointer data, guint action, GtkWidget *w)
{

	UNUSED(data);
	UNUSED(action);
	UNUSED(w);

	gtk_main_quit();
}

static void
reset(gpointer data, guint action, GtkWidget *w)
{

	UNUSED(data);
	UNUSED(action);
	UNUSED(w);

	WinX68k_Reset();
	if (Config.MIDI_SW && Config.MIDI_Reset)
		MIDI_Reset();
}

static void
nmi_reset(gpointer data, guint action, GtkWidget *w)
{

	UNUSED(data);
	UNUSED(action);
	UNUSED(w);

	IRQH_Int(7, NULL);
}

static void
framerate(gpointer data, guint action, GtkWidget *w)
{
	UNUSED(data);
	UNUSED(w);

	if (FrameRate != action)
		FrameRate = action;
}

static void
stretch(gpointer data, guint action, GtkWidget *w)
{
	UNUSED(data);
	UNUSED(w);

	if (Config.WinStrech != (int)action)
		Config.WinStrech = action;
}

static void
xvimode(gpointer data, guint action, GtkWidget *w)
{
	UNUSED(data);
	UNUSED(w);

	if (Config.XVIMode != (int)action)
		Config.XVIMode = action;
}

static void
videoreg_save(gpointer data, guint action, GtkWidget *w)
{
	char buf[256];

	UNUSED(data);
	UNUSED(action);
	UNUSED(w);

	DSound_Stop();
	g_snprintf(buf, sizeof(buf),
	             "VCReg 0:$%02X%02X 1:$%02x%02X 2:$%02X%02X  "
	             "CRTC00/02/05/06=%02X/%02X/%02X/%02X%02X  "
		     "BGHT/HD/VD=%02X/%02X/%02X   $%02X/$%02X",
	    VCReg0[0], VCReg0[1], VCReg1[0], VCReg1[1], VCReg2[0], VCReg2[1],
	    CRTC_Regs[0x01], CRTC_Regs[0x05], CRTC_Regs[0x0b], CRTC_Regs[0x0c],
	      CRTC_Regs[0x0d],
	    BG_Regs[0x0b], BG_Regs[0x0d], BG_Regs[0x0f],
	    CRTC_Regs[0x29], BG_Regs[0x11]);
	Error(buf);
	DSound_Play();
}

static void
fdd_open(gpointer data, guint action, GtkWidget *w)
{

	(void)w;
	(void)data;

	if (action < 2)
		file_selection(0, "Open FD image", filepath, (void *)action);
}

static void
fdd_eject(gpointer data, guint action, GtkWidget *w)
{

	(void)w;
	(void)data;

	FDD_EjectFD(action);
}

static void
toggle(gpointer data, guint action, GtkWidget *w)
{
	char *name = (char *)action;
	int i;

	UNUSED(data);
	UNUSED(w);

	if (!menu_inited)
		return;

	for (i = 0; i < NELEMENTS(toggle_items); ++i) {
		if (strcmp(toggle_items[i].name, name) == 0)
			break;
	}
	if (i == NELEMENTS(toggle_items))
		return;

	switch (toggle_items[i].cookie) {
		case 0: // fullscreen
#if 0
			xmenu_toggle_item("fullscreen", !FullScreenFlag, 0);
			ChangeFullScreen(!FullScreenFlag);
#endif
			break;

		case 1: // mouse
			UI_MouseFlag ^= 1;
			xmenu_toggle_item("mouse", UI_MouseFlag, 0);
			Mouse_StartCapture(UI_MouseFlag);
			break;

		case 2: // joykey
			Config.JoyKey ^= 1;
			if (Config.JoyKey) {
				enable_item("/Option/Reverse JoyKey Button");
				enable_item("/Option/Use JoyKey as Joy2");
			} else {
				disable_item("/Option/Reverse JoyKey Button");
				disable_item("/Option/Use JoyKey as Joy2");
			}
			xmenu_toggle_item("joykey", Config.JoyKey, 0);
			break;

		case 3: // joykeyrev
			Config.JoyKeyReverse ^= 1;
			xmenu_toggle_item("joykey_rev", Config.JoyKeyReverse,0);
			break;

		case 4: // joykeyjoy2
			Config.JoyKeyJoy2 ^= 1;
			xmenu_toggle_item("joykey_joy2", Config.JoyKeyJoy2, 0);
			break;

		case 5: // nowait
			NoWaitMode ^= 1;
			xmenu_toggle_item("nowait", NoWaitMode, 0);
			break;

		case 6: // db_trace
			traceflag ^= 1;
			xmenu_toggle_item("db_trace", traceflag, 0);
			break;

		case 7: // db_hddtrace
			hddtrace ^= 1;
			xmenu_toggle_item("db_hddtrace", hddtrace, 0);
			break;

		case 8: // db_dmatrace
			dmatrace ^= 1;
			xmenu_toggle_item("db_dmatrace", dmatrace, 0);
			break;

		case 9: // db_mfptrace
			timertrace ^= 1;
			xmenu_toggle_item("db_mfptrace", timertrace, 0);
			break;

		case 10: // db_text
			Debug_Text ^= 1;
			TVRAM_SetAllDirty();
			xmenu_toggle_item("db_text", Debug_Text, 0);
			break;

		case 11: // db_grp
			Debug_Grp ^= 1;
			TVRAM_SetAllDirty();
			xmenu_toggle_item("db_grp", Debug_Grp, 0);
			break;

		case 12: // db_spbg
			Debug_Sp ^= 1;
			TVRAM_SetAllDirty();
			xmenu_toggle_item("db_spbg", Debug_Sp, 0);
			break;
	}
}

/*
 * file selecter
 */
static int
FDType(const char *fname)
{
	const char *p;
	int len;

	len = strlen(fname);
	if (len > 4) {
		p = fname + len - 4;
		if (strcmp(p, ".D88") == 0 || strcmp(p, ".d88") == 0 ||
			strcmp(p, ".88D") == 0 || strcmp(p, ".88d") == 0) {
			return FD_D88;
		}
		if (strcmp(p, ".DIM") == 0 || strcmp(p, ".dim") == 0) {
			return FD_DIM;
		}
		return FD_XDF;
	}
	return FD_Non;
}

static void
file_selection_ok(GtkWidget *w, GtkFileSelection *gfs)
{
	file_selection_t *fsp = (file_selection_t *)gfs;
	GtkFileSelection *fs = (GtkFileSelection *)fsp->fs;
	char *p;

	(void)w;

	p = (char *)gtk_file_selection_get_filename(fs);
	if (p != NULL) {
		D88_HEADER d88;
		struct stat st;
		FILEH fp;
		int drive;
		int type;
		int ro = 0;

		if (stat(p, &st) == 0) {
			if (!S_ISDIR(st.st_mode)) {
				switch (fsp->type) {
				case 0:
					drive = (int)(long)fsp->arg;
					FDD_EjectFD(drive);
					strncpy(filepath, p, MAX_PATH);
					if ((st.st_mode & S_IWUSR) == 0)
						ro = 1;
					FDD_SetFD(drive, p, ro);
					break;
				}
			}
		} else  if (errno == ENOENT) {
			switch (fsp->type) {
			case 0:	// FDD
				type = FDType(p);
				if (type > 0) {
					int is_opened = 0;

					switch (type) {
					case FD_DIM:
						Error("DIM 形式のディスクは作成できません。\n");
						break;

					case FD_D88:
						fp = File_Create(p, FTYPE_D88);
						if (fp == NULL) {
							Error("ファイルの作成に失敗しました。\n");
						} else {
							bzero(&d88, sizeof(d88));
							d88.fd_size = sizeof(D88_HEADER);
							d88.fd_type = 0x20;
							File_Write(fp, &d88, sizeof(D88_HEADER));
							File_Close(fp);
							is_opened = 1;
						}
						break;

					case FD_XDF:
						fp = File_Create(p, FTYPE_NONE);
						if (fp == NULL) {
							Error("ファイルの作成に失敗しました。\n");
						} else {
							char t[0x100];
							int i;

							memset(t, 0xe5, sizeof(t));
							for (i = 0; i < 0x1340; ++i)
								File_Write(fp, t, sizeof(t));
							File_Close(fp);
							is_opened = 1;
						}
						break;
					}

					if (is_opened) {
						drive = (int)(long)fsp->arg;
						FDD_EjectFD(drive);
						strncpy(filepath, p, MAX_PATH);
						if ((st.st_mode & S_IWUSR) == 0)
							ro = 1;
						FDD_SetFD(drive, p, ro);
					}
				}
				break;
			}
		}
	}
	gtk_widget_destroy(GTK_WIDGET(fs));
}

static void
file_selection_destroy(GtkWidget *w, GtkWidget **wp)
{
	file_selection_t *fsp = (file_selection_t *)wp;

	free(fsp);
	gtk_widget_destroy(w);
}

static void
file_selection(int type, char *title, char *defstr, void *arg)
{
	GtkWidget *file_dialog;
	file_selection_t *fsp;

	fsp = malloc(sizeof(*fsp));
	if (fsp == NULL) {
		printf("file_selection: Can't alloc memory (size = %d)\n",
		    sizeof(*fsp));
		return;
	}
	bzero(fsp, sizeof(*fsp));

	file_dialog = gtk_file_selection_new(title);
	if (defstr != NULL) {
		gtk_file_selection_set_filename(
		    GTK_FILE_SELECTION(file_dialog), defstr);
	}

	fsp->fs = file_dialog;
	fsp->type = type;
	fsp->arg = arg;

	gtk_window_set_position(GTK_WINDOW(file_dialog), GTK_WIN_POS_CENTER);
	gtk_window_set_modal(GTK_WINDOW(file_dialog), TRUE);
	gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION(file_dialog));
	gtk_signal_connect(GTK_OBJECT(file_dialog), "destroy",
	    GTK_SIGNAL_FUNC(file_selection_destroy), fsp);
	gtk_signal_connect_object(
	    GTK_OBJECT(GTK_FILE_SELECTION(file_dialog)->cancel_button),
	    "clicked", GTK_SIGNAL_FUNC(gtk_widget_destroy),
	    GTK_OBJECT(file_dialog));
	gtk_signal_connect(
	    GTK_OBJECT(GTK_FILE_SELECTION(file_dialog)->ok_button),
	    "clicked", GTK_SIGNAL_FUNC(file_selection_ok), fsp);

	gtk_widget_show(file_dialog);
}
#endif
