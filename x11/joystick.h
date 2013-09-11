#ifndef winx68k_joy_h
#define winx68k_joy_h

#include "common.h"

#define	JOY_UP		0x01
#define	JOY_DOWN	0x02
#define	JOY_LEFT	0x04
#define	JOY_RIGHT	0x08
#define	JOY_TRG2	0x20
#define	JOY_TRG1	0x40

#define	JOY_TRG5	0x01
#define	JOY_TRG4	0x02
#define	JOY_TRG3	0x04
#define	JOY_TRG7	0x08
#define	JOY_TRG8	0x20
#define	JOY_TRG6	0x40

void Joystick_Init(void);
void Joystick_Cleanup(void);
void Joystick_Activate(UINT wParam);
BYTE FASTCALL Joystick_Read(BYTE num);
void FASTCALL Joystick_Write(BYTE num, BYTE data);
void FASTCALL Joystick_Update(void);

extern BYTE JoyKeyState;

#ifdef ANDROID
#include <SDL.h>

#define VBTN_MAX 32
#define VBTN_ON 2
#define VBTN_OFF 1
#define VBTN_NOUSE 0
#define FINGER_MAX 10

typedef struct _vbtn_rect {
	float x;
	float y;
	float x2;
	float y2;
} VBTN_RECT;

VBTN_RECT vbtn_rect[VBTN_MAX];
BYTE vbtn_state[VBTN_MAX];
int vbtn_width;
int vbtn_height;

#endif

#endif
