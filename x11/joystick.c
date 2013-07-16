// JOYSTICK.C - ジョイスティックサポート for WinX68k
// DInputの初期化／終了と、ジョイスティックポートチェック

#include "common.h"
#include "prop.h"
#include "joystick.h"

#if 0
LPDIRECTINPUT		dinput = NULL;
LPDIRECTINPUTDEVICE2	joy[2] = {NULL, NULL};
#endif

#ifndef MAX_BUTTON
#define MAX_BUTTON 32
#endif

char joyname[2][MAX_PATH];
char joybtnname[2][MAX_BUTTON][MAX_PATH];
BYTE joybtnnum[2] = {0, 0};

//static	int		joyactive = 0;
BYTE JoyKeyState = 0;
BYTE JoyKeyState0 = 0;
BYTE JoyKeyState1 = 0;
BYTE JoyState0[2] = {0xff, 0xff};
BYTE JoyState1[2] = {0xff, 0xff};
BYTE JoyPortData[2] = {0, 0};


#if 0
BOOL CALLBACK EnumButtonProc(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pv)
{
	BYTE j = *(BYTE*)pv;
	if (joybtnnum[j]<MAX_BUTTON)
	{
		strcpy(joybtnname[j][joybtnnum[j]], lpddoi->tszName);
		joybtnnum[j]++;
		return DIENUM_CONTINUE;
	}
	else
		return DIENUM_STOP;
}
 

BOOL CALLBACK JoyEnumProc(LPDIDEVICEINSTANCE pdidi, LPVOID pv)
{
	GUID *guid = NULL;
	LPDIRECTINPUTDEVICE tmpdev = NULL;

	guid = (GUID *)pv;
	*guid = pdidi->guidInstance;

	if (!joy[0])
	{
		if ((IDirectInput_CreateDevice(dinput, guid, &tmpdev, NULL)) == DI_OK)
		{
			if ((IDirectInputDevice_QueryInterface(tmpdev, &IID_IDirectInputDevice2, &joy[0])) == DI_OK)
			{
				IDirectInputDevice_Release(tmpdev);
				return DIENUM_CONTINUE;
			}
			else
			{
				joy[0] = NULL;
				return DIENUM_CONTINUE;
			}
		}
		else
		{
			joy[0] = NULL;
			return DIENUM_CONTINUE;
		}
	}
	if (!joy[1])
	{
		if ((IDirectInput_CreateDevice(dinput, guid, &tmpdev, NULL)) == DI_OK)
		{
			if ((IDirectInputDevice_QueryInterface(tmpdev, &IID_IDirectInputDevice2, &joy[1])) == DI_OK)
			{
				IDirectInputDevice_Release(tmpdev);
				return DIENUM_STOP;
			}
			else
			{
				joy[1] = NULL;
				return DIENUM_CONTINUE;
			}
		}
		else
		{
			joy[1] = NULL;
			return DIENUM_CONTINUE;
		}
	}
	return DIENUM_CONTINUE;
}
#endif


