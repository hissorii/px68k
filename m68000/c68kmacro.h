/******************************************************************************

	c68kmacro.h

	C68K 各種マクロ

******************************************************************************/

/*------------------------------- core macros -------------------------------*/

#define AY						CPU->A[Opcode & 7]
#define AX						CPU->A[(Opcode >> 9) & 7]
#define A7						CPU->A[7]

#define DY						CPU->D[Opcode & 7]
#define DX						CPU->D[(Opcode >> 9) & 7]
#define D0						CPU->D[0]

#define FLAG_X					CPU->flag_X
#define FLAG_C					CPU->flag_C
#define FLAG_V					CPU->flag_V
#define FLAG_N					CPU->flag_N
#define FLAG_Z					CPU->flag_Z

#define FLAG_S					CPU->flag_S
#define FLAG_I					CPU->flag_I

#define OP(name)				op_##name:

#define MAKE_INT_8(A)			(INT32)(INT8)(A)
#define MAKE_INT_16(A)			(INT32)(INT16)(A)
#define MAKE_INT_32(A)			(INT32)(A)

#define MAKE_UINT_8(A)			(UINT8)(A)
#define MAKE_UINT_16(A)			(UINT16)(A)
#define MAKE_UINT_32(A)			(UINT32)(A)

#define LOW_NIBBLE(A)			((A) & 0x0f)
#define HIGH_NIBBLE(A)			((A) & 0xf0)

#define USE_CYCLES(A)			CPU->ICount -= (A);
#define RELEASE_CYCLES()		CPU->ICount = 0;

#define READ_REG_8(A)			MAKE_UINT_8(A)
#define READ_REG_16(A)			MAKE_UINT_16(A)
#define READ_REG_32(A)			MAKE_UINT_32(A)

#define READSX_REG_8(A)			MAKE_INT_8(A)
#define READSX_REG_16(A)		MAKE_INT_16(A)
#define READSX_REG_32(A)		MAKE_INT_32(A)

#define WRITE_REG_8(A, D)		*(UINT8 *)(&A) = D
#define WRITE_REG_16(A, D)		*(UINT16 *)(&A) = D
#define WRITE_REG_32(A, D)		A = D

#define READ_IMM_8()			(*(UINT8 *)PC)
#define READ_IMM_16()			(*(UINT16 *)PC)
#ifdef C68K_BIG_ENDIAN
#define READ_IMM_32()			(*(UINT32 *)PC)
#else
#define READ_IMM_32()			(((*(UINT16 *)PC) << 16) | (*(UINT16 *)(PC + 2)))
#endif

#define READSX_IMM_8()			(INT32)(*(INT8 *)PC)
#define READSX_IMM_16()			(INT32)(*(INT16 *)PC)
#define READSX_IMM_32()			MAKE_INT_32(READ_IMM_32())

#define READ_MEM_8(A)			CPU->Read_Byte(A)
#define READ_MEM_16(A)			CPU->Read_Word(A)
#ifdef C68K_BIG_ENDIAN
#define READ_MEM_32(A)			(READ_MEM_16(A) | (READ_MEM_16((A) + 2) << 16))
#else
#define READ_MEM_32(A)			((READ_MEM_16(A) << 16) | READ_MEM_16((A) + 2))
#endif

#define READSX_MEM_8(A)			MAKE_INT_8(READ_MEM_8(A))
#define READSX_MEM_16(A)		MAKE_INT_16(READ_MEM_16(A))
#define READSX_MEM_32(A)		MAKE_INT_32(READ_MEM_32(A))

#define READ_PCREL_8(A)			CPU->Read_Byte_PC_Relative(A)
#define READ_PCREL_16(A)		CPU->Read_Word_PC_Relative(A)
#ifdef C68K_BIG_ENDIAN
#define READ_PCREL_32(A)		(READ_PCREL_16(A) | (READ_PCREL_16((A) + 2) << 16))
#else
#define READ_PCREL_32(A)		((READ_PCREL_16(A) << 16) | READ_PCREL_16((A) + 2))
#endif

#define READSX_PCREL_8(A)		MAKE_INT_8(READ_PCREL_8(A))
#define READSX_PCREL_16(A)		MAKE_INT_16(READ_PCREL_16(A))
#define READSX_PCREL_32(A)		MAKE_INT_32(READ_PCREL_32(A))

#define WRITE_MEM_8(A, D)		CPU->Write_Byte(A, D)
#define WRITE_MEM_16(A, D)		CPU->Write_Word(A, D)
#ifdef C68K_BIG_ENDIAN
#define WRITE_MEM_32(A, D)		WRITE_MEM_16((A), (D)); WRITE_MEM_16((A) + 2, (D) >> 16)
#else
#define WRITE_MEM_32(A, D)		WRITE_MEM_16((A), (D) >> 16); WRITE_MEM_16((A) + 2, (D))
#endif

#define WRITE_MEM_16PD(A, D)	WRITE_MEM_16(A, D)
#ifdef C68K_BIG_ENDIAN
#define WRITE_MEM_32PD(A, D)	WRITE_MEM_16((A) + 2, (D) >> 16); WRITE_MEM_16((A), (D))
#else
#define WRITE_MEM_32PD(A, D)	WRITE_MEM_16((A) + 2, (D)); WRITE_MEM_16((A), (D) >> 16)
#endif

#define GET_QUICK()				(((Opcode >> 9) - 1) & 7) + 1

#define PUSH_16_F(D)			A7 -= 2; WRITE_MEM_16(A7, D);
#define POP_16_F(D)				D = READ_MEM_16(A7); A7 += 2;

#define PUSH_32_F(D)			A7 -= 4; WRITE_MEM_32PD(A7, D);
#define POP_32_F(D)				D = READ_MEM_32(A7); A7 += 4;

#define DECODE_EXT_WORD														\
{																			\
	UINT32 ext = READ_IMM_16();												\
	PC += 2;																\
																			\
	adr += MAKE_INT_8(ext);													\
	if (ext & 0x0800) adr += MAKE_INT_32(CPU->D[ext >> 12]);				\
	else adr += MAKE_INT_16(CPU->D[ext >> 12]);								\
}

#define RET(A)																\
	USE_CYCLES(A)															\
	goto C68k_Exec_Next;

#define RET_INT(A)															\
	USE_CYCLES(A)															\
	goto C68k_Check_Interrupt;

#define RET_HALT()															\
	CPU->HaltState = 1;														\
	RELEASE_CYCLES();														\
	goto C68k_Check_Interrupt;

#define GET_PC()				(PC - CPU->BasePC)

#define SET_PC(A)															\
	CPU->BasePC = CPU->Fetch[((A) >> C68K_FETCH_SFT) & C68K_FETCH_MASK];	\
	CPU->BasePC -= (A) & 0xff000000;										\
	PC = (A) + CPU->BasePC;

#define ADJUST_PC()				PC -= CPU->BasePC;

#define GET_CCR()															\
	(((CPU->flag_C >> (C68K_SR_C_SFT - 0)) & 1) | 							\
	 ((CPU->flag_V >> (C68K_SR_V_SFT - 1)) & 2) | 							\
	 (((!CPU->flag_Z) & 1) << 2) | 											\
	 ((CPU->flag_N >> (C68K_SR_N_SFT - 3)) & 8) | 							\
	 ((CPU->flag_X >> (C68K_SR_X_SFT - 4)) & 0x10))

#define GET_SR()															\
	((CPU->flag_S << 0)  |													\
	 (CPU->flag_I << 8)  |													\
	 GET_CCR())

#define SET_CCR(A)															\
	CPU->flag_C = (A) << (C68K_SR_C_SFT - 0);								\
	CPU->flag_V = (A) << (C68K_SR_V_SFT - 1);								\
	CPU->flag_Z = ~(A) & 4;													\
	CPU->flag_N = (A) << (C68K_SR_N_SFT - 3);								\
	CPU->flag_X = (A) << (C68K_SR_X_SFT - 4);

#define SET_SR(A)															\
	SET_CCR(A)																\
	CPU->flag_I = ((A) >> 8) & 7;											\
	CPU->flag_S = (A) & C68K_SR_S;

#define SWAP_SP()															\
	if (!CPU->flag_S)														\
	{																		\
		res = CPU->USP;														\
		CPU->USP = A7;														\
		A7 = res;															\
	}

#define SWAP_SP_NOCHECK()													\
	res = CPU->USP;															\
	CPU->USP = A7;															\
	A7 = res;

#define EXCEPTION(number)													\
	PUSH_32_F(GET_PC())														\
	PUSH_16_F(GET_SR())														\
	CPU->flag_S = C68K_SR_S;												\
	PC = READ_MEM_32((number) << 2);										\
	SET_PC(PC)

#define CHECK_INT															\
	adr = CPU->IRQLine;														\
	if ((adr == 7) || (adr > CPU->flag_I))									\
	{																		\
		if (CPU->IRQState == HOLD_LINE)										\
			CPU->IRQState = CLEAR_LINE;										\
		CPU->IRQLine = 0;													\
		SWAP_SP()															\
		res = CPU->Interrupt_CallBack(adr);									\
		EXCEPTION(res)														\
		CPU->flag_I = adr;													\
		USE_CYCLES(44)														\
	}

/******************************************************************************
	c68k_op用マクロ
******************************************************************************/

/*------------------------------- opcode macros -----------------------------*/

#define CFLAG_8(A)				(A)
#define CFLAG_16(A)				((A) >> 8)

#define CFLAG_ADD_32(S, D, R)	(((S & D & 1) + (S >> 1) + (D >> 1)) >> 23)
#define CFLAG_SUB_32(S, D, R)	(((S & R & 1) + (S >> 1) + (R >> 1)) >> 23)

#define VFLAG_ADD_8(S, D, R)	((S ^ R) & (D ^ R))
#define VFLAG_ADD_16(S, D, R)	(((S ^ R) & (D ^ R)) >> 8)
#define VFLAG_ADD_32(S, D, R)	(((S ^ R) & (D ^ R)) >> 24)

#define VFLAG_SUB_8(S, D, R)	((S ^ D) & (R ^ D))
#define VFLAG_SUB_16(S, D, R)	(((S ^ D) & (R ^ D)) >> 8)
#define VFLAG_SUB_32(S, D, R)	(((S ^ D) & (R ^ D)) >> 24)

#define NFLAG_8(A)				(A)
#define NFLAG_16(A)				((A) >> 8)
#define NFLAG_32(A)				((A) >> 24)

