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

/* -------------------------------------------------------------------------- *
 *  PROP.C - 各種設定用プロパティシートと設定値管理                           *
 * -------------------------------------------------------------------------- */

#include <sys/stat.h>

#include "common.h"
#include "winx68k.h"
#include "keyboard.h"
#include "fileio.h"
#include "prop.h"

BYTE	LastCode = 0;
BYTE	KEYCONFFILE[] = "xkeyconf.dat";

int	CurrentHDDNo = 0;

BYTE ini_title[] = "WinX68k";

static const char MIDI_TYPE_NAME[4][3] = {
	"LA", "GM", "GS", "XG"
};

BYTE KeyTableBk[512];

Win68Conf Config;
Win68Conf ConfBk;

#ifndef MAX_BUTTON
#define MAX_BUTTON 32
#endif

extern char filepath[MAX_PATH];
extern char winx68k_ini[MAX_PATH];
extern int winx, winy;
extern char joyname[2][MAX_PATH];
extern char joybtnname[2][MAX_BUTTON][MAX_PATH];
extern BYTE joybtnnum[2];
extern BYTE FrameRate;

#define CFGLEN MAX_PATH

#if 0
static long solveHEX(char *str) {

	long	ret;
	int		i;
	char	c;

	ret = 0;
	for (i=0; i<8; i++) {
		c = *str++;
		if ((c >= '0') && (c <= '9')) {
			c -= '0';
		}
		else if ((c >= 'A') && (c <= 'F')) {
			c -= '7';
		}
		else if ((c >= 'a') && (c <= 'f')) {
			c -= 'W';
		}
		else {
			break;
		}
		ret <<= 4;
		ret += (long) c;
	}
	return(ret);
}
#endif

static char *makeBOOL(BYTE value) {

	if (value) {
		return("true");
	}
	return("false");
}

static BYTE Aacmp(char *cmp, char *str) {

	char	p;

	while(*str) {
		p = *cmp++;
		if (!p) {
			break;
		}
		if (p >= 'a' && p <= 'z') {
			p -= 0x20;
		}
		if (p != *str++) {
			return(-1);
		}
	}
	return(0);
}

static BYTE solveBOOL(char *str) {

	if ((!Aacmp(str, "TRUE")) || (!Aacmp(str, "ON")) ||
		(!Aacmp(str, "+")) || (!Aacmp(str, "1")) ||
		(!Aacmp(str, "ENABLE"))) {
		return(1);
	}
	return(0);
}

int
set_modulepath(char *path, size_t len)
{
	struct stat sb;
	char *homepath;

#ifdef ANDROID // xxx ちゃんと作ろう
        sprintf(path, "/sdcard/px68k");
        sprintf(winx68k_ini, "/sdcard/px68k/config");
        return 0;
#endif
#if TARGET_OS_IPHONE && TARGET_IPHONE_SIMULATOR == 0
        puts("Iphone...");
        sprintf(path, "/var/mobile/px68k");
        sprintf(winx68k_ini, "/var/mobile/px68k/config");
        return 0;
#endif

	homepath = getenv("HOME");
	if (homepath == 0)
		homepath = ".";

	snprintf(path, len, "%s/%s", homepath, ".keropi");
	if (stat(path, &sb) < 0) {
		if (mkdir(path, 0700) < 0) {
			perror(path);
			return 1;
		}
	} else {
		if ((sb.st_mode & S_IFDIR) == 0) {
			fprintf(stderr, "%s isn't directory.\n", path);
			return 1;
		}
	}
	snprintf(winx68k_ini, sizeof(winx68k_ini), "%s/%s", path, "config");
	if (stat(winx68k_ini, &sb) >= 0) {
		if (sb.st_mode & S_IFDIR) {
			fprintf(stderr, "%s is directory.\n", winx68k_ini);
			return 1;
		}
	}

	return 0;
}

