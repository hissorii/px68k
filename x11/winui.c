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
 *  WINUI.C - UI´ØÏ¢                                                          *
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

/******************************************************************************
 * init
 ******************************************************************************/
void
WinUI_Init(void)
{
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
