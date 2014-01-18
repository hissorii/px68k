// JOYSTICK.C - joystick support for WinX68k

#include "common.h"
#include "prop.h"
#include "joystick.h"
#include "winui.h"
#ifdef PSP
#include <pspctrl.h>
#include <keyboard.h>
#else
#include <SDL.h>
#endif
#if defined(ANDROID) || TARGET_OS_IPHONE
#include "mouse.h"
#endif

#if 0
LPDIRECTINPUT		dinput = NULL;
#endif

#ifndef MAX_BUTTON
#define MAX_BUTTON 32
#endif

char joyname[2][MAX_PATH];
char joybtnname[2][MAX_BUTTON][MAX_PATH];
BYTE joybtnnum[2] = {0, 0};

//static	int		joyactive = 0;
BYTE joy[2];
BYTE JoyKeyState;
BYTE JoyKeyState0;
BYTE JoyKeyState1;
BYTE JoyState0[2];
BYTE JoyState1[2];

// This stores whether the buttons were down. This avoids key repeats.
BYTE JoyDownState0;

// This stores whether the buttons were up. This avoids key repeats.
BYTE JoyUpState0;

#ifdef PSP
DWORD JoyDownStatePSP;
BYTE JoyAnaPadX;
BYTE JoyAnaPadY;
#endif
BYTE JoyPortData[2];

#if defined(ANDROID) || TARGET_OS_IPHONE

#define VBTN_MAX 8
#define VBTN_NOUSE 0
#define FINGER_MAX 10

// The value is (0..1.0), which compares with SDL_FINGER.
typedef struct _vbtn_rect {
	float x;
	float y;
	float x2;
	float y2;
} VBTN_RECT;

VBTN_RECT vbtn_rect[VBTN_MAX];
BYTE vbtn_state[VBTN_MAX];
SDL_TouchID touchId = -1;

#define SET_VBTN(id, bx, by, s)					\
{								\
	vbtn_state[id] = VBTN_OFF;				\
	vbtn_rect[id].x = (float)(bx) / 800.0;			\
	vbtn_rect[id].y = (float)(by) / 600.0;			\
	vbtn_rect[id].x2 = ((float)(bx) + 32.0*(s)) / 800.0;	\
	vbtn_rect[id].y2 = ((float)(by) + 32.0*(s)) / 600.0;	\
}

// base points of the virtual keys
VBTN_POINTS vbtn_points[] = {
	{20, 450}, {100, 450}, {60, 400}, {60, 500}, // d-pad
	{680, 450}, {750, 450}, // pad button
	{768, 0}, // software keyboard button
	{768, 52} // menu button
};

// fixed coordinates, which are fixed with any scalings.
#define VKEY_L_X 20 // x of the left d-pad key
#define VKEY_D_Y 532 // y of the base of the down d-pad key
#define VKEY_R_X 782 // x of the right side of the right pad button
#define VKEY_K_X 800 // x of the right side of keyboard button

// correction value for fixing keys
#define VKEY_DLX(scale) (VKEY_L_X * (scale) - VKEY_L_X)
#define VKEY_DDY(scale) (VKEY_D_Y * (scale) - VKEY_D_Y)
#define VKEY_DRX(scale) (VKEY_R_X * (scale) - VKEY_R_X)
#define VKEY_DKX(scale) (VKEY_K_X * (scale) - VKEY_K_X)

VBTN_POINTS scaled_vbtn_points[sizeof(vbtn_points)/sizeof(VBTN_POINTS)];

