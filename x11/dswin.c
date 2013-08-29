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

	short	playing = FALSE;

	#define PCMBUF_SIZE 2*2*48000
	BYTE	pcmbuffer[PCMBUF_SIZE];
	BYTE	*pbsp = pcmbuffer;
	BYTE	*pbrp = pcmbuffer, *pbwp = pcmbuffer;
	BYTE	*pbep = &pcmbuffer[PCMBUF_SIZE];
	DWORD	ratebase1000 = 22;
	DWORD	ratebase = 22050;
	long	DSound_PreCounter = 0;
	BYTE sdlsndbuf[PCMBUF_SIZE];
int inlen = 0;

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

	printf("pbsp 0x%x, pbep 0x%x\n", pbsp, pbep);

	ratebase1000 = rate / 1000;
	ratebase = rate;

	// Linuxは2倍(SDL1.2)、Android(SDL2.0)は4倍のlenでcallbackされた。
	// この値を小さくした方が音の遅延は少なくなるが負荷があがる
	samples = 2048;

	// ??????????????????????????????
	bzero(&fmt, sizeof(fmt));
	fmt.freq = rate;
	fmt.format = AUDIO_S16SYS;
	fmt.channels = 2;
	fmt.samples = samples;
	fmt.callback = sdlaudio_callback;
	fmt.userdata = NULL;
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

void FASTCALL
DSound_Send0(long clock)
{
	int length = 0;

	if (audio_fd >= 0) {
		DSound_PreCounter += (ratebase * clock);
		while (DSound_PreCounter >= 10000000L) {
			length++;
			DSound_PreCounter -= 10000000L;
		}
		if (length == 0) {
			return;
		}

		SDL_LockAudio();
		inlen += length * 4;
		ADPCM_Update((short *)pbwp, length, pbsp, pbep);
		OPM_Update((short *)pbwp, length, pbsp, pbep);
#ifndef	NO_MERCURY
		//Mcry_Update((short *)pcmbufp, length);
#endif
		pbwp += length * sizeof(WORD) * 2;
		if (pbwp >= pbep) {
			pbwp = pbsp;
		}
		SDL_UnlockAudio();
	}
}

static void
sdlaudio_callback(void *userdata, unsigned char *stream, int len)
{
	int lena, lenb, datalen;
	BYTE *buf, *pbrp0;
	static DWORD bef;
	DWORD now;
	static int under = 0;

	SDL_LockAudio();

	now = timeGetTime();
	
#ifdef ANDROID
	//__android_log_print(ANDROID_LOG_DEBUG,"Tag","tdiff %4d : pbrp = 0x%x, pbwp = 0x%x : len %d", now - bef, pbrp, pbwp, len);
#else
	//printf("tdiff %4d : pbrp = 0x%x, pbwp = 0x%x : ", now - bef, pbrp, pbwp);
#endif
	pbrp0 = pbrp;

	if (pbrp <= pbwp) {
		// pcmbuffer
		// +---------+-------------+----------+
		// |         |/////////////|          |     
		// +---------+-------------+----------+
		// A         A<--datalen-->A          A
		// |         |             |          |
		// pbsp     pbrp          pbwp       pbep

		datalen = pbwp - pbrp;
		if (datalen >= len) {
			buf = pbrp;
			pbrp += len;
			//printf("TYPEA: ");
		} else {
			memcpy(sdlsndbuf, pbrp, datalen);
			// xxx underrun : 不足分はとりあえず0で埋める
			memset(&sdlsndbuf[datalen], 0, len - datalen);
			buf = sdlsndbuf;
			pbrp += datalen;
			//printf("TYPEB: ");
		}
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
			//printf("***** TYPEC: ");
		} else {
			lenb = len - lena;
			if (pbwp - pbsp >= lenb) {
				memcpy(sdlsndbuf, pbrp, lena);
				memcpy(&sdlsndbuf[lena], pbsp, lenb);
				buf = sdlsndbuf;
				pbrp = pbsp + lenb;
				//printf("********** TYPED: ");
			} else {
				memcpy(sdlsndbuf, pbrp, lena);
				memcpy(&sdlsndbuf[lena], pbsp, pbwp - pbsp);
				// xxx underrun : 不足分はとりあえず0で埋める
				memset(&sdlsndbuf[lena + pbwp - pbsp], 0, lenb - (pbwp - pbsp));
				buf = sdlsndbuf;
				pbrp = pbwp;
				//printf("*************** TYPEE: ");
			}

		}
	}

	under = under + inlen - len;
	//printf("inlen %4d under %4d\n", inlen, under);
	inlen = 0;

#if SDL_VERSION_ATLEAST(2, 0, 0)	
	// SDL2.0ではstream bufferのクリアが必要
	memset(stream, 0, len);
#endif
	SDL_MixAudio(stream, buf, len, SDL_MIX_MAXVOLUME);

	//printf("pbrp0 0x%x, pbrp 0x%x\n", pbrp0, pbrp);

#if 0
	// ADPCMがバッファ書き込み -> OPMがバッファにデータをMix なので
	// ADPCMを無効にする場合はリングバッファのクリアが必要
	if (pbrp >= pbrp0) {
		memset(pbrp0, 0, pbrp - pbrp0);
	} else {
		memset(pbsp, 0, pbrp - pbsp);
		memset(pbrp0, 0, pbep - pbrp0);
	}
#endif

	bef = now;
	SDL_UnlockAudio();
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
