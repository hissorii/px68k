/*
  x11/keytbl.inc を生成するツールプログラム
 */

#include <stdio.h>
#include <string.h>

// 0xff: 終端, 0xfe, 増加繰り返し, 0x0: ダミーキー(リターンキーの装飾用)
unsigned char code_tbl[] = {
	0xfe, 0x61, 12, 0xfe, 0x5a, 4, 0xfe, 0x52, 3,
	0xfe, 0x01, 15, 0x36, 0x5e, 0x37, 0x3f, 0xfe, 0x40, 3,
	0xfe, 0x10, 13, 0x0, 0x1d, 0xfe, 0x38, 3, 0xfe, 0x43, 4,
	0x71, 0xfe, 0x1e, 12, 0x3c, 0xfe, 0x47, 4,
	0x70, 0xfe, 0x2a, 11, 0x70, 0x3b, 0x3e, 0x3d, 0xfe, 0x4b, 4,
	0x5f, 0x55, 0x56, 0x35, 0xfe, 0x57, 3, 0x60, 0x72, 0x73, 0xfe, 0x4f, 3
};

int ct_ptr = 0;
unsigned char get_code(void)
{
	static int cnt = 0;
	static unsigned char cc;
	unsigned char c;

	if (cnt > 0) {
		cnt--;
		return cc++;
	}

	c = code_tbl[ct_ptr++];

	if (c == 0xfe) {
		cc = code_tbl[ct_ptr++];
		cnt = code_tbl[ct_ptr++];
		return get_code();
	}

	return c;
}

void prt_tbl(int x, int y, int w, int h, char *s)
{
	printf("{%3d, %3d, %3d, %3d, \"%s\", 0x%x},\n",
	       x, y, w, h, s, get_code());
}
void prt_tblc(int x, int y, int w, int h, char c)
{
	printf("{%3d, %3d, %3d, %3d, \"%c\", 0x%x},\n",
	       x, y, w, h, c, get_code());
}

main()
{
	int x, y, w, h, i;
	char s[256];
	char str[][10] = {"KAN", "ROM", "COD", "CAP", "KIG", "TOR", "HLP", //0
			  "HOM", "INS", "DEL", "CLR", "/", "*", "-", // 7
			  "RUP", "RDN", "UND", "7", "8", "9", "+", // 14
			  "4", "5", "6", "=", // 21
			  "1", "2", "3", // 25
			  "0", ",", ".", // 28
	};

	char c[] = "1234567890-^QWERTYUIOP@[ASDFGHJKL;:]ZXCVBNM,./ ";

	char *p[10];

	// 1列目

	w = 32;
	h = 32;
	strcpy(s, "BRK");
	prt_tbl(2, 2, w, h, s);

	strcpy(s, "CPY");
	prt_tbl(40, 2, w, h, s);

	// function keys
	y = 16;
	w = 40;
	h = 18;
	for (i = 0; i < 5; i++) {
		sprintf(s, "F%2d", i + 1);
		prt_tbl(86 + (w + 2) * i, y, w, h, s);
	}

	for (i = 0; i < 5; i++) {
		sprintf(s, "F%2d", i + 6);
		prt_tbl(302 + (w + 2) * i, y, w, h, s);
	}

	y = 2;
	w = 32;
	h = 32;
	for (i = 0; i < 3; i++) {
		prt_tbl(520 + (w + 2) * i, y, w, h, str[i]);
	}
	for (i = 0; i < 4; i++) {
		prt_tbl(630 + (w + 2) * i, y, w, h, str[i + 3]);
	}

	// 2列目

	y = 48;

	prt_tbl(2, y, w, h, "ESC");
	for (i = 0; i < 12; i++) {
		prt_tblc(36 + (w + 2) * i, y, w, h, c[i]);
	}
	prt_tbl(444, y, w, h, "\\\\");	
	prt_tbl(478, y, w, h, "BS");

	for (i = 0; i < 3; i++) {
		prt_tbl(520 + (w + 2) * i, y, w, h, str[i + 7]);
	}
	for (i = 0; i < 4; i++) {
		prt_tbl(630 + (w + 2) * i, y, w, h, str[i + 10]);
	}

	// 3列目

	y = 82;

	prt_tbl(2, y, 44, 32, "TAB");

	for (i = 0; i < 12; i++) {
		prt_tblc(48 + (w + 2) * i, y, w, h, c[i + 12]);
	}
	prt_tbl(456, y, w, h, "");
	prt_tbl(468, y, 42, 66, "RET");

	for (i = 0; i < 3; i++) {
		prt_tbl(520 + (w + 2) * i, y, w, h, str[i + 14]);
	}
	for (i = 0; i < 4; i++) {
		prt_tbl(630 + (w + 2) * i, y, w, h, str[i + 17]);
	}


	// 4列目

	y = 116;

	prt_tbl(2, y, 56, 32, "CTRL");

	for (i = 0; i < 12; i++) {
		prt_tblc(60 + (w + 2) * i, y, w, h, c[i + 24]);
	}

	prt_tbl(554, y, 32, 32, "up");

	for (i = 0; i < 4; i++) {
		prt_tbl(630 + (w + 2) * i, y, w, h, str[i + 21]);
	}

	// 5列目

	y = 150;

	prt_tbl(2, y, 68, 32, "SHIFT");

	for (i = 0; i < 11; i++) {
		prt_tblc(72 + (w + 2) * i, y, w, h, c[i + 36]);
	}

	prt_tbl(446, y, 64, 32, "SHIFT");

	prt_tbl(520, 132, 32, 32, "left"); // left cursor
	prt_tbl(554, y, 32, 32, "down"); // down cursor
	prt_tbl(588, 132, 32, 32, "rigt"); // right cursor

	for (i = 0; i < 3; i++) {
		prt_tbl(630 + (w + 2) * i, y, w, h, str[i + 25]);
	}

	prt_tbl(732, y, 32, 66, "ENT");

	// 6列目

	y = 184;

	prt_tbl(60, y, 32, 32, "HIR");
	prt_tbl(94, y, 38, 32, "XF1");
	prt_tbl(134, y, 38, 32, "XF2");
	prt_tbl(174, y, 128, 32, "");
	prt_tbl(304, y, 38, 32, "XF3");
	prt_tbl(344, y, 42, 32, "XF4");
	prt_tbl(388, y, 42, 32, "XF5");
	prt_tbl(432, y, 32, 32, "ZEN");

	prt_tbl(520, y, 48, 32, "OPT1");
	prt_tbl(572, y, 48, 32, "OPT2");

	for (i = 0; i < 3; i++) {
		prt_tbl(630 + (w + 2) * i, y, w, h, str[i + 28]);
	}

	// 終端
	printf("{-1, -1, -1, -1, NULL}\n");
}