VBTN_POINTS *Joystick_get_btn_points(float scale)
{
	int i;

	for (i = 0; i < 4; i++) {
		scaled_vbtn_points[i].x
			= -VKEY_DLX(scale) + scale * vbtn_points[i].x;
		scaled_vbtn_points[i].y
			= -VKEY_DDY(scale) + scale * vbtn_points[i].y;
	}
	for (i = 4; i < 6; i++) {
		scaled_vbtn_points[i].x
			= -VKEY_DRX(scale) + scale * vbtn_points[i].x;
		scaled_vbtn_points[i].y
			= -VKEY_DDY(scale) + scale * vbtn_points[i].y;
	}

	// keyboard button
	scaled_vbtn_points[i].x
		= -VKEY_DKX(scale) + scale * vbtn_points[i].x;
	scaled_vbtn_points[i].y
		= 0;

	i++;

	// menu button
	scaled_vbtn_points[i].x
		= -VKEY_DKX(scale) + scale * vbtn_points[i].x;
	scaled_vbtn_points[i].y
		= 0 + scale * vbtn_points[i].y;

	return scaled_vbtn_points;
}

void Joystick_Vbtn_Update(float scale)
{
	int i;
	VBTN_POINTS *p;

	p = Joystick_get_btn_points(scale);

	for (i = 0; i < VBTN_MAX; i++) {
		vbtn_state[i] = VBTN_NOUSE;
		//p6logd("id: %d x: %f y: %f", i, p->x, p->y);
		SET_VBTN(i, p->x, p->y, scale);
		p++;
	}
}

BYTE Joystick_get_vbtn_state(WORD n)
{
	return vbtn_state[n];
}

#endif

void Joystick_Init(void)
{
	joy[0] = 1;  // active only one
	joy[1] = 0;
	JoyKeyState = 0;
	JoyKeyState0 = 0;
	JoyKeyState1 = 0;
	JoyState0[0] = 0xff;
	JoyState0[1] = 0xff;
	JoyState1[0] = 0xff;
	JoyState1[1] = 0xff;
	JoyPortData[0] = 0;
	JoyPortData[1] = 0;

#if defined(ANDROID) || TARGET_OS_IPHONE
	Joystick_Vbtn_Update(WinUI_get_vkscale());
#endif
}

void Joystick_Cleanup(void)
{
#if 0
	if (joy[0]) IDirectInputDevice2_Release(joy[0]);
	if (joy[1]) IDirectInputDevice2_Release(joy[1]);
	if (dinput) IDirectInput_Release(dinput);
#endif
}


void Joystick_Activate(UINT wParam)
{
#if 1
	(void)wParam;
#else
	if (wParam != WA_INACTIVE)
	{
		if (joy[0]) IDirectInputDevice2_Acquire(joy[0]);
		if (joy[1]) IDirectInputDevice2_Acquire(joy[1]);
	}
	else
	{
		if (joy[0]) IDirectInputDevice2_Unacquire(joy[0]);
		if (joy[1]) IDirectInputDevice2_Unacquire(joy[1]);
	}
#endif
}


BYTE FASTCALL Joystick_Read(BYTE num)
{
	BYTE joynum = num;
	BYTE ret0 = 0xff, ret1 = 0xff, ret;

	(void)joynum;
	(void)ret0;
	(void)ret1;
	ret = 0xff;

	Config.JoyKey = 1;

	if (Config.JoySwap) joynum ^= 1;
	if (joy[num]) {
		ret0 = JoyState0[num];
		ret1 = JoyState1[num];
	}

	if (Config.JoyKey)
	{
		if ((Config.JoyKeyJoy2)&&(num==1))
			ret0 ^= JoyKeyState;
		else if ((!Config.JoyKeyJoy2)&&(num==0))
			ret0 ^= JoyKeyState;
	}

	ret = ((~JoyPortData[num])&ret0)|(JoyPortData[num]&ret1);

	if (ret0 != 0xff || ret != 0xff)
		printf("ret0: 0x%x, ret: 0x%x\n", ret0, ret);

	return ret;
}


void FASTCALL Joystick_Write(BYTE num, BYTE data)
{
	if ( (num==0)||(num==1) ) JoyPortData[num] = data;
}

