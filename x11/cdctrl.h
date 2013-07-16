#ifndef CDCTRL_H_
#define	CDCTRL_H_

int CDCTRL_Open(void);
void CDCTRL_Close(void);
int CDCTRL_Wait(void);
int CDCTRL_ReadTOC(void* buf);
int CDCTRL_Read(long block, BYTE* buf);
int CDCTRL_IsOpen(void);

#endif // CDCTRL_H_
