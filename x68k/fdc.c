// ---------------------------------------------------------------------------------------
//  FDC.C - Floppy Disk Controller (uPD72065)
//  ToDo : 未実装コマンド、胡散臭い部分（多数）の見直し、DMACとの連携部の見直し
//    D88でのエラー処理とかマシになったはず……でもその分汚い……
// ---------------------------------------------------------------------------------------

#include "fdc.h"
#include "fdd.h"
#include "ioc.h"
#include "irqh.h"
#include "dmac.h"
#include "m68000.h"
#include "fileio.h"
#include "winx68k.h"

static const BYTE CMD_TABLE[32] = {0, 0, 8, 2, 1, 8, 8, 1, 0, 8, 1, 0, 8, 5, 0, 2,
                                   0, 8, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 8, 0, 0};
static const BYTE DAT_TABLE[32] = {0, 0, 7, 0, 1, 7, 7, 0, 2, 7, 7, 0, 7, 7, 0, 0,
                                   0, 7, 0, 0, 1, 0, 0, 0, 0, 7, 0, 0, 0, 7, 0, 0};

/*  Params for Read / ReadDel / ReadDiag / Write / WriteDel / ScanEq / ScanLEq / ScanHEq  */
typedef struct {
	BYTE cmd;
	BYTE us;
	BYTE c;
	BYTE h;
	BYTE r;
	BYTE n;
	BYTE eot;
	BYTE gsl;
	BYTE dtl;
} FDCPRM0;

/*  Params for ReadID / Seek / SenseDevStat */
typedef struct {
	BYTE cmd;
	BYTE us;
	BYTE n;
} FDCPRM1;

/*  Params for WriteID  */
typedef struct {
	BYTE cmd;
	BYTE us;
	BYTE n;
	BYTE sc;
	BYTE gap;
	BYTE d;
} FDCPRM2;


/*  Response for many commands  */
typedef struct {
	BYTE st0;
	BYTE st1;
	BYTE st2;
	BYTE c;
	BYTE h;
	BYTE r;
	BYTE n;
} FDCRSP;


typedef struct {
	int cmd;

	int cyl;
	int drv;
	int ready;
	int ctrl;
	int wexec;

	int rdptr;
	int wrptr;
	int rdnum;
	int wrnum;
	int bufnum;
	int st0;
	int st1;
	int st2;
	BYTE RspBuf[10];
	BYTE PrmBuf[10];
	BYTE DataBuf[0x8000];
	BYTE ScanBuf[0x8000];
} FDC;

static FDC fdc;


#define US(p) (p->us&3)
#define HD(p) ((p->us>>2)&1)
#define SK(p) ((p->us>>5)&1)
#define MF(p) ((p->us>>6)&1)
#define MT(p) ((p->us>>7)&1)

// -----------------------------------------------------------------------
//   割り込みベクタ
// -----------------------------------------------------------------------
DWORD FASTCALL FDC_Int(BYTE irq)
{
	IRQH_IRQCallBack(irq);
	if (irq==1)
		return ((DWORD)IOC_IntVect);
	else
		return -1;
}


// -----------------------------------------------------------------------
//   初期化
// -----------------------------------------------------------------------
void FDC_Init(void)
{
	memset(&fdc, 0, sizeof(FDC));
}


void FDC_SetForceReady(int n)
{
	fdc.ready = n;
}


static void FDC_SetInt(void)
{
	IOC_IntStat |= 0x80;
	if ( IOC_IntStat&4 ) IRQH_Int(1, &FDC_Int);
}


// -----------------------------------------------------------------------
//   Excution Phase の終了
// -----------------------------------------------------------------------
void FDC_EPhaseEnd(void)
{
	FDCID id;
	FDCRSP* rsp = (FDCRSP*)fdc.RspBuf;
	FDD_GetCurrentID(fdc.drv, &id);
	rsp->c = id.c;
	rsp->h = id.h;
	rsp->r = id.r;
	rsp->n = id.n;
	rsp->st0 = fdc.st0;
	rsp->st1 = fdc.st1;
	rsp->st2 = fdc.st2;
	fdc.bufnum = 0;
	fdc.rdptr = 0;
	fdc.wexec = 0;
	FDC_SetInt();
}


