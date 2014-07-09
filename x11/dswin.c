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

short	playing = FALSE;

#define PCMBUF_SIZE 2*2*48000
BYTE pcmbuffer[PCMBUF_SIZE];
BYTE *pcmbufp = pcmbuffer;
BYTE *pbsp = pcmbuffer;
BYTE *pbrp = pcmbuffer, *pbwp = pcmbuffer;
BYTE *pbep = &pcmbuffer[PCMBUF_SIZE];
DWORD ratebase = 22050;
long DSound_PreCounter = 0;
BYTE sdlsndbuf[PCMBUF_SIZE];

int audio_fd = -1;

static void sdlaudio_callback(void *userdata, unsigned char *stream, int len);

#ifndef NOSOUND
#include	"SDL.h"
#include	"SDL_audio.h"

int
DSound_Init(unsigned long rate, unsigned long buflen)
{
	SDL_AudioSpec fmt;
	DWORD samples;

	if (playing) {
		return FALSE;
	}

	if (rate == 0) {
		audio_fd = -1;
		return TRUE;
	}

	ratebase = rate;

	// Linuxは2倍(SDL1.2)、Android(SDL2.0)は4倍のlenでcallbackされた。
	// この値を小さくした方が音の遅延は少なくなるが負荷があがる
	samples = 2048;

	memset(&fmt, 0, sizeof(fmt));
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
	fmt.userdata = NULL;
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

static void sound_send(int length)
{
	int rate;

#ifdef PSP
	rate = Config.SampleRate;
#else
	rate = 0;
#endif
	SDL_LockAudio();
	ADPCM_Update((short *)pbwp, length, rate, pbsp, pbep);
	OPM_Update((short *)pbwp, length, rate, pbsp, pbep);
#ifndef	NO_MERCURY
	//Mcry_Update((short *)pcmbufp, length);
#endif
#ifdef PSP
	pbwp += length * sizeof(WORD) * 2 * (44100 / rate);
	if (pbwp >= pbep) {
		pbwp = pbsp + (pbwp - pbep);
	}
#else
	pbwp += length * sizeof(WORD) * 2;
	if (pbwp >= pbep) {
		pbwp = pbsp + (pbwp - pbep);
	}
#endif
	SDL_UnlockAudio();
}

void FASTCALL DSound_Send0(long clock)
{
	int length = 0;
	int rate;

	if (audio_fd < 0) {
		return;
	}

	DSound_PreCounter += (ratebase * clock);
	while (DSound_PreCounter >= 10000000L) {
		length++;
		DSound_PreCounter -= 10000000L;
	}
	if (length == 0) {
		return;
	}
	sound_send(length);
}

static void FASTCALL DSound_Send(int length)
{
	int rate;

	if (audio_fd < 0) {
		return;
	}
	sound_send(length);
}

static void
sdlaudio_callback(void *userdata, unsigned char *stream, int len)
{
	int lena, lenb, datalen, rate;
	BYTE *buf;
	static DWORD bef;
	DWORD now;

	now = timeGetTime();

	//p6logd("tdiff %4d : len %d ", now - bef, len);

#ifdef PSP
	rate = (int)userdata;
#endif

cb_start:
	if (pbrp <= pbwp) {
		// pcmbuffer
		// +---------+-------------+----------+
		// |         |/////////////|          |
		// +---------+-------------+----------+
		// A         A<--datalen-->A          A
		// |         |             |          |
		// pbsp     pbrp          pbwp       pbep

		datalen = pbwp - pbrp;
		if (datalen < len) {
			// needs more data
#ifdef PSP
			DSound_Send((len - datalen) / 4 / (44100 / rate));
#else
			DSound_Send((len - datalen) / 4);
#endif
		}
#if 0
		datalen = pbwp - pbrp;
		if (datalen < len) {
			printf("xxxxx not enough sound data xxxxx\n");
		}
#endif
		if (pbrp > pbwp) {
			// chage to TYPEC or TYPED
			goto cb_start;
		}

		buf = pbrp;
		pbrp += len;
		//printf("TYPEA: ");

	} else {
		// pcmbuffer
		// +---------+-------------+----------+
		// |/////////|             |//////////|
		// +------+--+-------------+----------+
		// <-lenb->  A             <---lena--->
		// A         |             A          A
		// |         |             |          |
		// pbsp     pbwp          pbrp       pbep

		lena = pbep - pbrp;
		if (lena >= len) {
			buf = pbrp;
			pbrp += len;
			//printf("TYPEC: ");
		} else {
			lenb = len - lena;
			if (pbwp - pbsp < lenb) {
#ifdef PSP
				DSound_Send((lenb - (pbwp - pbsp)) / 4 / (44100 / rate));
#else
				DSound_Send((lenb - (pbwp - pbsp)) / 4);
#endif
			}
#if 0
			if (pbwp - pbsp < lenb) {
				printf("xxxxx not enough sound data xxxxx\n");
			}
#endif
			memcpy(sdlsndbuf, pbrp, lena);
			memcpy(&sdlsndbuf[lena], pbsp, lenb);
			buf = sdlsndbuf;
			pbrp = pbsp + lenb;
			//printf("TYPED: ");
		}
	}

#if SDL_VERSION_ATLEAST(2, 0, 0)	
	// SDL2.0ではstream bufferのクリアが必要
	memset(stream, 0, len);
#endif
	SDL_MixAudio(stream, buf, len, SDL_MIX_MAXVOLUME);

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

void FASTCALL
DSound_Send0(long clock)
{
}
#endif	/* !NOSOUND */
