#ifndef _winx68k_mouse
#define _winx68k_mouse

#include "common.h"

extern	int	MousePosX;
extern	int	MousePosY;
extern	BYTE	MouseStat;
extern	BYTE	MouseSW;

void Mouse_Init(void);
void Mouse_Event(int wparam, float dx, float dy);
void Mouse_SetData(void);
void Mouse_StartCapture(int flag);
void Mouse_ChangePos(void);

#endif //_winx68k_mouse
