// ---------------------------------------------------------------------------------------
//  DMAC.C - DMAコントローラ（HD63450）
//  ToDo : もっと奇麗に ^^;
// ---------------------------------------------------------------------------------------

#include "common.h"
#include "winx68k.h"
#include "m68000.h"
#include "memory.h"
#include "irqh.h"
#include "fdc.h"
#include "sasi.h"
#include "adpcm.h"
#include "mercury.h"
#include "dmac.h"

dmac_ch	DMA[4];
int dmatrace = 0;

static int DMA_IntCH = 0;
static int DMA_LastInt = 0;
static int (*IsReady[4])(void) = { 0, 0, 0, 0 };

#define DMAINT(ch)     if ( DMA[ch].CCR&0x08 )	{ DMA_IntCH |= (1<<ch); IRQH_Int(3, &DMA_Int); }
#define DMAERR(ch,err) DMA[ch].CER  = err; \
                       DMA[ch].CSR |= 0x10; \
                       DMA[ch].CSR &= 0xf7; \
                       DMA[ch].CCR &= 0x7f; \
                       DMAINT(ch)


static int DMA_DummyIsReady(void)
{
	return 0;
}


void DMA_SetReadyCB(int ch, int (*func)(void))
{
	if ( (ch>=0)&&(ch<=3) ) IsReady[ch] = func;
}

// -----------------------------------------------------------------------
//   割り込みベクタを返す
// -----------------------------------------------------------------------
DWORD FASTCALL DMA_Int(BYTE irq)
{
	DWORD ret = 0xffffffff;
	int bit = 0;
	int i = DMA_LastInt;
	IRQH_IRQCallBack(irq);
	if ( irq==3 ) {
		do {
			bit = 1<<i;
			if ( DMA_IntCH&bit ) {
				if ( (DMA[i].CSR)&0x10 )
					ret = ((DWORD)(DMA[i].EIV));
				else
					ret = ((DWORD)(DMA[i].NIV));
				DMA_IntCH &= ~bit;
				break;
			}
			i = (i+1)&3;
		} while ( i!=DMA_LastInt );
	}
	DMA_LastInt = i;
	if ( DMA_IntCH ) IRQH_Int(3, &DMA_Int);
	return ret;
}


// -----------------------------------------------------------------------
//   I/O Read
// -----------------------------------------------------------------------
BYTE FASTCALL DMA_Read(DWORD adr)
{
	unsigned char* p;
	int off = adr&0x3f, ch = ((adr-0xe84000)>>6);

	if ( adr>=0xe84100 ) return 0;		// ばすえらー？

	p = (unsigned char*)&DMA[ch];

	switch ( off ) {
	case 0x00:
		if ( (ch==2)&&(off==0) ) {
#ifndef	NO_MERCURY
			DMA[ch].CSR = (DMA[ch].CSR&0xfe)|(Mcry_LRTiming&1);
#else
			DMA[ch].CSR = (DMA[ch].CSR&0xfe);
			Mcry_LRTiming ^= 1;
#endif
		}
		break;
	case 0x0a: case 0x0b: case 0x1a: case 0x1b:
		p += (off^1);
		break;
	case 0x0c: case 0x0d: case 0x0e: case 0x0f:
	case 0x14: case 0x15: case 0x16: case 0x17:
	case 0x1c: case 0x1d: case 0x1e: case 0x1f:
		p += ((off&0xfc)+3-(off&3));
		break;
	default:
		p += off;
	}
	return *p;
}


