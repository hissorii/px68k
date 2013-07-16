/*	$Id: winx68k.cpp,v 1.2 2003/12/05 18:07:17 nonaka Exp $	*/

#ifdef  __cplusplus
extern "C" {
#endif 

#include "common.h"
#include "fileio.h"
#include "timer.h"
#include "keyboard.h"
#include "prop.h"
#include "status.h"
#include "joystick.h"
#include "mkcgrom.h"
#include "winx68k.h"
#include "windraw.h"
#include "winui.h"
#include "m68000.h"
#include "memory.h"
#include "mfp.h"
#include "opm.h"
#include "bg.h"
#include "adpcm.h"
#include "mercury.h"
#include "crtc.h"
#include "mfp.h"
#include "fdc.h"
#include "fdd.h"
#include "dmac.h"
#include "irqh.h"
#include "ioc.h"
#include "rtc.h"
#include "sasi.h"
#include "scsi.h"
#include "bg.h"
#include "palette.h"
#include "crtc.h"
#include "pia.h"
#include "scc.h"
#include "midi.h"
#include "sram.h"
#include "gvram.h"
#include "tvram.h"
#include "mouse.h"

#include "dswin.h"
#include "fmg_wrap.h"

#ifdef WIN68DEBUG
#include "d68k.h"
#endif

#include "../icons/keropi_mono.xbm"

#define	APPNAME	"Keropi"

extern	WORD	BG_CHREND;
extern	WORD	BG_BGTOP;
extern	WORD	BG_BGEND;
extern	BYTE	BG_CHRSIZE;

const	BYTE	PrgName[] = "Keropi";
const	BYTE	PrgTitle[] = APPNAME;

char	winx68k_dir[MAX_PATH];
char	winx68k_ini[MAX_PATH];

WORD	VLINE_TOTAL = 567;
DWORD	VLINE = 0;
DWORD	vline = 0;

extern	int	SplashFlag;

BYTE DispFrame = 0;
extern BYTE FrameRate;
DWORD SoundSampleRate;
int CurFrameRate = 1;
int NoWaitMode = 0;

unsigned int hTimerID = 0;
DWORD TimerICount = 0;
extern DWORD timertick;
BYTE traceflag = 0;

BYTE ForceDebugMode = 0;
DWORD skippedframes = 0;

static int ClkUsed = 0;
static int FrameSkipCount = 0;
static int FrameSkipQueue = 0;

GtkWidget *window;
GtkWidget *main_vbox;
GtkWidget *menubar;
GtkWidget *drawarea;
extern GdkImage *surface;
extern GdkPixmap *pixmap;
extern GdkPixmap *splash_pixmap;

static void set_window_size(GtkWidget *);
static void set_icon_bitmap(GtkWidget *);
typedef void sigfunc(int);
static sigfunc *setup_signal(int, sigfunc *);
static void sighandler(int);

#ifdef __cplusplus
};
#endif

