/* 
 * Copyright (c) 2003 NONAKA Kimihiro
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgment:
 *      This product includes software developed by NONAKA Kimihiro.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
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

#ifndef	__NP2_DOSIO_H__
#define	__NP2_DOSIO_H__

#include "common.h"
#include "appftype.h"

#define		FILEH		HANDLE

#define		FSEEK_SET	0
#define		FSEEK_CUR	1
#define		FSEEK_END	2

#ifdef __cplusplus
extern "C" {
#endif
							// DOSIO:関数の準備
void dosio_init(void);
void dosio_term(void);
							// ファイル操作
FILEH file_open(LPSTR filename);
FILEH file_create(LPSTR filename, int ftype);
DWORD file_seek(FILEH handle, long pointer, short mode);
DWORD file_lread(FILEH handle, void *data, DWORD length);
DWORD file_lwrite(FILEH handle, void *data, DWORD length);
WORD file_read(FILEH handle, void *data, WORD length);
WORD file_write(FILEH handle, void *data, WORD length);
DWORD file_zeroclr(FILEH handle, DWORD length);
WORD file_lineread(FILEH handle, void *data, WORD length);
short file_close(FILEH handle);
short file_attr(LPSTR filename);
							// カレントファイル操作
void file_setcd(LPSTR exename);
LPSTR file_getcd(LPSTR filename);
FILEH file_open_c(LPSTR filename);
FILEH file_create_c(LPSTR filename, int ftype);
short file_attr_c(LPSTR filename);

int file_getftype(LPSTR filename);

							// 日時の取得
void dosdateset(BYTE *dat);
void dostimeset(BYTE *dat);


LPSTR getFileName(LPSTR filename);
void cutFileName(LPSTR filename);
LPSTR getExtName(LPSTR filename);
void cutExtName(LPSTR filename);
void plusyen(LPSTR str, int len);
void cutyen(LPSTR str);

int kanji1st(LPSTR str, int pos);
int ex_a2i(LPSTR str, int min, int max);

void fname_mix(LPSTR str, LPSTR mix, int size);

/*	$NetBSD: direntry.h,v 1.14 1997/11/17 15:36:32 ws Exp $	*/

/*-
 * Copyright (C) 1994, 1995, 1997 Wolfgang Solfrank.
 * Copyright (C) 1994, 1995, 1997 TooLs GmbH.
 * All rights reserved.
 * Original code by Paul Popelka (paulp@uts.amdahl.com) (see below).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by TooLs GmbH.
 * 4. The name of TooLs GmbH may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY TOOLS GMBH ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL TOOLS GMBH BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * Written by Paul Popelka (paulp@uts.amdahl.com)
 *
 * You can do anything you want with this software, just don't say you wrote
 * it, and don't remove this notice.
 *
 * This software is provided "as is".
 *
 * The author supplies this software to be publicly redistributed on the
 * understanding that the author is not responsible for the correct
 * functioning of this software in any circumstances and is not liable for
 * any damages caused by this software.
 *
 * October 1992
 */

/*
 * This is the format of the contents of the deTime field in the direntry
 * structure.
 * We don't use bitfields because we don't know how compilers for
 * arbitrary machines will lay them out.
 */
#define DT_2SECONDS_MASK	0x1F	/* seconds divided by 2 */
#define DT_2SECONDS_SHIFT	0
#define DT_MINUTES_MASK		0x7E0	/* minutes */
#define DT_MINUTES_SHIFT	5
#define DT_HOURS_MASK		0xF800	/* hours */
#define DT_HOURS_SHIFT		11

/*
 * This is the format of the contents of the deDate field in the direntry
 * structure.
 */
#define DD_DAY_MASK		0x1F	/* day of month */
#define DD_DAY_SHIFT		0
#define DD_MONTH_MASK		0x1E0	/* month */
#define DD_MONTH_SHIFT		5
#define DD_YEAR_MASK		0xFE00	/* year - 1980 */
#define DD_YEAR_SHIFT		9

void unix2dostime(time_t t, unsigned short *ddp, unsigned short *dtp, unsigned char *dhp);
void dos2unixtime(u_int dd, u_int dt, u_int dh, time_t *tp);

#ifdef __cplusplus
};
#endif

#endif	/* __NP2_DOSIO_H__ */
