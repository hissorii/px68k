/* 
 * Copyright (c) 2003 NONAKA Kimihiro
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include	"windows.h"
#include	"common.h"
#include	"dswin.h"
#include	"prop.h"
#include	"adpcm.h"
#include	"mercury.h"
#include	"fmg_wrap.h"

#ifdef ANDROID
#include <android/log.h>
#endif

#define PCMBUF_SIZE 2*2*48000*2

	short	playing = FALSE;
	BYTE	pcmbuffer[PCMBUF_SIZE];
	long	DSound_PreCounter = 0;

	int	audio_fd = -1;

// ????????????
static void sdlaudio_callback(void *userdata, unsigned char *stream, int len);

// ---------------------------------------------------------------------------
//  ??????
// ---------------------------------------------------------------------------
#ifndef NOSOUND
#include	"SDL.h"
#include	"SDL_audio.h"

int
DSound_Init(unsigned long rate, unsigned long buflen)
{
	SDL_AudioSpec fmt;
	DWORD samples;

	// ??????????????
	if (playing) {
		return FALSE;
	}

	// ????????????
	if (rate == 0) {
		audio_fd = -1;
		return TRUE;
	}

	//printf("pbsp 0x%x, pbep 0x%x\n", pbsp, pbep);

	// Linuxは2倍(SDL1.2)、Android(SDL2.0)は4倍のlenでcallbackされた。
	// この値を小さくした方が音の遅延は少なくなるが負荷があがる
	samples = 1024;

	// ??????????????????????????????
	bzero(&fmt, sizeof(fmt));
#ifdef PSP
	// PSPは常に44kを要求するので、rateが22Kの場合はデータを2倍にする
	// r0, l0, r1, l1, ... -> r0, l0, r0, l0, r1, l1, r1, l1, ...
	fmt.freq = 44100;
#else
	fmt.freq = rate;
#endif
	fmt.format = AUDIO_S16SYS;
	fmt.channels = 2;
	fmt.samples = samples;
	fmt.callback = sdlaudio_callback;
#ifdef PSP
	fmt.userdata = rate;
#else
	fmt.userdata = 0;
#endif
	audio_fd = SDL_OpenAudio(&fmt, NULL);
	if (audio_fd < 0) {
		SDL_Quit();
		return FALSE;
	}

	playing = TRUE;
	return TRUE;
}

void
DSound_Play(void)
{

	if (audio_fd >= 0)
		SDL_PauseAudio(0);
}

void
DSound_Stop(void)
{

	if (audio_fd >= 0)
		SDL_PauseAudio(1);
}


// ---------------------------------------------------------------------------
//  ????????
// ---------------------------------------------------------------------------

int
DSound_Cleanup(void)
{

	playing = FALSE;
	if (audio_fd >= 0) {
		SDL_CloseAudio();
		SDL_Quit();
		audio_fd = -1;
	}
	return TRUE;
}


// ---------------------------------------------------------------------------
//  ??????????????
// ---------------------------------------------------------------------------

static void FASTCALL
DSound_Send(long length, int rate)
{
	if (audio_fd >= 0) {
		ADPCM_Update((short *)pcmbuffer, length, rate);
		OPM_Update((short *)pcmbuffer, length, rate);
#ifndef	NO_MERCURY
		//Mcry_Update((short *)pcmbufp, length);
#endif
	}
}

static void
sdlaudio_callback(void *userdata, unsigned char *stream, int len)
{
	static DWORD bef;
	DWORD now;
	int rate;

	now = timeGetTime();

	//PSP以外はrateは0
	rate = (int)userdata;
#ifdef PSP
	DSound_Send(len / (44100 / rate) / 4, rate);
#else
	DSound_Send(len / 4, rate);
#endif
	
#ifdef ANDROID
	//__android_log_print(ANDROID_LOG_DEBUG,"Tag","tdiff %4d : pbrp = 0x%x, pbwp = 0x%x : len %d", now - bef, pbrp, pbwp, len);
#else
	//printf("tdiff %4d : pbrp = 0x%x, pbwp = 0x%x : len %d", now - bef, pbrp, pbwp, len);
#endif

#if SDL_VERSION_ATLEAST(2, 0, 0)	
	// SDL2.0ではstream bufferのクリアが必要
	memset(stream, 0, len);
#endif
	SDL_MixAudio(stream, pcmbuffer, len, SDL_MIX_MAXVOLUME);
#if 0
	// ADPCMがバッファ書き込み -> OPMがバッファにデータをMix なので
	// ADPCMを無効にする場合はリングバッファのクリアが必要なので
	// この#if 0を有効にする
	memset(pcmbuffer, 0, len);
#endif
	bef = now;
}

#else	/* NOSOUND */
int
DSound_Init(unsigned long rate, unsigned long buflen)
{
	return FALSE;
}

void
DSound_Play(void)
{
}

void
DSound_Stop(void)
{
}

int
DSound_Cleanup(void)
{

	return TRUE;
}

#endif	/* !NOSOUND */