void
WinX68k_SCSICheck(void)
{
	static const BYTE SCSIIMG[] = {
		0x00, 0xfc, 0x00, 0x14,			// $fc0000 SCSI起動用のエントリアドレス
		0x00, 0xfc, 0x00, 0x16,			// $fc0004 IOCSベクタ設定のエントリアドレス(必ず"Human"の8バイト前)
		0x00, 0x00, 0x00, 0x00,			// $fc0008 ?
		0x48, 0x75, 0x6d, 0x61,			// $fc000c ↓
		0x6e, 0x36, 0x38, 0x6b,			// $fc0010 ID "Human68k"	(必ず起動エントリポイントの直前)
		0x4e, 0x75,				// $fc0014 "rts"		(起動エントリポイント)
		0x23, 0xfc, 0x00, 0xfc, 0x00, 0x2a,	// $fc0016 ↓		(IOCSベクタ設定エントリポイント)
		0x00, 0x00, 0x07, 0xd4,			// $fc001c "move.l #$fc002a, $7d4.l"
		0x74, 0xff,				// $fc0020 "moveq #-1, d2"
		0x4e, 0x75,				// $fc0022 "rts"
//		0x53, 0x43, 0x53, 0x49, 0x49, 0x4e,	// $fc0024 ID "SCSIIN"
// 内蔵SCSIをONにすると、SASIは自動的にOFFになっちゃうらしい…
// よって、IDはマッチしないようにしておく…
		0x44, 0x55, 0x4d, 0x4d, 0x59, 0x20,	// $fc0024 ID "DUMMY "
		0x70, 0xff,				// $fc002a "moveq #-1, d0"	(SCSI IOCSコールエントリポイント)
		0x4e, 0x75,				// $fc002c "rts"
	};

	DWORD *p;
	int scsi;
	int i;

	scsi = 0;
	for (i = 0x30600; i < 0x30c00; i += 2) {
		p = (DWORD *)(&IPL[i]);
		if (*p == 0x0000fc00)
			scsi = 1;
	}

	// SCSIモデルのとき
	if (scsi) {
		ZeroMemory(IPL, 0x2000);		// 本体は8kb
		memset(&IPL[0x2000], 0xff, 0x1e000);	// 残りは0xff
		memcpy(IPL, SCSIIMG, sizeof(SCSIIMG));	// インチキSCSI BIOS
//		Memory_SetSCSIMode();
	} else {
		// SASIモデルはIPLがそのまま見える
		memcpy(IPL, &IPL[0x20000], 0x20000);
	}
}

int
WinX68k_LoadROMs(void)
{
	static const char *BIOSFILE[] = {
		"iplrom.dat", "iplrom30.dat", "iplromco.dat", "iplromxv.dat"
	};
	static const char FONTFILE[] = "cgrom.dat";
	static const char FONTFILETMP[] = "cgrom.tmp";
	FILEH fp;
	int i;
	BYTE tmp;

	for (fp = 0, i = 0; fp == 0 && i < NELEMENTS(BIOSFILE); ++i) {
		fp = File_OpenCurDir((char *)BIOSFILE[i]);
	}
	if (fp == 0) {
		Error("BIOS ROM イメージが見つかりません.");
		return FALSE;
	}

	File_Read(fp, &IPL[0x20000], 0x20000);
	File_Close(fp);

	WinX68k_SCSICheck();	// SCSI IPLなら、$fc0000〜にSCSI BIOSを置く

	for (i = 0; i < 0x40000; i += 2) {
		tmp = IPL[i];
		IPL[i] = IPL[i + 1];
		IPL[i + 1] = tmp;
	}

	fp = File_OpenCurDir((char *)FONTFILE);
	if (fp == 0) {
		// cgrom.tmpがある？
		fp = File_OpenCurDir((char *)FONTFILETMP);
		if (fp == 0) {
#if 1
			// フォント生成 XXX
			printf("フォントROMイメージが見つかりません\n");
			return FALSE;
#else
			MessageBox(hWndMain,
				"フォントROMイメージが見つかりません.\nWindowsフォントから新規に作成します.",
				"けろぴーのメッセージ", MB_ICONWARNING | MB_OK);
			SSTP_SendMes(SSTPMES_MAKEFONT);
			make_cgromdat(FONT, FALSE, "ＭＳ ゴシック", "ＭＳ 明朝");
			//WinX68k_MakeFont();
			//DialogBox(hInst, MAKEINTRESOURCE(IDD_PROGBAR),
			//		hWndMain, (DLGPROC)MakeFontProc);
			fp = File_CreateCurDir(FONTFILETMP);
			if (fp)
			{
				File_Write(fp, FONT, 0xc0000);
				File_Close(fp);
				return TRUE;
			}
			return TRUE;
#endif
		}
	}
	File_Read(fp, FONT, 0xc0000);
	File_Close(fp);

	return TRUE;
}

