#ifndef _winx68k_dim
#define _winx68k_dim

void DIM_Init(void);
void DIM_Cleanup(void);
int DIM_SetFD(int drive, char* filename);
int DIM_Eject(int drive);
int DIM_Seek(int drv, int trk, FDCID* id);
int DIM_ReadID(int drv, FDCID* id);
int DIM_WriteID(int drv, int trk, unsigned char* buf, int num);
int DIM_Read(int drv, FDCID* id, unsigned char* buf);
int DIM_ReadDiag(int drv, FDCID* id, FDCID* retid, unsigned char* buf);
int DIM_Write(int drv, FDCID* id, unsigned char* buf, int del);
int DIM_GetCurrentID(int drv, FDCID* id);

#endif