void Joystick_Init(void)
{
#if 0
	BYTE jnum;
	HRESULT	hres;
	GUID guid;
	DIPROPRANGE diprg;
	DIDEVICEINSTANCE dev;
#endif

	JoyKeyState0 = 0;
	JoyKeyState1 = 0;
	JoyState0[0] = 0xff;
	JoyState0[1] = 0xff;
	JoyState1[0] = 0xff;
	JoyState1[1] = 0xff;
	JoyPortData[0] = 0;
	JoyPortData[1] = 0;

#if 0
	if (FAILED(DirectInputCreate(hInst, DIRECTINPUT_VERSION, &dinput, NULL))) {
		return;
	}

	IDirectInput_EnumDevices(dinput, DIDEVTYPE_JOYSTICK,
					(LPDIENUMDEVICESCALLBACK)JoyEnumProc,
					&guid, DIEDFL_ATTACHEDONLY);

	if ((joy[0] != NULL) && (SUCCEEDED(IDirectInputDevice2_SetDataFormat(joy[0], &c_dfDIJoystick))))
	{
		hres = IDirectInputDevice2_SetCooperativeLevel(joy[0], hWndMain,
						DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
		if (SUCCEEDED(hres)) {
			ZeroMemory(&diprg, sizeof(diprg));
			diprg.diph.dwSize = sizeof(diprg);
			diprg.diph.dwHeaderSize	= sizeof(diprg.diph);
			diprg.diph.dwObj = DIJOFS_X;
			diprg.diph.dwHow = DIPH_BYOFFSET;
			diprg.lMin = 0;
			diprg.lMax = 255;
			IDirectInputDevice2_SetProperty(joy[0], DIPROP_RANGE, &diprg.diph);
			diprg.diph.dwObj = DIJOFS_Y;
			IDirectInputDevice2_SetProperty(joy[0], DIPROP_RANGE, &diprg.diph);
			IDirectInputDevice2_Acquire(joy[0]);

			ZeroMemory(&dev, sizeof(DIDEVICEINSTANCE));
			dev.dwSize = sizeof(DIDEVICEINSTANCE);
			if (IDirectInputDevice2_GetDeviceInfo(joy[0], &dev) == DI_OK)
				strcpy(joyname[0], dev.tszProductName);
			else
				strcpy(joyname[0], "不明なデバイス");

			joybtnnum[0] = 0;
			jnum = 0;
			IDirectInputDevice_EnumObjects(joy[0],
						(LPDIENUMDEVICEOBJECTSCALLBACK)EnumButtonProc,
						&jnum, DIDFT_BUTTON);
		}
	}
	if ((joy[1] != NULL) && (SUCCEEDED(IDirectInputDevice2_SetDataFormat(joy[1], &c_dfDIJoystick))))
	{
		hres = IDirectInputDevice2_SetCooperativeLevel(joy[1], hWndMain,
						DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
		if (SUCCEEDED(hres)) {
			ZeroMemory(&diprg, sizeof(diprg));
			diprg.diph.dwSize = sizeof(diprg);
			diprg.diph.dwHeaderSize	= sizeof(diprg.diph);
			diprg.diph.dwObj = DIJOFS_X;
			diprg.diph.dwHow = DIPH_BYOFFSET;
			diprg.lMin = 0;
			diprg.lMax = 255;
			IDirectInputDevice2_SetProperty(joy[1], DIPROP_RANGE, &diprg.diph);
			diprg.diph.dwObj = DIJOFS_Y;
			IDirectInputDevice2_SetProperty(joy[1], DIPROP_RANGE, &diprg.diph);
			IDirectInputDevice2_Acquire(joy[1]);
		}

			ZeroMemory(&dev, sizeof(DIDEVICEINSTANCE));
			dev.dwSize = sizeof(DIDEVICEINSTANCE);
			if (IDirectInputDevice2_GetDeviceInfo(joy[1], &dev) == DI_OK)
				strcpy(joyname[1], dev.tszProductName);
			else
				strcpy(joyname[1], "不明なデバイス");

			joybtnnum[1] = 0;
			jnum = 1;
			IDirectInputDevice_EnumObjects(joy[1],
						(LPDIENUMDEVICEOBJECTSCALLBACK)EnumButtonProc,
						&jnum, DIDFT_BUTTON);
	}
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
#if 1
	(void)joynum;
	(void)ret0;
	(void)ret1;
	ret = 0xff;
#else
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
//ret = ret0;
#endif
	return ret;

}


void FASTCALL Joystick_Write(BYTE num, BYTE data)
{
	if ( (num==0)||(num==1) ) JoyPortData[num] = data;
}


void FASTCALL Joystick_Update(void)
{
#if 0
	BYTE ret0, ret1;
	HRESULT hres;
	DIJOYSTATE state;
	int num;

	for (num=0; num<2; num++)
	{

	ret0 = 0xff;
	ret1 = 0xff;

	if (joy[num])
	{
		hres = IDirectInputDevice2_Poll(joy[num]);
		if (hres!=DI_OK)
		{
			if (hres == DIERR_INPUTLOST) {		// ロストしてたらAquireしてみる(S.J.PAD2)
				IDirectInputDevice2_Acquire(joy[num]);
				hres = IDirectInputDevice2_Poll(joy[num]);
				if (hres!=DI_OK)		// それでもダメなら諦める
					continue;
			}
		}
		hres = IDirectInputDevice2_GetDeviceState(joy[num], sizeof(DIJOYSTATE), &state);
		if (SUCCEEDED(hres))
		{
			if (state.lX < 32)
				ret0 ^= JOY_LEFT;
			else if (state.lX >= 224)
				ret0 ^= JOY_RIGHT;

			if (state.lY < 32)
				ret0 ^= JOY_UP;
			else if (state.lY >= 224)
				ret0 ^= JOY_DOWN;

			if (Config.JOY_BTN[num][0]<joybtnnum[num])
				if (state.rgbButtons[Config.JOY_BTN[num][0]] & 0x80)
					ret0 ^= JOY_TRG1;
			if (Config.JOY_BTN[num][1]<joybtnnum[num])
				if (state.rgbButtons[Config.JOY_BTN[num][1]] & 0x80)
					ret0 ^= JOY_TRG2;
			if (Config.JOY_BTN[num][2]<joybtnnum[num])
				if (state.rgbButtons[Config.JOY_BTN[num][2]] & 0x80)
					ret1 ^= JOY_TRG3;
			if (Config.JOY_BTN[num][3]<joybtnnum[num])
				if (state.rgbButtons[Config.JOY_BTN[num][3]] & 0x80)
					ret1 ^= JOY_TRG4;
			if (Config.JOY_BTN[num][4]<joybtnnum[num])
				if (state.rgbButtons[Config.JOY_BTN[num][4]] & 0x80)
					ret1 ^= JOY_TRG5;
			if (Config.JOY_BTN[num][5]<joybtnnum[num])
				if (state.rgbButtons[Config.JOY_BTN[num][5]] & 0x80)
					ret1 ^= JOY_TRG6;
			if (Config.JOY_BTN[num][6]<joybtnnum[num])
				if (state.rgbButtons[Config.JOY_BTN[num][6]] & 0x80)
					ret1 ^= JOY_TRG7;
			if (Config.JOY_BTN[num][7]<joybtnnum[num])
				if (state.rgbButtons[Config.JOY_BTN[num][7]] & 0x80)
					ret1 ^= JOY_TRG8;
		}
		else if (hres == DIERR_INPUTLOST)
			IDirectInputDevice2_Acquire(joy[num]);
	}

	JoyState0[num] = ret0;
	JoyState1[num] = ret1;
	}
#endif
}