void LoadConfig(void)
{
	int	i, j;
	char	buf[CFGLEN];
	FILEH fp;

	winx = GetPrivateProfileInt(ini_title, "WinPosX", 0, winx68k_ini);
	winy = GetPrivateProfileInt(ini_title, "WinPosY", 0, winx68k_ini);

	FrameRate = (BYTE)GetPrivateProfileInt(ini_title, "FrameRate", 7, winx68k_ini);
	if (!FrameRate) FrameRate=7;
	GetPrivateProfileString(ini_title, "StartDir", "", buf, MAX_PATH, winx68k_ini);
	if (buf[0] != 0)
		strncpy(filepath, buf, sizeof(filepath));
	else
		filepath[0] = 0;

	Config.OPM_VOL = GetPrivateProfileInt(ini_title, "OPM_Volume", 12, winx68k_ini);
	Config.PCM_VOL = GetPrivateProfileInt(ini_title, "PCM_Volume", 15, winx68k_ini);
	Config.MCR_VOL = GetPrivateProfileInt(ini_title, "MCR_Volume", 13, winx68k_ini);
	Config.SampleRate = GetPrivateProfileInt(ini_title, "SampleRate", 0, winx68k_ini);
	Config.BufferSize = GetPrivateProfileInt(ini_title, "BufferSize", 50, winx68k_ini);

	Config.MouseSpeed = GetPrivateProfileInt(ini_title, "MouseSpeed", 10, winx68k_ini);

	GetPrivateProfileString(ini_title, "FDDStatWin", "1", buf, CFGLEN, winx68k_ini);
	Config.WindowFDDStat = solveBOOL(buf);
	GetPrivateProfileString(ini_title, "FDDStatFullScr", "1", buf, CFGLEN, winx68k_ini);
	Config.FullScrFDDStat = solveBOOL(buf);

	GetPrivateProfileString(ini_title, "DSAlert", "1", buf, CFGLEN, winx68k_ini);
	Config.DSAlert = solveBOOL(buf);
	GetPrivateProfileString(ini_title, "SoundLPF", "1", buf, CFGLEN, winx68k_ini);
	Config.Sound_LPF = solveBOOL(buf);
	GetPrivateProfileString(ini_title, "UseRomeo", "0", buf, CFGLEN, winx68k_ini);
	Config.SoundROMEO = solveBOOL(buf);
	GetPrivateProfileString(ini_title, "MIDI_SW", "1", buf, CFGLEN, winx68k_ini);
	Config.MIDI_SW = solveBOOL(buf);
	GetPrivateProfileString(ini_title, "MIDI_Reset", "0", buf, CFGLEN, winx68k_ini);
	Config.MIDI_Reset = solveBOOL(buf);
	Config.MIDI_Type = GetPrivateProfileInt(ini_title, "MIDI_Type", 1, winx68k_ini);

	GetPrivateProfileString(ini_title, "JoySwap", "0", buf, CFGLEN, winx68k_ini);
	Config.JoySwap = solveBOOL(buf);

	GetPrivateProfileString(ini_title, "JoyKey", "0", buf, CFGLEN, winx68k_ini);
	Config.JoyKey = solveBOOL(buf);
	GetPrivateProfileString(ini_title, "JoyKeyReverse", "0", buf, CFGLEN, winx68k_ini);
	Config.JoyKeyReverse = solveBOOL(buf);
	GetPrivateProfileString(ini_title, "JoyKeyJoy2", "0", buf, CFGLEN, winx68k_ini);
	Config.JoyKeyJoy2 = solveBOOL(buf);
	GetPrivateProfileString(ini_title, "SRAMBootWarning", "1", buf, CFGLEN, winx68k_ini);
	Config.SRAMWarning = solveBOOL(buf);

	GetPrivateProfileString(ini_title, "WinDrvLFN", "1", buf, CFGLEN, winx68k_ini);
	Config.LongFileName = solveBOOL(buf);
	GetPrivateProfileString(ini_title, "WinDrvFDD", "1", buf, CFGLEN, winx68k_ini);
	Config.WinDrvFD = solveBOOL(buf);

	Config.WinStrech = GetPrivateProfileInt(ini_title, "WinStretch", 1, winx68k_ini);

	GetPrivateProfileString(ini_title, "DSMixing", "0", buf, CFGLEN, winx68k_ini);
	Config.DSMixing = solveBOOL(buf);

	Config.XVIMode = (BYTE)GetPrivateProfileInt(ini_title, "XVIMode", 0, winx68k_ini);

	GetPrivateProfileString(ini_title, "CDROM_ASPI", "1", buf, CFGLEN, winx68k_ini);
	Config.CDROM_ASPI = solveBOOL(buf);
	Config.CDROM_SCSIID = (BYTE)GetPrivateProfileInt(ini_title, "CDROM_SCSIID", 6, winx68k_ini);
	Config.CDROM_ASPI_Drive = (BYTE)GetPrivateProfileInt(ini_title, "CDROM_ASPIDrv", 0, winx68k_ini);
	Config.CDROM_IOCTRL_Drive = (BYTE)GetPrivateProfileInt(ini_title, "CDROM_CTRLDrv", 16, winx68k_ini);
	GetPrivateProfileString(ini_title, "CDROM_Enable", "1", buf, CFGLEN, winx68k_ini);
	Config.CDROM_Enable = solveBOOL(buf);

	GetPrivateProfileString(ini_title, "SSTP_Enable", "0", buf, CFGLEN, winx68k_ini);
	Config.SSTP_Enable = solveBOOL(buf);
	Config.SSTP_Port = GetPrivateProfileInt(ini_title, "SSTP_Port", 11000, winx68k_ini);

	GetPrivateProfileString(ini_title, "ToneMapping", "0", buf, CFGLEN, winx68k_ini);
	Config.ToneMap = solveBOOL(buf);
	GetPrivateProfileString(ini_title, "ToneMapFile", "", buf, MAX_PATH, winx68k_ini);
	if (buf[0] != 0)
		strcpy(Config.ToneMapFile, buf);
	else
		Config.ToneMapFile[0] = 0;

	Config.MIDIDelay = GetPrivateProfileInt(ini_title, "MIDIDelay", Config.BufferSize*5, winx68k_ini);
	Config.MIDIAutoDelay = GetPrivateProfileInt(ini_title, "MIDIAutoDelay", 1, winx68k_ini);

	Config.VkeyScale = GetPrivateProfileInt(ini_title, "VkeyScale", 4, winx68k_ini);

	Config.VbtnSwap = GetPrivateProfileInt(ini_title, "VbtnSwap", 0, winx68k_ini);

	Config.JoyOrMouse = GetPrivateProfileInt(ini_title, "JoyOrMouse", 0, winx68k_ini);

	for (i=0; i<2; i++)
	{
		for (j=0; j<8; j++)
		{
			sprintf(buf, "Joy%dButton%d", i+1, j+1);
			Config.JOY_BTN[i][j] = GetPrivateProfileInt(ini_title, buf, j, winx68k_ini);
		}
	}

	for (i = 0; i < 2; i++) {
		sprintf(buf, "FDD%d", i);
		GetPrivateProfileString(ini_title, buf, "", Config.FDDImage[i], MAX_PATH, winx68k_ini);
	}

	for (i=0; i<16; i++)
	{
		sprintf(buf, "HDD%d", i);
		GetPrivateProfileString(ini_title, buf, "", Config.HDImage[i], MAX_PATH, winx68k_ini);
	}

	fp = File_OpenCurDir(KEYCONFFILE);
	if (fp)
	{
		File_Read(fp, KeyTable, 512);
		File_Close(fp);
	}
}


