#include "common.h"
#include "fileio.h"
#include "fdc.h"
#include "fdd.h"
#include "disk_dim.h"

// DIM Image Header
typedef struct {
	BYTE	type;
	BYTE	trkflag[170];
	BYTE	headerinfo[15];
	BYTE	date[4];
	BYTE	time[4];
	BYTE	comment[61];
	BYTE	overtrack;
} DIM_HEADER;

// DIM Disk Type
enum {
	DIM_2HD = 0,
	DIM_2HS,
	DIM_2HC,
	DIM_2HDE,
	DIM_2HQ = 9,
};

static const int SctLength[10] = {
	1024*8, 1024*9, 512*15, 1024*9, 0, 0, 0, 0, 0, 512*18
};

static char           DIMFile[4][MAX_PATH];
static int            DIMCur[4] = {0, 0, 0, 0};
static int            DIMTrk[4] = {0, 0, 0, 0};
static unsigned char* DIMImg[4] = {0, 0, 0, 0};

void DIM_Init(void)
{
	int drv;

	for (drv=0; drv<4; drv++) {
		DIMCur[drv] = 0;
		DIMImg[drv] = 0;
		ZeroMemory(DIMFile[drv], MAX_PATH);
	}
}


void DIM_Cleanup(void)
{
	int drv;
	for (drv=0; drv<4; drv++) DIM_Eject(drv);
}


int DIM_SetFD(int drv, char* filename)
{
	FILEH fp;
	DIM_HEADER* dh;
	unsigned int i, len;
	unsigned char* p;

	strncpy(DIMFile[drv], filename, MAX_PATH);
	DIMFile[drv][MAX_PATH-1] = 0;

	DIMImg[drv] = (unsigned char*)malloc(1024*9*170+sizeof(DIM_HEADER));		// Maximum size
	if ( !DIMImg[drv] ) return FALSE;
	memset(DIMImg[drv], 0xe5, 1024*9*170+sizeof(DIM_HEADER));
	fp = File_Open(DIMFile[drv]);
	if ( !fp ) {
		ZeroMemory(DIMFile[drv], MAX_PATH);
		FDD_SetReadOnly(drv);
		return FALSE;
	}

	File_Seek(fp, 0, FSEEK_SET);
	if ( File_Read(fp, DIMImg[drv], sizeof(DIM_HEADER))!=sizeof(DIM_HEADER) ) goto dim_set_error;
	dh = (DIM_HEADER*)DIMImg[drv];
	if ( dh->type>9 ) goto dim_set_error;
	len = SctLength[dh->type];
	if ( !len ) goto dim_set_error;
	p = DIMImg[drv]+sizeof(DIM_HEADER);
	for (i=0; i<170; i++) {
		if ( dh->trkflag[i] ) {
			if ( File_Read(fp, p, len)!=len ) goto dim_set_error;
		}
		p += len;
	}
	File_Close(fp);
	if ( !dh->overtrack ) memset(dh->trkflag, 1, 170);
	return TRUE;

dim_set_error:
	File_Close(fp);
	FDD_SetReadOnly(drv);
	return FALSE;
}


int DIM_Eject(int drv)
{
	FILEH fp;
	DIM_HEADER* dh;
	unsigned int i, len;
	unsigned char* p;

	if ( !DIMImg[drv] ) {
		ZeroMemory(DIMFile[drv], MAX_PATH);
		return FALSE;
	}
	dh = (DIM_HEADER*)DIMImg[drv];
	len = SctLength[dh->type];
	p = DIMImg[drv]+sizeof(DIM_HEADER);
	if ( !FDD_IsReadOnly(drv) ) {
		fp = File_Open(DIMFile[drv]);
		if ( !fp ) goto dim_eject_error;
		File_Seek(fp, 0, FSEEK_SET);
		if ( File_Write(fp, DIMImg[drv], sizeof(DIM_HEADER))!=sizeof(DIM_HEADER) ) goto dim_eject_error;
		for (i=0; i<170; i++) {
			if ( dh->trkflag[i] ) {
				if ( File_Write(fp, p, len)!=len ) goto dim_eject_error;
			}
			p += len;
		}
		File_Close(fp);
	}
	free(DIMImg[drv]);
	DIMImg[drv] = 0;
	ZeroMemory(DIMFile[drv], MAX_PATH);
	return TRUE;

dim_eject_error:
	free(DIMImg[drv]);
	DIMImg[drv] = 0;
	ZeroMemory(DIMFile[drv], MAX_PATH);
	return FALSE;
}


