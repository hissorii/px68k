// ---------------------------------------------------------------------------------------
//  COMMON - 標準ヘッダ群（COMMON.H）とエラーダイアログ表示とか
// ---------------------------------------------------------------------------------------
#include	<windows.h>
#include	<stdio.h>

#include	"sstp.h"

extern HWND hWndMain;
extern const BYTE PrgTitle[];

void Error(const char* s)
{
	char title[80];
	sprintf(title, "%s エラー\0", PrgTitle);

	SSTP_SendMes(SSTPMES_ERROR);

	MessageBox(hWndMain, s, title, MB_ICONERROR | MB_OK);
}