int
WinX68k_Reset(void)
{
	OPM_Reset();

	ZeroMemory(&regs, sizeof(m68k_regs));
	regs.a[7] = regs.isp = (IPL[0x30001]<<24)|(IPL[0x30000]<<16)|(IPL[0x30003]<<8)|IPL[0x30002];
	regs.pc   = (IPL[0x30005]<<24)|(IPL[0x30004]<<16)|(IPL[0x30007]<<8)|IPL[0x30006];
	regs.sr_high = 0x27;
	M68KRESET();

	Memory_Init();
	CRTC_Init();
	DMA_Init();
	MFP_Init();
	FDC_Init();
	FDD_Reset();
	SASI_Init();
	SCSI_Init();
	IOC_Init();
	SCC_Init();
	PIA_Init();
	RTC_Init();
	TVRAM_Init();
	GVRAM_Init();
	BG_Init();
	Pal_Init();
	IRQH_Init();
	MIDI_Init();
	//WinDrv_Init();

	m68000_ICount = 0;
	m68000_ICountBk = 0;
	ICount = 0;

	DSound_Stop();
	SRAM_VirusCheck();
	//CDROM_Init();
	DSound_Play();

	return TRUE;
}


int
WinX68k_Init(void)
{

	IPL = (BYTE*)malloc(0x40000);
	MEM = (BYTE*)malloc(0xc00000);
	FONT = (BYTE*)malloc(0xc0000);

	if (MEM)
		ZeroMemory(MEM, 0xc00000);

	if (MEM && FONT && IPL)
		return TRUE;
	else
		return FALSE;
}

void
WinX68k_Cleanup(void)
{

	if (IPL) {
		free(IPL);
		IPL = 0;
	}
	if (MEM) {
		free(MEM);
		MEM = 0;
	}
	if (FONT) {
		free(FONT);
		FONT = 0;
	}
}