#define ZFLAG_8(A)				MAKE_UINT_8(A)
#define ZFLAG_16(A)				MAKE_UINT_16(A)
#define ZFLAG_32(A)				MAKE_UINT_32(A)

#define CFLAG_SET				0x100
#define CFLAG_CLEAR				0
#define XFLAG_SET				0x100
#define XFLAG_CLEAR				0
#define VFLAG_SET				0x80
#define VFLAG_CLEAR				0
#define NFLAG_SET				0x80
#define NFLAG_CLEAR				0
#define ZFLAG_SET				0
#define ZFLAG_CLEAR				1

#define XFLAG_AS_1()			((FLAG_X >> 8) & 1)
#define NFLAG_AS_1()			((FLAG_N >> 7) & 1)
#define VFLAG_AS_1()			((FLAG_V >> 7) & 1)
#define ZFLAG_AS_1()			(!FLAG_Z)
#define CFLAG_AS_1()			((FLAG_C >> 8) & 1)

#define COND_CS()				(FLAG_C & 0x100)
#define COND_CC()				(!COND_CS())
#define COND_VS()				(FLAG_V & 0x80)
#define COND_VC()				(!COND_VS())
#define COND_NE()				FLAG_Z
#define COND_EQ()				(!COND_NE())
#define COND_MI()				(FLAG_N & 0x80)
#define COND_PL()				(!COND_MI())
#define COND_LT()				((FLAG_N ^ FLAG_V) & 0x80)
#define COND_GE()				(!COND_LT())
#define COND_HI()				(COND_CC() && COND_NE())
#define COND_LS()				(COND_CS() || COND_EQ())
#define COND_GT()				(COND_GE() && COND_NE())
#define COND_LE()				(COND_LT() || COND_EQ())

#define COND_NOT_CS()			COND_CC()
#define COND_NOT_CC()			COND_CS()
#define COND_NOT_VS()			COND_VC()
#define COND_NOT_VC()			COND_VS()
#define COND_NOT_NE()			COND_EQ()
#define COND_NOT_EQ()			COND_NE()
#define COND_NOT_MI()			COND_PL()
#define COND_NOT_PL()			COND_MI()
#define COND_NOT_LT()			COND_GE()
#define COND_NOT_GE()			COND_LT()
#define COND_NOT_HI()			COND_LS()
#define COND_NOT_LS()			COND_HI()
#define COND_NOT_GT()			COND_LE()
#define COND_NOT_LE()			COND_GT()

/*--------------------------------- clocks ----------------------------------*/

#define EA_CLOCKS_D_8		0
#define EA_CLOCKS_AI_8		4
#define EA_CLOCKS_PI_8		4
#define EA_CLOCKS_PD_8		6
#define EA_CLOCKS_DI_8		8
#define EA_CLOCKS_IX_8		10
#define EA_CLOCKS_AW_8		8
#define EA_CLOCKS_AL_8		12
#define EA_CLOCKS_PCDI_8	8
#define EA_CLOCKS_PCIX_8	10
#define EA_CLOCKS_I_8		4
#define EA_CLOCKS_PI7_8		4
#define EA_CLOCKS_PD7_8		6

#define EA_CLOCKS_D_16		0
#define EA_CLOCKS_A_16		0
#define EA_CLOCKS_AI_16		4
#define EA_CLOCKS_PI_16		4
#define EA_CLOCKS_PD_16		6
#define EA_CLOCKS_DI_16		8
#define EA_CLOCKS_IX_16		10
#define EA_CLOCKS_AW_16		8
#define EA_CLOCKS_AL_16		12
#define EA_CLOCKS_PCDI_16	8
#define EA_CLOCKS_PCIX_16	10
#define EA_CLOCKS_I_16		4
#define EA_CLOCKS_PI7_16	4
#define EA_CLOCKS_PD7_16	6

#define EA_CLOCKS_D_32		0
#define EA_CLOCKS_A_32		0
#define EA_CLOCKS_AI_32		8
#define EA_CLOCKS_PI_32		8
#define EA_CLOCKS_PD_32		10
#define EA_CLOCKS_DI_32		12
#define EA_CLOCKS_IX_32		14
#define EA_CLOCKS_AW_32		12
#define EA_CLOCKS_AL_32		16
#define EA_CLOCKS_PCDI_32	12
#define EA_CLOCKS_PCIX_32	14
#define EA_CLOCKS_I_32		8
#define EA_CLOCKS_PI7_32	8
#define EA_CLOCKS_PD7_32	10

/*----------------------------- Effective Address ---------------------------*/

#define PI7_8()						A7; A7 += 2
#define PI7_16()					A7; A7 += 2
#define PI7_32()					A7; A7 += 4

#define PD7_8()						A7 - 2; A7 = adr
#define PD7_16()					A7 - 2; A7 = adr
#define PD7_32()					A7 - 4; A7 = adr

#define ADD_PC_8()					PC += 2
#define ADD_PC_16()					PC += 2
#define ADD_PC_32()					PC += 4

#define READ_D(size, xy)			READ_REG_##size(D##xy)
#define READ_A(size, xy)			READ_REG_##size(A##xy)
#define READ_AI(size, xy)			READ_MEM_##size(adr)
#define READ_PI(size, xy)			READ_MEM_##size(adr)
#define READ_PD(size, xy)			READ_MEM_##size(adr)
#define READ_DI(size, xy)			READ_MEM_##size(adr)
#define READ_IX(size, xy)			READ_MEM_##size(adr)
#define READ_AW(size, xy)			READ_MEM_##size(adr)
#define READ_AL(size, xy)			READ_MEM_##size(adr)
#define READ_PCDI(size, xy)			READ_PCREL_##size(adr)
#define READ_PCIX(size, xy)			READ_PCREL_##size(adr)
#define READ_I(size, xy)			READ_IMM_##size(); ADD_PC_##size()
#define READ_PI7(size, xy)			READ_MEM_##size(adr)
#define READ_PD7(size, xy)			READ_MEM_##size(adr)

#define READSX_D(size, xy)			READSX_REG_##size(D##xy)
#define READSX_A(size, xy)			READSX_REG_##size(A##xy)
#define READSX_AI(size, xy)			READSX_MEM_##size(adr)
#define READSX_PI(size, xy)			READSX_MEM_##size(adr)
#define READSX_PD(size, xy)			READSX_MEM_##size(adr)
#define READSX_DI(size, xy)			READSX_MEM_##size(adr)
#define READSX_IX(size, xy)			READSX_MEM_##size(adr)
#define READSX_AW(size, xy)			READSX_MEM_##size(adr)
#define READSX_AL(size, xy)			READSX_MEM_##size(adr)
#define READSX_PCDI(size, xy)		READSX_PCREL_##size(adr)
#define READSX_PCIX(size, xy)		READSX_PCREL_##size(adr)
#define READSX_I(size, xy)			READSX_IMM_##size(); ADD_PC_##size()
#define READSX_PI7(size, xy)		READSX_MEM_##size(adr)
#define READSX_PD7(size, xy)		READSX_MEM_##size(adr)

#define WRITE_D(size, xy, v)		WRITE_REG_##size(D##xy, v)
#define WRITE_A(size, xy, v)		WRITE_REG_32(A##xy, v)
#define WRITE_AI(size, xy, v)		WRITE_MEM_##size(adr, v)
#define WRITE_PI(size, xy, v)		WRITE_MEM_##size(adr, v)
#define WRITE_PD(size, xy, v)		WRITE_MEM_##size(adr, v)
#define WRITE_DI(size, xy, v)		WRITE_MEM_##size(adr, v)
#define WRITE_IX(size, xy, v)		WRITE_MEM_##size(adr, v)
#define WRITE_AW(size, xy, v)		WRITE_MEM_##size(adr, v)
#define WRITE_AL(size, xy, v)		WRITE_MEM_##size(adr, v)
#define WRITE_PCDI(size, xy, v)		WRITE_MEM_##size(adr, v)
#define WRITE_PCIX(size, xy, v)		WRITE_MEM_##size(adr, v)
#define WRITE_PI7(size, xy, v)		WRITE_MEM_##size(adr, v)
#define WRITE_PD7(size, xy, v)		WRITE_MEM_##size(adr, v)

#define WRITE_EA(size, mode, xy)	WRITE_##mode(size, xy, res)

#define EA_D(size, xy)
#define EA_A(size, xy)
#define EA_AI(size, xy)				{ adr = A##xy;                            }
#define EA_PI(size, xy)				{ adr = A##xy; A##xy += (size / 8);       }
#define EA_PD(size, xy)				{ adr = A##xy - (size / 8); A##xy = adr;  }
#define EA_DI(size, xy)				{ adr = A##xy + READSX_I(16, NA);         }
#define EA_IX(size, xy)				{ adr = A##xy; DECODE_EXT_WORD            }
#define EA_AW(size, xy)				{ adr = READSX_I(16, NA);                 }
#define EA_AL(size, xy)				{ adr = READSX_I(32, NA);                 }
#define EA_PCDI(size, xy)			{ adr = GET_PC() + READSX_I(16, NA);      }
#define EA_PCIX(size, xy)			{ adr = GET_PC(); DECODE_EXT_WORD         }
#define EA_I(size, xy)
#define EA_PI7(size, xy)			{ adr = PI7_##size();                     }
#define EA_PD7(size, xy)			{ adr = PD7_##size();                     }

#define EA_READ_D(s, xy, v)			{ v = READ_D(s, xy);                      }
#define EA_READ_A(s, xy, v)			{ v = READ_A(s, xy);                      }
#define EA_READ_AI(s, xy, v)		{ EA_AI(s, xy) v = READ_AI(s, xy);        }
#define EA_READ_PI(s, xy, v)		{ EA_PI(s, xy) v = READ_PI(s, xy);        }
#define EA_READ_PD(s, xy, v)		{ EA_PD(s, xy) v = READ_PD(s, xy);        }
#define EA_READ_DI(s, xy, v)		{ EA_DI(s, xy) v = READ_DI(s, xy);        }
#define EA_READ_IX(s, xy, v)		{ EA_IX(s, xy) v = READ_IX(s, xy);        }
#define EA_READ_AW(s, xy, v)		{ EA_AW(s, xy) v = READ_AW(s, xy);        }
#define EA_READ_AL(s, xy, v)		{ EA_AL(s, xy) v = READ_AL(s, xy);        }
#define EA_READ_PCDI(s, xy, v)		{ EA_PCDI(s, xy) v = READ_PCDI(s, xy);    }
#define EA_READ_PCIX(s, xy, v)		{ EA_PCIX(s, xy) v = READ_PCIX(s, xy);    }
#define EA_READ_I(s, xy, v)			{ v = READ_I(s, xy);                      }
#define EA_READ_PI7(s, xy, v)		{ EA_PI7(s, xy) v = READ_PI7(s, xy);      }
#define EA_READ_PD7(s, xy, v)		{ EA_PD7(s, xy) v = READ_PD7(s, xy);      }

