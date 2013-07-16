#ifndef _winx68k_midi
#define _winx68k_midi

#include "common.h"

void MIDI_Init(void);
void MIDI_Cleanup(void);
void MIDI_Reset(void);
BYTE FASTCALL MIDI_Read(DWORD adr);
void FASTCALL MIDI_Write(DWORD adr, BYTE data);
void MIDI_SetModule(void);
void FASTCALL MIDI_Timer(DWORD clk);
int MIDI_SetMimpiMap(char *filename);
int MIDI_EnableMimpiDef(int enable);
void MIDI_DelayOut(unsigned int delay);

#endif