int FDC_IsDataReady(void)
{
	return ((fdc.bufnum)?1:0);
}



static void FDC_ExecCmd(void)
{
	FDCPRM0* prm0 = (FDCPRM0*)fdc.PrmBuf;
	FDCPRM1* prm1 = (FDCPRM1*)fdc.PrmBuf;
	FDCPRM2* prm2 = (FDCPRM2*)fdc.PrmBuf;
	FDCRSP* rsp   = (FDCRSP*)fdc.RspBuf;
	FDCID id;
	int ret;

/*{
FILE* fp = fopen("_fdc.txt", "a");
fprintf(fp, "Cmd:%d  CurCy:%d  \n", fdc.cmd, fdc.cyl);
fclose(fp);
}*/

	switch ( fdc.cmd ) {
	case 2:		// ReadDiagnostic
		fdc.st0 = prm1->us&7;
		if ( (FDD_IsReady(fdc.drv))||(fdc.ready) ) {
			if ( !FDD_Seek(fdc.drv, (fdc.cyl<<1)+HD(prm0), &id) ) {
				fdc.st0 |= 0x40;
				fdc.st1 |= 0x04;
			} else {
				FDCID retid;
				id.c = prm0->c;
				id.h = prm0->h;
				id.r = prm0->r;
				id.n = prm0->n;
				ret = FDD_ReadDiag(fdc.drv, &id, &retid, fdc.DataBuf);
				if ( !ret ) {
					fdc.st0 |= 0x40;
					fdc.st1 |= 0x04;
				} else {
					fdc.bufnum = (retid.n)?(128<<prm0->n):(prm0->dtl);
					if ( memcmp(&id, &retid, sizeof(FDCID)) ) fdc.st1 |= 0x24;
					if ( ret&2 )    fdc.st2 |= 0x40;
					if ( ret&0x0c )	fdc.st1 |= 0x20;
					if ( ret&0x08 )	fdc.st2 |= 0x20;
					if ( ret&0x10 )	fdc.st2 |= 0x01;
				}
			}
		} else {
			fdc.st0 |= 0x48;
		}
		if ( fdc.st0&0x40 ) FDC_EPhaseEnd();
		break;
	case 3:		// Specify
		/* Nothing to do */
		break;
	case 4:		// SenseDeviceStatus
		fdc.st0 = prm1->us&7;
		rsp->st0 = fdc.st0;
		if ( !fdc.cyl )                            rsp->st0 |= 0x10;
		if ( (FDD_IsReady(fdc.drv))||(fdc.ready) ) rsp->st0 |= 0x20;
		if ( FDD_IsReadOnly(fdc.drv) )             rsp->st0 |= 0x40;
/*{
FILE* fp = fopen("_fdc.txt", "a");
fprintf(fp, "  ### SDS  Ret=$%02X\n", rsp->st0);
fclose(fp);
}*/
		break;
	case 5:		// WriteData
	case 9:		// WriteDeletedData
		fdc.st0 = prm1->us&7;
		if ( FDD_IsReadOnly(fdc.drv) ) {
			fdc.st0 |= 0x40;
			fdc.st1 |= 0x02;
		} else if ( (FDD_IsReady(fdc.drv))||(fdc.ready) ) {
			if ( !FDD_Seek(fdc.drv, (fdc.cyl<<1)+HD(prm0), &id) ) {
				fdc.st0 |= 0x40;
				fdc.st1 |= 0x04;
			} else {
				fdc.bufnum = (prm0->n)?(128<<prm0->n):(prm0->dtl);
				fdc.wexec = 1;
			}
		} else {
			fdc.st0 |= 0x48;
		}
		if ( fdc.st0&0x40 ) FDC_EPhaseEnd();
		break;
	case 6:		// ReadData
	case 12:	// ReadDeletedData
	case 17:	// ScanEqual
	case 25:	// ScanLowOrEqual
	case 29:	// ScanHighOrEqual
		fdc.st0 = prm1->us&7;
		if ( (FDD_IsReady(fdc.drv))||(fdc.ready) ) {
			if ( !FDD_Seek(fdc.drv, (fdc.cyl<<1)+HD(prm0), &id) ) {
				fdc.st0 |= 0x40;
				fdc.st1 |= 0x04;
			} else {
				id.c = prm0->c;
				id.h = prm0->h;
				id.r = prm0->r;
				id.n = prm0->n;
				if ( (fdc.cmd==6)||(fdc.cmd==12) )
					ret = FDD_Read(fdc.drv, &id, fdc.DataBuf);
				else
					ret = FDD_Read(fdc.drv, &id, fdc.ScanBuf);
				if ( !ret ) {
					fdc.st0 |= 0x40;
					fdc.st1 |= 0x04;
				} else {
					fdc.bufnum = (prm0->n)?(128<<prm0->n):(prm0->dtl);
					if ( (fdc.cmd!=6)&&(fdc.cmd!=12) ) fdc.wexec = 1;
					if ( ((ret&2)&&(fdc.cmd!=12))||((!(ret&2))&&(fdc.cmd==12)) ) fdc.st2 |= 0x40;
					if ( ret&0x1c ) fdc.st0 |= 0x40;
					if ( ret&0x0c )	fdc.st1 |= 0x20;
					if ( ret&0x08 )	fdc.st2 |= 0x20;
					if ( ret&0x10 )	fdc.st2 |= 0x01;
				}
			}
		} else {
			fdc.st0 |= 0x48;
		}
		if ( fdc.st0&0x40 ) FDC_EPhaseEnd();
/*{
FILE* fp = fopen("_fdc.txt", "a");
fprintf(fp, "  ### Read  C:$%02X H:$%02X R:$%02X N:$%02X  EOT=$%02X  ret=$%02X/$%02X/$%02X\n", id.c, id.h, id.r, id.n, prm0->eot, fdc.st0, fdc.st1, fdc.st2);
fclose(fp);
}*/
		break;
	case 7:		// Recalibrate
		fdc.st0 = prm1->us&7;
		fdc.cyl = 0;
		if ( (!FDD_IsReady(fdc.drv))&&(!fdc.ready) ) {
			fdc.st0 |= 0x48;		// FDなし
		} else if ( (fdc.drv>=0)&&(fdc.drv<2) ) {
			fdc.st0 |= 0x20;		// FDあり、ドライブあり
		} else {
			fdc.st0 |= 0x50;		// ドライブなし
		}
		FDC_SetInt();
		break;
	case 8:		// SenseIntStatus
		rsp->st0 = fdc.st0;
		rsp->st1 = fdc.cyl;
		fdc.st0 = 0x80;
		break;
	case 10:	// ReadID
		memset(&id, 0, sizeof(FDCID));
		fdc.st0 = prm1->us&7;
		if ( (FDD_IsReady(fdc.drv))||(fdc.ready) ) {
			if ( !FDD_Seek(fdc.drv, (fdc.cyl<<1)+HD(prm0), &id) ) {
				fdc.st0 |= 0x40;
				fdc.st1 |= 0x04;
			} else {
				ret = FDD_ReadID(fdc.drv, &id);
				if ( !ret ) {
					fdc.st0 |= 0x40;
					fdc.st1 |= 0x04;
				} else {
					if ( ret&2 ) fdc.st2 |= 0x40;
				}
			}
		} else {
			fdc.st0 |= 0x48;
		}
		rsp->c = id.c;
		rsp->h = id.h;
		rsp->r = id.r;
		rsp->n = id.n;
		rsp->st0 = fdc.st0;
		rsp->st1 = fdc.st1;
		rsp->st2 = fdc.st2;
		FDC_SetInt();
		break;
	case 13:	// WriteID
		fdc.st0 = prm1->us&7;
		if ( FDD_IsReadOnly(fdc.drv) ) {
			fdc.st0 |= 0x40;
			fdc.st1 |= 0x02;
		} else if ( (FDD_IsReady(fdc.drv))||(fdc.ready) ) {
			fdc.bufnum = (prm2->sc)<<2;
			fdc.wexec = 1;
		} else {
			fdc.st0 |= 0x48;
		}
		if ( fdc.st0&0x40 ) FDC_EPhaseEnd();
		break;
	case 15:	// Seek
		fdc.st0 = prm1->us&0x07;
		if ( FDD_IsReady(fdc.drv) ) {
			fdc.cyl = prm1->n;
			fdc.st0 |= 0x20;
		} else {
			fdc.st0 |= 0x48;
		}
		FDC_SetInt();
		break;
	}
}