#define EA_READSX_D(s, xy, v)		{ v = READSX_D(s, xy);                    }
#define EA_READSX_A(s, xy, v)		{ v = READSX_A(s, xy);                    }
#define EA_READSX_AI(s, xy, v)		{ EA_AI(s, xy) v = READSX_AI(s, xy);      }
#define EA_READSX_PI(s, xy, v)		{ EA_PI(s, xy) v = READSX_PI(s, xy);      }
#define EA_READSX_PD(s, xy, v)		{ EA_PD(s, xy) v = READSX_PD(s, xy);      }
#define EA_READSX_DI(s, xy, v)		{ EA_DI(s, xy) v = READSX_DI(s, xy);      }
#define EA_READSX_IX(s, xy, v)		{ EA_IX(s, xy) v = READSX_IX(s, xy);      }
#define EA_READSX_AW(s, xy, v)		{ EA_AW(s, xy) v = READSX_AW(s, xy);      }
#define EA_READSX_AL(s, xy, v)		{ EA_AL(s, xy) v = READSX_AL(s, xy);      }
#define EA_READSX_PCDI(s, xy, v)	{ EA_PCDI(s, xy) v = READSX_PCDI(s, xy);  }
#define EA_READSX_PCIX(s, xy, v)	{ EA_PCIX(s, xy) v = READSX_PCIX(s, xy);  }
#define EA_READSX_I(s, xy, v)		{ v = READSX_I(s, xy);                    }
#define EA_READSX_PI7(s, xy, v)		{ EA_PI7(s, xy) v = READSX_PI7(s, xy);    }
#define EA_READSX_PD7(s, xy, v)		{ EA_PD7(s, xy) v = READSX_PD7(s, xy);    }

#define EA_WRITE_D(s, xy, v)		{ WRITE_D(s, xy, v);                      }
#define EA_WRITE_A(s, xy, v)		{ WRITE_A(s, xy, v);                      }
#define EA_WRITE_AI(s, xy, v)		{ EA_AI(s, xy) WRITE_AI(s, xy, v);        }
#define EA_WRITE_PI(s, xy, v)		{ EA_PI(s, xy) WRITE_PI(s, xy, v);        }
#define EA_WRITE_PD(s, xy, v)		{ EA_PD(s, xy) WRITE_PD(s, xy, v);        }
#define EA_WRITE_DI(s, xy, v)		{ EA_DI(s, xy) WRITE_DI(s, xy, v);        }
#define EA_WRITE_IX(s, xy, v)		{ EA_IX(s, xy) WRITE_IX(s, xy, v);        }
#define EA_WRITE_AW(s, xy, v)		{ EA_AW(s, xy) WRITE_AW(s, xy, v);        }
#define EA_WRITE_AL(s, xy, v)		{ EA_AL(s, xy) WRITE_AL(s, xy, v);        }
#define EA_WRITE_PCDI(s, xy, v)		{ EA_PCDI(s, xy) WRITE_PCDI(s, xy, v);    }
#define EA_WRITE_PCIX(s, xy, v)		{ EA_PCIX(s, xy) WRITE_PCIX(s, xy, v);    }
#define EA_WRITE_PI7(s, xy, v)		{ EA_PI7(s, xy) WRITE_PI7(s, xy, v);      }
#define EA_WRITE_PD7(s, xy, v)		{ EA_PD7(s, xy) WRITE_PD7(s, xy, v);      }

#define EA_WRITE_RESULT(s, mode, xy)	WRITE_EA(s, mode, xy);

/*----------------------------- operation flags -----------------------------*/

#define FLAGS(size)															\
	FLAG_C = CFLAG_CLEAR;													\
	FLAG_V = VFLAG_CLEAR;													\
	FLAG_Z = res;															\
	FLAG_N = NFLAG_##size(res);

#define FLAGS_ADD_8()														\
	FLAG_N = FLAG_X = FLAG_C = CFLAG_8(res);								\
	FLAG_V = VFLAG_ADD_8(src, dst, res);									\
	FLAG_Z = ZFLAG_8(res);

#define FLAGS_ADD_16()														\
	FLAG_V = VFLAG_ADD_16(src, dst, res);									\
	FLAG_N = FLAG_X = FLAG_C = CFLAG_16(res);								\
	FLAG_Z = ZFLAG_16(res);

#define FLAGS_ADD_32()														\
	FLAG_Z = ZFLAG_32(res);													\
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);							\
	FLAG_V = VFLAG_ADD_32(src, dst, res);									\
	FLAG_N = NFLAG_32(res);

#define FLAGS_ADDX_8()														\
	FLAG_N = FLAG_X = FLAG_C = CFLAG_8(res);								\
	FLAG_V = VFLAG_ADD_8(src, dst, res);									\
	FLAG_Z |= ZFLAG_8(res);

#define FLAGS_ADDX_16()														\
	FLAG_V = VFLAG_ADD_16(src, dst, res);									\
	FLAG_N = FLAG_X = FLAG_C = CFLAG_16(res);								\
	FLAG_Z |= ZFLAG_16(res);

#define FLAGS_ADDX_32()														\
	FLAG_Z |= ZFLAG_32(res);												\
	FLAG_X = FLAG_C = CFLAG_ADD_32(src, dst, res);							\
	FLAG_V = VFLAG_ADD_32(src, dst, res);									\
	FLAG_N = NFLAG_32(res);

#define FLAGS_SUB_8()														\
	FLAG_N = FLAG_X = FLAG_C = CFLAG_8(res);								\
	FLAG_V = VFLAG_SUB_8(src, dst, res);									\
	FLAG_Z = ZFLAG_8(res);

#define FLAGS_SUB_16()														\
	FLAG_V = VFLAG_SUB_16(src, dst, res);									\
	FLAG_N = FLAG_X = FLAG_C = CFLAG_16(res);								\
	FLAG_Z = ZFLAG_16(res);

#define FLAGS_SUB_32()														\
	FLAG_Z = ZFLAG_32(res);													\
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);							\
	FLAG_V = VFLAG_SUB_32(src, dst, res);									\
	FLAG_N = NFLAG_32(res);

#define FLAGS_SUBX_8()														\
	FLAG_N = FLAG_X = FLAG_C = CFLAG_8(res);								\
	FLAG_V = VFLAG_SUB_8(src, dst, res);									\
	FLAG_Z |= ZFLAG_8(res);

#define FLAGS_SUBX_16()														\
	FLAG_V = VFLAG_SUB_16(src, dst, res);									\
	FLAG_N = FLAG_X = FLAG_C = CFLAG_16(res);								\
	FLAG_Z |= ZFLAG_16(res);

#define FLAGS_SUBX_32()														\
	FLAG_Z |= ZFLAG_32(res);												\
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);							\
	FLAG_V = VFLAG_SUB_32(src, dst, res);									\
	FLAG_N = NFLAG_32(res);

#define FLAGS_CMP_8()														\
	FLAG_N = FLAG_C = CFLAG_8(res);											\
	FLAG_V = VFLAG_SUB_8(src, dst, res);									\
	FLAG_Z = ZFLAG_8(res);

#define FLAGS_CMP_16()														\
	FLAG_V = VFLAG_SUB_16(src, dst, res);									\
	FLAG_N = FLAG_C = CFLAG_16(res);										\
	FLAG_Z = ZFLAG_16(res);

#define FLAGS_CMP_32()														\
	FLAG_Z = ZFLAG_32(res);													\
	FLAG_C = CFLAG_SUB_32(src, dst, res);									\
	FLAG_V = VFLAG_SUB_32(src, dst, res);									\
	FLAG_N = NFLAG_32(res);

#define FLAGS_NEGX_8()														\
	FLAG_V = res & src;														\
	FLAG_N = FLAG_X = FLAG_C = CFLAG_8(res);								\
	FLAG_Z |= ZFLAG_8(res);

#define FLAGS_NEGX_16()														\
	FLAG_V = (res & src) >> 8;												\
	FLAG_N = FLAG_X = FLAG_C = CFLAG_16(res);								\
	FLAG_Z |= ZFLAG_16(res);

#define FLAGS_NEGX_32()														\
	FLAG_Z |= ZFLAG_32(res);												\
	FLAG_V = (res & src) >> 24;												\
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);							\
	FLAG_N = NFLAG_32(res);

#define FLAGS_NEG_8()														\
	FLAG_V = res & src;														\
	FLAG_N = FLAG_X = FLAG_C = CFLAG_8(res);								\
	FLAG_Z = ZFLAG_8(res);

#define FLAGS_NEG_16()														\
	FLAG_V = (res & src) >> 8;												\
	FLAG_N = FLAG_X = FLAG_C = CFLAG_16(res);								\
	FLAG_Z = ZFLAG_16(res);

#define FLAGS_NEG_32()														\
	FLAG_Z = ZFLAG_32(res);													\
	FLAG_V = (res & src) >> 24;												\
	FLAG_X = FLAG_C = CFLAG_SUB_32(src, dst, res);							\
	FLAG_N = NFLAG_32(res);

/*--------------------------------- operation -------------------------------*/

#define OP_OR(size)															\
	res |= src;																\
	FLAGS(size)

#define OP_AND(size)														\
	res &= src;																\
	FLAGS(size)

#define OP_EOR(size)														\
	res ^= src;																\
	FLAGS(size)

#define OP_ADD(size)														\
	res = dst + src;														\
	FLAGS_ADD_##size()

#define OP_SUB(size)														\
	res = dst - src;														\
	FLAGS_SUB_##size()

#define OP_CMP(size)														\
	res = dst - src;														\
	FLAGS_CMP_##size()

/******************************************************************************
	OPCODE $0xxx
******************************************************************************/

/*-----------------------------------------------------------------------------
  ORI    inclusive-OR Immediate
-----------------------------------------------------------------------------*/

#define ORI_CLOCKS_D_8	8
#define ORI_CLOCKS_D_16	8
#define ORI_CLOCKS_D_32	16

#define ORI_CLOCKS_M_8	12
#define ORI_CLOCKS_M_16	12
#define ORI_CLOCKS_M_32	20

