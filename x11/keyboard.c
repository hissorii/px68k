/* 
 * Copyright (c) 2003,2008 NONAKA Kimihiro
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <SDL.h>
#include "common.h"
#include "joystick.h"
#include "prop.h"
#include "keyboard.h"
#include "mfp.h"
#include "windraw.h"

BYTE	KeyBufWP;
BYTE	KeyBufRP;
BYTE	KeyBuf[KeyBufSize];
BYTE	KeyEnable = 1;
BYTE	KeyIntFlag = 0;

struct keyboard_key kbd_key[] = {
#include "keytbl.inc"
};

extern BYTE traceflag;

#if defined(PSP) || defined(ANDROID) || TARGET_OS_IPHONE
// キーボードの座標
int kbd_x = 800, kbd_y = 0, kbd_w = 766, kbd_h = 218;
#endif

void
Keyboard_Init(void)
{

	KeyBufWP = 0;
	KeyBufRP = 0;
	ZeroMemory(KeyBuf, KeyBufSize);
	KeyEnable = 1;
	KeyIntFlag = 0;
#ifdef PSP
	// 全てのサイズを半分にする
	int i = 0;

	kbd_x = kbd_y = 0; // PSPは初期位置0
	kbd_w /= 2, kbd_h /= 2;

	while (kbd_key[i].x != -1) {
		kbd_key[i].x /= 2;
		kbd_key[i].y /= 2;
		kbd_key[i].w /= 2;
		kbd_key[i].h /= 2;
		i++;
	}
#endif
}

// ----------------------------------
//	てーぶる類
// ----------------------------------

#define	NC	0
#define KEYTABLE_MAX 512


BYTE KeyTable[KEYTABLE_MAX] = {
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x00
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	  BS, TAB,    ,    ,    , RET,    ,    		; 0x08
		0x0f,0x10,  NC,  NC,  NC,0x1d,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x10
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    , ESC,    ,    ,    ,    		; 0x18
		  NC,  NC,  NC,0x01,  NC,  NC,  NC,  NC,
	//	 SPC,  ! ,  " ,  # ,  $ ,  % ,  & ,  '		; 0x20
		0x35,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
	//	  ( ,  ) ,  * ,  + ,  , ,  - ,  . ,  /		; 0x28
		0x09,0x0a,0x28,0x27,0x31,0x0c,0x32,0x33,
	//	  0 ,  1 ,  2 ,  3 ,  4 ,  5 ,  6 ,  7		; 0x30
		0x0b,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
	//	  8 ,  9 ,  ; ,  : ,  < ,  = ,  > ,  ? 		; 0x38
		0x09,0x0a,0x28,0x27,0x31,0x0c,0x32,0x33,
	//	  @ ,  A ,  B ,  C ,  D ,  E ,  F ,  G		; 0x40
		0x1b,0x1e,0x2e,0x2c,0x20,0x13,0x21,0x22,
	//	  H ,  I ,  J ,  K ,  L ,  M ,  N ,  O		; 0x48
		0x23,0x18,0x24,0x25,0x26,0x30,0x2f,0x10,
	//	  P ,  Q ,  R ,  S ,  T ,  U ,  V ,  W		; 0x50
		0x1a,0x11,0x14,0x1f,0x15,0x17,0x2d,0x12,
	//	  X ,  Y ,  Z ,  [ ,  \ ,  ] ,  ^ ,  _		; 0x58
		0x2b,0x16,0x2a,0x1c,0x0e,0x29,0x0d,0x34,
	//	  ` ,  a ,  b ,  c ,  d ,  e ,  f ,  g		; 0x60
		0x1b,0x1e,0x2e,0x2c,0x20,0x13,0x21,0x22,
	//	  h ,  i ,  j ,  k ,  l ,  m ,  n ,  o		; 0x68
		0x23,0x18,0x24,0x25,0x26,0x30,0x2f,0x19,
	//	  p ,  q ,  r ,  s ,  t ,  u ,  v ,  w		; 0x70
		0x1a,0x11,0x14,0x1f,0x15,0x17,0x2d,0x12,
	//	  x ,  y ,  z ,  { ,  | ,  } ,  ~ ,   		; 0x78
		0x2b,0x16,0x2a,0x1c,0x0e,0x29,0x0d,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,  		; 0x80
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x88
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,  		; 0x90
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x98
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xa0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xa8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xb0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xb8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xc0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xc8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xd0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xd8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xe0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xe8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xf0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xf8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,

	//							;0x100

	//	    ,    ,    ,    ,    ,    ,    ,  		; 0x00
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	  BS, TAB,  LF, CLR,    , RET,    ,   		; 0x08
		0x0f,0x10,0x1d,  NC,  NC,0x1d,  NC,  NC,
	//	    ,  ↑,  ↓,  →,  ←,SYSQ,    ,  		; 0x10
		  NC,0x3c,0x3e,0x3d,0x3b,  NC,  NC,  NC,
	//	    ,    ,    , ESC,    ,    ,    ,   		; 0x18
		  NC,  NC,0x63,0x01,  NC,  NC,  NC,  NC,
	//	    ,KANJ,MUHE,HENM,HENK,RONM,HIRA,KATA		; 0x20
		  NC,  NC,0x56,  NC,  NC,  NC,  NC,  NC,
	//	HIKA,ZENK,HANK,ZNHN,    ,KANA,    ,   		; 0x28
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	SFTL,    ,    ,    ,    ,    ,    ,    		; 0x30
		0x70,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,ZKOU,MKOU,   		; 0x38
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x40
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x48
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	HOME,  ←,  ↑,  →,  ↓,RLDN,RLUP, END		; 0x50
		0x36,0x3b,0x3c,0x3d,0x3e,0x39,0x38,0x3a,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x58
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,PRNT, INS,    ,    ,    ,    ,    		; 0x60
		  NC,  NC,0x5e,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,BREA,    ,    ,    ,   		; 0x68
		  NC,  NC,  NC,0x61,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x70
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x78
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	<SPC,    ,    ,    ,    ,    ,    ,    		; 0x80
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,<TAB,    ,    ,    ,<ENT,    ,  		; 0x88
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,<HOM,<←>,<↑>		; 0x90
		  NC,  NC,  NC,  NC,  NC,0x36,0x3b,0x3c,
	//	<→>,<↓>,<RDN,<RUP,<END,    ,<INS,<DEL		; 0x98
		0x3d,0x3e,0x39,0x38,0x3a,  NC,0x5e,0x37,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xa0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    , <*>, <+>, <,>, <->, <.>, </>		; 0xa8
		  NC,  NC,0x41,0x46,  NC,0x42,0x51,0x40,
	//	 <0>, <1>, <2>, <3>, <4>, <5>, <6>, <7>		; 0xb0
		0x4f,0x4b,0x4c,0x4d,0x47,0x48,0x49,0x43,
	//	 <8>, <9>,    ,    ,    ,    , f.1, f.2		; 0xb8
		0x44,0x45,  NC,  NC,  NC,  NC,0x63,0x64,
	//	 f.3, f.4, f.5, f.6, f.7, f.8, f.9,f.10		; 0xc0
		0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,
	//	f.11,f.12,f.13,f.14,f.15,    ,    ,   		; 0xc8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xd0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xd8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,SFTL,SFTR,CTLL,CTLR,CAPS,    ,METL		; 0xe0
		  NC,0x70,0x70,0x71,0x71,0x5d,  NC,0x55,
	//	METR,ALTL,ALTR,    ,    ,    ,    ,    		; 0xe8
		0x55,0x55,0x55,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xf0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    , DEL		; 0xf8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,0x37
};

BYTE KeyTableMaster[KEYTABLE_MAX] = {
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x00
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x08
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x10
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x18
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	 SPC,  ! ,  " ,  # ,  $ ,  % ,  & ,  '		; 0x20
		0x35,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
	//	  ( ,  ) ,  * ,  + ,  , ,  - ,  . ,  /		; 0x28
		0x09,0x0a,0x28,0x27,0x31,0x0c,0x32,0x33,
	//	  0 ,  1 ,  2 ,  3 ,  4 ,  5 ,  6 ,  7		; 0x30
		0x0b,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
	//	  8 ,  9 ,  ; ,  : ,  < ,  = ,  > ,  ? 		; 0x38
		0x09,0x0a,0x28,0x27,0x31,0x0c,0x32,0x33,
	//	  @ ,  A ,  B ,  C ,  D ,  E ,  F ,  G		; 0x40
		0x1b,0x1e,0x2e,0x2c,0x20,0x13,0x21,0x22,
	//	  H ,  I ,  J ,  K ,  L ,  M ,  N ,  O		; 0x48
		0x23,0x18,0x24,0x25,0x26,0x30,0x2f,0x10,
	//	  P ,  Q ,  R ,  S ,  T ,  U ,  V ,  W		; 0x50
		0x1a,0x11,0x14,0x1f,0x15,0x17,0x2d,0x12,
	//	  X ,  Y ,  Z ,  [ ,  \ ,  ] ,  ^ ,  _		; 0x58
		0x2b,0x16,0x2a,0x1c,0x0e,0x29,0x0d,0x34,
	//	  ` ,  a ,  b ,  c ,  d ,  e ,  f ,  g		; 0x60
		0x1b,0x1e,0x2e,0x2c,0x20,0x13,0x21,0x22,
	//	  h ,  i ,  j ,  k ,  l ,  m ,  n ,  o		; 0x68
		0x23,0x18,0x24,0x25,0x26,0x30,0x2f,0x19,
	//	  p ,  q ,  r ,  s ,  t ,  u ,  v ,  w		; 0x70
		0x1a,0x11,0x14,0x1f,0x15,0x17,0x2d,0x12,
	//	  x ,  y ,  z ,  { ,  | ,  } ,  ~ ,   		; 0x78
		0x2b,0x16,0x2a,0x1c,0x0e,0x29,0x0d,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,  		; 0x80
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x88
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,  		; 0x90
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x98
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xa0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xa8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xb0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xb8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xc0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xc8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xd0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xd8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xe0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xe8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xf0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xf8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,

	//							;0x100

	//	    ,    ,    ,    ,    ,    ,    ,  		; 0x00
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	  BS, TAB,  LF, CLR,    , RET,    ,   		; 0x08
		0x0f,0x10,0x1d,  NC,  NC,0x1d,  NC,  NC,
	//	    ,    ,    ,PAUS,SCRL,SYSQ,    ,  		; 0x10
		  NC,  NC,  NC,0x61,  NC,  NC,  NC,  NC,
	//	    ,    ,    , ESC,    ,    ,    ,   		; 0x18
		  NC,  NC,0x63,0x01,  NC,  NC,  NC,  NC,
	//	    ,KANJ,MUHE,HENM,HENK,RONM,HIRA,KATA		; 0x20
		  NC,  NC,0x56,  NC,  NC,  NC,  NC,  NC,
	//	HIKA,ZENK,HANK,ZNHN,    ,KANA,    ,   		; 0x28
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	ALNU,    ,    ,    ,    ,    ,    ,    		; 0x30
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,ZKOU,MKOU,   		; 0x38
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x40
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x48
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	HOME,  ←,  ↑,  →,  ↓,RLDN,RLUP, END		; 0x50
		0x36,  NC,  NC,  NC,  NC,0x39,0x38,0x3a,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0x58
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,PRNT, INS,    ,    ,    ,    ,    		; 0x60
		  NC,  NC,0x5e,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,BREA,    ,    ,    ,   		; 0x68
		  NC,  NC,  NC,0x61,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x70
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0x78
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	<SPC,    ,    ,    ,    ,    ,    ,    		; 0x80
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,<TAB,    ,    ,    ,<ENT,    ,  		; 0x88
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,<HOM,<←>,<↑>		; 0x90
		  NC,  NC,  NC,  NC,  NC,0x36,  NC,  NC,
	//	<→>,<↓>,<RDN,<RUP,<END,    ,<INS,<DEL		; 0x98
		  NC,  NC,0x39,0x38,0x3a,  NC,0x5e,0x37,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xa0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    , <*>, <+>, <,>, <->, <.>, </>		; 0xa8
		  NC,  NC,0x41,0x46,  NC,0x42,  NC,0x40,
	//	 <0>, <1>, <2>, <3>, <4>, <5>, <6>, <7>		; 0xb0
		0x4f,0x4b,0x4c,0x4d,0x47,0x48,0x49,0x43,
	//	 <8>, <9>,    ,    ,    ,    , f.1, f.2		; 0xb8
		0x44,0x45,  NC,  NC,  NC,  NC,0x63,0x64,
	//	 f.3, f.4, f.5, f.6, f.7, f.8, f.9,f.10		; 0xc0
		0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,
	//	f.11,f.12,f.13,f.14,f.15,    ,    ,   		; 0xc8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xd0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,   		; 0xd8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,SFTL,SFTR,CTLL,CTLR,CAPS,    ,METL		; 0xe0
		  NC,0x70,0x70,0x71,0x71,0x5d,  NC,0x55,
	//	METR,ALTL,ALTR,    ,    ,    ,    ,    		; 0xe8
		0x55,0x55,0x55,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    ,    		; 0xf0
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
	//	    ,    ,    ,    ,    ,    ,    , DEL		; 0xf8
		  NC,  NC,  NC,  NC,  NC,  NC,  NC,0x37
};

// P6K: PX68K_KEYBOARD
//      ~ ~   ~
#define P6K_UP 1
#define P6K_DOWN 2

void send_keycode(BYTE code, int flag)
{
	BYTE newwp;

	if (code != NC) {
		newwp = ((KeyBufWP + 1) & (KeyBufSize - 1));
		if (newwp != KeyBufRP) {
			KeyBuf[KeyBufWP] = code | ((flag == P6K_UP)? 0x80 : 0);
			p6logd("KeyBuf: %x\n", KeyBuf[KeyBufWP]);
			KeyBufWP = newwp;
			p6logd("KeyBufWP: %d\n", KeyBufWP);
		}
	}
}

static BYTE get_x68k_keycode(DWORD wp)
{
	if (wp < KEYTABLE_MAX) {
		return KeyTable[wp];
	}

	switch (wp) {
	case SDLK_UP:
		return 0x3c;
	case SDLK_DOWN:
		return 0x3e;
	case SDLK_LEFT:
		return 0x3b;
	case SDLK_RIGHT:
		return 0x3d;
#ifndef PSP
#if !SDL_VERSION_ATLEAST(2, 0, 0)
#define SDLK_KP_0 SDLK_KP0
#define SDLK_KP_1 SDLK_KP1
#define SDLK_KP_2 SDLK_KP2
#define SDLK_KP_3 SDLK_KP3
#define SDLK_KP_4 SDLK_KP4
#define SDLK_KP_5 SDLK_KP5
#define SDLK_KP_6 SDLK_KP6
#define SDLK_KP_7 SDLK_KP7
#define SDLK_KP_8 SDLK_KP8
#define SDLK_KP_9 SDLK_KP9
#define SDLK_NUMLOCKCLEAR SDLK_NUMLOCK
#endif
	case SDLK_KP_0:
		return 0x4f;
	case SDLK_KP_1:
		return 0x4b;
	case SDLK_KP_2:
		return 0x4c;
	case SDLK_KP_3:
		return 0x4d;
	case SDLK_KP_4:
		return 0x47;
	case SDLK_KP_5:
		return 0x48;
	case SDLK_KP_6:
		return 0x49;
	case SDLK_KP_7:
		return 0x43;
	case SDLK_KP_8:
		return 0x44;
	case SDLK_KP_9:
		return 0x45;
	case SDLK_NUMLOCKCLEAR:
		return 0x3f;
#endif
	case SDLK_F1:
		return 0x63;
	case SDLK_F2:
		return 0x64;
	case SDLK_F3:
		return 0x65;
	case SDLK_F4:
		return 0x66;
	case SDLK_F5:
		return 0x67;
	case SDLK_F6:
		return 0x68;
	case SDLK_F7:
		return 0x69;
	case SDLK_F8:
		return 0x6a;
	case SDLK_F9:
		return 0x6b;
	case SDLK_F10:
		return 0x6c;
	case SDLK_LSHIFT:
	case SDLK_RSHIFT:
		return 0x70;
	case SDLK_LCTRL:
	case SDLK_RCTRL:
		return 0x71;
	case SDLK_KP_DIVIDE:
		return 0x40;
	case SDLK_KP_MULTIPLY:
		return 0x41;
	case SDLK_KP_MINUS:
		return 0x42;
	case SDLK_KP_PLUS:
		return 0x46;
	case SDLK_KP_ENTER:
		return 0x4e;
	case SDLK_INSERT:
		return 0x5e;
	case SDLK_HOME:
		return 0x36;
	case SDLK_END:
		return 0x3a;
	case SDLK_PAGEUP:
		return 0x38;
	case SDLK_PAGEDOWN:
		return 0x39;
	default:
		return -1;
	}
}

// ----------------------------------
//	WM_KEYDOWN〜
// ----------------------------------
void
Keyboard_KeyDown(DWORD wp)
{

	BYTE code;
	BYTE newwp;
#if 0
	if (wp & ~0xff) {
		if (wp == GDK_VoidSymbol)
			code = NC;
		else if ((wp & 0xff00) == 0xff00)
			code = KeyTable[(wp & 0xff) | 0x100];
		else
			code = NC;
	} else
		code = KeyTable[wp & 0xff];
#endif
	code = get_x68k_keycode(wp);
	if (code < 0) {
		return;
	}

	printf("Keyboard_KeyDown: ");
	printf("wp=0x%x, code=0x%x\n", wp, code);
	printf("SDLK_UP: 0x%x", SDLK_UP);

#if 0
	if (code != NC) {
		newwp = ((KeyBufWP + 1) & (KeyBufSize - 1));
		if (newwp != KeyBufRP) {
			KeyBuf[KeyBufWP] = code;
			KeyBufWP = newwp;
			printf("KeyBufWP: %d\n", KeyBufWP);
		}
	}
#else
	send_keycode(code, P6K_DOWN);
#endif

	printf("JoyKeyState: 0x%x\n", JoyKeyState);

	switch (wp) {
	case SDLK_UP:
		puts("key up");
		if (!(JoyKeyState&JOY_DOWN))
			JoyKeyState |= JOY_UP;
		break;

	case SDLK_DOWN:
		if (!(JoyKeyState&JOY_UP))
			JoyKeyState |= JOY_DOWN;
		break;

	case SDLK_LEFT:
		if (!(JoyKeyState&JOY_RIGHT))
			JoyKeyState |= JOY_LEFT;
		break;

	case SDLK_RIGHT:
		if (!(JoyKeyState&JOY_LEFT))
			JoyKeyState |= JOY_RIGHT;
		break;

	case SDLK_z:
		puts("key z");
		if (Config.JoyKeyReverse)
			JoyKeyState |= JOY_TRG2;
		else
			JoyKeyState |= JOY_TRG1;
		break;

	case SDLK_x:
		puts("key x");
		if (Config.JoyKeyReverse)
			JoyKeyState |= JOY_TRG1;
		else
			JoyKeyState |= JOY_TRG2;
		break;
	}
}

// ----------------------------------
//	WM_KEYUP
// ----------------------------------
void
Keyboard_KeyUp(DWORD wp)
{
	BYTE code;
	BYTE newwp;
#if 0
	if (wp & ~0xff) {
		if (wp == GDK_VoidSymbol)
			code = NC;
		else if ((wp & 0xff00) == 0xff00)
			code = KeyTable[(wp & 0xff) | 0x100];
		else
			code = NC;
	} else
		code = KeyTable[wp & 0xff];
#endif
	code = get_x68k_keycode(wp);
	if (code < 0) {
		return;
	}
#if 0
	if (code != NC) {
		newwp = ((KeyBufWP + 1) & (KeyBufSize - 1));
		if (newwp != KeyBufRP) {
			KeyBuf[KeyBufWP] = code | 0x80;
			KeyBufWP = newwp;
		}
	}
#else
	send_keycode(code, P6K_UP);
#endif

	printf("JoyKeyState: 0x%x\n", JoyKeyState);

	switch(wp) {
	case SDLK_UP:
		JoyKeyState &= ~JOY_UP;
		break;

	case SDLK_DOWN:
		JoyKeyState &= ~JOY_DOWN;
		break;

	case SDLK_LEFT:
		JoyKeyState &= ~JOY_LEFT;
		break;

	case SDLK_RIGHT:
		JoyKeyState &= ~JOY_RIGHT;
		break;

	case SDLK_z:
		if (Config.JoyKeyReverse)
			JoyKeyState &= ~JOY_TRG2;
		else
			JoyKeyState &= ~JOY_TRG1;
		break;

	case SDLK_x:
		if (Config.JoyKeyReverse)
			JoyKeyState &= ~JOY_TRG1;
		else
			JoyKeyState &= ~JOY_TRG2;
		break;
	}

}

// ----------------------------------
//	Key Check
//	1フレーム中に4回（2400bps/10bit/60fpsとすれば、だが）呼ばれて、MFPにデータを送る
// ----------------------------------

void
Keyboard_Int(void)
{
	if (KeyBufRP != KeyBufWP) {
		printf("KeyBufRP:%d, KeyBufWP:%d\n", KeyBufRP, KeyBufWP);
		if (!KeyIntFlag) {
			LastKey = KeyBuf[KeyBufRP];
			KeyBufRP = ((KeyBufRP+1)&(KeyBufSize-1));
			KeyIntFlag = 1;
			MFP_Int(3);
		}
	} else if (!KeyIntFlag) {
		LastKey = 0;
	}
}

/********** ソフトウェアキーボード **********/