#define CLOCK_SLICE 200
// -----------------------------------------------------------------------------------
//  コアのめいんるーぷ
// -----------------------------------------------------------------------------------
void WinX68k_Exec(void)
{
	//char *test = NULL;
	int clk_total, clkdiv, usedclk, hsync, clk_next, clk_count, clk_line=0;
	int KeyIntCnt = 0, MouseIntCnt = 0;
	DWORD t_start = timeGetTime(), t_end;

	if ( FrameRate!=7 ) {
		DispFrame = (DispFrame+1)%FrameRate;
	} else {				// Auto Frame Skip
		if ( FrameSkipQueue ) {
			if ( FrameSkipCount>15 ) {
				FrameSkipCount = 0;
				FrameSkipQueue++;
				DispFrame = 0;
			} else {
				FrameSkipCount++;
				FrameSkipQueue--;
				DispFrame = 1;
			}
		} else {
			FrameSkipCount = 0;
			DispFrame = 0;
		}
	}

	vline = 0;
	clk_count = -ICount;
	clk_total = (CRTC_Regs[0x29] & 0x10) ? VSYNC_HIGH : VSYNC_NORM;
	if (Config.XVIMode == 1) {
		clk_total = (clk_total*16)/10;
		clkdiv = 16;
	} else if (Config.XVIMode == 2) {
		clk_total = (clk_total*24)/10;
		clkdiv = 24;
	} else {
		clkdiv = 10;
	}
	ICount += clk_total;
	clk_next = (clk_total/VLINE_TOTAL);
	hsync = 1;

	do {
		int m, n = (ICount>CLOCK_SLICE)?CLOCK_SLICE:ICount;
		m68000_ICount = m68000_ICountBk = 0;			// 割り込み発生前に与えておかないとダメ（CARAT）

		if ( hsync ) {
			hsync = 0;
			clk_line = 0;
			MFP_Int(0);
			if ( (vline>=CRTC_VSTART)&&(vline<CRTC_VEND) )
				VLINE = ((vline-CRTC_VSTART)*CRTC_VStep)/2;
			else
				VLINE = (DWORD)-1;
			if ( (!(MFP[MFP_AER]&0x40))&&(vline==CRTC_IntLine) )
				MFP_Int(1);
			if ( MFP[MFP_AER]&0x10 ) {
				if ( vline==CRTC_VSTART )
					MFP_Int(9);
			} else {
				if ( CRTC_VEND>=VLINE_TOTAL ) {
					if ( (long)vline==(CRTC_VEND-VLINE_TOTAL) ) MFP_Int(9);		// エキサイティングアワーとか（TOTAL<VEND）
				} else {
					if ( (long)vline==(VLINE_TOTAL-1) ) MFP_Int(9);			// クレイジークライマーはコレでないとダメ？
				}
			}
		}

#ifdef WIN68DEBUG
		if (traceflag/*&&fdctrace*/)
		{
			FILE *fp;
			static DWORD oldpc;
			int i;
			char buf[200];
			fp=fopen("_trace68.txt", "a");
			for (i=0; i<HSYNC_CLK; i++)
			{
				m68k_disassemble(buf, regs.pc);
//				if (MEM[0xa84c0]) /**test=1; */tracing=1000;
//				if (regs.pc==0x9d2a) tracing=5000;
//				if ((regs.pc>=0x2000)&&((regs.pc<=0x8e0e0))) tracing=50000;
//				if (regs.pc<0x10000) tracing=1;
//				if ( (regs.pc&1) )
//				fp=fopen("_trace68.txt", "a");
//				if ( (regs.pc==0x7176) /*&& (Memory_ReadW(oldpc)==0xff1a)*/ ) tracing=100;
//				if ( (/*((regs.pc>=0x27000) && (regs.pc<=0x29000))||*/((regs.pc>=0x27000) && (regs.pc<=0x29000))) && (oldpc!=regs.pc))
				if (/*fdctrace&&*/(oldpc!=regs.pc))
				{
//					//tracing--;
					fprintf(fp, "D0:%08X D1:%08X D2:%08X D3:%08X D4:%08X D5:%08X D6:%08X D7:%08X CR:%04X\n", regs.d[0], regs.d[1], regs.d[2], regs.d[3], regs.d[4], regs.d[5], regs.d[6], regs.d[7], regs.ccr);
					fprintf(fp, "A0:%08X A1:%08X A2:%08X A3:%08X A4:%08X A5:%08X A6:%08X A7:%08X SR:%04X\n", regs.a[0], regs.a[1], regs.a[2], regs.a[3], regs.a[4], regs.a[5], regs.a[6], regs.a[7], regs.sr_high);
					fprintf(fp, "<%04X> (%08X ->) %08X : %s\n", Memory_ReadW(regs.pc), oldpc, regs.pc, buf);
				}
				oldpc = regs.pc;
				m68000_ICount = 1;
				M68KRUN();
			}
			fclose(fp);
			usedclk = clk_line = HSYNC_CLK;
			clk_count = clk_next;
		}
		else
#endif
		{
			m68000_ICount = n;
			M68KRUN();
			m = (n-m68000_ICount-m68000_ICountBk);			// 経過クロック数
			ClkUsed += m*10;
			usedclk = ClkUsed/clkdiv;
			clk_line += usedclk;
			ClkUsed -= usedclk*clkdiv;
			ICount -= m;
			clk_count += m;
			m68000_ICount = m68000_ICountBk = 0;
		}

		MFP_Timer(usedclk);
		RTC_Timer(usedclk);
		DMA_Exec(0);
		DMA_Exec(1);
		DMA_Exec(2);

		if ( clk_count>=clk_next ) {
			//OPM_RomeoOut(Config.BufferSize*5);
			MIDI_DelayOut((Config.MIDIAutoDelay)?(Config.BufferSize*5):Config.MIDIDelay);
			MFP_TimerA();
			if ( (MFP[MFP_AER]&0x40)&&(vline==CRTC_IntLine) )
				MFP_Int(1);
			if ( (!DispFrame)&&(vline>=CRTC_VSTART)&&(vline<CRTC_VEND) ) {
				if ( CRTC_VStep==1 ) {				// HighReso 256dot（2度読み）
					if ( vline%2 )
						WinDraw_DrawLine();
				} else if ( CRTC_VStep==4 ) {		// LowReso 512dot
					WinDraw_DrawLine();				// 1走査線で2回描く（インターレース）
					VLINE++;
					WinDraw_DrawLine();
				} else {							// High 512dot / Low 256dot
					WinDraw_DrawLine();
				}
			}

			ADPCM_PreUpdate(clk_line);
			OPM_Timer(clk_line);
			MIDI_Timer(clk_line);
#ifndef	NO_MERCURY
			Mcry_PreUpdate(clk_line);
#endif

			KeyIntCnt++;
			if ( KeyIntCnt>(VLINE_TOTAL/4) ) {
				KeyIntCnt = 0;
				Keyboard_Int();
			}
			MouseIntCnt++;
			if ( MouseIntCnt>(VLINE_TOTAL/8) ) {
				MouseIntCnt = 0;
				SCC_IntCheck();
			}
			DSound_Send0(clk_line);			// Bufferがでかいとき用

			vline++;
			clk_next  = (clk_total*(vline+1))/VLINE_TOTAL;
			hsync = 1;
		}
	} while ( vline<VLINE_TOTAL );

	if ( CRTC_Mode&2 ) {		// FastClrビットの調整（PITAPAT）
		if ( CRTC_FastClr ) {	// FastClr=1 且つ CRTC_Mode&2 なら 終了
			CRTC_FastClr--;
			if ( !CRTC_FastClr )
				CRTC_Mode &= 0xfd;
		} else {				// FastClr開始
			if ( CRTC_Regs[0x29]&0x10 )
				CRTC_FastClr = 1;
			else
				CRTC_FastClr = 2;
			TVRAM_SetAllDirty();
			GVRAM_FastClear();
		}
	}

	Joystick_Update();
	DSound_Send();
	FDD_SetFDInt();
	if ( !DispFrame )
		WinDraw_Draw();
	TimerICount += clk_total;

	t_end = timeGetTime();
	if ( (int)(t_end-t_start)>((CRTC_Regs[0x29]&0x10)?14:16) ) {
		FrameSkipQueue += ((t_end-t_start)/((CRTC_Regs[0x29]&0x10)?14:16))+1;
		if ( FrameSkipQueue>100 )
			FrameSkipQueue = 100;
	}
}

