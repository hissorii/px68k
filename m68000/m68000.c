/******************************************************************************

	m68000.c

	M68000 CPUインタフェース関数

******************************************************************************/

#include "emumain.h"


/******************************************************************************
	CPS2暗号化ROM用
******************************************************************************/

#if (EMU_SYSTEM == CPS2)
static UINT32 m68k_encrypt_start;
static UINT32 m68k_encrypt_end;
static UINT8  *m68k_decrypted_rom;

/*--------------------------------------------------------
	暗号化ROM範囲設定
--------------------------------------------------------*/

void m68000_set_encrypted_range(UINT32 start, UINT32 end, void *decrypted_rom)
{
	m68k_encrypt_start = start;
	m68k_encrypt_end   = end;
	m68k_decrypted_rom = (UINT8 *)decrypted_rom;
}


/*--------------------------------------------------------
	PC依存メモリリード (byte)
--------------------------------------------------------*/

static UINT8 m68000_read_pcrelative_8(UINT32 offset)
{
	if (offset >= m68k_encrypt_start && offset <= m68k_encrypt_end)
		return m68k_decrypted_rom[offset ^ 1];
	else
		return m68000_read_memory_8(offset);
}


/*--------------------------------------------------------
	PC依存メモリリード (word)
--------------------------------------------------------*/

static UINT16 m68000_read_pcrelative_16(UINT32 offset)
{
	if (offset >= m68k_encrypt_start && offset <= m68k_encrypt_end)
		return *(UINT16 *)&m68k_decrypted_rom[offset];
	else
		return m68000_read_memory_16(offset);
}
#endif


/******************************************************************************
	M68000インタフェース関数
******************************************************************************/

/*--------------------------------------------------------
	CPU初期化
--------------------------------------------------------*/

void m68000_init(void)
{
	C68k_Init(&C68K);
	C68k_Set_ReadB(&C68K, m68000_read_memory_8);
	C68k_Set_ReadW(&C68K, m68000_read_memory_16);
	C68k_Set_WriteB(&C68K, m68000_write_memory_8);
	C68k_Set_WriteW(&C68K, m68000_write_memory_16);
#if (EMU_SYSTEM == CPS1)
	C68k_Set_Fetch(&C68K, 0x000000, 0x1fffff, (UINT32)memory_region_cpu1);
	C68k_Set_Fetch(&C68K, 0x900000, 0x92ffff, (UINT32)cps1_gfxram);
	C68k_Set_Fetch(&C68K, 0xff0000, 0xffffff, (UINT32)cps1_ram);
#elif (EMU_SYSTEM == CPS2)
	if (memory_length_user1)
		C68k_Set_Fetch(&C68K, 0x000000, 0x3fffff, (UINT32)memory_region_user1);
	else
		C68k_Set_Fetch(&C68K, 0x000000, 0x3fffff, (UINT32)memory_region_cpu1);
	C68k_Set_Fetch(&C68K, 0x660000, 0x663fff, (UINT32)cps2_ram);
	C68k_Set_Fetch(&C68K, 0x900000, 0x92ffff, (UINT32)cps1_gfxram);
	C68k_Set_Fetch(&C68K, 0xff0000, 0xffffff, (UINT32)cps1_ram);
	if (memory_length_user1)
	{
		C68k_Set_ReadB_PC_Relative(&C68K, m68000_read_pcrelative_8);
		C68k_Set_ReadW_PC_Relative(&C68K, m68000_read_pcrelative_16);
	}
#elif (EMU_SYSTEM == MVS)
	C68k_Set_Fetch(&C68K, 0x000000, 0x0fffff, (UINT32)memory_region_cpu1);
	C68k_Set_Fetch(&C68K, 0x100000, 0x10ffff, (UINT32)neogeo_ram);
	if (memory_length_cpu1 > 0x100000)
		C68k_Set_Fetch(&C68K, 0x200000, 0x2fffff, (UINT32)&memory_region_cpu1[0x100000]);
	else
		C68k_Set_Fetch(&C68K, 0x200000, 0x2fffff, (UINT32)memory_region_cpu1);
	C68k_Set_Fetch(&C68K, 0xc00000, 0xc00000 + (memory_length_user1 - 1), (UINT32)memory_region_user1);
#elif (EMU_SYSTEM == NCDZ)
	C68k_Set_Fetch(&C68K, 0x000000, 0x1fffff, (UINT32)memory_region_cpu1);
	C68k_Set_Fetch(&C68K, 0xc00000, 0xc7ffff, (UINT32)memory_region_user1);
	C68k_Reset(&C68K);
#endif
}


