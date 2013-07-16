// ---------------------------------------------------------------------------------------
//  KEYCONF.C - KeyConfig 入力待ちダイアログ
// ---------------------------------------------------------------------------------------
#include	"common.h"
#include	"resource.h"
#include	"keyconf.h"
#include	"winx68k.h"

HWND	hWndKeyConf = NULL;
WORD	KeyConf_CodeW = 0;
DWORD	KeyConf_CodeL = 0;
char	KeyConfMessage[255];
int	KeyConfFlag = 0;


LRESULT CALLBACK KeyConfProc(HWND hdlg, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_INITDIALOG:
//		if (!KeyConfFlag) SetFocus(hWndKeyConf);
		KeyConfFlag = 1;
		KeyConf_CodeW = 0;
		KeyConf_CodeL = 0;
//		SetDlgItemText(hdlg, IDC_KEYCONF_DLG, KeyConfMessage);
//ShowWindow(hdlg, SW_HIDE);
		return 0;

	case WM_COMMAND:
		if (IDCANCEL == LOWORD(wp))
		{
			EndDialog(hdlg, TRUE);
			break;
		}
		return TRUE;

	case WM_SYSCOMMAND:
		return TRUE;

	case WM_ACTIVATE:
		SetFocus(hWndKeyConf);
		break;

//	case WM_CLOSE:
//		EndDialog(hdlg, FALSE);
//		return TRUE;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		if (wp==VK_F12)
		{
			EndDialog(hdlg, TRUE);
			return TRUE;
		}
//		Error("Key Pressed.");
		KeyConf_CodeW = wp;
		KeyConf_CodeL = lp;
		EndDialog(hdlg, TRUE);
		return TRUE;

	default:
//		SetFocus(hWndKeyConf);
		return FALSE;
	}
	return FALSE;
}