static void FDC_NextTrack(void)
{
	FDCPRM0* prm0 = (FDCPRM0*)fdc.PrmBuf;
	if ( prm0->r==prm0->eot ) {
		if ( (MT(prm0))&&(!HD(prm0))&&(!prm0->h) ) {
			prm0->us |= 4;
			prm0->h = 1;
			prm0->r = 1;
		} else {
			FDC_EPhaseEnd();
			return;
		}
	} else {
		prm0->r++;
	}
	FDC_ExecCmd();
}


static void FDC_WriteBuffer(void)
{
	FDCPRM0* prm0 = (FDCPRM0*)fdc.PrmBuf;
	FDCPRM2* prm2 = (FDCPRM2*)fdc.PrmBuf;
	FDCID id;
	int i;

	switch ( fdc.cmd ) {
	case 5:		// WriteData
	case 9:		// WriteDeletedData
		id.c = prm0->c;
		id.h = prm0->h;
		id.r = prm0->r;
		id.n = prm0->n;
		if ( !FDD_Write(fdc.drv, &id, fdc.DataBuf, (fdc.cmd==9)?1:0) ) {
			fdc.st0 |= 0x40;
			fdc.st1 |= 0x04;
			FDC_SetInt();
		} else {
			FDC_NextTrack();
		}
		break;
	case 13:	// WriteID
		fdc.DataBuf[fdc.wrptr] = prm2->d;
		if ( !FDD_WriteID(fdc.drv, (fdc.cyl<<1)+HD(prm2), fdc.DataBuf, prm2->sc) ) {
			fdc.st0 |= 0x40;
			fdc.st1 |= 0x04;
		}
		FDC_EPhaseEnd();
		break;
	case 17:	// ScanEqual
		for (i=0; i<fdc.bufnum; i++) {
			if ( (fdc.DataBuf[i]!=0xff)&&(fdc.ScanBuf[i]!=fdc.DataBuf[i]) ) {
				fdc.st0 |= 0x40;
				fdc.st2 &= 0xf7;
				fdc.st2 |= 0x04;
				FDC_EPhaseEnd();
				break;
			}
		}
		FDC_NextTrack();
		break;
	case 25:	// ScanLowOrEqual
		for (i=0; i<fdc.bufnum; i++) {
			if ( fdc.DataBuf[i]!=0xff ) {
				if ( fdc.ScanBuf[i]>fdc.DataBuf[i] ) {
					fdc.st0 |= 0x40;
					fdc.st2 &= 0xf7;
					fdc.st2 |= 0x04;
					FDC_EPhaseEnd();
					break;
				} else if ( fdc.ScanBuf[i]<fdc.DataBuf[i] ) {
					break;
				}
			}
		}
		FDC_NextTrack();
		break;
	case 29:	// ScanHighOrEqual
		for (i=0; i<fdc.bufnum; i++) {
			if ( fdc.DataBuf[i]!=0xff ) {
				if ( fdc.ScanBuf[i]<fdc.DataBuf[i] ) {
					fdc.st0 |= 0x40;
					fdc.st2 &= 0xf7;
					fdc.st2 |= 0x04;
					FDC_EPhaseEnd();
					break;
				} else if ( fdc.ScanBuf[i]>fdc.DataBuf[i] ) {
					break;
				}
			}
		}
		FDC_NextTrack();
		break;
	}
}


