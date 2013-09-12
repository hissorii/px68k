#ifndef _winx68k_winui_h
#define _winx68k_winui_h

extern	BYTE	Debug_Text, Debug_Grp, Debug_Sp;
extern	DWORD	LastClock[4];

void WinUI_Init(void);
int WinUI_Menu(void);

#define WUM_MENU_END 1
#define WUM_EMU_QUIT 2

enum MenuState {ms_key, ms_value, ms_file};

#define MFL_MAX 1000

struct menu_flist {
	char name[MFL_MAX][MAX_PATH];
	char type[MFL_MAX];
	char dir[2][MAX_PATH];
	int ptr;
	int num;
	int y;
};

extern char menu_item_key[][15];
extern char menu_items[][10][30];

#ifndef _winx68k_gtkui_h
#define _winx68k_gtkui_h
#endif //winx68k_gtkui_h
#endif //winx68k_winui_h