/*
 * signal handler
 */
static sigfunc *
setup_signal(int signo, sigfunc *func)
{
	struct sigaction act, oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (sigaction(signo, &act, &oact) < 0)
		return SIG_ERR;
	return oact.sa_handler;
}

static void
sighandler(int signo)
{

	UNUSED(signo);

	gtk_main_quit();
}

/*
 * misc
 */
static void
set_window_size(GtkWidget *w)
{

	/* リサイズ不可 */
	gtk_window_set_policy(GTK_WINDOW(w), FALSE, FALSE, TRUE);
}

static void
set_icon_bitmap(GtkWidget *w)
{
	GdkPixmap *icon_pixmap;

	gdk_window_set_icon_name(w->window, APPNAME);
	icon_pixmap = gdk_bitmap_create_from_data(
	    w->window, keropi_mono_bits, keropi_mono_width, keropi_mono_height);
	gdk_window_set_icon(w->window, NULL, icon_pixmap, NULL);
}

//
// IDLE process
//
static gint
idle_process_splash(gpointer *p)
{

	UNUSED(p);

	// OPM_RomeoOut(Config.BufferSize * 5);
	if (NoWaitMode || Timer_GetCount()) {
		WinX68k_Exec();
		if (SplashFlag) {
			SplashFlag--;
			if (SplashFlag == 0)
				WinDraw_HideSplash();
		}
	}
	return TRUE;
}

static int idle_id = -1;

BOOL
is_installed_idle_process(void)
{

	return (idle_id == -1) ? FALSE : TRUE;
}

void
install_idle_process(void)
{

	idle_id = gtk_idle_add((GtkFunction)idle_process_splash, NULL);
}

void
uninstall_idle_process(void)
{

	gtk_idle_remove(idle_id);
	idle_id = -1;
}


