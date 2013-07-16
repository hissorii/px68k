// ---------------------------------------------------------------------------------------
//  IRQH.C - IRQ Handler (架空のデバイスにょ)
// ---------------------------------------------------------------------------------------

#include "common.h"
#include "m68000.h"
#include "irqh.h"

	BYTE	IRQH_IRQ[8];
	void	*IRQH_CallBack[8];

// -----------------------------------------------------------------------
//   初期化
// -----------------------------------------------------------------------
void IRQH_Init(void)
{
	ZeroMemory(IRQH_IRQ, 8);
}


// -----------------------------------------------------------------------
//   デフォルトのベクタを返す（これが起こったら変だお）
// -----------------------------------------------------------------------
DWORD FASTCALL IRQH_DefaultVector(BYTE irq)
{
	IRQH_IRQCallBack(irq);
	return -1;
}


// -----------------------------------------------------------------------
//   他の割り込みのチェック
//   各デバイスのベクタを返すルーチンから呼ばれます
// -----------------------------------------------------------------------
void IRQH_IRQCallBack(BYTE irq)
{
	int i;
	IRQH_IRQ[irq] = 0;
	regs.IRQ_level = 0;
	for (i=7; i>0; i--)
	{
		if (IRQH_IRQ[i])
		{
			regs.irq_callback = IRQH_CallBack[i];
			regs.IRQ_level = i;
			if ( m68000_ICount ) {					// 多重割り込み時（CARAT）
				m68000_ICountBk += m68000_ICount;		// 強制的に割り込みチェックをさせる
				m68000_ICount = 0;				// 苦肉の策 ^^;
			}
			break;
		}
	}
}


// -----------------------------------------------------------------------
//   割り込み発生
// -----------------------------------------------------------------------
void IRQH_Int(BYTE irq, void* handler)
{
	int i;
	IRQH_IRQ[irq] = 1;
	if (handler==NULL)
		IRQH_CallBack[irq] = &IRQH_DefaultVector;
	else
		IRQH_CallBack[irq] = handler;
	for (i=7; i>0; i--)
	{
		if (IRQH_IRQ[i])
		{
			regs.irq_callback = IRQH_CallBack[i];
			regs.IRQ_level = i;
			if ( m68000_ICount ) {					// 多重割り込み時（CARAT）
				m68000_ICountBk += m68000_ICount;		// 強制的に割り込みチェックをさせる
				m68000_ICount = 0;				// 苦肉の策 ^^;
			}
			return;
		}
	}
}