void FASTCALL Joystick_Update(void)
{
#if defined(PSP)
	BYTE ret0 = 0xff, ret1 = 0xff;
	int num = 0; //xxx active only one
	static BYTE pre_ret0 = 0xff;
	static DWORD button_down = 0;
	DWORD button_changing;

	SceCtrlData psppad;
	sceCtrlPeekBufferPositive(&psppad, 1);

	if (psppad.Buttons & PSP_CTRL_LEFT) {
		ret0 ^= JOY_LEFT;
	}
	if (psppad.Buttons & PSP_CTRL_RIGHT) {
		ret0 ^= JOY_RIGHT;
	}
	if (psppad.Buttons & PSP_CTRL_UP) {
		ret0 ^= JOY_UP;
	}
	if (psppad.Buttons & PSP_CTRL_DOWN) {
		ret0 ^= JOY_DOWN;
	}
	if (psppad.Buttons & PSP_CTRL_CIRCLE) {
		ret0 ^= JOY_TRG1;
	}
	if (psppad.Buttons & PSP_CTRL_CROSS) {
		ret0 ^= JOY_TRG2;
	}

	JoyDownState0 = ~(ret0 ^ pre_ret0) | ret0;
	JoyUpState0 = (ret0 ^ pre_ret0) & ret0;
	pre_ret0 = ret0;

	// up the bits which changed the states
	button_changing = psppad.Buttons ^ button_down;
	// first down = changing the state & down now
	JoyDownStatePSP = button_changing & psppad.Buttons;
	// invert the bits which changed the states
	button_down ^= button_changing;

	// save the states of Analog pad
	JoyAnaPadX = psppad.Lx;
	JoyAnaPadY = psppad.Ly;

	// disable Joystick when software keyboard is active
	if (!Keyboard_IsSwKeyboard()) {
		JoyState0[num] = ret0;
		JoyState1[num] = ret1;
	}
#elif defined(ANDROID) || TARGET_OS_IPHONE
	BYTE ret0 = 0xff, ret1 = 0xff;
	int num = 0; //xxx only joy1
	static BYTE pre_ret0 = 0xff;

	SDL_Finger *finger;
	SDL_FingerID fid;
	float fx, fy;
	int i, j;
	float scale, asb_x, asb_y; // play x, play y of a button

	if (touchId == -1)
		return;

	// all active buttons are set to off
	for (i = 0; i < VBTN_MAX; i++) {
		if (vbtn_state[i] != VBTN_NOUSE) {
			vbtn_state[i] = VBTN_OFF;
		}
	}

	// A play of the button changes size according to scale.
	scale = WinUI_get_vkscale();
	asb_x = (float)20 * scale / 800.0;
	asb_y = (float)20 * scale / 600.0;

	// set the button on, only which is pushed just now
	for (i = 0; i < FINGER_MAX; i++) {
		finger = SDL_GetTouchFinger(touchId, i);
		if (!finger)
			continue;

		fx = finger->x;
		fy = finger->y;

		//p6logd("id: %d x: %f y: %f", i, fx, fy);

		for (j = 0; j < VBTN_MAX; j++) {
			if (vbtn_state[j] == VBTN_NOUSE)
				continue;

			if (vbtn_rect[j].x - asb_x > fx)
				continue;
			if (vbtn_rect[j].x2 + asb_x < fx)
				continue;
			if (vbtn_rect[j].y - asb_y > fy)
				continue;
			if (vbtn_rect[j].y2 + asb_y < fy)
				continue;

			vbtn_state[j] = VBTN_ON;
			// The buttons don't overlap.
			break;
		}
	}

	if (vbtn_state[0] == VBTN_ON) {
		ret0 ^= JOY_LEFT;
	}
	if (vbtn_state[1] == VBTN_ON) {
		ret0 ^= JOY_RIGHT;
	}
	if (vbtn_state[2] == VBTN_ON) {
		ret0 ^= JOY_UP;
	}
	if (vbtn_state[3] == VBTN_ON) {
		ret0 ^= JOY_DOWN;
	}
	if (vbtn_state[4] == VBTN_ON) {
		ret0 ^= (Config.VbtnSwap == 0)? JOY_TRG1 : JOY_TRG2;
	}
	if (vbtn_state[5] == VBTN_ON) {
		ret0 ^= (Config.VbtnSwap == 0)? JOY_TRG2 : JOY_TRG1;
	}

	JoyDownState0 = ~(ret0 ^ pre_ret0) | ret0;
	JoyUpState0 = (ret0 ^ pre_ret0) & ret0;
	pre_ret0 = ret0;

#ifdef USE_OGLES11
	// disable Joystick when software keyboard or mouse is active
	if (!Keyboard_IsSwKeyboard() && !Config.JoyOrMouse) {
		JoyState0[num] = ret0;
		JoyState1[num] = ret1;
	}
	// update the states of the mouse buttons
	// when sw keyboard is inactive and mouse is active.
	// state is updated when menu is open, but don't care
	// because emulation doesn't work.
	if (!Keyboard_IsSwKeyboard() && Config.JoyOrMouse) {
		if (!(JoyDownState0 & JOY_TRG1) | (JoyUpState0 & JOY_TRG1)) {
			printf("mouse btn1 event\n");
			Mouse_Event(1, (JoyUpState0 & JOY_TRG1)? 0 : 1.0, 0);
		}
		if (!(JoyDownState0 & JOY_TRG2) | (JoyUpState0 & JOY_TRG2)) {
			printf("mouse btn2 event\n");
			Mouse_Event(2, (JoyUpState0 & JOY_TRG2)? 0 : 1.0, 0);
		}
	}

#endif

#endif
}

