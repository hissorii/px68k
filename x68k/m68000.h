#ifndef _winx68k_m68000
#define _winx68k_m68000

#include "common.h"

extern	void	Exception(int nr, DWORD oldpc);
extern	int	m68000_ICount;
extern	int	m68000_ICountBk;
extern	int	ICount;
extern	void	M68KRUN(void);
extern	void	M68KRESET(void);

typedef struct
{
	DWORD d[8];
	DWORD a[8];

	DWORD isp;

	DWORD sr_high;
	DWORD ccr;
	DWORD x_carry;

	DWORD pc;
	DWORD IRQ_level;

	DWORD sr;

	void *irq_callback;

	DWORD ppc;
	DWORD (*reset_callback)(void);

	DWORD sfc;
	DWORD dfc;
	DWORD usp;
	DWORD vbr;

	DWORD bank;

	DWORD memmin;
	DWORD memmax;

	DWORD cputype;
} m68k_regs;

extern m68k_regs regs;

#endif //_winx68k_m68000__