// -----------------------------------------------------------------------
//   I/O Write
// -----------------------------------------------------------------------
void FASTCALL DMA_Write(DWORD adr, BYTE data)
{
	unsigned char* p;
	int off = adr&0x3f, ch = ((adr-0xe84000)>>6);
	BYTE old;

	if ( adr>=0xe84100 ) return;		// ばすえらー？

	p = (unsigned char*)&DMA[ch];

/*if (ch==3) {
FILE* fp = fopen("_dma.txt", "a");
fprintf(fp, "W $%02X $%02X\n", off, data);
fclose(fp);
}*/

	switch ( off ) {
	case 0x0a: case 0x0b: case 0x1a: case 0x1b:
		p[off^1] = data;
		break;
	case 0x0c: case 0x0d: case 0x0e: case 0x0f:
	case 0x14: case 0x15: case 0x16: case 0x17:
	case 0x1c: case 0x1d: case 0x1e: case 0x1f:
		p[(off&0xfc)+3-(off&3)] = data;
		break;
	case 0x00:
		p[off] &= ((~data)|0x09);
		break;
	case 0x01:
		p[off] &= (~data);
		break;
	case 0x07:
		old = DMA[ch].CCR;
		DMA[ch].CCR = (data&0xef) | (DMA[ch].CCR&0x80);	// CCRのSTRは書き込みでは落とせない
		if ( (data&0x10)&&(DMA[ch].CCR&0x80) ) {		// Software Abort
			DMAERR(ch,0x11)
			break;
		}
		if ( data&0x20 ) {					// Halt
//			DMA[ch].CSR &= 0xf7;			// 本来は落ちるはず。Nemesis'90で調子悪いので…
			break;
		}
		if ( data&0x80 ) {							// 動作開始
			if ( old&0x20 ) {				// Halt解除
				DMA[ch].CSR |= 0x08;
				DMA_Exec(ch);
			} else {
				if ( DMA[ch].CSR&0xf8 ) {					// タイミングエラー
					DMAERR(ch,0x02)
					break;
				}
				DMA[ch].CSR |= 0x08;
				if ( (DMA[ch].OCR&8)/*&&(!DMA[ch].MTC)*/ ) {	// アレイ／リンクアレイチェイン
					DMA[ch].MAR = dma_readmem24_dword(DMA[ch].BAR)&0xffffff;
					DMA[ch].MTC = dma_readmem24_word(DMA[ch].BAR+4);
					if (DMA[ch].OCR&4) {
						DMA[ch].BAR = dma_readmem24_dword(DMA[ch].BAR+6);
					} else {
						DMA[ch].BAR += 6;
						if ( !DMA[ch].BTC ) {			// これもカウントエラー
						DMAERR(ch,0x0f)
							break;
						}
					}
				}
				if ( !DMA[ch].MTC ) {					// カウントエラー
					DMAERR(ch,0x0d)
					break;
				}
				DMA[ch].CER  = 0x00;
				DMA_Exec(ch);								// 開始直後にカウンタを見て動作チェックする場合があるので、少しだけ実行しておく
			}
		}
		if ( (data&0x40)&&(!DMA[ch].MTC) ) {			// Continuous Op.
			if ( DMA[ch].CCR&0x80 ) {
				if ( DMA[ch].CCR&0x40 ) {
					DMAERR(ch,0x02)
				} else if ( DMA[ch].OCR&8 ) {				// アレイ／リンクアレイチェイン
					DMAERR(ch,0x01)
				} else {
					DMA[ch].MAR = DMA[ch].BAR;
					DMA[ch].MTC = DMA[ch].BTC;
					DMA[ch].CSR |= 0x08;
					DMA[ch].BAR = 0;
					DMA[ch].BTC = 0;
					if ( !DMA[ch].MAR ) {
						DMA[ch].CSR |= 0x40;			// ブロック転送終了ビット／割り込み
						DMAINT(ch)
						break;
					} else if ( !DMA[ch].MTC ) {
						DMAERR(ch,0x0d)
						break;
					}
					DMA[ch].CCR &= 0xbf;
					DMA_Exec(ch);
				}
			} else {									// 非Active時のCNTビットは動作タイミングエラー
				DMAERR(ch,0x02)
			}
		}
		break;
	case 0x3f:
		if ( ch!=3 ) break;
	default:
		p[off] = data;
		break;
	}
}


