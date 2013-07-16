#ifndef CDASPI_H_
#define	CDASPI_H_

int CDASPI_Open(void);
void CDASPI_Close(void);
int CDASPI_Wait(void);
int CDASPI_ReadTOC(void* buf);
int CDASPI_Read(long block, BYTE* buf);
int CDASPI_ExecCmd(BYTE *cdb, int cdb_size, BYTE *out_buff, int out_size);
int CDASPI_IsOpen(void);

void CDASPI_EnumCD(void);

void CDASPI_Init(void);
void CDASPI_Cleanup(void);

extern BYTE CDASPI_CDName[8][4];
extern int CDASPI_CDNum;

extern HINSTANCE ASPIDLL;
DWORD (*pSendASPI32Command)(LPSRB);
DWORD (*pGetASPI32SupportInfo)(VOID);

#endif // CDASPI_H_
