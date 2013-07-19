/*	$Id: dswin.c,v 1.3 2010/11/07 14:22:50 nonaka Exp $	*/

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

	BYTE	pcmbuffer[2*2*48000];
	BYTE	*pcmbufp = pcmbuffer;
	DWORD	pcmfreemax = 22050;
	DWORD	pcmfree = 22050;
	DWORD	ds_halfbuffer = 22050 * 4;
	DWORD	ratebase1000 = 22;
	DWORD	ratebase = 22050;
	long	DSound_PreCounter = 0;

#ifndef	NAUDIOBUFFER
#define	NAUDIOBUFFER	3
#endif

	int	audio_fd = -1;
	int	audio_nextbuf;
	char	*audio_event;
	char	*audio_buffer[NAUDIOBUFFER];

// ????????????
DWORD calc_blocksize(DWORD size);
static void sdlaudio_callback(void *userdata, unsigned char *stream, int len);
static BOOL buffer_init(DWORD rate, DWORD samples);
static void buffer_cleanup(void);

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

	// pcmfreemax = Dsound??????????????????????????
	pcmfree = pcmfreemax = (DWORD)rate * buflen / 200 / 2;
	ds_halfbuffer = pcmfreemax * 4;
	pcmbufp = pcmbuffer;
	ratebase1000 = rate / 1000;
	ratebase = rate;

	samples = calc_blocksize(pcmfreemax);
	audio_nextbuf = 0;
	audio_event = 0;

	// ??????????????????????????????
	bzero(&fmt, sizeof(fmt));
	fmt.freq = rate;
	fmt.format = AUDIO_S16SYS;
	fmt.channels = 2;
	fmt.samples = samples;
	fmt.callback = sdlaudio_callback;
	fmt.userdata = (void *)ds_halfbuffer;
	audio_fd = SDL_OpenAudio(&fmt, NULL);
	if (audio_fd < 0) {
		SDL_Quit();
		return FALSE;
	}
	buffer_init(rate, samples);

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
		buffer_cleanup();
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
		if (length) {
			if ((long)pcmfree > (long)length) {
				ADPCM_Update((short *)pcmbufp, length);
				OPM_Update((short *)pcmbufp, length);
#ifndef	NO_MERCURY
				Mcry_Update((short *)pcmbufp, length);
#endif
				pcmbufp += length * sizeof(WORD) * 2;
				pcmfree -= length;
			} else {
				ADPCM_Update((short *)pcmbufp, pcmfree);
				OPM_Update((short *)pcmbufp, pcmfree);
#ifndef	NO_MERCURY
				Mcry_Update((short *)pcmbufp, pcmfree);
#endif
				pcmbufp += pcmfree * sizeof(WORD) * 2;
				pcmfree = 0;
			}
		}
	}
}


void FASTCALL
DSound_Send(void)
{
	short *q;

	if (audio_fd >= 0) {
		if (audio_event) {
			q = (short *)audio_event;
			audio_event = 0;

			ADPCM_Update((short *)pcmbufp, pcmfree);
			OPM_Update((short *)pcmbufp, pcmfree);
#ifndef	NO_MERCURY
			Mcry_Update((short *)pcmbufp, pcmfree);
#endif

			memcpy(q, pcmbuffer, ds_halfbuffer);

			// ????????
			pcmbufp = pcmbuffer;
			pcmfree = pcmfreemax;
		}
	}
}

static void
sdlaudio_callback(void *userdata, unsigned char *stream, int len)
{

	if (audio_event != NULL)
		bzero(audio_event, (DWORD)userdata);

	SDL_MixAudio(stream, audio_buffer[audio_nextbuf], len,
	    SDL_MIX_MAXVOLUME);

	audio_nextbuf = (audio_nextbuf + 1) % NAUDIOBUFFER;
	audio_event = audio_buffer[audio_nextbuf];
}

// ---------------------------------------------------------------------------
//  ??????
// ---------------------------------------------------------------------------

DWORD
calc_blocksize(DWORD size)
{
	DWORD s = size;

	if (size & (size - 1))
		for (s = 32; s < size; s <<= 1)
			continue;
	return s;
}

static BOOL
buffer_init(DWORD rate, DWORD samples)
{
	int i;

	UNUSED(rate);

	for (i = 0; i < NAUDIOBUFFER; i++) {
		audio_buffer[i] = (char *)calloc(samples * 4, 1);
		if (audio_buffer[i] == NULL) {
			fprintf(stderr, "buffer_init: can't alloc memory\n");
			return FAILURE;
		}
	}

	return SUCCESS;
}

static void
buffer_cleanup(void)
{
	int i;

	for (i = 0; i < NAUDIOBUFFER; i++) {
		if (audio_buffer[i]) {
			free(audio_buffer[i]);
			audio_buffer[i] = 0;
		}
	}
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

void FASTCALL
DSound_Send(void)
{
}
#endif	/* !NOSOUND */