void SaveConfig(void)
{
	int	i, j;
	char	buf[CFGLEN], buf2[CFGLEN];
	FILEH fp;

	wsprintf(buf, "%d", winx);
	WritePrivateProfileString(ini_title, "WinPosX", buf, winx68k_ini);
	wsprintf(buf, "%d", winy);
	WritePrivateProfileString(ini_title, "WinPosY", buf, winx68k_ini);
	wsprintf(buf, "%d", FrameRate);
	WritePrivateProfileString(ini_title, "FrameRate", buf, winx68k_ini);
	WritePrivateProfileString(ini_title, "StartDir", filepath, winx68k_ini);

	wsprintf(buf, "%d", Config.OPM_VOL);
	WritePrivateProfileString(ini_title, "OPM_Volume", buf, winx68k_ini);
	wsprintf(buf, "%d", Config.PCM_VOL);
	WritePrivateProfileString(ini_title, "PCM_Volume", buf, winx68k_ini);
	wsprintf(buf, "%d", Config.MCR_VOL);
	WritePrivateProfileString(ini_title, "MCR_Volume", buf, winx68k_ini);
	wsprintf(buf, "%d", Config.SampleRate);
	WritePrivateProfileString(ini_title, "SampleRate", buf, winx68k_ini);
	wsprintf(buf, "%d", Config.BufferSize);
	WritePrivateProfileString(ini_title, "BufferSize", buf, winx68k_ini);

	wsprintf(buf, "%d", Config.MouseSpeed);
	WritePrivateProfileString(ini_title, "MouseSpeed", buf, winx68k_ini);

	WritePrivateProfileString(ini_title, "FDDStatWin", makeBOOL((BYTE)Config.WindowFDDStat), winx68k_ini);
	WritePrivateProfileString(ini_title, "FDDStatFullScr", makeBOOL((BYTE)Config.FullScrFDDStat), winx68k_ini);

	WritePrivateProfileString(ini_title, "DSAlert", makeBOOL((BYTE)Config.DSAlert), winx68k_ini);
	WritePrivateProfileString(ini_title, "SoundLPF", makeBOOL((BYTE)Config.Sound_LPF), winx68k_ini);
	WritePrivateProfileString(ini_title, "UseRomeo", makeBOOL((BYTE)Config.SoundROMEO), winx68k_ini);
	WritePrivateProfileString(ini_title, "MIDI_SW", makeBOOL((BYTE)Config.MIDI_SW), winx68k_ini);
	WritePrivateProfileString(ini_title, "MIDI_Reset", makeBOOL((BYTE)Config.MIDI_Reset), winx68k_ini);
	wsprintf(buf, "%d", Config.MIDI_Type);
	WritePrivateProfileString(ini_title, "MIDI_Type", buf, winx68k_ini);

	WritePrivateProfileString(ini_title, "JoySwap", makeBOOL((BYTE)Config.JoySwap), winx68k_ini);

	WritePrivateProfileString(ini_title, "JoyKey", makeBOOL((BYTE)Config.JoyKey), winx68k_ini);
	WritePrivateProfileString(ini_title, "JoyKeyReverse", makeBOOL((BYTE)Config.JoyKeyReverse), winx68k_ini);
	WritePrivateProfileString(ini_title, "JoyKeyJoy2", makeBOOL((BYTE)Config.JoyKeyJoy2), winx68k_ini);
	WritePrivateProfileString(ini_title, "SRAMBootWarning", makeBOOL((BYTE)Config.SRAMWarning), winx68k_ini);

	WritePrivateProfileString(ini_title, "WinDrvLFN", makeBOOL((BYTE)Config.LongFileName), winx68k_ini);
	WritePrivateProfileString(ini_title, "WinDrvFDD", makeBOOL((BYTE)Config.WinDrvFD), winx68k_ini);

	wsprintf(buf, "%d", Config.WinStrech);
	WritePrivateProfileString(ini_title, "WinStretch", buf, winx68k_ini);

	WritePrivateProfileString(ini_title, "DSMixing", makeBOOL((BYTE)Config.DSMixing), winx68k_ini);

	wsprintf(buf, "%d", Config.XVIMode);
	WritePrivateProfileString(ini_title, "XVIMode", buf, winx68k_ini);

	WritePrivateProfileString(ini_title, "CDROM_ASPI", makeBOOL((BYTE)Config.CDROM_ASPI), winx68k_ini);
	wsprintf(buf, "%d", Config.CDROM_SCSIID);
	WritePrivateProfileString(ini_title, "CDROM_SCSIID", buf, winx68k_ini);
	wsprintf(buf, "%d", Config.CDROM_ASPI_Drive);
	WritePrivateProfileString(ini_title, "CDROM_ASPIDrv", buf, winx68k_ini);
	wsprintf(buf, "%d", Config.CDROM_IOCTRL_Drive);
	WritePrivateProfileString(ini_title, "CDROM_CTRLDrv", buf, winx68k_ini);
	WritePrivateProfileString(ini_title, "CDROM_Enable", makeBOOL((BYTE)Config.CDROM_Enable), winx68k_ini);

	WritePrivateProfileString(ini_title, "SSTP_Enable", makeBOOL((BYTE)Config.SSTP_Enable), winx68k_ini);
	wsprintf(buf, "%d", Config.SSTP_Port);
	WritePrivateProfileString(ini_title, "SSTP_Port", buf, winx68k_ini);

	WritePrivateProfileString(ini_title, "ToneMapping", makeBOOL((BYTE)Config.ToneMap), winx68k_ini);
	WritePrivateProfileString(ini_title, "ToneMapFile", Config.ToneMapFile, winx68k_ini);

	wsprintf(buf, "%d", Config.MIDIDelay);
	WritePrivateProfileString(ini_title, "MIDIDelay", buf, winx68k_ini);
	WritePrivateProfileString(ini_title, "MIDIAutoDelay", makeBOOL((BYTE)Config.MIDIAutoDelay), winx68k_ini);

	wsprintf(buf, "%d", Config.VkeyScale);
	WritePrivateProfileString(ini_title, "VkeyScale", buf, winx68k_ini);

	wsprintf(buf, "%d", Config.VbtnSwap);
	WritePrivateProfileString(ini_title, "VbtnSwap", buf, winx68k_ini);

	wsprintf(buf, "%d", Config.JoyOrMouse);
	WritePrivateProfileString(ini_title, "JoyOrMouse", buf, winx68k_ini);

	for (i=0; i<2; i++)
	{
		for (j=0; j<8; j++)
		{
			sprintf(buf, "Joy%dButton%d", i+1, j+1);
			wsprintf(buf2, "%d", Config.JOY_BTN[i][j]);
			WritePrivateProfileString(ini_title, buf, buf2, winx68k_ini);
		}
	}

	for (i = 0; i < 2; i++)
	{
		printf("i: %d", i);
		sprintf(buf, "FDD%d", i);
		WritePrivateProfileString(ini_title, buf, Config.FDDImage[i], winx68k_ini);
	}

	for (i=0; i<16; i++)
	{
		sprintf(buf, "HDD%d", i);
		WritePrivateProfileString(ini_title, buf, Config.HDImage[i], winx68k_ini);
	}

	fp = File_OpenCurDir(KEYCONFFILE);
	if (!fp)
		fp = File_CreateCurDir(KEYCONFFILE, FTYPE_TEXT);
	if (fp)
	{
		File_Write(fp, KeyTable, 512);
		File_Close(fp);
	}
}


/* --------------- */

#if 0
typedef struct {
	GtkWidget *fs;
	int type;
	void *arg;
} file_selection_t;

