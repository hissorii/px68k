// JOYSTICK.C - ジョイスティックサポート for WinX68k
// DInputの初期化／終了と、ジョイスティックポートチェック

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

// ボタンが「押された」かを記憶。押しっぱなしでリピートしたくない場合に使用
BYTE JoyDownState0;

// ボタンが「放された」かを記憶。押しっぱなしでリピートしたくない場合に使用
BYTE JoyUpState0;

#ifdef PSP
DWORD JoyDownStatePSP;
BYTE JoyAnaPadX;
BYTE JoyAnaPadY;
#endif
BYTE JoyPortData[2];

#ifdef ANDROID

#define VBTN_MAX 7
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

#define SET_VBTN(id, bx, by, s)					\
{								\
	vbtn_state[id] = VBTN_OFF;				\
	vbtn_rect[id].x = (float)(bx) / 800.0;			\
	vbtn_rect[id].y = (float)(by) / 600.0;			\
	vbtn_rect[id].x2 = ((float)(bx) + 32.0*(s)) / 800.0;	\
	vbtn_rect[id].y2 = ((float)(by) + 32.0*(s)) / 600.0;	\
}

// 基準となる仮想キーの位置
VBTN_POINTS vbtn_points[] = {
	{20, 450}, {100, 450}, {60, 400}, {60, 500}, // 仮想d-pad
	{680, 450}, {750, 450}, // 仮想ボタン
	{768, 0} // ソフトウェアキーボードonボタン (右上隅/invisible)
};

// これらの座標を常に固定して、スケーリングする
#define VKEY_L_X 20 //一番左の仮想キーのX座標
#define VKEY_D_Y 532 //一番下の仮想キーの底辺のY座標
#define VKEY_R_X 782 // 一番右の仮想キーの右辺のX座標
#define VKEY_K_X 800 // キーボードonボタンの右辺のX座標

// キー固定用補正値
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

	// キーボードonスイッチ
	scaled_vbtn_points[i].x
		= -VKEY_DKX(scale) + scale * vbtn_points[i].x;
	scaled_vbtn_points[i].y
		= 0;

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
	int num = 0; //xxx とりあえずJOY1のみ。
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

	// 前回と変化のあったbitを立てる
	button_changing = psppad.Buttons ^ button_down;
	// 今回初めて押された = 前回と変化がある & 今ボタンが押されている
	JoyDownStatePSP = button_changing & psppad.Buttons;
	// 変化のあったbitを反転させる
	button_down ^= button_changing;

	// Analog padの情報を保存
	JoyAnaPadX = psppad.Lx;
	JoyAnaPadY = psppad.Ly;

	// ソフトウェアキーボードを出しているときにはJoystick無効
	if (!Keyboard_IsSwKeyboard()) {
		JoyState0[num] = ret0;
		JoyState1[num] = ret1;
	}
#elif defined(ANDROID)
	BYTE ret0 = 0xff, ret1 = 0xff;
	int num = 0; //xxx とりあえずJOY1のみ。
	static BYTE pre_ret0 = 0xff;

	SDL_Finger *finger;
	SDL_FingerID fid;
	float fx, fy;
	int i, j;
	float scale, asb_x, asb_y; // あそびx, あそびy

	if (touchId == -1)
		return;

	// 使用中の物は全てオフにリセットする
	for (i = 0; i < VBTN_MAX; i++) {
		if (vbtn_state[i] != VBTN_NOUSE) {
			vbtn_state[i] = VBTN_OFF;
		}
	}

	// 仮想キーの大きさに従ってあそびも大きくする
	scale = WinUI_get_vkscale();
	asb_x = (float)20 * scale / 800.0;
	asb_y = (float)20 * scale / 600.0;

	// この瞬間押されているボタンだけをオンにする
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
			// 性能を考え一個ずつ判定。少し遊びをもたせる。
			if (vbtn_rect[j].x - asb_x > fx)
				continue;
			if (vbtn_rect[j].x2 + asb_x < fx)
				continue;
			if (vbtn_rect[j].y - asb_y > fy)
				continue;
			if (vbtn_rect[j].y2 + asb_y < fy)
				continue;

			//マッチしたらオンにする
			vbtn_state[j] = VBTN_ON;
			//仮想ボタンは重ならない
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
	// ソフトウェアキーボードを出しているときにはJoystick無効
	if (!Keyboard_IsSwKeyboard()) {
		JoyState0[num] = ret0;
		JoyState1[num] = ret1;
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
	// アナログスティックの倒し加減で加速させる
	if (av > 255 / 2 + 32) {
		(*v)++; // ちょい動かし
		if (av > 255 - 5) {
			(*v)++; // 加速
			if (av == 255) {
				*v += 2; // 最加速
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

