/*	$Id: mmsystem.h,v 1.1.1.1 2003/04/28 18:06:55 nonaka Exp $	*/

#ifndef	MMSYSTEM_H__
#define	MMSYSTEM_H__

#include "windows.h"

#define	WINMMAPI
typedef	UINT		MMRESULT;
typedef	HANDLE		HMIDIOUT;
typedef	HMIDIOUT *	LPHMIDIOUT;

typedef struct midihdr {
	LPSTR			lpData;
	DWORD			dwBufferLength;
	DWORD			dwBytesRecorded;
	DWORD			dwUser;
	DWORD			dwFlags;
	struct midihdr *	lpNext;
	DWORD			reserved;
	DWORD			dwOffset;
	DWORD			dwReserved[8];
} MIDIHDR, *PMIDIHDR, *NPMIDIHDR, *LPMIDIHDR;


#define	MMSYSERR_NOERROR	0
#define	MIDIERR_STILLPLAYING	2

#define	MIDI_MAPPER		-1

#define	CALLBACK_NULL		0x00000000L


#ifdef __cplusplus
extern "C" {
#endif

WINMMAPI MMRESULT WINAPI midiOutPrepareHeader(HMIDIOUT hmo, LPMIDIHDR pmh, UINT cbmh);
WINMMAPI MMRESULT WINAPI midiOutUnprepareHeader(HMIDIOUT hmo, LPMIDIHDR pmh, UINT cbmh);
WINMMAPI MMRESULT WINAPI midiOutShortMsg(HMIDIOUT hmo, DWORD dwMsg);
WINMMAPI MMRESULT WINAPI midiOutLongMsg(HMIDIOUT hmo, LPMIDIHDR pmh, UINT cbmh);
WINMMAPI MMRESULT WINAPI midiOutOpen(LPHMIDIOUT phmo, UINT uDeviceID, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen);
WINMMAPI MMRESULT WINAPI midiOutClose(HMIDIOUT hmo);
WINMMAPI MMRESULT WINAPI midiOutReset(HMIDIOUT hmo);

#ifdef __cplusplus
};
#endif

#endif	/* MMSYSTEM_H__ */