static void file_selection_ok(GtkWidget *w, GtkFileSelection *gfs);
static void file_selection_destroy(GtkWidget *w, GtkWidget **wp);
static void file_selection(int type, char *title, char *defstr, void *arg);

static Win68Conf ConfigProp;

static GtkWidget *create_sound_note(void);
static GtkWidget *create_midi_note(void);
static GtkWidget *create_mouse_note(void);
static GtkWidget *create_others_note(void);

static void
dialog_destroy(GtkWidget *w, GtkWidget **wp)
{

        UNUSED(wp);

	gtk_widget_destroy(w);
	install_idle_process();
}

/*
 * Config dialog
 */
static void config_ok_button_clicked(GtkButton *b, gpointer d);
static void config_accept_button_clicked(GtkButton *b, gpointer d);

void
PropPage_Init(void)
{
	GtkWidget *dialog;
	GtkWidget *dialog_table;
	GtkWidget *notebook;
	GtkWidget *note;
	GtkWidget *ok_button;
	GtkWidget *cancel_button;
	GtkWidget *accept_button;

	uninstall_idle_process();

	ConfigProp = Config;

	dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(dialog), "Configure Keropi");
	gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
	gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
	gtk_signal_connect(GTK_OBJECT(dialog), "destroy",
	    GTK_SIGNAL_FUNC(dialog_destroy), NULL);

	dialog_table = gtk_table_new(10, 4, FALSE);
	gtk_container_add(GTK_CONTAINER(dialog), dialog_table);
	gtk_widget_show(dialog_table);

	/* notebook */
	notebook = gtk_notebook_new();
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
	gtk_table_attach_defaults(GTK_TABLE(dialog_table),notebook, 0, 4, 0, 9);
	gtk_widget_show(notebook);

	/* Sound note */
	note = create_sound_note();
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), note,
	    gtk_label_new("Sound"));
	gtk_widget_show(note);

	/* MIDI note */
	note = create_midi_note();
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), note,
	    gtk_label_new("MIDI"));
	gtk_widget_show(note);

	/* Keyboard note */
	/* Joystick1 note */
	/* Joystick2 note */

	/* Mouse note */
	note = create_mouse_note();
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), note,
	    gtk_label_new("Mouse"));
	gtk_widget_show(note);

	/* SCSI note */

	/* Others note */
	note = create_others_note();
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), note,
	    gtk_label_new("Others"));
	gtk_widget_show(note);

	/* ページ下部ボタン */
	ok_button = gtk_button_new_with_label("OK");
	gtk_table_attach_defaults(GTK_TABLE(dialog_table), ok_button,
	    1, 2, 9, 10);
	gtk_signal_connect(GTK_OBJECT(ok_button), "clicked",
		    GTK_SIGNAL_FUNC(config_ok_button_clicked), dialog);
	GTK_WIDGET_SET_FLAGS(ok_button, GTK_CAN_DEFAULT);
	GTK_WIDGET_SET_FLAGS(ok_button, GTK_HAS_DEFAULT);
	gtk_widget_grab_default(ok_button);
	gtk_widget_show(ok_button);

	cancel_button = gtk_button_new_with_label("Cancel");
	gtk_container_set_border_width(GTK_CONTAINER(cancel_button), 5);
	gtk_table_attach_defaults(GTK_TABLE(dialog_table), cancel_button,
	    2, 3, 9, 10);
	gtk_signal_connect_object(GTK_OBJECT(cancel_button), "clicked",
	    GTK_SIGNAL_FUNC(gtk_widget_destroy), GTK_OBJECT(dialog));
	gtk_widget_show(cancel_button);

	accept_button = gtk_button_new_with_label("Accept");
	gtk_container_set_border_width(GTK_CONTAINER(accept_button), 5);
	gtk_table_attach_defaults(GTK_TABLE(dialog_table), accept_button,
	    3, 4, 9, 10);
	gtk_signal_connect(GTK_OBJECT(accept_button), "clicked",
		    GTK_SIGNAL_FUNC(config_accept_button_clicked), dialog);
	gtk_widget_show(accept_button);

	gtk_widget_show(dialog);
}

static void
config_ok_button_clicked(GtkButton *b, gpointer d)
{

	(void)b;

	Config = ConfigProp;
	gtk_widget_destroy(GTK_WIDGET(d));
}

static void
config_accept_button_clicked(GtkButton *b, gpointer d)
{

	(void)b;
	(void)d;

	Config = ConfigProp;
}

/*
 * Sound note
 */
static void sample_rate_button_clicked(GtkButton *b, gpointer d);
static void adpcm_lpf_button_clicked(GtkButton *b, gpointer d);
static void vol_adj_value_changed(GtkAdjustment *adj, gpointer d);

static const struct {
	const char *str;
	DWORD rate;
} sample_rate[] = {
	{ "No Sound", 0     },
	{ "11kHz",    11025 },
	{ "22kHz",    22050 },
	{ "44kHz",    44100 },
	{ "48kHz",    48000 },
};

static const struct {
	const char *name;
	gfloat value;
	gfloat min;
	gfloat max;
	size_t offset;
} sound_right_frame[] = {
	{ "Buffer size",   500.0, 20.0,1000.0, offsetof(Win68Conf,BufferSize) },
	{ "ADPCM volume",   15.0,  0.0,  16.0, offsetof(Win68Conf,PCM_VOL)    },
	{ "OPM volume",     12.0,  0.0,  16.0, offsetof(Win68Conf,OPM_VOL)    },
	{ "Mercury volume", 13.0,  0.0,  16.0, offsetof(Win68Conf,MCR_VOL)    },
};