static void SetID(int drv, FDCID* id, int c, int h, int r)
{
	int type = DIMImg[drv][0];
	switch (type) {
		case DIM_2HD:				// 1024byte/sct, 8sct/trk
			id->n = 3; break;
		case DIM_2HS:				// 1024byte/sct, 9sct/trk
			if ( (c)||(h)||(r!=1) ) r += 9;
			id->n = 3; break;
		case DIM_2HDE:
			if ( (c)||(h)||(r!=1) ) h += 0x80;
			id->n = 3; break;
		case DIM_2HC:				// 512byte/sct, 15sct/trk
		case DIM_2HQ:				// 512byte/sct, 18sct/trk
			id->n = 2; break;
	}
	id->c = c;
	id->h = h;
	id->r = r;
}


static int IncTrk(int drv, int r)
{
	int type = DIMImg[drv][0];
	switch (type) {
		case DIM_2HD:				// 1024byte/sct, 8sct/trk
			r = (r+1)&7;
			break;
		case DIM_2HS:				// 1024byte/sct, 9sct/trk
			if ( r>8 ) r -= 9;		// 9SCDRVÍÑ
		case DIM_2HDE:
			r = (r+1)%9;
			break;
		case DIM_2HC:				// 512byte/sct, 15sct/trk
			r = (r+1)%15;
			break;
		case DIM_2HQ:				// 512byte/sct, 18sct/trk
			r = (r+1)%18;
			break;
	}
	return r;
}


static int GetPos(int drv, FDCID* id)
{
	int ret, c = id->c, h = id->h, r = id->r, n = id->n;
	int type = DIMImg[drv][0];
	switch (type) {
		case DIM_2HD:				// 1024byte/sct, 8sct/trk
			if ( (c<0)||(c>84)||(h<0)||(h>1)||(r<1)||(r>8)||(n!=3) ) return 0;
			ret = SctLength[type]*(c*2+h)+((r-1)<<10);
			ret += sizeof(DIM_HEADER);
			break;
		case DIM_2HS:				// 1024byte/sct, 9sct/trk
			if ( r>9 ) r -= 9;		// 9SCDRVÍÑ
			if ( (c<0)||(c>84)||(h<0)||(h>1)||(r<1)||(r>9)||(n!=3) ) return 0;
			ret = SctLength[type]*(c*2+h)+((r-1)<<10);
			ret += sizeof(DIM_HEADER);
			break;
		case DIM_2HDE:
			h &= 1;					// 9SCDRVÍÑ
			if ( (c<0)||(c>84)||(h<0)||(h>1)||(r<1)||(r>9)||(n!=3) ) return 0;
			ret = SctLength[type]*(c*2+h)+((r-1)<<10);
			ret += sizeof(DIM_HEADER);
			break;
		case DIM_2HC:				// 512byte/sct, 15sct/trk
			if ( (c<0)||(c>84)||(h<0)||(h>1)||(r<1)||(r>15)||(n!=2) ) return 0;
			ret = SctLength[type]*(c*2+h)+((r-1)<<9);
			ret += sizeof(DIM_HEADER);
			break;
		case DIM_2HQ:				// 512byte/sct, 18sct/trk
			if ( (c<0)||(c>84)||(h<0)||(h>1)||(r<1)||(r>18)||(n!=2) ) return 0;
			ret = SctLength[type]*(c*2+h)+((r-1)<<9);
			ret += sizeof(DIM_HEADER);
			break;
		default:
			ret = 0;
			break;
	}
	return ret;
}


static int CheckTrack(int drv, int trk)
{
	DIM_HEADER* dh = (DIM_HEADER*)DIMImg[drv];
	switch (dh->type) {
		case DIM_2HD:				// 1024byte/sct, 8sct/trk
			if ( ((trk>153)&&(!dh->overtrack))||(!dh->trkflag[trk]) ) return 0;
			break;
		case DIM_2HS:				// 1024byte/sct, 9sct/trk
			if ( ((trk>159)&&(!dh->overtrack))||(!dh->trkflag[trk]) ) return 0;
			break;
		case DIM_2HDE:
			if ( ((trk>159)&&(!dh->overtrack))||(!dh->trkflag[trk]) ) return 0;
			break;
		case DIM_2HC:				// 512byte/sct, 15sct/trk
			if ( ((trk>159)&&(!dh->overtrack))||(!dh->trkflag[trk]) ) return 0;
			break;
		case DIM_2HQ:				// 512byte/sct, 18sct/trk
			if ( ((trk>159)&&(!dh->overtrack))||(!dh->trkflag[trk]) ) return 0;
			break;
		default:
			return FALSE;
	}
	return TRUE;
}