// -----------------------------------------------------------------------
//   I/O Read
// -----------------------------------------------------------------------
BYTE FASTCALL FDC_Read(DWORD adr)
{
	BYTE ret = 0x00;
	if ( adr==0xe94001 ) {					/* FDC Status */
		ret  = 0x80;
		ret |= ((fdc.rdnum)&&(!fdc.wexec))?0x40:0;
		ret |= ((fdc.wrnum)||(fdc.rdnum))?0x10:0;
		ret &= ((fdc.rdnum==1)&&(fdc.cmd==8))?0xaf:0xff;
	} else if ( adr==0xe94003 ) {           /* Read data */
		if ( fdc.bufnum ) {
			ret = fdc.DataBuf[fdc.rdptr++];
			if ( fdc.rdptr>=fdc.bufnum ) {
				fdc.rdptr = 0;
				FDC_NextTrack();
			}
		} else if ( fdc.rdnum ) {
			ret = fdc.RspBuf[fdc.rdptr++];
			fdc.rdnum--;
		}
	} else if ( adr==0xe94005 ) {
		if ( (fdc.ctrl&1)&&(FDD_IsReady(0)) ) ret = 0x80;
		if ( (fdc.ctrl&2)&&(FDD_IsReady(1)) ) ret = 0x80;
	}
/*{
FILE* fp = fopen("_fdc.txt", "a");
fprintf(fp, "Adr:$%08X  ret=$%02X\n", adr, ret);
fclose(fp);
}*/
	return ret;
}