static GtkWidget *
create_sound_note(void)
{
	GtkWidget *main_vbox;
	GtkWidget *hbox;
	GtkWidget *vbox;
	GtkWidget *frame;
	GtkWidget *button[NELEMENTS(sample_rate)];
	GSList *gslist;
	int i;

	main_vbox = gtk_vbox_new(FALSE, 2);

	hbox = gtk_hbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(main_vbox), hbox, FALSE, FALSE, 10);
	gtk_widget_show(hbox);

	frame = gtk_frame_new("Sample rate");
	gtk_container_set_border_width(GTK_CONTAINER(frame), 5);
	gtk_box_pack_start(GTK_BOX(hbox), frame, FALSE, FALSE, 0);
	gtk_widget_show(frame);

	vbox = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), vbox);
	gtk_widget_show(vbox);

	/* Sample rate */
	for (gslist = 0, i = 0; i < NELEMENTS(sample_rate); ++i) {
		button[i] = gtk_radio_button_new_with_label(gslist,
		    sample_rate[i].str);
		GTK_WIDGET_UNSET_FLAGS(button[i], GTK_CAN_FOCUS);
		gslist = gtk_radio_button_group(GTK_RADIO_BUTTON(button[i]));
		gtk_box_pack_start(GTK_BOX(vbox), button[i], FALSE, FALSE, 0);
		gtk_signal_connect(GTK_OBJECT(button[i]), "clicked",
		    GTK_SIGNAL_FUNC(sample_rate_button_clicked),
		    (gpointer)sample_rate[i].rate);
		gtk_widget_show(button[i]);
	}
	for (i = 0; i < NELEMENTS(sample_rate); ++i) {
		if (sample_rate[i].rate == Config.SampleRate)
			break;
	}
	if (i == NELEMENTS(sample_rate))
		i = 0;
	gtk_signal_emit_by_name(GTK_OBJECT(button[i]), "clicked");

	vbox = gtk_vbox_new(FALSE, 4);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, 10);
	gtk_widget_show(vbox);

	for (i = 0; i < NELEMENTS(sound_right_frame); ++i) {
		GtkObject *adjust;
		GtkWidget *hscale;
		GtkWidget *align;

		frame = gtk_frame_new(sound_right_frame[i].name);
		gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 0);
		gtk_widget_show(frame);

		align = gtk_alignment_new(0.5, 0.5, 0.9, 0.9);
		gtk_container_add(GTK_CONTAINER(frame), align);
		gtk_widget_show(align);

		adjust = gtk_adjustment_new(sound_right_frame[i].value,
		    sound_right_frame[i].min, sound_right_frame[i].max,
		    1.0, 0.0, 0.0);
		hscale = gtk_hscale_new(GTK_ADJUSTMENT(adjust));
		gtk_scale_set_value_pos(GTK_SCALE(hscale), GTK_POS_RIGHT);
		gtk_scale_set_digits(GTK_SCALE(hscale), 0);
		gtk_container_add(GTK_CONTAINER(align), hscale);
		gtk_signal_connect(GTK_OBJECT(adjust), "value_changed",
			    GTK_SIGNAL_FUNC(vol_adj_value_changed),(gpointer)i);
		gtk_widget_show(hscale);
		if (i == 0)
			gtk_adjustment_set_value(GTK_ADJUSTMENT(adjust),
			    Config.BufferSize);
		else {
			int *p = (int *)(((char *)&Config) + sound_right_frame[i].offset);
			gtk_adjustment_set_value(GTK_ADJUSTMENT(adjust), *p);
		}
	}

	button[0] = gtk_check_button_new_with_label("Enable ADPCM LPF");
	gtk_container_set_border_width(GTK_CONTAINER(button[0]), 5);
	gtk_box_pack_start(GTK_BOX(main_vbox), button[0], TRUE, TRUE, 0);
	gtk_signal_connect(GTK_OBJECT(button[0]), "clicked",
		    GTK_SIGNAL_FUNC(adpcm_lpf_button_clicked), 0);
	gtk_widget_show(button[0]);
	if (Config.Sound_LPF)
		gtk_signal_emit_by_name(GTK_OBJECT(button[0]), "clicked");

	return main_vbox;
}

static void
sample_rate_button_clicked(GtkButton *b, gpointer d)
{
	
	UNUSED(b);

	ConfigProp.SampleRate = (DWORD)((long)d);
}

static void
adpcm_lpf_button_clicked(GtkButton *b, gpointer d)
{

	UNUSED(d);

	ConfigProp.Sound_LPF = GTK_TOGGLE_BUTTON(b)->active;
}

static void
vol_adj_value_changed(GtkAdjustment *adj, gpointer d)
{
	int val = (int)(GTK_ADJUSTMENT(adj)->value);
	int idx = (int)(long)d;
	size_t offset = sound_right_frame[idx].offset;

	if (idx == 0)
		ConfigProp.BufferSize = (DWORD)val;
	else {
		int *p = (int *)(((char *)&ConfigProp) + offset);
		*p = val;
	}
}

/*
 * Midi note
 */
static void midi_enable_button_clicked(GtkButton *b, gpointer d);
static void midi_send_reset_button_clicked(GtkButton *b, gpointer d);
static void midi_mimpi_browse_button_clicked(GtkButton *b, gpointer d);
static void midi_mimpi_button_clicked(GtkButton *b, gpointer d);
static void midi_init_entry_changed(GtkEditable *e, gpointer d);

typedef struct {
	GtkWidget *init_combo;
	GtkWidget *init_label;
	GtkWidget *reset_button;
	GtkWidget *mimpi_button;
	GtkWidget *mimpi_entry;
	GtkWidget *mimpi_browse_button;
} midi_init_item_t;

