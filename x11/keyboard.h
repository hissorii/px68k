#ifndef _winx68k_keyboard
#define _winx68k_keyboard

#include "common.h"

#define KeyBufSize 128

extern	BYTE	KeyBuf[KeyBufSize];
extern	BYTE	KeyBufWP;
extern	BYTE	KeyBufRP;
extern	BYTE	KeyTable[512];
extern	BYTE	KeyTableMaster[512];
extern	BYTE	KeyEnable;
extern	BYTE	KeyIntFlag;

struct keyboard_key {
	int x;
	int y;
	int w;
	int h;
	char *s;
	unsigned char c;
};

extern struct keyboard_key kbd_key[];
extern int  kbd_kx, kbd_ky;
extern int kbd_x, kbd_y, kbd_w, kbd_h;

void Keyboard_Init(void);
void Keyboard_KeyDown(DWORD vkcode);
void Keyboard_KeyUp(DWORD vkcode);
void Keyboard_Int(void);
void send_keycode(BYTE code, int flag);
int Keyboard_get_key_ptr(int x, int y);
void Keyboard_skbd(void);
int Keyboard_IsSwKeyboard(void);
void Keyboard_ToggleSkbd(void);

#endif //_winx68k_keyboard