int DIM_Seek(int drv, int trk, FDCID* id)
{
	if ( (drv<0)||(drv>3) ) return FALSE;
	if ( (trk<0)||(trk>169) ) return FALSE;
	if ( !DIMImg[drv] ) return FALSE;
	if ( DIMTrk[drv]!=trk ) DIMCur[drv] = 0;
	SetID(drv, id, trk>>1, trk&1, DIMCur[drv]+1);
	DIMTrk[drv] = trk;
	return TRUE;
}


int DIM_GetCurrentID(int drv, FDCID* id)
{
	if ( (drv<0)||(drv>3) ) return FALSE;
	if ( (DIMTrk[drv]<0)||(DIMTrk[drv]>169) ) return FALSE;
	if ( !DIMImg[drv] ) return FALSE;
	if ( !CheckTrack(drv, DIMTrk[drv]) ) return FALSE;
	SetID(drv, id, DIMTrk[drv]>>1,DIMTrk[drv]&1, DIMCur[drv]+1);
	return TRUE;
}


int DIM_ReadID(int drv, FDCID* id)
{
	if ( (drv<0)||(drv>3) ) return FALSE;
	if ( (DIMTrk[drv]<0)||(DIMTrk[drv]>169) ) return FALSE;
	if ( !DIMImg[drv] ) return FALSE;
	if ( !CheckTrack(drv, DIMTrk[drv]) ) return FALSE;
	SetID(drv, id, DIMTrk[drv]>>1,DIMTrk[drv]&1, DIMCur[drv]+1);
	DIMCur[drv] = IncTrk(drv, DIMCur[drv]);
	return TRUE;
}


int DIM_WriteID(int drv, int trk, unsigned char* buf, int num)
{
#if 0
	int i;
	unsigned char c = buf[num<<2];
	if ( (drv<0)||(drv>3) ) return FALSE;
	if ( (trk<0)||(trk>169) ) return FALSE;
	if ( !DIMImg[drv] ) return FALSE;
	if ( num!=8 ) return FALSE;
	for (i=0; i<8; i++, buf+=4) {
		if ( (((buf[0]<<1)+buf[1])!=trk)||(buf[2]<1)||(buf[2]>8)||(buf[3]!=3) ) return FALSE;
	}
	DIMTrk[drv] = trk;
	return TRUE;
#else
	(void)drv;
	(void)trk;
	(void)buf;
	(void)num;
	return FALSE;
#endif
}


int DIM_Read(int drv, FDCID* id, unsigned char* buf)
{
	int pos;
	if ( (drv<0)||(drv>3) ) return FALSE;
	if ( !DIMImg[drv] ) return FALSE;
	if ( (((id->c<<1)+(id->h&1))!=DIMTrk[drv]) ) return FALSE;
	if ( !CheckTrack(drv, (id->c<<1)+(id->h&1)) ) return FALSE;
	pos = GetPos(drv, id);
	if ( !pos ) return FALSE;
	memcpy(buf, DIMImg[drv]+pos, (id->n==2)?512:1024);
	DIMCur[drv] = IncTrk(drv, id->r-1);
	return TRUE;
}


int DIM_ReadDiag(int drv, FDCID* id, FDCID* retid, unsigned char* buf)
{
	int pos;
	(void)id;
	if ( (drv<0)||(drv>3) ) return FALSE;
	if ( !DIMImg[drv] ) return FALSE;
	if ( !CheckTrack(drv, DIMTrk[drv]) ) return FALSE;
	SetID(drv, retid, DIMTrk[drv]>>1, DIMTrk[drv]&1, DIMCur[drv]+1);
	pos = GetPos(drv, retid);
	if ( !pos ) return FALSE;
	memcpy(buf, DIMImg[drv]+pos, (retid->n==2)?512:1024);
	DIMCur[drv] = IncTrk(drv, DIMCur[drv]);
	return TRUE;
}


int DIM_Write(int drv, FDCID* id, unsigned char* buf, int del)
{
	int pos;
	(void)del;
	if ( (drv<0)||(drv>3) ) return FALSE;
	if ( !DIMImg[drv] ) return FALSE;
	if ( (((id->c<<1)+(id->h&1))!=DIMTrk[drv]) ) return FALSE;
	if ( !CheckTrack(drv, (id->c<<1)+(id->h&1)) ) return FALSE;
	pos = GetPos(drv, id);
	if ( !pos ) return FALSE;
	memcpy(DIMImg[drv]+pos, buf, (id->n==2)?512:1024);
	DIMCur[drv] = IncTrk(drv, id->r-1);
	return TRUE;
}