static GtkWidget *
create_midi_note(void)
{
	GtkWidget *vbox;
	GtkWidget *button;
	GtkWidget *enable_button;
	GtkWidget *mimpi_button;
	GtkWidget *label;
	GtkWidget *combo;
	GtkWidget *entry;
	GtkWidget *hbox;
	static midi_init_item_t mii;
	GList *items;
	int i;

	vbox = gtk_vbox_new(FALSE, 4);

	hbox = gtk_hbox_new(FALSE, 3);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);

	enable_button = gtk_check_button_new_with_label(
	    "Enable MIDI");
	gtk_container_set_border_width(GTK_CONTAINER(enable_button), 5);
	gtk_box_pack_start(GTK_BOX(hbox), enable_button, FALSE, FALSE, 0);
	gtk_widget_show(enable_button);

	mii.init_combo = combo = gtk_combo_new();
	gtk_container_set_border_width(GTK_CONTAINER(combo), 5);
	gtk_combo_set_value_in_list(GTK_COMBO(combo), TRUE, TRUE);
	gtk_combo_set_use_arrows_always(GTK_COMBO(combo), TRUE);
	for (items = 0, i = 0; i < NELEMENTS(MIDI_TYPE_NAME); ++i)
		items = g_list_append(items, (gpointer)MIDI_TYPE_NAME[i]);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo), items);
	g_list_free(items);
	gtk_box_pack_end(GTK_BOX(hbox), combo, FALSE, FALSE, 0);
	gtk_widget_show(combo);

	entry = GTK_COMBO(combo)->entry;
	gtk_entry_set_editable(GTK_ENTRY(entry), FALSE);
	gtk_signal_connect(GTK_OBJECT(entry), "changed",
		    GTK_SIGNAL_FUNC(midi_init_entry_changed), 0);
	gtk_widget_show(entry);

	mii.init_label = label = gtk_label_new(
	    "- MIDI initialized method:");
	gtk_box_pack_end(GTK_BOX(hbox), label, TRUE, TRUE, 0);
	gtk_widget_show(label);

	mii.reset_button = button = gtk_check_button_new_with_label(
	    "Send initialize command when X68k is reseted");
	gtk_container_set_border_width(GTK_CONTAINER(button), 5);
	gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(button), "clicked",
		    GTK_SIGNAL_FUNC(midi_send_reset_button_clicked), 0);
	gtk_widget_show(button);
	if (Config.MIDI_Reset)
		gtk_signal_emit_by_name(GTK_OBJECT(button), "clicked");

	mii.mimpi_button = mimpi_button = gtk_check_button_new_with_label(
	    "Use MIMPI tone map");
	gtk_container_set_border_width(GTK_CONTAINER(mimpi_button), 5);
	gtk_box_pack_start(GTK_BOX(vbox), mimpi_button, FALSE, FALSE, 0);
	gtk_widget_show(mimpi_button);

	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);

	mii.mimpi_entry = entry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(entry), MAX_PATH - 1);
	gtk_entry_set_editable(GTK_ENTRY(entry), FALSE);
	gtk_box_pack_start(GTK_BOX(hbox), entry, TRUE, TRUE, 10);
	gtk_widget_show(entry);

	mii.mimpi_browse_button = button = gtk_button_new_with_label(
	    "Browse...");
	gtk_container_set_border_width(GTK_CONTAINER(button), 5);
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(button), "clicked",
		    GTK_SIGNAL_FUNC(midi_mimpi_browse_button_clicked),
		    (gpointer)&mii);
	gtk_widget_show(button);

	/* MIMPI ToneMap */
	gtk_signal_connect(GTK_OBJECT(mimpi_button), "clicked",
		    GTK_SIGNAL_FUNC(midi_mimpi_button_clicked),
		    (gpointer)&mii);
	if (!Config.ToneMap)
		gtk_signal_emit_by_name(GTK_OBJECT(mimpi_button), "clicked");
	gtk_signal_emit_by_name(GTK_OBJECT(mimpi_button), "clicked");

	/* Enable MIDI */
	gtk_signal_connect(GTK_OBJECT(enable_button), "clicked",
		    GTK_SIGNAL_FUNC(midi_enable_button_clicked),
		    (gpointer)&mii);
	if (!Config.MIDI_SW)
		gtk_signal_emit_by_name(GTK_OBJECT(enable_button), "clicked");
	gtk_signal_emit_by_name(GTK_OBJECT(enable_button), "clicked");

	return vbox;
}

static void
midi_enable_button_clicked(GtkButton *b, gpointer d)
{
	midi_init_item_t *mii = (midi_init_item_t *)d;

	ConfigProp.MIDI_SW = GTK_TOGGLE_BUTTON(b)->active;

	if (ConfigProp.MIDI_SW) {
		gtk_widget_set_sensitive(mii->init_combo, TRUE);
		gtk_widget_set_sensitive(mii->init_label, TRUE);
		gtk_widget_set_sensitive(mii->reset_button, TRUE);
		gtk_widget_set_sensitive(mii->mimpi_button, TRUE);
		if (ConfigProp.ToneMap) {
			gtk_widget_set_sensitive(mii->mimpi_entry, TRUE);
			gtk_widget_set_sensitive(mii->mimpi_browse_button,TRUE);
		}
	} else {
		gtk_widget_set_sensitive(mii->init_combo, FALSE);
		gtk_widget_set_sensitive(mii->init_label, FALSE);
		gtk_widget_set_sensitive(mii->reset_button, FALSE);
		gtk_widget_set_sensitive(mii->mimpi_button, FALSE);
		gtk_widget_set_sensitive(mii->mimpi_entry, FALSE);
		gtk_widget_set_sensitive(mii->mimpi_browse_button, FALSE);
	}
}

static void
midi_send_reset_button_clicked(GtkButton *b, gpointer d)
{

	UNUSED(d);

	ConfigProp.MIDI_Reset = GTK_TOGGLE_BUTTON(b)->active;
}

static void
midi_mimpi_browse_button_clicked(GtkButton *b, gpointer d)
{
	midi_init_item_t *mii = (midi_init_item_t *)d;
	gchar *path = gtk_entry_get_text(GTK_ENTRY(mii->mimpi_entry));

	UNUSED(b);

	file_selection(1, "MIMPI tone map", path, mii->mimpi_entry);
}

static void
midi_mimpi_button_clicked(GtkButton *b, gpointer d)
{
	midi_init_item_t *mii = (midi_init_item_t *)d;

	ConfigProp.ToneMap = GTK_TOGGLE_BUTTON(b)->active;
	if (ConfigProp.ToneMap) {
		gtk_widget_set_sensitive(mii->mimpi_entry, TRUE);
		gtk_widget_set_sensitive(mii->mimpi_browse_button, TRUE);
	} else {
		gtk_widget_set_sensitive(mii->mimpi_entry, FALSE);
		gtk_widget_set_sensitive(mii->mimpi_browse_button, FALSE);
	}
}

static void
midi_init_entry_changed(GtkEditable *e, gpointer d)
{
	gchar *str;
	int i;

	UNUSED(d);

	str = gtk_editable_get_chars(GTK_EDITABLE(e), 0, -1);
	for (i = 0; i < NELEMENTS(MIDI_TYPE_NAME); ++i) {
		if (strcmp(MIDI_TYPE_NAME[i], str) == 0) {
			ConfigProp.MIDI_Type = i;
			break;
		}
	}
	g_free(str);
}

/*
 * Mouse note
 */
static void mouse_speed_rate_value_changed(GtkAdjustment *adj, gpointer d);