#if defined(PSP) || defined(USE_OGLES11)

// 選択しているキーの座標 (初期値'Q')
int  kbd_kx = 1, kbd_ky = 2;

#define KEYXMAX 32767
int Keyboard_get_key_ptr(int x, int y)
{
	int i, j;
	int p = 0;
	int tx, ty; // tmp x, tmp y

	tx = KEYXMAX;
	ty = 0;
	i = 0;
	while (kbd_key[i].x != -1) {
		// skip dummy
		if (kbd_key[i].c == 0) {
			i++;
			continue;
		}
		if (tx > kbd_key[i].x) {
			if (ty == y)
				break;
			ty++;
		}
		tx = kbd_key[i++].x;
	}

	return i + x;
}

static void set_ptr_to_kxy(int p)
{
	int kx = 0, ky = 0;
	int i;

	for (i = 0; i < p; i++) {
		if (kbd_key[i].x > kbd_key[i + 1].x) {
			ky++;
			kx = 0;
		} else {
			kx++;
		}
	}

	kbd_kx = kx, kbd_ky = ky;
}

static void set_near_key(int p, int tx)
{
	while (kbd_key[p].x < kbd_key[p + 1].x) {
		if (kbd_key[p].x >= tx) {
			// x座標の一番近いキーに移動する
			if (abs(kbd_key[p].x - tx)
			    > abs(tx - kbd_key[p - 1].x)) {
				p--;
			}
			break;
		}
		p++;
	}
	set_ptr_to_kxy(p);
}

