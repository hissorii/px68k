#ifndef _winx68k_xdf
#define _winx68k_xdf

void XDF_Init(void);
void XDF_Cleanup(void);
int XDF_SetFD(int drive, char* filename);
int XDF_Eject(int drive);
int XDF_Seek(int drv, int trk, FDCID* id);
int XDF_ReadID(int drv, FDCID* id);
int XDF_WriteID(int drv, int trk, unsigned char* buf, int num);
int XDF_Read(int drv, FDCID* id, unsigned char* buf);
int XDF_ReadDiag(int drv, FDCID* id, FDCID* retid, unsigned char* buf);
int XDF_Write(int drv, FDCID* id, unsigned char* buf, int del);
int XDF_GetCurrentID(int drv, FDCID* id);

#endif