static GtkWidget *
create_mouse_note(void)
{
	GtkWidget *w;
	GtkWidget *frame;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *align;
	GtkWidget *hscale;
	GtkWidget *label;
	GtkObject *adjust;

	w = gtk_vbox_new(FALSE, 1);

	frame = gtk_frame_new("Mouse speed");
	gtk_box_pack_start(GTK_BOX(w), frame, FALSE, FALSE, 0);
	gtk_widget_show(frame);

	align = gtk_alignment_new(0.5, 0.5, 0.9, 0.9);
	gtk_container_add(GTK_CONTAINER(frame), align);
	gtk_widget_show(align);

	vbox = gtk_vbox_new(FALSE, 2);
	gtk_container_add(GTK_CONTAINER(align), vbox);
	gtk_widget_show(vbox);

	adjust = gtk_adjustment_new(10.0, 1.0, 20.0, 1.0, 1.0, 0.0);
	hscale = gtk_hscale_new(GTK_ADJUSTMENT(adjust));
	gtk_scale_set_draw_value(GTK_SCALE(hscale), FALSE);
	gtk_box_pack_start(GTK_BOX(vbox), hscale, TRUE, TRUE, 0);
	gtk_widget_show(hscale);

	hbox = gtk_hbox_new(TRUE, 3);
	gtk_box_pack_end(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);
	gtk_widget_show(hbox);

	label = gtk_label_new("x0.1");
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.0);
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
	gtk_widget_show(label);

	label = gtk_label_new("x1.0");
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
	gtk_misc_set_alignment(GTK_MISC(label), 0.5, 0.0);
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
	gtk_widget_show(label);

	label = gtk_label_new("x2.0");
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_RIGHT);
	gtk_misc_set_alignment(GTK_MISC(label), 1.0, 0.0);
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
	gtk_widget_show(label);

	/* Mouse speed rate */
	gtk_signal_connect(GTK_OBJECT(adjust), "value_changed",
	    GTK_SIGNAL_FUNC(mouse_speed_rate_value_changed), 0);
	gtk_adjustment_set_value(GTK_ADJUSTMENT(adjust),
	    (gfloat)Config.MouseSpeed);

	return w;
}

static void
mouse_speed_rate_value_changed(GtkAdjustment *adj, gpointer d)
{

	UNUSED(d);

	ConfigProp.MouseSpeed = (int)adj->value;
	if (ConfigProp.MouseSpeed < 1)
		ConfigProp.MouseSpeed = 1;
	if (ConfigProp.MouseSpeed > 20)
		ConfigProp.MouseSpeed = 20;
}

/*
 * Others note
 */
static void others_sasi_hdd_index_entry_changed(GtkEditable *e, gpointer d);
static void others_sasi_hdd_new_button_clicked(GtkButton *b, gpointer d);
static void others_sasi_hdd_remove_button_clicked(GtkButton *b, gpointer d);
static void others_sasi_hdd_browse_button_clicked(GtkButton *b, gpointer d);
static void others_show_fdd_status_button_clicked(GtkButton *b, gpointer d);
static void others_enable_virus_button_clicked(GtkButton *b, gpointer d);

typedef struct {
	int idx;
	GtkWidget *entry;
} sasi_hdd_t;

static GtkWidget *
create_others_note(void)
{
	static const char *sasi_drive_str[16] = {
		"0", "1", "2", "3", "4", "5", "6", "7", "8",
		"9", "10", "11", "12", "13", "14", "15",
	};
	static sasi_hdd_t sasi_hdd = { 0, 0 };

	GtkWidget *w;
	GtkWidget *frame;
	GtkWidget *align;
	GtkWidget *table;
	GtkWidget *label;
	GtkWidget *combo;
	GtkWidget *entry;
	GtkWidget *button;
	GList *items;
	int i;

	w = gtk_vbox_new(FALSE, 3);

	frame = gtk_frame_new("HDD (SASI) Drive");
	gtk_box_pack_start(GTK_BOX(w), frame, FALSE, FALSE, 0);
	gtk_widget_show(frame);

	align = gtk_alignment_new(0.5, 0.5, 0.9, 0.9);
	gtk_container_add(GTK_CONTAINER(frame), align);
	gtk_widget_show(align);

	table = gtk_table_new(2, 5, FALSE);
	gtk_container_add(GTK_CONTAINER(align), table);
	gtk_widget_show(table);

	label = gtk_label_new("Drive #:");
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 0, 1);
	gtk_widget_show(label);

	combo = gtk_combo_new();
	gtk_combo_set_value_in_list(GTK_COMBO(combo), TRUE, TRUE);
	gtk_combo_set_use_arrows_always(GTK_COMBO(combo), TRUE);
	for (items = 0, i = 0; i < NELEMENTS(sasi_drive_str); ++i)
		items = g_list_append(items, (gpointer)sasi_drive_str[i]);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo), items);
	g_list_free(items);
	gtk_table_attach_defaults(GTK_TABLE(table), combo, 1, 2, 0, 1);
	gtk_widget_show(combo);

	entry = GTK_COMBO(combo)->entry;
	gtk_entry_set_editable(GTK_ENTRY(entry), FALSE);
	gtk_signal_connect(GTK_OBJECT(entry), "changed",
		    GTK_SIGNAL_FUNC(others_sasi_hdd_index_entry_changed),
		    &sasi_hdd);
	gtk_widget_show(entry);

	button = gtk_button_new_with_label("Remove");
	gtk_container_set_border_width(GTK_CONTAINER(button), 5);
	gtk_table_attach_defaults(GTK_TABLE(table), button, 3, 4, 0, 1);
	gtk_signal_connect(GTK_OBJECT(button), "clicked",
		    GTK_SIGNAL_FUNC(others_sasi_hdd_remove_button_clicked),
		    &sasi_hdd);
	gtk_widget_show(button);

	button = gtk_button_new_with_label("New");
	gtk_container_set_border_width(GTK_CONTAINER(button), 5);
	gtk_table_attach_defaults(GTK_TABLE(table), button, 4, 5, 0, 1);
	gtk_signal_connect(GTK_OBJECT(button), "clicked",
		    GTK_SIGNAL_FUNC(others_sasi_hdd_new_button_clicked),
		    &sasi_hdd);
#if 1
	gtk_widget_set_sensitive(button, FALSE);
