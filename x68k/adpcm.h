#ifndef _winx68k_adpcm_h
#define _winx68k_adpcm_h

extern BYTE ADPCM_Clock;
extern DWORD ADPCM_ClockRate;

void FASTCALL ADPCM_PreUpdate(DWORD clock);
void FASTCALL ADPCM_Update(signed short *buffer, DWORD length, int rate);

void FASTCALL ADPCM_Write(DWORD adr, BYTE data);
BYTE FASTCALL ADPCM_Read(DWORD adr);

void ADPCM_SetVolume(BYTE vol);
void ADPCM_SetPan(int n);
void ADPCM_SetClock(int n);

void ADPCM_Init(DWORD samplerate);
int ADPCM_IsReady(void);

#endif