#if !defined(PSP) && !defined(ANDROID)
void menukey_update(signed int key)
{
	BYTE ret0 = 0xff;
	switch (key) {
	case SDLK_UP :
		ret0 ^= JOY_UP;
		break;
	case SDLK_DOWN:
		ret0 ^= JOY_DOWN;
		break;
	case SDLK_LEFT:
		ret0 ^= JOY_LEFT;
		break;
	case SDLK_RIGHT:
		ret0 ^= JOY_RIGHT;
		break;
	case SDLK_RETURN:
		ret0 ^= JOY_TRG1;
		break;
	case SDLK_ESCAPE:
		ret0 ^= JOY_TRG2;
		break;

	}
	JoyDownState0 = ret0;

}
#endif

BYTE get_joy_downstate(void)
{
	return JoyDownState0;
}
void reset_joy_downstate(void)
{
	JoyDownState0 = 0xff;
}
BYTE get_joy_upstate(void)
{
	return JoyUpState0;
}
void reset_joy_upstate(void)
{
	JoyUpState0 = 0x00;
}


#ifdef PSP
DWORD Joystick_get_downstate_psp(DWORD ctrl_bit)
{
	return (JoyDownStatePSP & ctrl_bit);
}	  
void Joystick_reset_downstate_psp(DWORD ctrl_bit)
{
	JoyDownStatePSP ^= ctrl_bit;
}	  

void Joystatic_reset_anapad_psp(void)
{
	JoyAnaPadX = JoyAnaPadY = 128;
}

void _get_anapad_sub(BYTE av, int *v, int max)
{
	// accelerate accroding to an angle of the stick
	if (av > 255 / 2 + 32) {
		(*v)++; // move a little bit
		if (av > 255 - 5) {
			(*v)++; // accelerate
			if (av == 255) {
				*v += 2; // more accelerate
			}
		}
		if (*v > max) {
			*v = max;
		}
	}
	if (av < 255 / 2 - 32) {
		(*v)--;
		if (av < 5) {
			(*v)--;
			if (av == 0) {
				*v -= 2;
			}
		}
		if (*v < 0) {
			*v = 0;
		}
	}
}

void Joystick_mv_anapad_psp(void)
{
	_get_anapad_sub(JoyAnaPadX, &kbd_x, 450);
	_get_anapad_sub(JoyAnaPadY, &kbd_y, 250);
}
#endif

