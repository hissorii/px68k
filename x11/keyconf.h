#ifndef _x68_keyconf
#define _x68_keyconf

extern HWND hWndKeyConf;
extern WORD  KeyConf_CodeW;
extern DWORD KeyConf_CodeL;
extern char KeyConfMessage[255];
LRESULT CALLBACK KeyConfProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

#endif // _x68_keyconf