#define ORI(size, clk, mode)												\
{																			\
	EA_READ_I(size, NA, src)												\
	EA_READ_##mode(size, Y, res)											\
	OP_OR(size)																\
	EA_WRITE_RESULT(size, mode, Y)											\
	RET(ORI_CLOCKS_##clk##_##size + EA_CLOCKS_##mode##_##size)				\
}

/*-----------------------------------------------------------------------------
  ANDI    And Immediate
-----------------------------------------------------------------------------*/

#define ANDI_CLOCKS_D_8		8
#define ANDI_CLOCKS_D_16	8
#define ANDI_CLOCKS_D_32	14

#define ANDI_CLOCKS_M_8		12
#define ANDI_CLOCKS_M_16	12
#define ANDI_CLOCKS_M_32	20

#define ANDI(size, clk, mode)												\
{																			\
	EA_READ_I(size, NA, src)												\
	EA_READ_##mode(size, Y, res)											\
	OP_AND(size)															\
	EA_WRITE_RESULT(size, mode, Y)											\
	RET(ANDI_CLOCKS_##clk##_##size + EA_CLOCKS_##mode##_##size)				\
}

/*-----------------------------------------------------------------------------
  EORI    Exclusive-OR Immediate
-----------------------------------------------------------------------------*/

#define EORI_CLOCKS_D_8		8
#define EORI_CLOCKS_D_16	8
#define EORI_CLOCKS_D_32	16

#define EORI_CLOCKS_M_8		12
#define EORI_CLOCKS_M_16	12
#define EORI_CLOCKS_M_32	20

#define EORI(size, clk, mode)												\
{																			\
	EA_READ_I(size, NA, src)												\
	EA_READ_##mode(size, Y, res)											\
	OP_EOR(size)															\
	EA_WRITE_RESULT(size, mode, Y)											\
	RET(EORI_CLOCKS_##clk##_##size + EA_CLOCKS_##mode##_##size)				\
}

/*-----------------------------------------------------------------------------
  ORI to CCR     inclusive-OR Immidiate to the Condition Code Register
  ANDI to CCR    And Immisiate to Condition the Code Register
  EORI to CCR    Exclusive-OR Immediate to the Condition Code Register
-----------------------------------------------------------------------------*/

#define LOGOP_TOC(operation)												\
{																			\
	EA_READ_I(8, NA, res)													\
	res &= C68K_CCR_MASK;													\
	res operation GET_CCR();												\
	SET_CCR(res)															\
	RET(20)																	\
}

/*-----------------------------------------------------------------------------
  ORI to SR     inclusive-OR Immidiate to the Stats Register
  ANDI to SR    AND Immisiate to the Status Register
  EORI to SR    Exclusive-OR Immediate to the Status Register
-----------------------------------------------------------------------------*/

#define LOGOP_TOS(operation)												\
{																			\
	if (FLAG_S)																\
	{																		\
		EA_READ_I(16, NA, res)												\
		res &= C68K_SR_MASK;												\
		res operation GET_SR();												\
		SET_SR(res)															\
		SWAP_SP()															\
		RET_INT(20)															\
	}																		\
	SWAP_SP_NOCHECK()														\
	EXCEPTION(C68K_PRIVILEGE_VIOLATION_EX)									\
	RET(34)																	\
}

/*-----------------------------------------------------------------------------
  SUBI    Subtract Immediate
-----------------------------------------------------------------------------*/

#define SUBI_CLOCKS_D_8		8
#define SUBI_CLOCKS_D_16	8
#define SUBI_CLOCKS_D_32	16

#define SUBI_CLOCKS_M_8		12
#define SUBI_CLOCKS_M_16	12
#define SUBI_CLOCKS_M_32	20

#define SUBI(size, clk, mode)												\
{																			\
	EA_READ_I(size, NA, src)												\
	EA_READ_##mode(size, Y, dst)											\
	OP_SUB(size)															\
	EA_WRITE_RESULT(size, mode, Y)											\
	RET(SUBI_CLOCKS_##clk##_##size + EA_CLOCKS_##mode##_##size)				\
}

/*-----------------------------------------------------------------------------
  ADDI    Add Immediate
-----------------------------------------------------------------------------*/

#define ADDI_CLOCKS_D_8		8
#define ADDI_CLOCKS_D_16	8
#define ADDI_CLOCKS_D_32	16

#define ADDI_CLOCKS_M_8		12
#define ADDI_CLOCKS_M_16	12
#define ADDI_CLOCKS_M_32	20

#define ADDI(size, clk, mode)												\
{																			\
	EA_READ_I(size, NA, src)												\
	EA_READ_##mode(size, Y, dst)											\
	OP_ADD(size)															\
	EA_WRITE_RESULT(size, mode, Y)											\
	RET(ADDI_CLOCKS_##clk##_##size + EA_CLOCKS_##mode##_##size)				\
}

/*-----------------------------------------------------------------------------
  CMPI    Compare Immediate
-----------------------------------------------------------------------------*/

#define CMPI_CLOCKS_D_8		8
#define CMPI_CLOCKS_D_16	8
#define CMPI_CLOCKS_D_32	14

#define CMPI_CLOCKS_M_8		8
#define CMPI_CLOCKS_M_16	8
#define CMPI_CLOCKS_M_32	12

#define CMPI(size, clk, mode)												\
{																			\
	EA_READ_I(size, NA, src)												\
	EA_READ_##mode(size, Y, dst)											\
	OP_CMP(size)															\
	RET(CMPI_CLOCKS_M_##size + EA_CLOCKS_##mode##_##size)					\
}

/*-----------------------------------------------------------------------------
  BTST    Test a Bit
  BCHG    Test a Bit and Change
  BCLR    Test a Bit and Clear
  BSET    Test a Bit and Set
-----------------------------------------------------------------------------*/

#define BTST_CLOCKS_D_8		4
#define BTST_CLOCKS_D_32	6
#define BTST_CLOCKS_S_8		8
#define BTST_CLOCKS_S_32	10

#define BCHG_CLOCKS_D_8		8
#define BCHG_CLOCKS_D_32	8
#define BCHG_CLOCKS_S_8		12
#define BCHG_CLOCKS_S_32	12

#define BCLR_CLOCKS_D_8		8
#define BCLR_CLOCKS_D_32	10
#define BCLR_CLOCKS_S_8		12
#define BCLR_CLOCKS_S_32	14

#define BSET_CLOCKS_D_8		8
#define BSET_CLOCKS_D_32	8
#define BSET_CLOCKS_S_8		12
#define BSET_CLOCKS_S_32	12

#define BTST_WRITE(size, mode, xy)
#define BCHG_WRITE(size, mode, xy)	res ^= src;  EA_WRITE_RESULT(size, mode, xy)
#define BCLR_WRITE(size, mode, xy)	res &= ~src; EA_WRITE_RESULT(size, mode, xy)
#define BSET_WRITE(size, mode, xy)	res |= src;  EA_WRITE_RESULT(size, mode, xy)

#define BITOP_DYNAMIC(op, size, mode)										\
{																			\
	EA_READ_D(size, X, src)													\
	src = 1 << (src & (size - 1));											\
	EA_READ_##mode(size, Y, res)											\
	FLAG_Z = res & src;														\
	B##op##_WRITE(size, mode, Y)											\
	RET(B##op##_CLOCKS_D_##size + EA_CLOCKS_##mode##_##size)				\
}

#define BITOP_STATIC(op, size, mode)										\
{																			\
	EA_READ_I(8, NA, src)													\
	src = 1 << (src & (size - 1));											\
	EA_READ_##mode(size, Y, res)											\
	FLAG_Z = res & src;														\
	B##op##_WRITE(size, mode, Y)											\
	RET(B##op##_CLOCKS_S_##size + EA_CLOCKS_##mode##_##size)				\
}

/******************************************************************************
	OPCODE $1xxx - #3xxx
******************************************************************************/

/*-----------------------------------------------------------------------------
  MOVE    Move Data from Source to Destination
-----------------------------------------------------------------------------*/

#define MOVE_CLOCKS_D_8		4
#define MOVE_CLOCKS_A_8		4
#define MOVE_CLOCKS_AI_8	8
#define MOVE_CLOCKS_PI_8	8
#define MOVE_CLOCKS_PD_8	8
#define MOVE_CLOCKS_DI_8	12
#define MOVE_CLOCKS_IX_8	14
#define MOVE_CLOCKS_AW_8	12
#define MOVE_CLOCKS_AL_8	16
#define MOVE_CLOCKS_PI7_8	8
#define MOVE_CLOCKS_PD7_8	8

#define MOVE_CLOCKS_D_16	4
#define MOVE_CLOCKS_A_16	4
#define MOVE_CLOCKS_AI_16	8
#define MOVE_CLOCKS_PI_16	8
#define MOVE_CLOCKS_PD_16	8
#define MOVE_CLOCKS_DI_16	12
#define MOVE_CLOCKS_IX_16	14
#define MOVE_CLOCKS_AW_16	12
#define MOVE_CLOCKS_AL_16	16
#define MOVE_CLOCKS_PI7_16	8
#define MOVE_CLOCKS_PD7_16	8

#define MOVE_CLOCKS_D_32	4
#define MOVE_CLOCKS_A_32	4
#define MOVE_CLOCKS_AI_32	12
#define MOVE_CLOCKS_PI_32	12
#define MOVE_CLOCKS_PD_32	12
#define MOVE_CLOCKS_DI_32	18
#define MOVE_CLOCKS_IX_32	18
#define MOVE_CLOCKS_AW_32	16
#define MOVE_CLOCKS_AL_32	20
#define MOVE_CLOCKS_PI7_32	12
#define MOVE_CLOCKS_PD7_32	12

#define MOVE(size, modex, modey)											\
{																			\
	EA_READ_##modey(size, Y, res)											\
	FLAGS(size)																\
	EA_WRITE_##modex(size, X, res)											\
	RET(MOVE_CLOCKS_##modex##_##size + EA_CLOCKS_##modey##_##size)			\
}

/*-----------------------------------------------------------------------------
  MOVEA    Move Address
-----------------------------------------------------------------------------*/

#define MOVEA(size, mode)													\
{																			\
	EA_READSX_##mode(size, Y, AX)											\
	RET(4 + EA_CLOCKS_##mode##_##size)										\
}

/******************************************************************************
	OPCODE $4xxx
******************************************************************************/

/*-----------------------------------------------------------------------------
  NEGX    Negate with Extend
-----------------------------------------------------------------------------*/

#define NEGX_CLOCKS_D_8		4
#define NEGX_CLOCKS_D_16	4
#define NEGX_CLOCKS_D_32	6

#define NEGX_CLOCKS_M_8		8
#define NEGX_CLOCKS_M_16	8
#define NEGX_CLOCKS_M_32	12

#define NEGX(size, clk, mode)												\
{																			\
	EA_READ_##mode(size, Y, src)											\
	res = -src - XFLAG_AS_1();												\
	FLAGS_NEGX_##size()														\
	EA_WRITE_RESULT(size, mode, Y)											\
	RET(NEGX_CLOCKS_##clk##_##size + EA_CLOCKS_##mode##_##size)				\
}

/*-----------------------------------------------------------------------------
  CLR    Clear an Operand
-----------------------------------------------------------------------------*/

#define CLR_CLOCKS_D_8		4
#define CLR_CLOCKS_D_16		4
#define CLR_CLOCKS_D_32		6

#define CLR_CLOCKS_M_8		8
#define CLR_CLOCKS_M_16		8
#define CLR_CLOCKS_M_32		12

#define CLR(size, clk, mode)												\
{																			\
	FLAG_C = CFLAG_CLEAR;													\
	FLAG_V = VFLAG_CLEAR;													\
	FLAG_N = NFLAG_CLEAR;													\
	FLAG_Z = ZFLAG_SET;														\
	EA_WRITE_##mode(size, Y, 0)												\
	RET(CLR_CLOCKS_M_##size + EA_CLOCKS_##mode##_##size)					\
}

/*-----------------------------------------------------------------------------
  NEG    Negate
-----------------------------------------------------------------------------*/

#define NEG_CLOCKS_D_8		4
#define NEG_CLOCKS_D_16		4
#define NEG_CLOCKS_D_32		6

#define NEG_CLOCKS_M_8		8
#define NEG_CLOCKS_M_16		8
#define NEG_CLOCKS_M_32		12

#define NEG(size, clk, mode)												\
{																			\
	EA_READ_##mode(size, Y, src)											\
	res = -src;																\
	FLAGS_NEG_##size()														\
	EA_WRITE_RESULT(size, mode, Y)											\
	RET(NEG_CLOCKS_##clk##_##size + EA_CLOCKS_##mode##_##size)				\
}

/*-----------------------------------------------------------------------------
  NOT    Logical Complement
-----------------------------------------------------------------------------*/

#define NOT_CLOCKS_D_8		4
#define NOT_CLOCKS_D_16		4
#define NOT_CLOCKS_D_32		6

#define NOT_CLOCKS_M_8		8
#define NOT_CLOCKS_M_16		8
#define NOT_CLOCKS_M_32		12

#define NOT(size, clk, mode)												\
{																			\
	EA_READ_##mode(size, Y, src)											\
	res = ~src;																\
	FLAG_C = CFLAG_CLEAR;													\
	FLAG_V = VFLAG_CLEAR;													\
	FLAG_N = NFLAG_##size(res);												\
	FLAG_Z = ZFLAG_##size(res);												\
	EA_WRITE_RESULT(size, mode, Y)											\
	RET(NOT_CLOCKS_##clk##_##size + EA_CLOCKS_##mode##_##size)				\
}

/*-----------------------------------------------------------------------------
  MOVE from SR    Move from the Status Register
-----------------------------------------------------------------------------*/

#define MOVE_FRS_CLOCKS_D	6
#define MOVE_FRS_CLOCKS_M	8

#define MOVE_FRS(clk, mode)													\
{																			\
	res = GET_SR();															\
	EA_WRITE_##mode(16, Y, res)												\
	RET(MOVE_FRS_CLOCKS_##clk + EA_CLOCKS_##mode##_16)						\
}

/*-----------------------------------------------------------------------------
  MOVE to CCR    Move to Condition Code Register
-----------------------------------------------------------------------------*/

#define MOVE_TOC(mode)														\
{																			\
	EA_READ_##mode(16, Y, res)												\
	SET_CCR(res)															\
	RET(12 + EA_CLOCKS_##mode##_16)											\
}

/*-----------------------------------------------------------------------------
  MOVE to SR    Move to the Status Register
-----------------------------------------------------------------------------*/

#define MOVE_TOS(mode)														\
{																			\
	if (FLAG_S)																\
	{																		\
		EA_READ_##mode(16, Y, res)											\
		SET_SR(res)															\
		SWAP_SP()															\
		RET_INT(12 + EA_CLOCKS_##mode##_16)									\
	}																		\
	SWAP_SP_NOCHECK()														\
	EXCEPTION(C68K_PRIVILEGE_VIOLATION_EX)									\
	RET(34 + EA_CLOCKS_##mode##_16)											\
}

/*-----------------------------------------------------------------------------
  NBCD    Negate Decimal with Extend
-----------------------------------------------------------------------------*/

#define NBCD_CLOCKS_D	6
#define NBCD_CLOCKS_M	8

#define NBCD(clk, mode)														\
{																			\
	EA_READ_##mode(8, Y, res)												\
	res = 0x9a - res - XFLAG_AS_1();										\
	if (res != 0x9a)														\
	{																		\
		FLAG_V = ~res;														\
		if (LOW_NIBBLE(res) == 0x0a) res = HIGH_NIBBLE(res) + 0x10;			\
		res &= 0xff;														\
		FLAG_V &= res;														\
		EA_WRITE_RESULT(8, mode, Y)											\
		FLAG_X = XFLAG_SET;													\
		FLAG_C = CFLAG_SET;													\
		FLAG_Z |= res;														\
	}																		\
	else																	\
	{																		\
		FLAG_X = XFLAG_CLEAR;												\
		FLAG_C = CFLAG_CLEAR;												\
		FLAG_V = VFLAG_CLEAR;												\
	}																		\
	FLAG_N = res;															\
	RET(NBCD_CLOCKS_##clk + EA_CLOCKS_##mode##_8)							\
}

/*-----------------------------------------------------------------------------
  PEA    Push Effective Address
-----------------------------------------------------------------------------*/

#define PEA_CLOCKS_AI		12
#define PEA_CLOCKS_DI		16
#define PEA_CLOCKS_IX		20
#define PEA_CLOCKS_AW		16
#define PEA_CLOCKS_AL		20
#define PEA_CLOCKS_PCDI		16
#define PEA_CLOCKS_PCIX		20

#define PEA(mode)															\
{																			\
	EA_##mode(NA, Y)														\
	PUSH_32_F(adr)															\
	RET(PEA_CLOCKS_##mode)													\
}

/*-----------------------------------------------------------------------------
  MOVEM    Move Multiple Registers
-----------------------------------------------------------------------------*/

#define MOVEM_CLOCKS_RE_AI		8
#define MOVEM_CLOCKS_RE_PD		8
#define MOVEM_CLOCKS_RE_DI		12
#define MOVEM_CLOCKS_RE_IX		14
#define MOVEM_CLOCKS_RE_AW		12
#define MOVEM_CLOCKS_RE_AL		14

#define MOVEM_CLOCKS_ER_AI		12
#define MOVEM_CLOCKS_ER_PI		12
#define MOVEM_CLOCKS_ER_DI		16
#define MOVEM_CLOCKS_ER_IX		18
#define MOVEM_CLOCKS_ER_AW		16
#define MOVEM_CLOCKS_ER_AL		20
#define MOVEM_CLOCKS_ER_PCDI	16
#define MOVEM_CLOCKS_ER_PCIX	18

#define MOVEM_RE(size, mode)												\
{																			\
	EA_READ_I(16, NA, res)													\
	EA_##mode(NA, Y)														\
	src = (UINT32)(&D0);													\
	dst = adr;																\
	do																		\
	{																		\
		if (res & 1)														\
		{																	\
			WRITE_MEM_##size(adr, *(u##size *)src);							\
			adr += (size / 8);												\
		}																	\
		src += 4;															\
	} while (res >>= 1);													\
	RET(MOVEM_CLOCKS_RE_##mode + ((adr - dst) << 1))						\
}

#define MOVEM_RE_PD(size, y)												\
{																			\
	EA_READ_I(16, NA, res)													\
	adr = A##y;																\
	src = (UINT32)(&A7);													\
	dst = adr;																\
	do																		\
	{																		\
		if (res & 1)														\
		{																	\
			adr -= (size / 8);												\
			WRITE_MEM_##size##PD(adr, *(u##size *)src);						\
		}																	\
		src -= 4;															\
	} while (res >>= 1);													\
	A##y = adr;																\
	RET(MOVEM_CLOCKS_RE_PD + ((dst - adr) << 1))							\
}

#define MOVEM_ER(size, mode)												\
{																			\
	EA_READ_I(16, NA, res)													\
	EA_##mode(NA, Y)														\
	src = (UINT32)(&D0);													\
	dst = adr;																\
	do																		\
	{																		\
		if (res & 1)														\
		{																	\
			*(INT32 *)src = READSX_##mode(size, NA);						\
			adr += (size / 8);												\
		}																	\
		src += 4;															\
	} while (res >>= 1);													\
	RET(MOVEM_CLOCKS_ER_##mode + ((adr - dst) << 1))						\
}

#define MOVEM_ER_PI(size, y)												\
{																			\
	EA_READ_I(16, NA, res)													\
	adr = A##y;																\
	src = (UINT32)(&D0);													\
	dst = adr;																\
	do																		\
	{																		\
		if (res & 1)														\
		{																	\
			*(INT32 *)src = READSX_MEM_##size(adr);							\
			adr += (size / 8);												\
		}																	\
		src += 4;															\
	} while (res >>= 1);													\
	A##y = adr;																\
	RET(MOVEM_CLOCKS_ER_PI + ((adr - dst) << 1))							\
}

/*-----------------------------------------------------------------------------
  EXT    Sign-Extend
-----------------------------------------------------------------------------*/

#define EXT(size1, size2)													\
{																			\
	EA_READSX_D(size1, Y, res)												\
	FLAGS(size1)															\
	EA_WRITE_RESULT(size2, D, Y)											\
	RET(4)																	\
}

/*-----------------------------------------------------------------------------
  TST    Test an Operand
-----------------------------------------------------------------------------*/

#define TST(size, mode)														\
{																			\
	EA_READ_##mode(size, Y, res)											\
	FLAGS(size)																\
	RET(4 + EA_CLOCKS_##mode##_##size)										\
}

/*-----------------------------------------------------------------------------
  TAS    Test and Set an Operand
-----------------------------------------------------------------------------*/

#define TAS_CLOCKS_D	4
#define TAS_CLOCKS_M	14

#define TAS(clk, mode)														\
{																			\
	EA_READ_##mode(8, Y, res)												\
	FLAGS(8)																\
	res |= 0x80;															\
	EA_WRITE_RESULT(8, mode, Y)												\
	RET(TAS_CLOCKS_##clk + EA_CLOCKS_##mode##_8)							\
}

/*-----------------------------------------------------------------------------
  MOVE USP    Move User Stack Pointer
-----------------------------------------------------------------------------*/

#define MOVEUSP(d, s)														\
{																			\
	if (FLAG_S)																\
	{																		\
		d = s;																\
		RET(4)																\
	}																		\
	SWAP_SP_NOCHECK()														\
	EXCEPTION(C68K_PRIVILEGE_VIOLATION_EX)									\
	RET(34)																	\
}

/*-----------------------------------------------------------------------------
  JSR    Jump to Subroutine
-----------------------------------------------------------------------------*/

#define JSR_CLOCKS_AI		16
#define JSR_CLOCKS_DI		18
#define JSR_CLOCKS_IX		22
#define JSR_CLOCKS_AW		18
#define JSR_CLOCKS_AL		20
#define JSR_CLOCKS_PCDI		18
#define JSR_CLOCKS_PCIX		22

#define JSR(mode)															\
{																			\
	EA_##mode(NA, Y)														\
	PUSH_32_F(GET_PC())														\
	SET_PC(adr)																\
	RET(JSR_CLOCKS_##mode)													\
}

/*-----------------------------------------------------------------------------
  JMP    Jump
-----------------------------------------------------------------------------*/

#define JMP_CLOCKS_AI		8
#define JMP_CLOCKS_DI		10
#define JMP_CLOCKS_IX		14
#define JMP_CLOCKS_AW		10
#define JMP_CLOCKS_AL		12
#define JMP_CLOCKS_PCDI		10
#define JMP_CLOCKS_PCIX		14

#define JMP(mode)															\
{																			\
	EA_##mode(NA, Y)														\
	SET_PC(adr)																\
	RET(JMP_CLOCKS_##mode)													\
}

/*-----------------------------------------------------------------------------
  CHK    Check Register Against Bounds
-----------------------------------------------------------------------------*/

#define CHK(mode)															\
{																			\
	EA_READ_##mode(16, Y, src)												\
	EA_READ_D(16, X, res)													\
	if (((INT32)res < 0) || (res > src))									\
	{																		\
		FLAG_N = NFLAG_16(res);												\
		SWAP_SP()															\
		EXCEPTION(C68K_CHK_EX)												\
		RET(40 + EA_CLOCKS_##mode##_16)										\
	}																		\
	RET(10 + EA_CLOCKS_##mode##_16)											\
}

/*-----------------------------------------------------------------------------
  LEA    Load Effective Address
-----------------------------------------------------------------------------*/

#define LEA_CLOCKS_AI		4
#define LEA_CLOCKS_DI		8
#define LEA_CLOCKS_IX		12
#define LEA_CLOCKS_AW		8
#define LEA_CLOCKS_AL		12
#define LEA_CLOCKS_PCDI		8
#define LEA_CLOCKS_PCIX		12

#define LEA(mode)															\
{																			\
	EA_##mode(NA, Y)														\
	AX = adr;																\
	RET(LEA_CLOCKS_##mode)													\
}

/******************************************************************************
	OPCODE $5xxx
******************************************************************************/

/*-----------------------------------------------------------------------------
  Scc    Set According to Condition
-----------------------------------------------------------------------------*/

#define ST_D()																\
{																			\
	EA_WRITE_D(8, Y, 0xff)													\
	RET(6)																	\
}

#define SF_D()																\
{																			\
	EA_WRITE_D(8, Y, 0)														\
	RET(4)																	\
}

#define Scc_D(cond)															\
{																			\
	if (COND_##cond()) ST_D()												\
	SF_D()																	\
}

#define ST(mode)															\
{																			\
	EA_WRITE_##mode(8, Y, 0xff)												\
	RET(8 + EA_CLOCKS_##mode##_8)											\
}

#define SF(mode)															\
{																			\
	EA_WRITE_##mode(8, Y, 0)												\
	RET(8 + EA_CLOCKS_##mode##_8)											\
}

#define Scc(mode, cond)														\
{																			\
	EA_##mode(8, Y)															\
	if (COND_##cond())														\
	{																		\
		WRITE_MEM_8(adr, 0xff);												\
		RET(8 + EA_CLOCKS_##mode##_8)										\
	}																		\
	WRITE_MEM_8(adr, 0);													\
	RET(8 + EA_CLOCKS_##mode##_8)											\
}

/*-----------------------------------------------------------------------------
  DBcc    Test Condition, Decrement, and Branch
-----------------------------------------------------------------------------*/

#define DBT()																\
{																			\
	PC += 2;																\
	RET(12)																	\
}

#define DBF()																\
{																			\
	EA_READ_D(16, Y, res)													\
	res--;																	\
	EA_WRITE_RESULT(16, D, Y)												\
	if ((INT32)res != -1)													\
	{																		\
		PC += READSX_IMM_16();												\
		ADJUST_PC()															\
		SET_PC(PC)															\
		RET(10)																\
	}																		\
	PC += 2;																\
	RET(14)																	\
}

#define DBcc(cond)															\
{																			\
	if (COND_NOT_##cond()) DBF()											\
	DBT()																	\
}

/*-----------------------------------------------------------------------------
  ADDQ    Add Quick
-----------------------------------------------------------------------------*/

#define ADDQ_CLOCKS_D_8		4
#define ADDQ_CLOCKS_D_16	4
#define ADDQ_CLOCKS_D_32	8

#define ADDQ_CLOCKS_M_8		8
#define ADDQ_CLOCKS_M_16	8
#define ADDQ_CLOCKS_M_32	12

#define ADDQ(size, clk, mode)												\
{																			\
	src = GET_QUICK();														\
	EA_READ_##mode(size, Y, dst)											\
	OP_ADD(size)															\
	EA_WRITE_RESULT(size, mode, Y)											\
	RET(ADDQ_CLOCKS_##clk##_##size + EA_CLOCKS_##mode##_##size)				\
}

#define ADDQ_A(size)														\
{																			\
	src = GET_QUICK();														\
	AY += src;																\
	RET(8)																	\
}

/*-----------------------------------------------------------------------------
  SUBQ    Subtract Quick
-----------------------------------------------------------------------------*/

#define SUBQ_CLOCKS_D_8		4
#define SUBQ_CLOCKS_D_16	4
#define SUBQ_CLOCKS_D_32	8

#define SUBQ_CLOCKS_M_8		8
#define SUBQ_CLOCKS_M_16	8
#define SUBQ_CLOCKS_M_32	12

#define SUBQ(size, clk, mode)												\
{																			\
	src = GET_QUICK();														\
	EA_READ_##mode(size, Y, dst)											\
	OP_SUB(size)															\
	EA_WRITE_RESULT(size, mode, Y)											\
	RET(SUBQ_CLOCKS_##clk##_##size + EA_CLOCKS_##mode##_##size)				\
}

#define SUBQ_A(size)														\
{																			\
	src = GET_QUICK();														\
	AY -= src;																\
	RET(8)																	\
}

/******************************************************************************
	OPCODE $6xxx
******************************************************************************/

/*-----------------------------------------------------------------------------
  Bcc    Branch Conditionally
-----------------------------------------------------------------------------*/

#define Bcc_8(cond)															\
{																			\
	if (COND_##cond())														\
	{																		\
		PC += MAKE_INT_8(Opcode);											\
		RET(10)																\
	}																		\
	RET(8)																	\
}

#define Bcc_16(cond)														\
{																			\
	if (COND_##cond())														\
	{																		\
		PC += READSX_IMM_16();												\
		ADJUST_PC()															\
		SET_PC(PC)															\
		RET(10)																\
	}																		\
	PC += 2;																\
	RET(12)																	\
}

/******************************************************************************
	OPCODE $8xxx
******************************************************************************/

/*-----------------------------------------------------------------------------
  OR    inclusive-OR logical
-----------------------------------------------------------------------------*/

/*
  (*) The base time of six clock periods is increased to eight if the
      effective address mode is register direct or immediate (effective
      address time should also be added).
*/

#define OR_CLOCKS_ER_8		4
#define OR_CLOCKS_ER_16		4
#define OR_CLOCKS_ER_32		6	// *

#define OR_CLOCKS_RE_8		8
#define OR_CLOCKS_RE_16		8
#define OR_CLOCKS_RE_32		12

#define OR_ER_RI32(mode)													\
{																			\
	EA_READ_##mode(32, Y, src)												\
	EA_READ_D(32, X, res)													\
	OP_OR(32)																\
	EA_WRITE_RESULT(32, D, X)												\
	RET(8 + EA_CLOCKS_##mode##_32)											\
}

#define OR_ER(size, mode)													\
{																			\
	EA_READ_##mode(size, Y, src)											\
	EA_READ_D(size, X, res)													\
	OP_OR(size)																\
	EA_WRITE_RESULT(size, D, X)												\
	RET(OR_CLOCKS_ER_##size + EA_CLOCKS_##mode##_##size)					\
}

#define OR_RE(size, mode)													\
{																			\
	EA_READ_D(size, X, src)													\
	EA_READ_##mode(size, Y, res)											\
	OP_OR(size)																\
	EA_WRITE_RESULT(size, mode, Y)											\
	RET(OR_CLOCKS_RE_##size + EA_CLOCKS_##mode##_##size)					\
}

/*-----------------------------------------------------------------------------
  SBCD    Subtract Decimal with Extend
-----------------------------------------------------------------------------*/

#define SBCD_CLOCKS_D	6
#define SBCD_CLOCKS_M	18

#define SBCD(clk, modex, modey)												\
{																			\
	EA_READ_##modey(8, Y, src)												\
	EA_READ_##modex(8, X, dst)												\
	FLAG_V = VFLAG_CLEAR;													\
	res = LOW_NIBBLE(dst) - LOW_NIBBLE(src) - XFLAG_AS_1();					\
	if (res > 9) res -= 6;													\
	res += HIGH_NIBBLE(dst) - HIGH_NIBBLE(src);								\
	if (res > 0x99)															\
	{																		\
		res += 0xa0;														\
		FLAG_X = XFLAG_SET;													\
		FLAG_C = CFLAG_SET;													\
		FLAG_N = NFLAG_SET;													\
	}																		\
	else																	\
	{																		\
		FLAG_X = XFLAG_CLEAR;												\
		FLAG_C = CFLAG_CLEAR;												\
		FLAG_N = NFLAG_CLEAR;												\
	}																		\
	FLAG_Z |= ZFLAG_8(res);													\
	EA_WRITE_RESULT(8, modex, X)											\
	RET(SBCD_CLOCKS_##clk)													\
}

/*-----------------------------------------------------------------------------
  DIVU    Unsigned Divide
-----------------------------------------------------------------------------*/

#define DIVU(mode)															\
{																			\
	EA_READ_##mode(16, Y, src)												\
	if (src)																\
	{																		\
		EA_READ_D(32, X, dst)												\
		res = dst / src;													\
		if (res & 0xffff0000) /* overflow */								\
		{																	\
			FLAG_V = VFLAG_SET;												\
			RET(70 + EA_CLOCKS_##mode##_16)									\
		}																	\
		res &= 0x0000ffff;													\
		FLAG_C = CFLAG_CLEAR;												\
		FLAG_V = VFLAG_CLEAR;												\
		FLAG_N = NFLAG_16(res);												\
		FLAG_Z = res;														\
		res |= (dst % src) << 16;											\
		EA_WRITE_RESULT(32, D, X)											\
		RET(90 + EA_CLOCKS_##mode##_16)										\
	}																		\
	SWAP_SP()																\
	EXCEPTION(C68K_ZERO_DIVIDE_EX)											\
	RET(38 + EA_CLOCKS_##mode##_16)											\
}

/*-----------------------------------------------------------------------------
  DIVS    Signed Divide
-----------------------------------------------------------------------------*/

#define DIVS(mode)															\
{																			\
	EA_READSX_##mode(16, Y, src)											\
	if (src)																\
	{																		\
		EA_READ_D(32, X, dst)												\
		if (((UINT32)dst == 0x80000000) && ((INT32)src == -1))				\
		{																	\
			FLAG_C = CFLAG_CLEAR;											\
			FLAG_V = VFLAG_CLEAR;											\
			FLAG_N = NFLAG_CLEAR;											\
			FLAG_Z = ZFLAG_SET;												\
			WRITE_REG_32(DX, 0);											\
			RET(50 + EA_CLOCKS_##mode##_16)									\
		}																	\
		else																\
		{																	\
			INT32 quotient = (INT32)dst / (INT32)src;						\
			if (quotient > 0x7fff || quotient < -0x8000)					\
			{																\
				FLAG_V = VFLAG_SET;											\
				RET(80 + EA_CLOCKS_##mode##_16)								\
			}																\
			else															\
			{																\
				INT32 remainder = (INT32)dst % (INT32)src;					\
				quotient &= 0x0000ffff;										\
				FLAG_C = CFLAG_CLEAR;										\
				FLAG_V = VFLAG_CLEAR;										\
				FLAG_N = NFLAG_16(quotient);								\
				FLAG_Z = quotient;											\
				res = quotient | (remainder << 16);							\
				EA_WRITE_RESULT(32, D, X)									\
				RET(108 + EA_CLOCKS_##mode##_16)							\
			}																\
		}																	\
	}																		\
	SWAP_SP()																\
	EXCEPTION(C68K_ZERO_DIVIDE_EX)											\
	RET(38 + EA_CLOCKS_##mode##_16)											\
}

/******************************************************************************
	OPCODE $9xxx
******************************************************************************/

/*-----------------------------------------------------------------------------
  SUB    Subtract
-----------------------------------------------------------------------------*/

/*
  (*) The base time of six clock periods is increased to eight if the
      effective address mode is register direct or immediate (effective
      address time should also be added).
*/

#define SUB_CLOCKS_ER_8		4
#define SUB_CLOCKS_ER_16	4
#define SUB_CLOCKS_ER_32	6	// *

#define SUB_CLOCKS_RE_8		8
#define SUB_CLOCKS_RE_16	8
#define SUB_CLOCKS_RE_32	12

#define SUB_ER_RI32(mode)													\
{																			\
	EA_READ_##mode(32, Y, src)												\
	EA_READ_D(32, X, dst)													\
	OP_SUB(32)																\
	EA_WRITE_RESULT(32, D, X)												\
	RET(8 + EA_CLOCKS_##mode##_32)											\
}

#define SUB_ER(size, mode)													\
{																			\
	EA_READ_##mode(size, Y, src)											\
	EA_READ_D(size, X, dst)													\
	OP_SUB(size)															\
	EA_WRITE_RESULT(size, D, X)												\
	RET(SUB_CLOCKS_ER_##size + EA_CLOCKS_##mode##_##size)					\
}

#define SUB_RE(size, mode)													\
{																			\
	EA_READ_D(size, X, src)													\
	EA_READ_##mode(size, Y, dst)											\
	OP_SUB(size)															\
	EA_WRITE_RESULT(size, mode, Y)											\
	RET(SUB_CLOCKS_RE_##size + EA_CLOCKS_##mode##_##size)					\
}

/*-----------------------------------------------------------------------------
  SUBX    Subtract with Extend
-----------------------------------------------------------------------------*/

#define SUBX_CLOCKS_D_8		4
#define SUBX_CLOCKS_D_16	4
#define SUBX_CLOCKS_D_32	8

#define SUBX_CLOCKS_M_8		18
#define SUBX_CLOCKS_M_16	18
#define SUBX_CLOCKS_M_32	30

#define SUBX(size, clk, modex, modey)										\
{																			\
	EA_READ_##modey(size, Y, src)											\
	EA_READ_##modex(size, X, dst)											\
	res = dst - src - XFLAG_AS_1();											\
	FLAGS_SUBX_##size()														\
	EA_WRITE_RESULT(size, modex, X)											\
	RET(SUBX_CLOCKS_##clk##_##size)											\
}

/*-----------------------------------------------------------------------------
  SUBA    Subtract Address
-----------------------------------------------------------------------------*/

/*
  (*) The base time of six clock periods is increased to eight if the
      effective address mode is register direct or immediate (effective
      address time should also be added).
*/

#define SUBA_CLOCKS_16		8
#define SUBA_CLOCKS_32		6	// *

#define SUBA_RI32(mode)														\
{																			\
	EA_READSX_##mode(32, Y, src)											\
	AX -= src;																\
	RET(8 + EA_CLOCKS_##mode##_32)											\
}

#define SUBA(size, mode)													\
{																			\
	EA_READSX_##mode(size, Y, src)											\
	AX -= src;																\
	RET(SUBA_CLOCKS_##size + EA_CLOCKS_##mode##_##size)						\
}

/******************************************************************************
	OPCODE $Bxxx
******************************************************************************/

/*-----------------------------------------------------------------------------
  CMP    Compare
-----------------------------------------------------------------------------*/

#define CMP_CLOCKS_8		4
#define CMP_CLOCKS_16		4
#define CMP_CLOCKS_32		6

#define CMP(size, mode)														\
{																			\
	EA_READ_##mode(size, Y, src)											\
	EA_READ_D(size, X, dst)													\
	OP_CMP(size)															\
	RET(CMP_CLOCKS_##size + EA_CLOCKS_##mode##_##size)						\
}

/*-----------------------------------------------------------------------------
  CMPM    Compare Memory
-----------------------------------------------------------------------------*/

#define CMPM_CLOCKS_8	12
#define CMPM_CLOCKS_16	12
#define CMPM_CLOCKS_32	20

#define CMPM(size, modex, modey)											\
{																			\
	EA_READ_##modey(size, Y, src)											\
	EA_READ_##modex(size, X, dst)											\
	OP_CMP(size)															\
	RET(CMPM_CLOCKS_##size)													\
}

/*-----------------------------------------------------------------------------
  EOR    Exclusive-OR logical
-----------------------------------------------------------------------------*/

#define EOR_CLOCKS_D_8		4
#define EOR_CLOCKS_D_16		4
#define EOR_CLOCKS_D_32		8

#define EOR_CLOCKS_M_8		8
#define EOR_CLOCKS_M_16		8
#define EOR_CLOCKS_M_32		12

#define EOR(size, clk, mode)												\
{																			\
	EA_READ_D(size, X, src)													\
	EA_READ_##mode(size, Y, res)											\
	OP_EOR(size)															\
	EA_WRITE_RESULT(size, mode, Y)											\
	RET(EOR_CLOCKS_##clk##_##size + EA_CLOCKS_##mode##_##size)				\
}

/*-----------------------------------------------------------------------------
  CMPA    Compare
-----------------------------------------------------------------------------*/

#define CMPA(size, mode)													\
{																			\
	EA_READSX_##mode(size, Y, src)											\
	EA_READ_A(32, X, dst)													\
	OP_CMP(32)																\
	RET(6 + EA_CLOCKS_##mode##_##size)										\
}

/******************************************************************************
	OPCODE $Cxxx
******************************************************************************/

/*-----------------------------------------------------------------------------
  AND    AND logical
-----------------------------------------------------------------------------*/

/*
  (*) The base time of six clock periods is increased to eight if the
      effective address mode is register direct or immediate (effective
      address time should also be added).
*/

#define AND_CLOCKS_ER_8		4
#define AND_CLOCKS_ER_16	4
#define AND_CLOCKS_ER_32	6	// *

#define AND_CLOCKS_RE_8		8
#define AND_CLOCKS_RE_16	8
#define AND_CLOCKS_RE_32	12

#define AND_ER_RI32(mode)													\
{																			\
	EA_READ_##mode(32, Y, src)												\
	EA_READ_D(32, X, res)													\
	OP_AND(32)																\
	EA_WRITE_RESULT(32, D, X)												\
	RET(8 + EA_CLOCKS_##mode##_32)											\
}

#define AND_ER(size, mode)													\
{																			\
	EA_READ_##mode(size, Y, src)											\
	EA_READ_D(size, X, res)													\
	OP_AND(size)															\
	EA_WRITE_RESULT(size, D, X)												\
	RET(AND_CLOCKS_ER_##size + EA_CLOCKS_##mode##_##size)					\
}

#define AND_RE(size, mode)													\
{																			\
	EA_READ_D(size, X, src)													\
	EA_READ_##mode(size, Y, res)											\
	OP_AND(size)															\
	EA_WRITE_RESULT(size, mode, Y)											\
	RET(AND_CLOCKS_RE_##size + EA_CLOCKS_##mode##_##size)					\
}

/*-----------------------------------------------------------------------------
  ABCD    Add Decimal with Extend
-----------------------------------------------------------------------------*/

#define ABCD_CLOCKS_D	6
#define ABCD_CLOCKS_M	18

#define ABCD(clk, modex, modey)												\
{																			\
	EA_READ_##modey(8, Y, src)												\
	EA_READ_##modex(8, X, dst)												\
	res = LOW_NIBBLE(dst) + LOW_NIBBLE(src) + XFLAG_AS_1();					\
	FLAG_V = ~res;															\
	if (res > 9) res += 6;													\
	res += HIGH_NIBBLE(dst) + HIGH_NIBBLE(src);								\
	if (res > 0x99)															\
	{																		\
		res -= 0xa0;														\
		FLAG_X = XFLAG_SET;													\
		FLAG_C = CFLAG_SET;													\
	}																		\
	else																	\
	{																		\
		FLAG_X = XFLAG_CLEAR;												\
		FLAG_C = CFLAG_CLEAR;												\
	}																		\
	FLAG_V &= res;															\
	FLAG_N = res;															\
	FLAG_Z |= ZFLAG_8(res);													\
	EA_WRITE_RESULT(8, modex, X)											\
	RET(ABCD_CLOCKS_##clk)													\
}

/*-----------------------------------------------------------------------------
  MULU    Unsigned Multiply
-----------------------------------------------------------------------------*/

#define MULU(mode)															\
{																			\
	EA_READ_##mode(16, Y, src)												\
	EA_READ_D(16, X, res)													\
	res = res * src;														\
	FLAGS(32)																\
	EA_WRITE_RESULT(32, D, X)												\
	RET(50 + EA_CLOCKS_##mode##_16)											\
}

/*-----------------------------------------------------------------------------
  MULS    Signed Multiply
-----------------------------------------------------------------------------*/

#define MULS(mode)															\
{																			\
	EA_READSX_##mode(16, Y, src)											\
	EA_READSX_D(16, X, res)													\
	res = (INT32)res * (INT32)src;											\
	FLAGS(32)																\
	EA_WRITE_RESULT(32, D, X)												\
	RET(50 + EA_CLOCKS_##mode##_16)											\
}

/*-----------------------------------------------------------------------------
  EXG    Exchange Registers
-----------------------------------------------------------------------------*/

#define EXG(regx, regy)														\
{																			\
	res = regy##Y;															\
	regy##Y = regx##X;														\
	regx##X = res;															\
	RET(6)																	\
}

/******************************************************************************
	OPCODE $Dxxx
******************************************************************************/

/*-----------------------------------------------------------------------------
  ADD    Add
-----------------------------------------------------------------------------*/

/*
  (*) The base time of six clock periods is increased to eight if the
      effective address mode is register direct or immediate (effective
      address time should also be added).
*/

#define ADD_CLOCKS_ER_8		4
#define ADD_CLOCKS_ER_16	4
#define ADD_CLOCKS_ER_32	6	// *

#define ADD_CLOCKS_RE_8		8
#define ADD_CLOCKS_RE_16	8
#define ADD_CLOCKS_RE_32	12

#define ADD_ER_RI32(mode)													\
{																			\
	EA_READ_##mode(32, Y, src)												\
	EA_READ_D(32, X, dst)													\
	OP_ADD(32)																\
	EA_WRITE_RESULT(32, D, X)												\
	RET(8 + EA_CLOCKS_##mode##_32)											\
}

#define ADD_ER(size, mode)													\
{																			\
	EA_READ_##mode(size, Y, src)											\
	EA_READ_D(size, X, dst)													\
	OP_ADD(size)															\
	EA_WRITE_RESULT(size, D, X)												\
	RET(ADD_CLOCKS_ER_##size + EA_CLOCKS_##mode##_##size)					\
}

#define ADD_RE(size, mode)													\
{																			\
	EA_READ_D(size, X, src)													\
	EA_READ_##mode(size, Y, dst)											\
	OP_ADD(size)															\
	EA_WRITE_RESULT(size, mode, Y)											\
	RET(ADD_CLOCKS_RE_##size + EA_CLOCKS_##mode##_##size)					\
}

/*-----------------------------------------------------------------------------
  ADDX    Subtract with Extend
-----------------------------------------------------------------------------*/

#define ADDX_CLOCKS_D_8		4
#define ADDX_CLOCKS_D_16	4
#define ADDX_CLOCKS_D_32	8

#define ADDX_CLOCKS_M_8		18
#define ADDX_CLOCKS_M_16	18
#define ADDX_CLOCKS_M_32	30

#define ADDX(size, clk, modex, modey)										\
{																			\
	EA_READ_##modey(size, Y, src)											\
	EA_READ_##modex(size, X, dst)											\
	res = dst + src + XFLAG_AS_1();											\
	FLAGS_ADDX_##size()														\
	EA_WRITE_RESULT(size, modex, X)											\
	RET(ADDX_CLOCKS_##clk##_##size)											\
}

/*-----------------------------------------------------------------------------
  ADDA    Add Address
-----------------------------------------------------------------------------*/

/*
  (*) The base time of six clock periods is increased to eight if the
      effective address mode is register direct or immediate (effective
      address time should also be added).
*/

#define ADDA_CLOCKS_16		8
#define ADDA_CLOCKS_32		6	// *

#define ADDA_RI32(mode)														\
{																			\
	EA_READSX_##mode(32, Y, src)											\
	AX += src;																\
	RET(8 + EA_CLOCKS_##mode##_32)											\
}

#define ADDA(size, mode)													\
{																			\
	EA_READSX_##mode(size, Y, src)											\
	AX += src;																\
	RET(ADDA_CLOCKS_##size + EA_CLOCKS_##mode##_##size)						\
}

/******************************************************************************
	OPCODE $Exxx
******************************************************************************/

#define SHIFT_ROTATE_CLOCKS_8	6
#define SHIFT_ROTATE_CLOCKS_16	6
#define SHIFT_ROTATE_CLOCKS_32	8

/*-----------------------------------------------------------------------------
  ASR    Arithmetic Shift Right
-----------------------------------------------------------------------------*/

#define ASR_EA(mode)														\
{																			\
	EA_READ_##mode(16, Y, src)												\
	res = (src >> 1) | (src & (1 << 15));									\
	FLAG_X = FLAG_C = src << 8;												\
	FLAG_V = VFLAG_CLEAR;													\
	FLAG_N = NFLAG_16(res);													\
	FLAG_Z = res;															\
	EA_WRITE_RESULT(16, mode, Y)											\
	RET(8 + EA_CLOCKS_##mode##_16)											\
}

/*-----------------------------------------------------------------------------
  LSR    Logical Shift Right
-----------------------------------------------------------------------------*/

#define LSR_EA(mode)														\
{																			\
	EA_READ_##mode(16, Y, src)												\
	res = src >> 1;															\
	FLAG_X = FLAG_C = src << 8;												\
	FLAG_V = VFLAG_CLEAR;													\
	FLAG_N = NFLAG_CLEAR;													\
	FLAG_Z = res;															\
	EA_WRITE_RESULT(16, mode, Y)											\
	RET(8 + EA_CLOCKS_##mode##_16)											\
}

/*-----------------------------------------------------------------------------
  ROXR    Rotate Right with Extend
-----------------------------------------------------------------------------*/

#define ROXR_EA(mode)														\
{																			\
	EA_READ_##mode(16, Y, src)												\
	res = (src >> 1) | (XFLAG_AS_1() << 15);								\
	FLAG_X = FLAG_C = src << 8;												\
	FLAG_V = VFLAG_CLEAR;													\
	FLAG_N = NFLAG_16(res);													\
	FLAG_Z = res;															\
	EA_WRITE_RESULT(16, mode, Y)											\
	RET(8 + EA_CLOCKS_##mode##_16)											\
}

/*-----------------------------------------------------------------------------
  ROR    Rotate Right
-----------------------------------------------------------------------------*/

#define ROR_EA(mode)														\
{																			\
	EA_READ_##mode(16, Y, src)												\
	res = (src >> 1) | (src << 15);											\
	FLAG_C = src << 8;														\
	FLAG_V = VFLAG_CLEAR;													\
	FLAG_N = NFLAG_16(res);													\
	FLAG_Z = ZFLAG_16(res);													\
	EA_WRITE_RESULT(16, mode, Y)											\
	RET(8 + EA_CLOCKS_##mode##_16)											\
}

/*-----------------------------------------------------------------------------
  ASL    Arithmetic Shift Left
-----------------------------------------------------------------------------*/

#define ASL_EA(mode)														\
{																			\
	EA_READ_##mode(16, Y, src)												\
	res = src << 1;															\
	FLAG_X = FLAG_C = src >> 7;												\
	FLAG_V = (src ^ res) >> 8;												\
	FLAG_N = NFLAG_16(res);													\
	FLAG_Z = ZFLAG_16(res);													\
	EA_WRITE_RESULT(16, mode, Y)											\
	RET(8 + EA_CLOCKS_##mode##_16)											\
}

/*-----------------------------------------------------------------------------
  LSL    Logical Shift Left
-----------------------------------------------------------------------------*/

#define LSL_EA(mode)														\
{																			\
	EA_READ_##mode(16, Y, src)												\
	res = src << 1;															\
	FLAG_X = FLAG_C = src >> 7;												\
	FLAG_V = VFLAG_CLEAR;													\
	FLAG_N = NFLAG_16(res);													\
	FLAG_Z = ZFLAG_16(res);													\
	EA_WRITE_RESULT(16, mode, Y)											\
	RET(8 + EA_CLOCKS_##mode##_16)											\
}

/*-----------------------------------------------------------------------------
  ROXL    Rotate Right with Extend
-----------------------------------------------------------------------------*/

#define ROXL_EA(mode)														\
{																			\
	EA_READ_##mode(16, Y, src)												\
	res = (src << 1) | XFLAG_AS_1();										\
	FLAG_X = FLAG_C = src >> 7;												\
	FLAG_V = VFLAG_CLEAR;													\
	FLAG_N = NFLAG_16(res);													\
	FLAG_Z = ZFLAG_16(res);													\
	EA_WRITE_RESULT(16, mode, Y)											\
	RET(8 + EA_CLOCKS_##mode##_16)											\
}

/*-----------------------------------------------------------------------------
  ROL    Rotate Left
-----------------------------------------------------------------------------*/

#define ROL_EA(mode)														\
{																			\
	EA_READ_##mode(16, Y, src)												\
	res = (src << 1) | (src >> 15);											\
	FLAG_C = src >> 7;														\
	FLAG_V = VFLAG_CLEAR;													\
	FLAG_N = NFLAG_16(res);													\
	FLAG_Z = ZFLAG_16(res);													\
	EA_WRITE_RESULT(16, mode, Y)											\
	RET(8 + EA_CLOCKS_##mode##_16)											\
}
