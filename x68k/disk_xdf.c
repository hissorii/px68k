#include "common.h"
#include "fileio.h"
#include "fdc.h"
#include "fdd.h"
#include "disk_xdf.h"

static char           XDFFile[4][MAX_PATH];
static int            XDFCur[4] = {0, 0, 0, 0};
static int            XDFTrk[4] = {0, 0, 0, 0};
static unsigned char* XDFImg[4] = {0, 0, 0, 0};

void XDF_Init(void)
{
	int drv;

	for (drv=0; drv<4; drv++) {
		XDFCur[drv] = 0;
		XDFImg[drv] = 0;
		ZeroMemory(XDFFile[drv], MAX_PATH);
	}
}


void XDF_Cleanup(void)
{
	int drv;
	for (drv=0; drv<4; drv++) XDF_Eject(drv);
}


int XDF_SetFD(int drv, char* filename)
{
	FILEH fp;

	strncpy(XDFFile[drv], filename, MAX_PATH);
	XDFFile[drv][MAX_PATH-1] = 0;

	XDFImg[drv] = (unsigned char*)malloc(1261568);
	if ( !XDFImg[drv] ) return FALSE;
	memset(XDFImg[drv], 0xe5, 1261568);
	fp = File_Open(XDFFile[drv]);
	if ( !fp ) {
		ZeroMemory(XDFFile[drv], MAX_PATH);
		FDD_SetReadOnly(drv);
		return FALSE;
	}
	File_Seek(fp, 0, FSEEK_SET);
	File_Read(fp, XDFImg[drv], 1261568);
	File_Close(fp);
	return TRUE;
}


int XDF_Eject(int drv)
{
	FILEH fp;

	if ( !XDFImg[drv] ) {
		ZeroMemory(XDFFile[drv], MAX_PATH);
		return FALSE;
	}
	if ( !FDD_IsReadOnly(drv) ) {
		fp = File_Open(XDFFile[drv]);
		if ( !fp ) goto xdf_eject_error;
		File_Seek(fp, 0, FSEEK_SET);
		if ( File_Write(fp, XDFImg[drv], 1261568)!=1261568 ) goto xdf_eject_error;
		File_Close(fp);
	}
	free(XDFImg[drv]);
	XDFImg[drv] = 0;
	ZeroMemory(XDFFile[drv], MAX_PATH);
	return TRUE;

xdf_eject_error:
	free(XDFImg[drv]);
	XDFImg[drv] = 0;
	ZeroMemory(XDFFile[drv], MAX_PATH);
	return FALSE;
}


int XDF_Seek(int drv, int trk, FDCID* id)
{
	if ( (drv<0)||(drv>3) ) return FALSE;
	if ( (trk<0)||(trk>153) ) return FALSE;
	if ( !XDFImg[drv] ) return FALSE;
	if ( XDFTrk[drv]!=trk ) XDFCur[drv] = 0;
	id->c = trk>>1;
	id->h = trk&1;
	id->r = XDFCur[drv]+1;
	id->n = 3;
	XDFTrk[drv] = trk;
	return TRUE;
}


int XDF_GetCurrentID(int drv, FDCID* id)
{
	if ( (drv<0)||(drv>3) ) return FALSE;
	if ( (XDFTrk[drv]<0)||(XDFTrk[drv]>153) ) return FALSE;
	if ( !XDFImg[drv] ) return FALSE;
	id->c = XDFTrk[drv]>>1;
	id->h = XDFTrk[drv]&1;
	id->r = XDFCur[drv]+1;
	id->n = 3;
	return TRUE;
}


int XDF_ReadID(int drv, FDCID* id)
{
	if ( (drv<0)||(drv>3) ) return FALSE;
	if ( (XDFTrk[drv]<0)||(XDFTrk[drv]>153) ) return FALSE;
	if ( !XDFImg[drv] ) return FALSE;
	id->c = XDFTrk[drv]>>1;
	id->h = XDFTrk[drv]&1;
	id->r = XDFCur[drv]+1;
	id->n = 3;
	XDFCur[drv] = (XDFCur[drv]+1)&7;
	return TRUE;
}


int XDF_WriteID(int drv, int trk, unsigned char* buf, int num)
{
	int i;
	if ( (drv<0)||(drv>3) ) return FALSE;
	if ( (trk<0)||(trk>153) ) return FALSE;
	if ( !XDFImg[drv] ) return FALSE;
	if ( num!=8 ) return FALSE;
	for (i=0; i<8; i++, buf+=4) {
		if ( (((buf[0]<<1)+buf[1])!=trk)||(buf[2]<1)||(buf[2]>8)||(buf[3]!=3) ) return FALSE;
	}
	XDFTrk[drv] = trk;
	return TRUE;
}


int XDF_Read(int drv, FDCID* id, unsigned char* buf)
{
	int pos;
	if ( (drv<0)||(drv>3) ) return FALSE;
	if ( (XDFTrk[drv]<0)||(XDFTrk[drv]>153) ) return FALSE;
	if ( !XDFImg[drv] ) return FALSE;
	if ( (((id->c<<1)+id->h)!=XDFTrk[drv]) ) return FALSE;
	if ( (id->r<1)||(id->r>8) ) return FALSE;
	if ( (id->h!=0)&&(id->h!=1) ) return FALSE;
	if ( id->n!=3 ) return FALSE;
	pos = ((((id->c<<1)+(id->h))*8)+(id->r-1))<<10;
	memcpy(buf, XDFImg[drv]+pos, 1024);
	XDFCur[drv] = (id->r)&7;
	return TRUE;
}


int XDF_ReadDiag(int drv, FDCID* id, FDCID* retid, unsigned char* buf)
{
	int pos;
	(void)id;
	if ( (drv<0)||(drv>3) ) return FALSE;
	if ( (XDFTrk[drv]<0)||(XDFTrk[drv]>153) ) return FALSE;
	if ( (XDFCur[drv]<0)||(XDFCur[drv]>8) ) return FALSE;
	if ( !XDFImg[drv] ) return FALSE;
	pos = ((XDFTrk[drv]*8)+XDFCur[drv])<<10;
	memcpy(buf, XDFImg[drv]+pos, 1024);
	retid->c = XDFTrk[drv]>>1;
	retid->h = XDFTrk[drv]&1;
	retid->r = XDFCur[drv]+1;
	retid->n = 3;
	XDFCur[drv] = (XDFCur[drv]+1)&7;
	return TRUE;
}


int XDF_Write(int drv, FDCID* id, unsigned char* buf, int del)
{
	int pos;
	(void)del;
	if ( (drv<0)||(drv>3) ) return FALSE;
	if ( (XDFTrk[drv]<0)||(XDFTrk[drv]>153) ) return FALSE;
	if ( !XDFImg[drv] ) return FALSE;
	if ( (((id->c<<1)+id->h)!=XDFTrk[drv]) ) return FALSE;
	if ( (id->r<1)||(id->r>8) ) return FALSE;
	if ( (id->h!=0)&&(id->h!=1) ) return FALSE;
	if ( id->n!=3 ) return FALSE;
	pos = ((((id->c<<1)+(id->h))*8)+(id->r-1))<<10;
	memcpy(XDFImg[drv]+pos, buf, 1024);
	XDFCur[drv] = (id->r)&7;
	return TRUE;
}