static void key_up(void)
{
	int p;
	int c = 2;
	int tx; // tmp x

	p = Keyboard_get_key_ptr(kbd_kx, kbd_ky);
	tx = kbd_key[p].x;

	// 上の行の先頭を探す
	while (p > 0) {
		if (kbd_key[p - 1].x > kbd_key[p].x) {
			c--;
			if (c == 0) {
				break;
			}
		}
		p--;
	}

	if (kbd_key[p].x == tx) {
		set_ptr_to_kxy(p);
		return;
	}

	set_near_key(p, tx);
}

static void key_down(void)
{
	int p;
	int tx; // tmp x

	p = Keyboard_get_key_ptr(kbd_kx, kbd_ky);
	tx = kbd_key[p].x;

	// 下の行の先頭を探す
	while (kbd_key[p].x != -1) {
		if (kbd_key[p + 1].x < kbd_key[p].x) {
			p++;
			break;
		}
		p++;
	}

	set_near_key(p, tx);
}


// dx と dy はどちらか一方は 0 とする。 dx: -1/0/+1 dy: -1/0/+1
static void mv_key(int dx, int dy)
{
	int p;

	if ((kbd_ky == 0 && dy == -1) || (kbd_ky == 5 && dy == +1)) {
		return;
	}

	if (kbd_kx == 0 && dx == -1) {
		return;
	}
	p = Keyboard_get_key_ptr(kbd_kx, kbd_ky);
	if (dx == +1 &&
	    kbd_key[p + 1].x != -1 && kbd_key[p].x > kbd_key[p + 1].x) {
		return;
	}

	WinDraw_reverse_key(kbd_kx, kbd_ky);

	// 飛ばし先の微調整が必要なものたち
	// カーソルキー周りは面倒なので微調整なし
	if (kbd_kx == 12 && kbd_ky == 3 && dx == +1) {
		// ] の右となりは RET に飛ばす
		kbd_kx = 14, kbd_ky = 2;
	} else if (kbd_kx == 12 && kbd_ky == 5 && dx == +1) {
		// テンキーの . の右となりは ENT に飛ばす
		kbd_kx = 19, kbd_ky = 4;
	} else {
		// 微調整の必要なし
		kbd_kx += dx;
	}

	if (dy < 0) {
		key_up();
	}
	if (dy > 0) {
		key_down();
	}

	// RETの左側のダミーキーはRETに移動する
	if (kbd_kx == 13 && kbd_ky == 2) {
		if (dx == 0) {
			// 上下からダミーキーに行った場合
			kbd_kx = 14;
		} else {
			// 左右からダミーキーに行った場合
			kbd_kx += dx;
		}
	}

	WinDraw_reverse_key(kbd_kx, kbd_ky);
}

