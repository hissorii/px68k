/******************************************************************************
 *
 * C68K (68000 CPU emulator) version 0.80
 * Compiled with Dev-C++
 * Copyright 2003-2004 Stephane Dallongeville
 *
 * (Modified by NJ)
 *
 *****************************************************************************/

#ifndef C68K_H
#define C68K_H

#ifdef __cplusplus
extern "C" {
#endif


// setting
///////////

//#define C68K_BIG_ENDIAN

#define C68K_FETCH_BITS 8		// [4-12]   default = 8


// Compiler dependant defines
///////////////////////////////

#ifndef UINT8
#define UINT8	unsigned char
#endif

#ifndef INT8
#define INT8	char
#endif

#ifndef UINT16
#define UINT16	unsigned short
#endif

#ifndef INT16
#define INT16	short
#endif

#ifndef UINT32
#define UINT32	unsigned int
#endif

#ifndef INT32
#define INT32	int
#endif


// 68K core types definitions
//////////////////////////////

#define C68K_ADR_BITS	24

#define C68K_FETCH_SFT	(C68K_ADR_BITS - C68K_FETCH_BITS)
#define C68K_FETCH_BANK	(1 << C68K_FETCH_BITS)
#define C68K_FETCH_MASK	(C68K_FETCH_BANK - 1)

#define C68K_SR_C_SFT	8
#define C68K_SR_V_SFT	7
#define C68K_SR_Z_SFT	0
#define C68K_SR_N_SFT	7
#define C68K_SR_X_SFT	8

#define C68K_SR_S_SFT	13

#define C68K_SR_C		(1 << C68K_SR_C_SFT)
#define C68K_SR_V		(1 << C68K_SR_V_SFT)
#define C68K_SR_Z		0
#define C68K_SR_N		(1 << C68K_SR_N_SFT)
#define C68K_SR_X		(1 << C68K_SR_X_SFT)

#define C68K_SR_S		(1 << C68K_SR_S_SFT)

#define C68K_CCR_MASK	0x1F
#define C68K_SR_MASK	(0x2700 | C68K_CCR_MASK)

// exception defines taken from musashi core
#define C68K_RESET_EX					1
#define C68K_BUS_ERROR_EX				2
#define C68K_ADDRESS_ERROR_EX			3
#define C68K_ILLEGAL_INSTRUCTION_EX		4
#define C68K_ZERO_DIVIDE_EX				5
#define C68K_CHK_EX						6
#define C68K_TRAPV_EX					7
#define C68K_PRIVILEGE_VIOLATION_EX		8
#define C68K_TRACE_EX					9
#define C68K_1010_EX					10
#define C68K_1111_EX					11
#define C68K_FORMAT_ERROR_EX			14
#define C68K_UNINITIALIZED_INTERRUPT_EX 15
#define C68K_SPURIOUS_INTERRUPT_EX		24
#define C68K_INTERRUPT_AUTOVECTOR_EX	24
#define C68K_TRAP_BASE_EX				32

#define C68K_INT_ACK_AUTOVECTOR			-1

#ifndef IRQ_LINE_STATE
#define IRQ_LINE_STATE
#define CLEAR_LINE		0		/* clear (a fired, held or pulsed) line */
#define ASSERT_LINE		1		/* assert an interrupt immediately */
#define HOLD_LINE		2		/* hold interrupt line until acknowledged */
#define PULSE_LINE		3		/* pulse interrupt line for one instruction */
#define IRQ_LINE_NMI	127		/* IRQ line for NMIs */
#endif

enum
{
	C68K_PC = 1,
	C68K_USP,
	C68K_MSP,
	C68K_SR,
	C68K_D0,
	C68K_D1,
	C68K_D2,
	C68K_D3,
	C68K_D4,
	C68K_D5,
	C68K_D6,
	C68K_D7,
	C68K_A0,
	C68K_A1,
	C68K_A2,
	C68K_A3,
	C68K_A4,
	C68K_A5,
	C68K_A6,
	C68K_A7
};

typedef struct c68k_t
{
	UINT32 D[8];
	UINT32 A[8];

	UINT32 flag_C;
	UINT32 flag_V;
	UINT32 flag_Z;
	UINT32 flag_N;

	UINT32 flag_X;
	UINT32 flag_I;
	UINT32 flag_S;

	UINT32 USP;
	UINT32 PC;

	UINT32 HaltState;
	INT32 IRQLine;
	INT32 IRQState;
	INT32 ICount;

	UINT32 BasePC;
	UINT32 Fetch[C68K_FETCH_BANK];

	UINT8  (*Read_Byte)(UINT32 address);
	UINT16 (*Read_Word)(UINT32 address);
	UINT8  (*Read_Byte_PC_Relative)(UINT32 address);
	UINT16 (*Read_Word_PC_Relative)(UINT32 address);
	void (*Write_Byte)(UINT32 address, UINT8 data);
	void (*Write_Word)(UINT32 address, UINT16 data);

	INT32  (*Interrupt_CallBack)(INT32 irqline);
	void (*Reset_CallBack)(void);

} c68k_struc;


// 68K core var declaration
////////////////////////////

extern c68k_struc C68K;


// 68K core function declaration
/////////////////////////////////

void C68k_Init(c68k_struc *cpu);

void C68k_Reset(c68k_struc *cpu);

INT32  C68k_Exec(c68k_struc *cpu, INT32 cycle);

void C68k_Set_IRQ(c68k_struc *cpu, INT32 line, INT32 state);

UINT32  C68k_Get_Reg(c68k_struc *cpu, INT32 regnum);
void C68k_Set_Reg(c68k_struc *cpu, INT32 regnum, UINT32 val);

void C68k_Set_Fetch(c68k_struc *cpu, UINT32 low_adr, UINT32 high_adr, UINT32 fetch_adr);

void C68k_Set_ReadB(c68k_struc *cpu, UINT8 (*Func)(UINT32 address));
void C68k_Set_ReadW(c68k_struc *cpu, UINT16 (*Func)(UINT32 address));

void C68k_Set_ReadB_PC_Relative(c68k_struc *cpu, UINT8 (*Func)(UINT32 address));
void C68k_Set_ReadW_PC_Relative(c68k_struc *cpu, UINT16 (*Func)(UINT32 address));

void C68k_Set_WriteB(c68k_struc *cpu, void (*Func)(UINT32 address, UINT8 data));
void C68k_Set_WriteW(c68k_struc *cpu, void (*Func)(UINT32 address, UINT16 data));

void C68k_Set_IRQ_Callback(c68k_struc *cpu, INT32 (*Func)(INT32 irqline));
void C68k_Set_Reset_Callback(c68k_struc *cpu, void (*Func)(void));

#ifdef __cplusplus
}
#endif

#endif	/* C68K_H */
