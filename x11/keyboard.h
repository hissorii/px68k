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

void Keyboard_Init(void);
void Keyboard_KeyDown(DWORD vkcode);
void Keyboard_KeyUp(DWORD vkcode);
void Keyboard_Int(void);

#endif //_winx68k_keyboard