#endif
	gtk_widget_show(button);

	label = gtk_label_new("Filename:");
	gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 1, 2);
	gtk_widget_show(label);

	sasi_hdd.entry = entry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(entry), MAX_PATH - 1);
	gtk_entry_set_editable(GTK_ENTRY(entry), FALSE);
	gtk_table_attach_defaults(GTK_TABLE(table), entry, 1, 4, 1, 2);
	gtk_widget_show(entry);
	gtk_entry_set_text(GTK_ENTRY(entry), Config.HDImage[0]);

	button = gtk_button_new_with_label("Browse...");
	gtk_container_set_border_width(GTK_CONTAINER(button), 5);
	gtk_table_attach_defaults(GTK_TABLE(table), button, 4, 5, 1, 2);
	gtk_signal_connect(GTK_OBJECT(button), "clicked",
		    GTK_SIGNAL_FUNC(others_sasi_hdd_browse_button_clicked),
		    &sasi_hdd);
	gtk_widget_show(button);

	button = gtk_check_button_new_with_label("Show FDD status");
	gtk_container_set_border_width(GTK_CONTAINER(button), 5);
	gtk_box_pack_start(GTK_BOX(w), button, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(button), "clicked",
		    GTK_SIGNAL_FUNC(others_show_fdd_status_button_clicked), 0);
#if 1
	gtk_widget_set_sensitive(button, FALSE);
#else
	if (Config.WindowFDDStat)
		gtk_signal_emit_by_name(GTK_OBJECT(button), "clicked");
#endif
	gtk_widget_show(button);

	button = gtk_check_button_new_with_label("Enable SRAM virus warning");
	gtk_container_set_border_width(GTK_CONTAINER(button), 5);
	gtk_box_pack_start(GTK_BOX(w), button, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(button), "clicked",
		    GTK_SIGNAL_FUNC(others_enable_virus_button_clicked), 0);
	if (Config.SRAMWarning)
		gtk_signal_emit_by_name(GTK_OBJECT(button), "clicked");
	gtk_widget_show(button);

	return w;
}

static void
others_sasi_hdd_index_entry_changed(GtkEditable *e, gpointer d)
{
	sasi_hdd_t *sasi_hdd = (sasi_hdd_t *)d;
	char *str;
	char *ep;

	str = gtk_editable_get_chars(GTK_EDITABLE(e), 0, -1);
	if (str) {
		unsigned long val;

		errno = 0;
		val = strtoul(str, &ep, 10);
		/* Not a number */
		if (str[0] == '\0' || *ep != '\0')
			val = 0;
		/* Out of range */
		if (errno == ERANGE && val == ULONG_MAX)
			val = 0;
		sasi_hdd->idx = (int)val;

		if (sasi_hdd->idx >= 0 && sasi_hdd->idx < 16) {
			gtk_entry_set_text(GTK_ENTRY(sasi_hdd->entry),
			    ConfigProp.HDImage[sasi_hdd->idx]);
		}
	}
}

static void
others_sasi_hdd_new_button_clicked(GtkButton *b, gpointer d)
{
	sasi_hdd_t *sasi_hdd = (sasi_hdd_t *)d;
	gchar *path = gtk_entry_get_text(GTK_ENTRY(sasi_hdd->entry));

	UNUSED(b);

	file_selection(3, "Create SASI HDD", path, d);
}

static void
others_sasi_hdd_remove_button_clicked(GtkButton *b, gpointer d)
{
	sasi_hdd_t *sasi_hdd = (sasi_hdd_t *)d;

	UNUSED(b);

	bzero(ConfigProp.HDImage[sasi_hdd->idx], sizeof(ConfigProp.HDImage));
	gtk_entry_set_text(GTK_ENTRY(sasi_hdd->entry), ConfigProp.HDImage[sasi_hdd->idx]);
}

static void
others_sasi_hdd_browse_button_clicked(GtkButton *b, gpointer d)
{
	sasi_hdd_t *sasi_hdd = (sasi_hdd_t *)d;
	gchar *path = gtk_entry_get_text(GTK_ENTRY(sasi_hdd->entry));

	UNUSED(b);

	file_selection(2, "SASI HDD", path, d);
}

static void
others_show_fdd_status_button_clicked(GtkButton *b, gpointer d)
{

	UNUSED(d);

	ConfigProp.WindowFDDStat = GTK_TOGGLE_BUTTON(b)->active;
}

static void
others_enable_virus_button_clicked(GtkButton *b, gpointer d)
{

	UNUSED(d);

	ConfigProp.SRAMWarning = GTK_TOGGLE_BUTTON(b)->active;
}

/*
 * ---------- file selection
 */

static void
file_selection_ok(GtkWidget *w, GtkFileSelection *gfs)
{
	file_selection_t *fsp = (file_selection_t *)gfs;
	GtkFileSelection *fs = (GtkFileSelection *)fsp->fs;
	char *p;

	UNUSED(w);

	p = (char *)gtk_file_selection_get_filename(fs);
	if (p) {
		struct stat st;

		if (stat(p, &st) == 0) {
			if (!S_ISDIR(st.st_mode)) {
				switch (fsp->type) {
				case 1:	// open MIMPI tone map
					gtk_entry_set_text(GTK_ENTRY(fsp->arg), p);
					strncpy(ConfigProp.ToneMapFile, p, sizeof(ConfigProp.ToneMapFile));
					break;

				case 2:	// open SASI HDD
					{
					sasi_hdd_t *sasi_hdd = (sasi_hdd_t *)fsp->arg;
					strncpy(ConfigProp.HDImage[sasi_hdd->idx], p, sizeof(ConfigProp.HDImage[0]));
					gtk_entry_set_text(GTK_ENTRY(sasi_hdd->entry), p);
					}
					break;
				}
			}
		} else if (errno == ENOENT) {
			switch (fsp->type) {
			case 3:	// create SASI HDD image
				{
				sasi_hdd_t *sasi_hdd = (sasi_hdd_t *)fsp->arg;
				char buf[0x1000];
				FILEH *fh;
				int i;

				fh = File_Create(p, FTYPE_NONE);
				if (fh == INVALID_HANDLE_VALUE)
					break;

				bzero(buf, sizeof(buf));
				for (i = 0; i < 0x2793 /* 40Mb/0x1000 */; ++i)
					File_Write(fh, buf, 0x1000);
				File_Close(fh);

				strncpy(ConfigProp.HDImage[sasi_hdd->idx], p, sizeof(ConfigProp.HDImage[0]));
				gtk_entry_set_text(GTK_ENTRY(sasi_hdd->entry), p);
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
	if (fsp == 0) {
		printf("file_selection: Can't alloc memory (size = %d)\n",
		    sizeof(*fsp));
		return;
	}
	bzero(fsp, sizeof(*fsp));

	file_dialog = gtk_file_selection_new(title);
	if (defstr) {
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
