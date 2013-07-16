#ifndef _x68k_rtc
#define _x68k_rtc

void RTC_Init(void);
BYTE FASTCALL RTC_Read(DWORD adr);
void FASTCALL RTC_Write(DWORD adr, BYTE data);
void RTC_Timer(int clock);

#endif