//
// main
//
int
main(int argc, char *argv[])
{

	if (set_modulepath(winx68k_dir, sizeof(winx68k_dir)))
		return 1;

	dosio_init();
	file_setcd(winx68k_dir);

	LoadConfig();

	gtk_set_locale();
	gtk_rc_add_default_file(".xkeropirc");
	gtk_init(&argc, &argv);

	/* ウィンドウの作成 */
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	main_vbox = gtk_vbox_new(FALSE, 2);
	gtk_container_border_width(GTK_CONTAINER(main_vbox), 1);
	gtk_container_add(GTK_CONTAINER(window), main_vbox);
	gtk_widget_show(main_vbox);

	/* メニューバー	*/
	menubar = create_menu(window);
	gtk_box_pack_start(GTK_BOX(main_vbox), menubar, FALSE, TRUE, 0);
	gtk_widget_show(menubar);

	/* 画面領域 */
	drawarea = gtk_drawing_area_new();
	gtk_drawing_area_size(GTK_DRAWING_AREA(drawarea),
	    FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT);
	gtk_box_pack_start(GTK_BOX(main_vbox), drawarea, FALSE, TRUE, 0);
	gtk_widget_show(drawarea);

	gtk_widget_realize(window);
	gtk_widget_set_uposition(GTK_WIDGET(window), winx, winy);

	/* タイトルやらアイコンやらのセットアップ */
	gtk_window_set_title(GTK_WINDOW(window), APPNAME);
	set_window_size(window);
	set_icon_bitmap(window);

	SplashFlag = 20;
	SoundSampleRate = Config.SampleRate;

	StatBar_Show(Config.WindowFDDStat);
	WinDraw_ChangeSize();
	WinDraw_ChangeMode(FALSE);

	WinUI_Init();
	WinDraw_StartupScreen();

	if (!WinDraw_Init()) {
		WinDraw_Cleanup();
		Error("Error: Can't init screen.\n");
		return 1;
	}

	/* window 表示 */
	gtk_widget_show(window);

	if (!WinX68k_Init()) {
		WinX68k_Cleanup();
		WinDraw_Cleanup();
		return 1;
	}

	if (!WinX68k_LoadROMs()) {
		WinX68k_Cleanup();
		WinDraw_Cleanup();
		return 1;
	}

	if ( SoundSampleRate ) {
		ADPCM_Init(SoundSampleRate);
		OPM_Init(4000000/*3579545*/, SoundSampleRate);
#ifndef	NO_MERCURY
		Mcry_Init(SoundSampleRate, winx68k_dir);
#endif
	} else {
		ADPCM_Init(100);
		OPM_Init(4000000/*3579545*/, 100);
#ifndef	NO_MERCURY
		Mcry_Init(100, winx68k_dir);
#endif
	}

	Mouse_Init();
	Joystick_Init();
	SRAM_Init();
	WinX68k_Reset();
	Timer_Init();

	MIDI_Init();
	MIDI_SetMimpiMap(Config.ToneMapFile);	// 音色設定ファイル使用反映
	MIDI_EnableMimpiDef(Config.ToneMap);

	if (!DSound_Init(Config.SampleRate, Config.BufferSize)) {
		if (Config.DSAlert)
			fprintf(stderr, "Can't init sound.\n");
	}

	ADPCM_SetVolume((BYTE)Config.PCM_VOL);
	OPM_SetVolume((BYTE)Config.OPM_VOL);
#ifndef	NO_MERCURY
	Mcry_SetVolume((BYTE)Config.MCR_VOL);
#endif
	DSound_Play();

	//SetCmdLineFD();	// コマンドラインでFD挿入を指示している場合
	switch (argc) {
	case 3:
		FDD_SetFD(1, argv[2], 0);
	case 2:
		FDD_SetFD(0, argv[1], 0);
		break;
	}

	setup_signal(SIGINT, sighandler);
	setup_signal(SIGTERM, sighandler);

	/* メインループ */
	install_idle_process();
	gtk_main();
	uninstall_idle_process();

	Memory_WriteB(0xe8e00d, 0x31);	// SRAM書き込み許可
	Memory_WriteD(0xed0040, Memory_ReadD(0xed0040)+1); // 積算稼働時間(min.)
	Memory_WriteD(0xed0044, Memory_ReadD(0xed0044)+1); // 積算起動回数

	OPM_Cleanup();
#ifndef	NO_MERCURY
	Mcry_Cleanup();
#endif

	SRAM_Cleanup();
	FDD_Cleanup();
	//CDROM_Cleanup();
	MIDI_Cleanup();
	Joystick_Cleanup();
	DSound_Cleanup();
	WinX68k_Cleanup();
	WinDraw_Cleanup();
	WinDraw_CleanupScreen();

	SaveConfig();

	return 0;
}
