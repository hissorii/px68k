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
#include "winui.h"

#include <dirent.h>
#include <sys/stat.h>

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

/******************************************************************************
 * init
 ******************************************************************************/

// xxx configの値にセットしなければならない
int mval_y[] = {0, 0, 0, 0, 2, 1, 1};

struct menu_flist mfl;

void
WinUI_Init(void)
{
	if (FrameRate == 7) {
		mval_y[3] = 0;
	} else if (FrameRate == 8) {
		mval_y[3] = 7;
	} else {
		mval_y[3] = FrameRate;
	}

	mval_y[6] = NoWaitMode;

	// xxx configのFDイメージファイル名のディレクトリを取得する
	strcpy(mfl.dir[0], "./");
	strcpy(mfl.dir[1], "./");
}


#if 0
/*
 * item function
 */
static void
exit_from_menu(gpointer data, guint action, GtkWidget *w)
{
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
#endif

/********** menu 関連ルーチン **********/

// xxx 上下スクロール未実装なのでキー数固定
char menu_item_key[][15] = {"SYSTEM", "FDD1", "FDD2", "Frame Skip", "VKey Size", "VKey Pos", "No Wait Mode", ""};

// 文字列は30文字まで。各アイテム要素は終端含め10個まで。
// 追加/拡張する場合は配列要素を適宜増加すること。
// アイテム数はいくら追加してもよい。
// 各アイテム要素は終端として""を置くこと。
char menu_items[][10][30] = {
	{"RESET", "NMI RESET", "QUIT", ""},
	{"dummy", "EJECT", ""},
	{"dummy", "EJECT", ""},
	{"Auto Frame Skip", "Full Frame", "1/2 Frame", "1/3 Frame", "1/4 Frame", "1/5 Frame", "1/6 Frame", "1/8 Frame", ""},
	{"Huge", "Large", "Medium", "Small", ""},
	{"High", "Middle", "Low", ""},
	{"Off", "On", ""}
};

int menu_state = ms_key;
int mkey_y = 0;

static void menu_system(int v)
{
	printf("menu_system func called %d\n", v);
	switch (v) {
	case 0 :
		WinX68k_Reset();
		break;
	case 1:
		IRQH_Int(7, NULL);
		break;
	}
}

static void upper(char *s)
{
	while (*s != '\0') {
		if (*s >= 'a' && *s <= 'z') {
			*s = 'A' + *s - 'a';
		}
		s++;
	}
}

static void menu_create_flist(int v)
{
	if (v == 1) {
		FDD_EjectFD(mkey_y - 1);
		Config.FDDImage[mkey_y - 1][0] = '\0';
		strcpy(mfl.dir[mkey_y - 1], "./");
		return;
	}

	// ファイル一覧取得ルーチン
	DIR *dp;
	struct dirent *dent;
	struct stat buf;
	int i, len;
	char *n, ext[4], *p;
	char support[] = "D8888DHDMDUP2HDDIMXDFIMG"; //FDイメージ拡張子
	char ent_name[MAX_PATH];

	dp = opendir(mfl.dir[mkey_y - 1]);
	// xxx とりあえずファイルをMFL_MAX個取得
	for (i = 0 ; i < MFL_MAX; i++) {
		dent = readdir(dp);
		if (dent == NULL) {
			break;
		}
		n = dent->d_name;
		strcpy(ent_name, mfl.dir[mkey_y - 1]);
		strcat(ent_name, n);
		stat(ent_name, &buf);

		// ファイルなら拡張子チェック
		if (!S_ISDIR(buf.st_mode)) {
			len = strlen(n);
			if (len < 4 || *(n + len - 4) != '.') {
				i--;
				continue;
			}
			strcpy(ext, n + len - 3);
			upper(ext);
			p = strstr(support, ext);
			if (p == NULL || (p - support) % 3 != 0) {
				i--;
				continue;
			}
		} else {
			if (!strcmp(n, ".")) {
				i--;
				continue;
			}
		}

		strcpy(mfl.name[i], n);
		// ディレクトリなら1をセット
		mfl.type[i] = S_ISDIR(buf.st_mode)? 1 : 0;
		printf("%s 0x%x\n", n, buf.st_mode);
	}
	closedir(dp);

	strcpy(mfl.name[i], "");
	mfl.num = i;

	printf("mfl.num %d\n", mfl.num);

	mfl.ptr = 0;
}

static void menu_frame_skip(int v)
{
	if (v == 0) {
		FrameRate = 7;
	} else if (v == 7) {
		FrameRate = 8;
	} else {
		FrameRate = v;
	}
}

static void menu_vkey_size(int v)
{
}

static void menu_vkey_loc(int v)
{
}

static void menu_nowait(int v)
{
	NoWaitMode = v;
}

static void (*menu_func[])(int v) = {
	menu_system,
	menu_create_flist,
	menu_create_flist,
	menu_frame_skip,
	menu_vkey_size,
	menu_vkey_loc,
	menu_nowait
};

// ディレクトリを降りて登ってきたら元にもどす
// ex. ./hoge/.. -> ./
static void shortcut_dir(int drv)
{
	int i, len, found = 0;
	char *p;

	// len is larger than 2
	len = strlen(mfl.dir[drv]);
	p = mfl.dir[drv] + len - 2;
	for (i = len - 2; i >= 0; i--) {
		if (*p == '/') {
			found = 1;
			break;
		}
		p--;
	}

	if (found && strcmp(p, "/../")) {
		*(p + 1) = '\0';
	} else {
		strcat(mfl.dir[drv], "../");
	}
}

int WinUI_Menu(void)
{
	int i;
	static int first = 1;
	int cursor0;
	BYTE joy;
	int menu_redraw = 0;
	int mfile_redraw = 0;

	if (first == 1) {
		menu_state = ms_key;
		mkey_y = 0;
		menu_redraw = 1;
		first = 0;
		// ダブルバッファだとキー触らないと画面変わらないので
		// 最初だけ二つのバッファにそれぞれ描画する
		WinDraw_ClearScreen(1);
		WinDraw_DrawMenu(menu_state, mkey_y, mval_y);
	}

	cursor0 = mkey_y;
	joy = get_joy_downstate();
	//printf("joy state 0x%x", joy);
	reset_joy_downstate();

	if (!(joy & JOY_UP)) {
		switch (menu_state) {
		case ms_key:
			mkey_y = (mkey_y == 0)? 6 : mkey_y - 1;
			break;
		case ms_value:
			if (mval_y[mkey_y] > 0) {
				mval_y[mkey_y]--;
				menu_redraw = 1;
			}
			break;
		case ms_file:
			if (mfl.y == 0) {
				if (mfl.ptr > 0) {
					mfl.ptr--;
				}
			} else {
				mfl.y--;
			}
			mfile_redraw = 1;
			break;
		}
		//printf("joy up!!! %d\n", mkey_y);
	}

	if (!(joy & JOY_DOWN)) {
		switch (menu_state) {
		case ms_key:
			mkey_y = (mkey_y == 6)? 0 : mkey_y + 1;
			break;
		case ms_value:
			if (menu_items[mkey_y][mval_y[mkey_y] + 1][0] != '\0') {
				mval_y[mkey_y]++;
				menu_redraw = 1;
			}
			break;
		case ms_file:
			if (mfl.y == 13) {
				if (mfl.ptr + 14 < mfl.num && mfl.ptr < MFL_MAX - 13) {
					mfl.ptr++;
				}
			} else if (mfl.y + 1 < mfl.num) {
				mfl.y++;
				printf("mfl.y %d\n", mfl.y);
			}
			mfile_redraw = 1;
			break;
		}
		//printf("joy up!!! %d\n", mkey_y);
	}

	if (!(joy & JOY_TRG1)) {
		if (menu_state == ms_key) {
			menu_state = ms_value;
			menu_redraw = 1;
		} else if (menu_state == ms_value) {

			menu_func[mkey_y](mval_y[mkey_y]);

			//値をセットしたらキーモードに戻る
			//ただしファイル名を選択したときはファイラモード
			menu_state = ms_key;
			menu_redraw = 1;

			if (mkey_y == 1 || mkey_y == 2) {
				if (mval_y[mkey_y] == 0) {
					// ファイラモードへ移行
					printf("hoge:%d", mval_y[mkey_y]);
					menu_state = ms_file;
					menu_redraw = 0; //リセット
					mfile_redraw = 1;
				} else { // mval_y[mkey_y] == 1
					// FDイジェクト後処理
					mval_y[mkey_y] = 0;
				}
			} else if (mkey_y == 0) {
				if (mval_y[mkey_y] == 2) {
					return WUM_EMU_QUIT;
				}
				first = 1;
				return WUM_MENU_END;
			}
		} else if (menu_state == ms_file) {
			int drv, y;
			drv = mkey_y - 1;
			y = mfl.ptr + mfl.y;
			printf("file slect %s\n", mfl.name[y]);
			if (mfl.type[y]) {
				// ディレクトリ
				if (!strcmp(mfl.name[y], "..")) {
					shortcut_dir(drv);
				} else {
					strcat(mfl.dir[drv], mfl.name[y]);
					strcat(mfl.dir[drv], "/");
				}
				menu_func[mkey_y](0);
				mfl.y = 0;
				mfile_redraw = 1;
			} else {
				// ファイル
				char tmpstr[MAX_PATH];
				strcpy(tmpstr, mfl.dir[drv]);
				strcat(tmpstr, mfl.name[y]);
				FDD_SetFD(drv, tmpstr, 0);
				strcpy(Config.FDDImage[drv], tmpstr);
				menu_state = ms_key;
				menu_redraw = 1;
			}
		}
	}

	if (!(joy & JOY_TRG2)) {
		if (menu_state == ms_file) {
			menu_state = ms_value;
			// reset position of file cursor
			mfl.y = 0;
			mfl.ptr = 0;
			menu_redraw = 1;
		} else if (menu_state == ms_value) {
			menu_state = ms_key;
			menu_redraw = 1;
		} else if (menu_state == ms_key) {
			first = 1;
			return WUM_MENU_END;
		}

	}

	if (cursor0 != mkey_y) {
		menu_redraw = 1;
	}

	if (mfile_redraw) {
		WinDraw_DrawMenufile(&mfl);
		mfile_redraw = 0;
	}

	if (menu_redraw) {
		// xxx Androidでvkeyの設定をいじるときはvkeyは
		// 表示しておくようにする

		//WinDraw_Draw();
		WinDraw_ClearScreen(1);
		WinDraw_DrawMenu(menu_state, mkey_y, mval_y);
	}

	return 0;
}
