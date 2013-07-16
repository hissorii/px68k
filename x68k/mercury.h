#ifndef _winx68k_mercury_h
#define _winx68k_mercury_h

extern BYTE Mcry_LRTiming;

void FASTCALL Mcry_Update(signed short *buffer, DWORD length);
void FASTCALL Mcry_PreUpdate(DWORD clock);

void FASTCALL Mcry_Write(DWORD adr, BYTE data);
BYTE FASTCALL Mcry_Read(DWORD adr);

void Mcry_SetClock(void);
void Mcry_SetVolume(BYTE vol);

void Mcry_Init(DWORD samplerate, const char* path);
void Mcry_Cleanup(void);
int Mcry_IsReady(void);

void FASTCALL Mcry_Int(void);

#endif

