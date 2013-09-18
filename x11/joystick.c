// JOYSTICK.C - ジョイスティックサポート for WinX68k
// DInputの初期化／終了と、ジョイスティックポートチェック

#include "common.h"
#include "prop.h"
#include "joystick.h"
#if defined(PSP)
#include <pspctrl.h>
#elif defined(ANDROID)
#include <SDL.h>
#include <android/log.h>
#else
#include <SDL.h> // for SDLK_...
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

// ボタンが押されたかを記憶。押しっぱなしでリピートしたくない場合に使用
BYTE JoyDownState0;

#ifdef PSP
DWORD JoyStatePSP;
#endif
BYTE JoyPortData[2];

#ifdef ANDROID

#define VBTN_MAX 32
#define VBTN_ON 2
#define VBTN_OFF 1
#define VBTN_NOUSE 0
#define FINGER_MAX 10

// SDL_FINGERと比較するので範囲は0~1.0
typedef struct _vbtn_rect {
	float x;
	float y;
	float x2;
	float y2;
} VBTN_RECT;

VBTN_RECT vbtn_rect[VBTN_MAX];
BYTE vbtn_state[VBTN_MAX];
SDL_TouchID touchId = -1;

#endif

#define SET_VBTN(id, bx, by)					\
{								\
	vbtn_state[id] = VBTN_OFF;				\
	vbtn_rect[id].x = (float)(bx) / 800.0;			\
	vbtn_rect[id].y = (float)(by) / 600.0;			\
	vbtn_rect[id].x2 = ((float)(bx) + 32.0) / 800.0;	\
	vbtn_rect[id].y2 = ((float)(by) + 32.0) / 600.0;	\
}

void Joystick_Init(void)
{
	joy[0] = 1;  // とりあえず一つ目だけ有効
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
#ifdef ANDROID
	int i;

	for (i = 0; i < VBTN_MAX; i++) {
		vbtn_state[i] = VBTN_NOUSE;
	}

	//テクスチャの設定といっしょにしたいがとりあえず別々に設定
	//ボタンのTexture idは1 origin、ボタンidは0 originなので注意
	// 左右上下 (上上下下左右左右BAではない)
	SET_VBTN(0, 20, 450);
	SET_VBTN(1, 100, 450);
	SET_VBTN(2, 60, 400);
	SET_VBTN(3, 60, 500);

	// ボタン
	SET_VBTN(4, 680, 450);
	SET_VBTN(5, 750, 450);
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
	int num = 0; //xxx とりあえずJOY1のみ。
	static BYTE pre_ret0 = 0xff;

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
	pre_ret0 = ret0;

	JoyStatePSP = psppad.Buttons;

	JoyState0[num] = ret0;
	JoyState1[num] = ret1;
#elif defined(ANDROID)
	SDL_Finger *finger;
	SDL_FingerID fid;
	float fx, fy;
	int i, j;

	if (touchId == -1)
		return;

	// 使用中の物は全てオフにリセットする
	for (i = 0; i < VBTN_MAX; i++) {
		if (vbtn_state[i] != VBTN_NOUSE) {
			vbtn_state[i] = VBTN_OFF;
		}
	}

	// この瞬間押されているボタンだけをオンにする
	for (i = 0; i < FINGER_MAX; i++) {
		finger = SDL_GetTouchFinger(touchId, i);
		if (!finger)
			continue;

		fx = finger->x;
		fy = finger->y;

		//__android_log_print(ANDROID_LOG_DEBUG,"Tag","id: %d x: %f y: %f", i, fx, fy);

		for (j = 0; j < VBTN_MAX; j++) {
			if (vbtn_state[j] == VBTN_NOUSE)
				continue;
			// 性能を考え一個ずつ判定
			if (vbtn_rect[j].x > fx)
				continue;
			if (vbtn_rect[j].x2 < fx)
				continue;
			if (vbtn_rect[j].y > fy)
				continue;
			if (vbtn_rect[j].y2 < fy)
				continue;

			//マッチしたらオンにする
			vbtn_state[j] = VBTN_ON;
			//仮想ボタンは重ならない
			break;
		}
	}

	BYTE ret0 = 0xff, ret1 = 0xff;
	int num = 0; //xxx とりあえずJOY1のみ。

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
		ret0 ^= JOY_TRG1;
	}
	if (vbtn_state[5] == VBTN_ON) {
		ret0 ^= JOY_TRG2;
	}

	JoyState0[num] = ret0;
	JoyState1[num] = ret1;
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

#ifdef PSP

int PspPad_Start(void)
{
	return (JoyStatePSP & PSP_CTRL_START)? 1 : 0;
}	  
#endif