// -----------------------------------------------------------------------
//   I/O Write
// -----------------------------------------------------------------------
void FASTCALL FDC_Write(DWORD adr, BYTE data)
{
	if ( adr==0xe94003 ) {
		if ( fdc.bufnum ) {                 // WriteData
			fdc.DataBuf[fdc.wrptr++] = data;
			if ( fdc.wrptr>=fdc.bufnum ) {
				FDC_WriteBuffer();
				fdc.wrptr = 0;
			}
		} else {
			if ( fdc.wrnum ) {          // Writing params
				fdc.PrmBuf[fdc.wrptr++] = data;
				fdc.wrnum--;
			} else {                    // Command start
				fdc.cmd = data&0x1f;
				fdc.rdptr = 0;
				fdc.wrptr = 0;
				fdc.rdnum = 0;
				fdc.wrnum = CMD_TABLE[fdc.cmd];
				fdc.PrmBuf[fdc.wrptr++] = data;
			}
			if ( !fdc.wrnum ) {
				fdc.wrptr = 0;
				fdc.rdnum = DAT_TABLE[fdc.cmd];
				fdc.st1 = 0;
				fdc.st2 = 0;
				if ( (fdc.cmd==17)||(fdc.cmd==25)||(fdc.cmd==29) ) fdc.st2 |= 8;
				FDC_ExecCmd();
			}
		}
	} else if ( adr==0xe94005 ) {
		if ( (fdc.ctrl&0x01)&&(!(data&0x01)) ) {	// Drive0 control (Down edge)
			if ( fdc.ctrl&0x20 ) FDD_EjectFD(0);
			FDD_SetEMask(0, (fdc.ctrl&0x40)?1:0);
			FDD_SetBlink(0, (fdc.ctrl&0x80)?1:0);
		}
		if ( (fdc.ctrl&0x02)&&(!(data&0x02)) ) {	// Drive1 control (Down edge)
			if ( fdc.ctrl&0x20 ) FDD_EjectFD(1);
			FDD_SetEMask(1, (fdc.ctrl&0x40)?1:0);
			FDD_SetBlink(1, (fdc.ctrl&0x80)?1:0);
		}
		fdc.ctrl = data;
	} else if ( adr==0xe94007 ) {
		fdc.drv = (data&0x80)?(data&3):(-1);
		FDD_SetAccess(fdc.drv);
/*{
FILE* fp = fopen("_fdc.txt", "a");
fprintf(fp, "ActiveDrive=%d\n", fdc.drv);
fclose(fp);
}*/
	}
}