static void send_key(int flag)
{

	BYTE code;

	code = kbd_key[Keyboard_get_key_ptr(kbd_kx, kbd_ky)].c;
	//p6logd("sendkey: %d", code);

	send_keycode(code, flag);
}

void Keyboard_skbd(void)
{
	static kx = 1, ky = 2;
	BYTE joy;

	joy = get_joy_downstate();
	reset_joy_downstate();

	if (!(joy & JOY_UP)) {
		mv_key(0, -1);
	}
	if (!(joy & JOY_DOWN)) {
		mv_key(0, +1);
	}
	if (!(joy & JOY_LEFT)) {
		mv_key(-1, 0);
	}
	if (!(joy & JOY_RIGHT)) {
		mv_key(+1, 0);
	}
	if (!(joy & JOY_TRG1)) {
		send_key(P6K_DOWN);
	}
	if (!(joy & JOY_TRG2)) {
		// BSキーをkey down
		send_keycode(0xf, P6K_DOWN);
	}

	joy = get_joy_upstate();
	reset_joy_upstate();
	if (joy & JOY_TRG1) {
		send_key(P6K_UP);
	}
	if (joy & JOY_TRG2) {
		// BSキーをkey up
		send_keycode(0xf, P6K_UP);
	}

}

int skbd_mode = FALSE;

void Keyboard_ToggleSkbd(void)
{
	skbd_mode = (skbd_mode == TRUE)? FALSE : TRUE;
}

#endif //defined(PSP) || defined(USE_OGLES11)

int Keyboard_IsSwKeyboard(void)
{
#if defined(PSP)
	return skbd_mode;
#elif defined(USE_OGLES11)
	if (kbd_x < 700) {
		return TRUE;
	} else {
		return FALSE;
	}
#else
	return FALSE;
#endif
}