/*--------------------------------------------------------
	CPUリセット
--------------------------------------------------------*/

void m68000_reset(void)
{
	C68k_Reset(&C68K);
}


/*--------------------------------------------------------
	CPU停止
--------------------------------------------------------*/

void m68000_exit(void)
{
}


/*--------------------------------------------------------
	CPU実行
--------------------------------------------------------*/

int m68000_execute(int cycles)
{
	return C68k_Exec(&C68K, cycles);
}


/*--------------------------------------------------------
	CPU実行 (NEOGEO CDZ専用: ロード画面用)
--------------------------------------------------------*/

#if (EMU_SYSTEM == NCDZ)
void m68000_execute2(UINT32 start, UINT32 break_point)
{
	int nest_counter = 0;
	UINT32 pc, old_pc, opcode;
	c68k_struc C68K_temp;

	old_pc = C68k_Get_Reg(&C68K, M68K_PC);

	memcpy(&C68K_temp, &C68K, sizeof(c68k_struc));

	C68k_Set_Reg(&C68K_temp, C68K_PC, start);
	C68K_temp.A[5] = 0x108000;
	C68K_temp.A[7] -= 4 * 8 * 2;

	while ((pc = C68k_Get_Reg(&C68K_temp, M68K_PC)) != break_point)
	{
		opcode = m68000_read_memory_16(pc);
		if (opcode == 0x4e75)
		{
			// rts
			nest_counter--;
			if (nest_counter < 0) break;
		}
		else if (opcode == 0x6100)
		{
			// bsr 16
			nest_counter++;
		}
		else if ((opcode & 0xff00) == 0x6100)
		{
			// bsr 8
			nest_counter++;
		}
		else if ((opcode & 0xffc0) == 0x4e80)
		{
			// jsr
			nest_counter++;
		}

		C68k_Exec(&C68K_temp, 1);
	}

	C68k_Set_Reg(&C68K, C68K_PC, old_pc);
}
#endif


/*--------------------------------------------------------
	割り込み処理
--------------------------------------------------------*/

void m68000_set_irq_line(int irqline, int state)
{
	if (irqline == IRQ_LINE_NMI)
		irqline = 7;

	C68k_Set_IRQ(&C68K, irqline, state);
}


/*--------------------------------------------------------
	割り込みコールバック関数設定
--------------------------------------------------------*/

void m68000_set_irq_callback(int (*callback)(int line))
{
	C68k_Set_IRQ_Callback(&C68K, callback);
}


/*--------------------------------------------------------
	レジスタ取得
--------------------------------------------------------*/

UINT32 m68000_get_reg(int regnum)
{
	switch (regnum)
	{
	case M68K_PC:  return C68k_Get_Reg(&C68K, C68K_PC);
	case M68K_USP: return C68k_Get_Reg(&C68K, C68K_USP);
	case M68K_MSP: return C68k_Get_Reg(&C68K, C68K_MSP);
	case M68K_SR:  return C68k_Get_Reg(&C68K, C68K_SR);
	case M68K_D0:  return C68k_Get_Reg(&C68K, C68K_D0);
	case M68K_D1:  return C68k_Get_Reg(&C68K, C68K_D1);
	case M68K_D2:  return C68k_Get_Reg(&C68K, C68K_D2);
	case M68K_D3:  return C68k_Get_Reg(&C68K, C68K_D3);
	case M68K_D4:  return C68k_Get_Reg(&C68K, C68K_D4);
	case M68K_D5:  return C68k_Get_Reg(&C68K, C68K_D5);
	case M68K_D6:  return C68k_Get_Reg(&C68K, C68K_D6);
	case M68K_D7:  return C68k_Get_Reg(&C68K, C68K_D7);
	case M68K_A0:  return C68k_Get_Reg(&C68K, C68K_A0);
	case M68K_A1:  return C68k_Get_Reg(&C68K, C68K_A1);
	case M68K_A2:  return C68k_Get_Reg(&C68K, C68K_A2);
	case M68K_A3:  return C68k_Get_Reg(&C68K, C68K_A3);
	case M68K_A4:  return C68k_Get_Reg(&C68K, C68K_A4);
	case M68K_A5:  return C68k_Get_Reg(&C68K, C68K_A5);
	case M68K_A6:  return C68k_Get_Reg(&C68K, C68K_A6);
	case M68K_A7:  return C68k_Get_Reg(&C68K, C68K_A7);
	default: return 0;
	}
}


