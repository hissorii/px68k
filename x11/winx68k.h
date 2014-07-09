#ifndef winx68k_wincore_h
#define winx68k_wincore_h

#include "common.h"

#ifdef RFMDRV
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

extern int rfd_sock;
#endif

#define vline HOGEvline // workaround for redefinition of 'vline'

#define		SCREEN_WIDTH		768
#define		SCREEN_HEIGHT		512

#define		FULLSCREEN_WIDTH	800
#define		FULLSCREEN_HEIGHT	600
#define		FULLSCREEN_POSX		((FULLSCREEN_WIDTH - SCREEN_WIDTH) / 2)
#define		FULLSCREEN_POSY		((FULLSCREEN_HEIGHT - SCREEN_HEIGHT) / 2)

#define TOSTR(s) #s
#define _TOSTR(s) TOSTR(s)
#define PX68KVERSTR _TOSTR(PX68K_VERSION)

extern	BYTE*	FONT;

extern	WORD	VLINE_TOTAL;
extern	DWORD	VLINE;
extern	DWORD	vline;

extern	char	winx68k_dir[MAX_PATH];
extern	char	winx68k_ini[MAX_PATH];
extern	int	BIOS030Flag;
extern	BYTE	FrameChanged;

extern const BYTE PrgTitle[];

#if defined(ANDROID) || TARGET_OS_IPHONE
extern int realdisp_w, realdisp_h;
#endif

int WinX68k_Reset(void);

#ifndef	winx68k_gtkwarpper_h
#define	winx68k_gtkwarpper_h

#include <unistd.h>
#include <signal.h>

#if 0
#include <gdk/gdk.h>
#include <gtk/gtk.h>

extern GtkWidget *window;
extern GtkWidget *main_vbox;
extern GtkWidget *menubar;
extern GtkWidget *drawarea;
extern GdkPixmap *splash_pixmap;
#endif

BOOL is_installed_idle_process(void);
void install_idle_process(void);
void uninstall_idle_process(void);

#define	NELEMENTS(array)	((int)(sizeof(array) / sizeof(array[0])))

#endif //winx68k_gtkwarpper_h

#endif //winx68k_wincore_h