// -----------------------------------------------------------------------
//   DMA実行
// -----------------------------------------------------------------------
int FASTCALL DMA_Exec(int ch)
{
	DWORD *src, *dst;

//	if ( DMA_IntCH&(1<<ch) ) return 1;

	if ( DMA[ch].OCR&0x80 ) {		// Device->Memory
		src = &DMA[ch].DAR;
		dst = &DMA[ch].MAR;
	} else {				// Memory->Device
		src = &DMA[ch].MAR;
		dst = &DMA[ch].DAR;
	}

	while ( (DMA[ch].CSR&0x08) && (!(DMA[ch].CCR&0x20)) && (!(DMA[ch].CSR&0x80)) && (DMA[ch].MTC) && (((DMA[ch].OCR&3)!=2)||(IsReady[ch]())) ) {
		BusErrFlag = 0;
		switch ( ((DMA[ch].OCR>>4)&3)+((DMA[ch].DCR>>1)&4) ) {
			case 0:
			case 3:
				dma_writemem24(*dst, dma_readmem24(*src));
				if ( DMA[ch].SCR&4 ) DMA[ch].MAR += 1; else if ( DMA[ch].SCR&8 ) DMA[ch].MAR -= 1;
				if ( DMA[ch].SCR&1 ) DMA[ch].DAR += 2; else if ( DMA[ch].SCR&2 ) DMA[ch].DAR -= 2;
				break;
			case 1:
				dma_writemem24(*dst, dma_readmem24(*src));
				if ( DMA[ch].SCR&4 ) DMA[ch].MAR += 1; else if ( DMA[ch].SCR&8 ) DMA[ch].MAR -= 1;
				if ( DMA[ch].SCR&1 ) DMA[ch].DAR += 2; else if ( DMA[ch].SCR&2 ) DMA[ch].DAR -= 2;
				dma_writemem24(*dst, dma_readmem24(*src));
				if ( DMA[ch].SCR&4 ) DMA[ch].MAR += 1; else if ( DMA[ch].SCR&8 ) DMA[ch].MAR -= 1;
				if ( DMA[ch].SCR&1 ) DMA[ch].DAR += 2; else if ( DMA[ch].SCR&2 ) DMA[ch].DAR -= 2;
				break;
			case 2:
				dma_writemem24(*dst, dma_readmem24(*src));
				if ( DMA[ch].SCR&4 ) DMA[ch].MAR += 1; else if ( DMA[ch].SCR&8 ) DMA[ch].MAR -= 1;
				if ( DMA[ch].SCR&1 ) DMA[ch].DAR += 2; else if ( DMA[ch].SCR&2 ) DMA[ch].DAR -= 2;
				dma_writemem24(*dst, dma_readmem24(*src));
				if ( DMA[ch].SCR&4 ) DMA[ch].MAR += 1; else if ( DMA[ch].SCR&8 ) DMA[ch].MAR -= 1;
				if ( DMA[ch].SCR&1 ) DMA[ch].DAR += 2; else if ( DMA[ch].SCR&2 ) DMA[ch].DAR -= 2;
				dma_writemem24(*dst, dma_readmem24(*src));
				if ( DMA[ch].SCR&4 ) DMA[ch].MAR += 1; else if ( DMA[ch].SCR&8 ) DMA[ch].MAR -= 1;
				if ( DMA[ch].SCR&1 ) DMA[ch].DAR += 2; else if ( DMA[ch].SCR&2 ) DMA[ch].DAR -= 2;
				dma_writemem24(*dst, dma_readmem24(*src));
				if ( DMA[ch].SCR&4 ) DMA[ch].MAR += 1; else if ( DMA[ch].SCR&8 ) DMA[ch].MAR -= 1;
				if ( DMA[ch].SCR&1 ) DMA[ch].DAR += 2; else if ( DMA[ch].SCR&2 ) DMA[ch].DAR -= 2;
				break;
			case 4:
				dma_writemem24(*dst, dma_readmem24(*src));
				if ( DMA[ch].SCR&4 ) DMA[ch].MAR += 1; else if ( DMA[ch].SCR&8 ) DMA[ch].MAR -= 1;
				if ( DMA[ch].SCR&1 ) DMA[ch].DAR += 1; else if ( DMA[ch].SCR&2 ) DMA[ch].DAR -= 1;
				break;
			case 5:
				dma_writemem24_word(*dst, dma_readmem24_word(*src));
				if ( DMA[ch].SCR&4 ) DMA[ch].MAR += 2; else if ( DMA[ch].SCR&8 ) DMA[ch].MAR -= 2;
				if ( DMA[ch].SCR&1 ) DMA[ch].DAR += 2; else if ( DMA[ch].SCR&2 ) DMA[ch].DAR -= 2;
				break;
			case 6:
				dma_writemem24_dword(*dst, dma_readmem24_dword(*src));
				if ( DMA[ch].SCR&4 ) DMA[ch].MAR += 4; else if ( DMA[ch].SCR&8 ) DMA[ch].MAR -= 4;
				if ( DMA[ch].SCR&1 ) DMA[ch].DAR += 4; else if ( DMA[ch].SCR&2 ) DMA[ch].DAR -= 4;
				break;
		}

		if ( BusErrFlag ) {
			switch ( BusErrFlag ) {
			case 1:					// BusErr/Read
				if ( DMA[ch].OCR&0x80 )		// Device->Memory
					DMAERR(ch,0x0a)
				else
					DMAERR(ch,0x09)
				break;
			case 2:					// BusErr/Write
				if ( DMA[ch].OCR&0x80 )		// Device->Memory
					DMAERR(ch,0x09)
				else
					DMAERR(ch,0x0a)
				break;
			case 3:					// AdrErr/Read
				if ( DMA[ch].OCR&0x80 )		// Device->Memory
					DMAERR(ch,0x06)
				else
					DMAERR(ch,0x05)
				break;
			case 4:					// BusErr/Write
				if ( DMA[ch].OCR&0x80 )		// Device->Memory
					DMAERR(ch,0x05)
				else
					DMAERR(ch,0x06)
				break;
			}
			BusErrFlag = 0;
			break;
		}

		DMA[ch].MTC--;
		if ( !DMA[ch].MTC ) {					// 指定分のバイト数転送終了
			if ( DMA[ch].OCR&8 ) {				// チェインモードで動いている場合
				if ( DMA[ch].OCR&4 ) {			// リンクアレイチェイン
					if ( DMA[ch].BAR ) {
						DMA[ch].MAR = dma_readmem24_dword(DMA[ch].BAR);
						DMA[ch].MTC = dma_readmem24_word(DMA[ch].BAR+4);
						DMA[ch].BAR = dma_readmem24_dword(DMA[ch].BAR+6);
						if ( BusErrFlag ) {
							if ( BusErrFlag==1 )
								DMAERR(ch,0x0b)
							else
								DMAERR(ch,0x07)
							BusErrFlag = 0;
							break;
						} else if ( !DMA[ch].MTC ) {
							DMAERR(ch,0x0d)
							break;
						}
					}
				} else {						// アレイチェイン
					DMA[ch].BTC--;
					if ( DMA[ch].BTC ) {		// 次のブロックがある
						DMA[ch].MAR = dma_readmem24_dword(DMA[ch].BAR);
						DMA[ch].MTC = dma_readmem24_word(DMA[ch].BAR+4);
						DMA[ch].BAR += 6;
						if ( BusErrFlag ) {
							if ( BusErrFlag==1 )
								DMAERR(ch,0x0b)
							else
								DMAERR(ch,0x07)
							BusErrFlag = 0;
							break;
						} else if ( !DMA[ch].MTC ) {
							DMAERR(ch,0x0d)
							break;
						}
					}
				}
			} else {								// 通常モード（1ブロックのみ）終了
				if ( DMA[ch].CCR&0x40 ) {			// Countinuous動作中
					DMA[ch].CSR |= 0x40;			// ブロック転送終了ビット／割り込み
					DMAINT(ch)
					if ( DMA[ch].BAR ) {
						DMA[ch].MAR  = DMA[ch].BAR;
						DMA[ch].MTC  = DMA[ch].BTC;
						DMA[ch].CSR |= 0x08;
						DMA[ch].BAR  = 0x00;
						DMA[ch].BTC  = 0x00;
						if ( !DMA[ch].MTC ) {
							DMAERR(ch,0x0d)
							break;
						}
						DMA[ch].CCR &= 0xbf;
					}
				}
			}
			if ( !DMA[ch].MTC ) {
				DMA[ch].CSR |= 0x80;
				DMA[ch].CSR &= 0xf7;
				DMAINT(ch)
			}
		}
		if ( (DMA[ch].OCR&3)!=1 ) break;
	}
	return 0;
}


// -----------------------------------------------------------------------
//   初期化
// -----------------------------------------------------------------------
void DMA_Init(void)
{
	int i;
	DMA_IntCH = 0;
	DMA_LastInt = 0;
	for (i=0; i<4; i++) {
		memset(&DMA[i], 0, sizeof(dmac_ch));
		DMA[i].CSR = 0;
		DMA[i].CCR = 0;
		DMA_SetReadyCB(i, DMA_DummyIsReady);
	}
	DMA_SetReadyCB(0, FDC_IsDataReady);
	DMA_SetReadyCB(1, SASI_IsReady);
#ifndef	NO_MERCURY
	DMA_SetReadyCB(2, Mcry_IsReady);
#endif
	DMA_SetReadyCB(3, ADPCM_IsReady);
}