/*--------------------------------------------------------
	レジスタ設定
--------------------------------------------------------*/

void m68000_set_reg(int regnum, UINT32 val)
{
	switch (regnum)
	{
	case M68K_PC:  C68k_Set_Reg(&C68K, C68K_PC, val); break;
	case M68K_USP: C68k_Set_Reg(&C68K, C68K_USP, val); break;
	case M68K_MSP: C68k_Set_Reg(&C68K, C68K_MSP, val); break;
	case M68K_SR:  C68k_Set_Reg(&C68K, C68K_SR, val); break;
	case M68K_D0:  C68k_Set_Reg(&C68K, C68K_D0, val); break;
	case M68K_D1:  C68k_Set_Reg(&C68K, C68K_D1, val); break;
	case M68K_D2:  C68k_Set_Reg(&C68K, C68K_D2, val); break;
	case M68K_D3:  C68k_Set_Reg(&C68K, C68K_D3, val); break;
	case M68K_D4:  C68k_Set_Reg(&C68K, C68K_D4, val); break;
	case M68K_D5:  C68k_Set_Reg(&C68K, C68K_D5, val); break;
	case M68K_D6:  C68k_Set_Reg(&C68K, C68K_D6, val); break;
	case M68K_D7:  C68k_Set_Reg(&C68K, C68K_D7, val); break;
	case M68K_A0:  C68k_Set_Reg(&C68K, C68K_A0, val); break;
	case M68K_A1:  C68k_Set_Reg(&C68K, C68K_A1, val); break;
	case M68K_A2:  C68k_Set_Reg(&C68K, C68K_A2, val); break;
	case M68K_A3:  C68k_Set_Reg(&C68K, C68K_A3, val); break;
	case M68K_A4:  C68k_Set_Reg(&C68K, C68K_A4, val); break;
	case M68K_A5:  C68k_Set_Reg(&C68K, C68K_A5, val); break;
	case M68K_A6:  C68k_Set_Reg(&C68K, C68K_A6, val); break;
	case M68K_A7:  C68k_Set_Reg(&C68K, C68K_A7, val); break;
	default: break;
	}
}


/*------------------------------------------------------
	セーブ/ロード ステート
------------------------------------------------------*/

#ifdef SAVE_STATE

STATE_SAVE( m68000 )
{
	int i;
	UINT32 pc = C68k_Get_Reg(&C68K, C68K_PC);

	for (i = 0; i < 8; i++)
		state_save_long(&C68K.D[i], 1);
	for (i = 0; i < 8; i++)
		state_save_long(&C68K.A[i], 1);

	state_save_long(&C68K.flag_C, 1);
	state_save_long(&C68K.flag_V, 1);
	state_save_long(&C68K.flag_Z, 1);
	state_save_long(&C68K.flag_N, 1);
	state_save_long(&C68K.flag_X, 1);
	state_save_long(&C68K.flag_I, 1);
	state_save_long(&C68K.flag_S, 1);
	state_save_long(&C68K.USP, 1);
	state_save_long(&pc, 1);
	state_save_long(&C68K.HaltState, 1);
	state_save_long(&C68K.IRQLine, 1);
	state_save_long(&C68K.IRQState, 1);
}

STATE_LOAD( m68000 )
{
	int i;
	UINT32 pc;

	for (i = 0; i < 8; i++)
		state_load_long(&C68K.D[i], 1);
	for (i = 0; i < 8; i++)
		state_load_long(&C68K.A[i], 1);

	state_load_long(&C68K.flag_C, 1);
	state_load_long(&C68K.flag_V, 1);
	state_load_long(&C68K.flag_Z, 1);
	state_load_long(&C68K.flag_N, 1);
	state_load_long(&C68K.flag_X, 1);
	state_load_long(&C68K.flag_I, 1);
	state_load_long(&C68K.flag_S, 1);
	state_load_long(&C68K.USP, 1);
	state_load_long(&pc, 1);
	state_load_long(&C68K.HaltState, 1);
	state_load_long(&C68K.IRQLine, 1);
	state_load_long(&C68K.IRQState, 1);

	C68k_Set_Reg(&C68K, C68K_PC, pc);
}

#endif /* SAVE_STATE */
