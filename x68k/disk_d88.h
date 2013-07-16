#ifndef _winx68k_d88
#define _winx68k_d88

typedef struct {			// Header Part (size:2B0h)
	BYTE	fd_name[17];		// Disk Name
	BYTE	reserved1[9]; 		// Reserved
	BYTE	protect;		// Write Protect bit:4
	BYTE	fd_type;		// Disk Format
	long	fd_size;		// Disk Size
	long	trackp[164];		// Track_pointer
} D88_HEADER;

void D88_Init(void);
void D88_Cleanup(void);
int D88_SetFD(int drive, char* filename);
int D88_Eject(int drive);
int D88_Seek(int drv, int trk, FDCID* id);
int D88_ReadID(int drv, FDCID* id);
int D88_WriteID(int drv, int trk, unsigned char* buf, int num);
int D88_Read(int drv, FDCID* id, unsigned char* buf);
int D88_ReadDiag(int drv, FDCID* id, FDCID* retid, unsigned char* buf);
int D88_Write(int drv, FDCID* id, unsigned char* buf, int del);
int D88_GetCurrentID(int drv, FDCID* id);

#endif
