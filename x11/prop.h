#ifndef _winx68k_config
#define _winx68k_config

#include "common.h"

typedef struct
{
	DWORD SampleRate;
	DWORD BufferSize;
	int WinPosX;
	int WinPosY;
	int OPM_VOL;
	int PCM_VOL;
	int MCR_VOL;
	int JOY_BTN[2][8];
	int MouseSpeed;
	int WindowFDDStat;
	int FullScrFDDStat;
	int DSAlert;
	int MIDI_SW;
	int MIDI_Type;
	int MIDI_Reset;
	int JoyKey;
	int JoyKeyReverse;
	int JoyKeyJoy2;
	int SRAMWarning;
	char HDImage[16][MAX_PATH];
	int ToneMap;
	char ToneMapFile[MAX_PATH];
	int XVIMode;
	int JoySwap;
	int LongFileName;
	int WinDrvFD;
	int WinStrech;
	int DSMixing;
	int CDROM_ASPI;
	int CDROM_ASPI_Drive;
	int CDROM_IOCTRL_Drive;
	int CDROM_SCSIID;
	int CDROM_Enable;
	int SSTP_Enable;
	int SSTP_Port;
	int Sound_LPF;
	int SoundROMEO;
	int MIDIDelay;
	int MIDIAutoDelay;
	char FDDImage[2][MAX_PATH];
	int VkeyScale;
	int VbtnSwap;
	int JoyOrMouse;
	int HwJoyAxis[2];
	int HwJoyHat;
	int HwJoyBtn[8];
	int NoWaitMode;
	BYTE FrameRate;
} Win68Conf;

extern Win68Conf Config;

void LoadConfig(void);
void SaveConfig(void);
void PropPage_Init(void);

int set_modulepath(char *path, size_t len);

#endif //_winx68k_config
