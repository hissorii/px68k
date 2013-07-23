/******************************************************************************

	c68k_op.c

	C68K opcodeÉ}ÉNÉç

******************************************************************************/

/******************************************************************************
	OPCODE $0xxx
******************************************************************************/

/*-----------------------------------------------------------------------------
  ORI    inclusive-OR Immediate
-----------------------------------------------------------------------------*/

OP(ori_8_d)            { ORI(8, D, D)                          }	// 0000
OP(ori_8_ai)           { ORI(8, M, AI)                         }	// 0010
OP(ori_8_pi)           { ORI(8, M, PI)                         }	// 0018
OP(ori_8_pd)           { ORI(8, M, PD)                         }	// 0020
OP(ori_8_di)           { ORI(8, M, DI)                         }	// 0028
OP(ori_8_ix)           { ORI(8, M, IX)                         }	// 0030
OP(ori_8_aw)           { ORI(8, M, AW)                         }	// 0038
OP(ori_8_al)           { ORI(8, M, AL)                         }	// 0039
OP(ori_8_pi7)          { ORI(8, M, PI7)                        }	// 001f
OP(ori_8_pd7)          { ORI(8, M, PD7)                        }	// 0027

OP(ori_16_d)           { ORI(16, D, D)                         }	// 0040
OP(ori_16_ai)          { ORI(16, M, AI)                        }	// 0050
OP(ori_16_pi)          { ORI(16, M, PI)                        }	// 0058
OP(ori_16_pd)          { ORI(16, M, PD)                        }	// 0060
OP(ori_16_di)          { ORI(16, M, DI)                        }	// 0068
OP(ori_16_ix)          { ORI(16, M, IX)                        }	// 0070
OP(ori_16_aw)          { ORI(16, M, AW)                        }	// 0078
OP(ori_16_al)          { ORI(16, M, AL)                        }	// 0079

OP(ori_32_d)           { ORI(32, D, D)                         }	// 0080
OP(ori_32_ai)          { ORI(32, M, AI)                        }	// 0090
OP(ori_32_pi)          { ORI(32, M, PI)                        }	// 0098
OP(ori_32_pd)          { ORI(32, M, PD)                        }	// 00a0
OP(ori_32_di)          { ORI(32, M, DI)                        }	// 00a8
OP(ori_32_ix)          { ORI(32, M, IX)                        }	// 00b0
OP(ori_32_aw)          { ORI(32, M, AW)                        }	// 00b8
OP(ori_32_al)          { ORI(32, M, AL)                        }	// 00b9

OP(ori_16_toc)         { LOGOP_TOC(|=)                         }	// 003c
OP(ori_16_tos)         { LOGOP_TOS(|=)                         }	// 007c

/*-----------------------------------------------------------------------------
  ANDI    And Immediate
-----------------------------------------------------------------------------*/

OP(andi_8_d)           { ANDI(8, D, D)                         }	// 0200
OP(andi_8_ai)          { ANDI(8, M, AI)                        }	// 0210
OP(andi_8_pi)          { ANDI(8, M, PI)                        }	// 0218
OP(andi_8_pd)          { ANDI(8, M, PD)                        }	// 0220
OP(andi_8_di)          { ANDI(8, M, DI)                        }	// 0228
OP(andi_8_ix)          { ANDI(8, M, IX)                        }	// 0230
OP(andi_8_aw)          { ANDI(8, M, AW)                        }	// 0238
OP(andi_8_al)          { ANDI(8, M, AL)                        }	// 0239
OP(andi_8_pi7)         { ANDI(8, M, PI7)                       }	// 021f
OP(andi_8_pd7)         { ANDI(8, M, PD7)                       }	// 0227

OP(andi_16_d)          { ANDI(16, D, D)                        }	// 0240
OP(andi_16_ai)         { ANDI(16, M, AI)                       }	// 0250
OP(andi_16_pi)         { ANDI(16, M, PI)                       }	// 0258
OP(andi_16_pd)         { ANDI(16, M, PD)                       }	// 0260
OP(andi_16_di)         { ANDI(16, M, DI)                       }	// 0268
OP(andi_16_ix)         { ANDI(16, M, IX)                       }	// 0270
OP(andi_16_aw)         { ANDI(16, M, AW)                       }	// 0278
OP(andi_16_al)         { ANDI(16, M, AL)                       }	// 0279

OP(andi_32_d)          { ANDI(32, D, D)                        }	// 0280
OP(andi_32_ai)         { ANDI(32, M, AI)                       }	// 0290
OP(andi_32_pi)         { ANDI(32, M, PI)                       }	// 0298
OP(andi_32_pd)         { ANDI(32, M, PD)                       }	// 02a0
OP(andi_32_di)         { ANDI(32, M, DI)                       }	// 02a8
OP(andi_32_ix)         { ANDI(32, M, IX)                       }	// 02b0
OP(andi_32_aw)         { ANDI(32, M, AW)                       }	// 02b8
OP(andi_32_al)         { ANDI(32, M, AL)                       }	// 02b9

OP(andi_16_toc)        { LOGOP_TOC(&=)                         }	// 023c
OP(andi_16_tos)        { LOGOP_TOS(&=)                         }	// 027c

/*-----------------------------------------------------------------------------
  EORI    Exclusive-OR Immediate
-----------------------------------------------------------------------------*/

OP(eori_8_d)           { EORI(8, D, D)                         }	// 0a00
OP(eori_8_ai)          { EORI(8, M, AI)                        }	// 0a10
OP(eori_8_pi)          { EORI(8, M, PI)                        }	// 0a18
OP(eori_8_pd)          { EORI(8, M, PD)                        }	// 0a20
OP(eori_8_di)          { EORI(8, M, DI)                        }	// 0a28
OP(eori_8_ix)          { EORI(8, M, IX)                        }	// 0a30
OP(eori_8_aw)          { EORI(8, M, AW)                        }	// 0a38
OP(eori_8_al)          { EORI(8, M, AL)                        }	// 0a39
OP(eori_8_pi7)         { EORI(8, M, PI7)                       }	// 0a1f
OP(eori_8_pd7)         { EORI(8, M, PD7)                       }	// 0a27

OP(eori_16_d)          { EORI(16, D, D)                        }	// 0a40
OP(eori_16_ai)         { EORI(16, M, AI)                       }	// 0a50
OP(eori_16_pi)         { EORI(16, M, PI)                       }	// 0a58
OP(eori_16_pd)         { EORI(16, M, PD)                       }	// 0a60
OP(eori_16_di)         { EORI(16, M, DI)                       }	// 0a68
OP(eori_16_ix)         { EORI(16, M, IX)                       }	// 0a70
OP(eori_16_aw)         { EORI(16, M, AW)                       }	// 0a78
OP(eori_16_al)         { EORI(16, M, AL)                       }	// 0a79

OP(eori_32_d)          { EORI(32, D, D)                        }	// 0a80
OP(eori_32_ai)         { EORI(32, M, AI)                       }	// 0a90
OP(eori_32_pi)         { EORI(32, M, PI)                       }	// 0a98
OP(eori_32_pd)         { EORI(32, M, PD)                       }	// 0aa0
OP(eori_32_di)         { EORI(32, M, DI)                       }	// 0aa8
OP(eori_32_ix)         { EORI(32, M, IX)                       }	// 0ab0
OP(eori_32_aw)         { EORI(32, M, AW)                       }	// 0ab8
OP(eori_32_al)         { EORI(32, M, AL)                       }	// 0ab9

OP(eori_16_toc)        { LOGOP_TOC(^=)                         }	// 0a3c
OP(eori_16_tos)        { LOGOP_TOS(^=)                         }	// 0a7c

/*-----------------------------------------------------------------------------
  SUBI    Subtract Immediate
-----------------------------------------------------------------------------*/

OP(subi_8_d)           { SUBI(8, D, D)                         }	// 0400
OP(subi_8_ai)          { SUBI(8, M, AI)                        }	// 0410
OP(subi_8_pi)          { SUBI(8, M, PI)                        }	// 0418
OP(subi_8_pd)          { SUBI(8, M, PD)                        }	// 0420
OP(subi_8_di)          { SUBI(8, M, DI)                        }	// 0428
OP(subi_8_ix)          { SUBI(8, M, IX)                        }	// 0430
OP(subi_8_aw)          { SUBI(8, M, AW)                        }	// 0438
OP(subi_8_al)          { SUBI(8, M, AL)                        }	// 0439
OP(subi_8_pi7)         { SUBI(8, M, PI7)                       }	// 041f
OP(subi_8_pd7)         { SUBI(8, M, PD7)                       }	// 0427

OP(subi_16_d)          { SUBI(16, D, D)                        }	// 0440
OP(subi_16_ai)         { SUBI(16, M, AI)                       }	// 0450
OP(subi_16_pi)         { SUBI(16, M, PI)                       }	// 0458
OP(subi_16_pd)         { SUBI(16, M, PD)                       }	// 0460
OP(subi_16_di)         { SUBI(16, M, DI)                       }	// 0468
OP(subi_16_ix)         { SUBI(16, M, IX)                       }	// 0470
OP(subi_16_aw)         { SUBI(16, M, AW)                       }	// 0478
OP(subi_16_al)         { SUBI(16, M, AL)                       }	// 0479

OP(subi_32_d)          { SUBI(32, D, D)                        }	// 0480
OP(subi_32_ai)         { SUBI(32, M, AI)                       }	// 0490
OP(subi_32_pi)         { SUBI(32, M, PI)                       }	// 0498
OP(subi_32_pd)         { SUBI(32, M, PD)                       }	// 04a0
OP(subi_32_di)         { SUBI(32, M, DI)                       }	// 04a8
OP(subi_32_ix)         { SUBI(32, M, IX)                       }	// 04b0
OP(subi_32_aw)         { SUBI(32, M, AW)                       }	// 04b8
OP(subi_32_al)         { SUBI(32, M, AL)                       }	// 04b9

/*-----------------------------------------------------------------------------
  ADDI    Add Immediate
-----------------------------------------------------------------------------*/

OP(addi_8_d)           { ADDI(8, D, D)                         }	// 0600
OP(addi_8_ai)          { ADDI(8, M, AI)                        }	// 0610
OP(addi_8_pi)          { ADDI(8, M, PI)                        }	// 0618
OP(addi_8_pd)          { ADDI(8, M, PD)                        }	// 0620
OP(addi_8_di)          { ADDI(8, M, DI)                        }	// 0628
OP(addi_8_ix)          { ADDI(8, M, IX)                        }	// 0630
OP(addi_8_aw)          { ADDI(8, M, AW)                        }	// 0638
OP(addi_8_al)          { ADDI(8, M, AL)                        }	// 0639
OP(addi_8_pi7)         { ADDI(8, M, PI7)                       }	// 061f
OP(addi_8_pd7)         { ADDI(8, M, PD7)                       }	// 0627

OP(addi_16_d)          { ADDI(16, D, D)                        }	// 0640
OP(addi_16_ai)         { ADDI(16, M, AI)                       }	// 0650
OP(addi_16_pi)         { ADDI(16, M, PI)                       }	// 0658
OP(addi_16_pd)         { ADDI(16, M, PD)                       }	// 0660
OP(addi_16_di)         { ADDI(16, M, DI)                       }	// 0668
OP(addi_16_ix)         { ADDI(16, M, IX)                       }	// 0670
OP(addi_16_aw)         { ADDI(16, M, AW)                       }	// 0678
OP(addi_16_al)         { ADDI(16, M, AL)                       }	// 0679

OP(addi_32_d)          { ADDI(32, D, D)                        }	// 0680
OP(addi_32_ai)         { ADDI(32, M, AI)                       }	// 0690
OP(addi_32_pi)         { ADDI(32, M, PI)                       }	// 0698
OP(addi_32_pd)         { ADDI(32, M, PD)                       }	// 06a0
OP(addi_32_di)         { ADDI(32, M, DI)                       }	// 06a8
OP(addi_32_ix)         { ADDI(32, M, IX)                       }	// 06b0
OP(addi_32_aw)         { ADDI(32, M, AW)                       }	// 06b8
OP(addi_32_al)         { ADDI(32, M, AL)                       }	// 06b9

/*-----------------------------------------------------------------------------
  CMPI    Compare Immediate
-----------------------------------------------------------------------------*/

OP(cmpi_8_d)           { CMPI(8, D, D)                         }	// 0c00
OP(cmpi_8_ai)          { CMPI(8, M, AI)                        }	// 0c10
OP(cmpi_8_pi)          { CMPI(8, M, PI)                        }	// 0c18
OP(cmpi_8_pd)          { CMPI(8, M, PD)                        }	// 0c20
OP(cmpi_8_di)          { CMPI(8, M, DI)                        }	// 0c28
OP(cmpi_8_ix)          { CMPI(8, M, IX)                        }	// 0c30
OP(cmpi_8_aw)          { CMPI(8, M, AW)                        }	// 0c38
OP(cmpi_8_al)          { CMPI(8, M, AL)                        }	// 0c39
OP(cmpi_8_pi7)         { CMPI(8, M, PI7)                       }	// 0c1f
OP(cmpi_8_pd7)         { CMPI(8, M, PD7)                       }	// 0c27

OP(cmpi_16_d)          { CMPI(16, D, D)                        }	// 0c40
OP(cmpi_16_ai)         { CMPI(16, M, AI)                       }	// 0c50
OP(cmpi_16_pi)         { CMPI(16, M, PI)                       }	// 0c58
OP(cmpi_16_pd)         { CMPI(16, M, PD)                       }	// 0c60
OP(cmpi_16_di)         { CMPI(16, M, DI)                       }	// 0c68
OP(cmpi_16_ix)         { CMPI(16, M, IX)                       }	// 0c70
OP(cmpi_16_aw)         { CMPI(16, M, AW)                       }	// 0c78
OP(cmpi_16_al)         { CMPI(16, M, AL)                       }	// 0c79

OP(cmpi_32_d)          { CMPI(32, D, D)                        }	// 0c80
OP(cmpi_32_ai)         { CMPI(32, M, AI)                       }	// 0c90
OP(cmpi_32_pi)         { CMPI(32, M, PI)                       }	// 0c98
OP(cmpi_32_pd)         { CMPI(32, M, PD)                       }	// 0ca0
OP(cmpi_32_di)         { CMPI(32, M, DI)                       }	// 0ca8
OP(cmpi_32_ix)         { CMPI(32, M, IX)                       }	// 0cb0
OP(cmpi_32_aw)         { CMPI(32, M, AW)                       }	// 0cb8
OP(cmpi_32_al)         { CMPI(32, M, AL)                       }	// 0cb9

/*-----------------------------------------------------------------------------
  BTST    Test a Bit
  BCHG    Test a Bit and Change
  BCLR    Test a Bit and Clear
  BSET    Test a Bit and Set
-----------------------------------------------------------------------------*/

OP(btst_32_s_d)        { BITOP_STATIC(TST, 32, D)              }	// 0800
OP(btst_8_s_ai)        { BITOP_STATIC(TST, 8, AI)              }	// 0810
OP(btst_8_s_pi)        { BITOP_STATIC(TST, 8, PI)              }	// 0818
OP(btst_8_s_pd)        { BITOP_STATIC(TST, 8, PD)              }	// 0820
OP(btst_8_s_di)        { BITOP_STATIC(TST, 8, DI)              }	// 0828
OP(btst_8_s_ix)        { BITOP_STATIC(TST, 8, IX)              }	// 0830
OP(btst_8_s_aw)        { BITOP_STATIC(TST, 8, AW)              }	// 0838
OP(btst_8_s_al)        { BITOP_STATIC(TST, 8, AL)              }	// 0839
OP(btst_8_s_pcdi)      { BITOP_STATIC(TST, 8, PCDI)            }	// 083a
OP(btst_8_s_pcix)      { BITOP_STATIC(TST, 8, PCIX)            }	// 083b
OP(btst_8_s_pi7)       { BITOP_STATIC(TST, 8, PI7)             }	// 081f
OP(btst_8_s_pd7)       { BITOP_STATIC(TST, 8, PD7)             }	// 0827

OP(bchg_32_s_d)        { BITOP_STATIC(CHG, 32, D)              }	// 0840
OP(bchg_8_s_ai)        { BITOP_STATIC(CHG, 8, AI)              }	// 0850
OP(bchg_8_s_pi)        { BITOP_STATIC(CHG, 8, PI)              }	// 0858
OP(bchg_8_s_pd)        { BITOP_STATIC(CHG, 8, PD)              }	// 0860
OP(bchg_8_s_di)        { BITOP_STATIC(CHG, 8, DI)              }	// 0868
OP(bchg_8_s_ix)        { BITOP_STATIC(CHG, 8, IX)              }	// 0870
OP(bchg_8_s_aw)        { BITOP_STATIC(CHG, 8, AW)              }	// 0878
OP(bchg_8_s_al)        { BITOP_STATIC(CHG, 8, AL)              }	// 0879
OP(bchg_8_s_pi7)       { BITOP_STATIC(CHG, 8, PI7)             }	// 085f
OP(bchg_8_s_pd7)       { BITOP_STATIC(CHG, 8, PD7)             }	// 0867

OP(bclr_32_s_d)        { BITOP_STATIC(CLR, 32, D)              }	// 0880
OP(bclr_8_s_ai)        { BITOP_STATIC(CLR, 8, AI)              }	// 0890
OP(bclr_8_s_pi)        { BITOP_STATIC(CLR, 8, PI)              }	// 0898
OP(bclr_8_s_pd)        { BITOP_STATIC(CLR, 8, PD)              }	// 08a0
OP(bclr_8_s_di)        { BITOP_STATIC(CLR, 8, DI)              }	// 08a8
OP(bclr_8_s_ix)        { BITOP_STATIC(CLR, 8, IX)              }	// 08b0
OP(bclr_8_s_aw)        { BITOP_STATIC(CLR, 8, AW)              }	// 08b8
OP(bclr_8_s_al)        { BITOP_STATIC(CLR, 8, AL)              }	// 08b9
OP(bclr_8_s_pi7)       { BITOP_STATIC(CLR, 8, PI7)             }	// 089f
OP(bclr_8_s_pd7)       { BITOP_STATIC(CLR, 8, PD7)             }	// 08a7

OP(bset_32_s_d)        { BITOP_STATIC(SET, 32, D)              }	// 08c0
OP(bset_8_s_ai)        { BITOP_STATIC(SET, 8, AI)              }	// 08d0
OP(bset_8_s_pi)        { BITOP_STATIC(SET, 8, PI)              }	// 08d8
OP(bset_8_s_pd)        { BITOP_STATIC(SET, 8, PD)              }	// 08e0
OP(bset_8_s_di)        { BITOP_STATIC(SET, 8, DI)              }	// 08e8
OP(bset_8_s_ix)        { BITOP_STATIC(SET, 8, IX)              }	// 08f0
OP(bset_8_s_aw)        { BITOP_STATIC(SET, 8, AW)              }	// 08f8
OP(bset_8_s_al)        { BITOP_STATIC(SET, 8, AL)              }	// 08f9
OP(bset_8_s_pi7)       { BITOP_STATIC(SET, 8, PI7)             }	// 08df
OP(bset_8_s_pd7)       { BITOP_STATIC(SET, 8, PD7)             }	// 08e7

OP(btst_32_r_d)        { BITOP_DYNAMIC(TST, 32, D)             }	// 0100
OP(btst_8_r_ai)        { BITOP_DYNAMIC(TST, 8, AI)             }	// 0110
OP(btst_8_r_pi)        { BITOP_DYNAMIC(TST, 8, PI)             }	// 0118
OP(btst_8_r_pd)        { BITOP_DYNAMIC(TST, 8, PD)             }	// 0120
OP(btst_8_r_di)        { BITOP_DYNAMIC(TST, 8, DI)             }	// 0128
OP(btst_8_r_ix)        { BITOP_DYNAMIC(TST, 8, IX)             }	// 0130
OP(btst_8_r_aw)        { BITOP_DYNAMIC(TST, 8, AW)             }	// 0138
OP(btst_8_r_al)        { BITOP_DYNAMIC(TST, 8, AL)             }	// 0139
OP(btst_8_r_pcdi)      { BITOP_DYNAMIC(TST, 8, PCDI)           }	// 013a
OP(btst_8_r_pcix)      { BITOP_DYNAMIC(TST, 8, PCIX)           }	// 013b
OP(btst_8_r_i)         { BITOP_DYNAMIC(TST, 8, I)              }	// 013c
OP(btst_8_r_pi7)       { BITOP_DYNAMIC(TST, 8, PI7)            }	// 011f
OP(btst_8_r_pd7)       { BITOP_DYNAMIC(TST, 8, PD7)            }	// 0127

OP(bchg_32_r_d)        { BITOP_DYNAMIC(CHG, 32, D)             }	// 0140
OP(bchg_8_r_ai)        { BITOP_DYNAMIC(CHG, 8, AI)             }	// 0150
OP(bchg_8_r_pi)        { BITOP_DYNAMIC(CHG, 8, PI)             }	// 0158
OP(bchg_8_r_pd)        { BITOP_DYNAMIC(CHG, 8, PD)             }	// 0160
OP(bchg_8_r_di)        { BITOP_DYNAMIC(CHG, 8, DI)             }	// 0168
OP(bchg_8_r_ix)        { BITOP_DYNAMIC(CHG, 8, IX)             }	// 0170
OP(bchg_8_r_aw)        { BITOP_DYNAMIC(CHG, 8, AW)             }	// 0178
OP(bchg_8_r_al)        { BITOP_DYNAMIC(CHG, 8, AL)             }	// 0179
OP(bchg_8_r_pi7)       { BITOP_DYNAMIC(CHG, 8, PI7)            }	// 015f
OP(bchg_8_r_pd7)       { BITOP_DYNAMIC(CHG, 8, PD7)            }	// 0167

OP(bclr_32_r_d)        { BITOP_DYNAMIC(CLR, 32, D)             }	// 0180
OP(bclr_8_r_ai)        { BITOP_DYNAMIC(CLR, 8, AI)             }	// 0190
OP(bclr_8_r_pi)        { BITOP_DYNAMIC(CLR, 8, PI)             }	// 0198
OP(bclr_8_r_pd)        { BITOP_DYNAMIC(CLR, 8, PD)             }	// 01a0
OP(bclr_8_r_di)        { BITOP_DYNAMIC(CLR, 8, DI)             }	// 01a8
OP(bclr_8_r_ix)        { BITOP_DYNAMIC(CLR, 8, IX)             }	// 01b0
OP(bclr_8_r_aw)        { BITOP_DYNAMIC(CLR, 8, AW)             }	// 01b8
OP(bclr_8_r_al)        { BITOP_DYNAMIC(CLR, 8, AL)             }	// 01b9
OP(bclr_8_r_pi7)       { BITOP_DYNAMIC(CLR, 8, PI7)            }	// 019f
OP(bclr_8_r_pd7)       { BITOP_DYNAMIC(CLR, 8, PD7)            }	// 01a7

OP(bset_32_r_d)        { BITOP_DYNAMIC(SET, 32, D)             }	// 01c0
OP(bset_8_r_ai)        { BITOP_DYNAMIC(SET, 8, AI)             }	// 01d0
OP(bset_8_r_pi)        { BITOP_DYNAMIC(SET, 8, PI)             }	// 01d8
OP(bset_8_r_pd)        { BITOP_DYNAMIC(SET, 8, PD)             }	// 01e0
OP(bset_8_r_di)        { BITOP_DYNAMIC(SET, 8, DI)             }	// 01e8
OP(bset_8_r_ix)        { BITOP_DYNAMIC(SET, 8, IX)             }	// 01f0
OP(bset_8_r_aw)        { BITOP_DYNAMIC(SET, 8, AW)             }	// 01f8
OP(bset_8_r_al)        { BITOP_DYNAMIC(SET, 8, AL)             }	// 01f9
OP(bset_8_r_pi7)       { BITOP_DYNAMIC(SET, 8, PI7)            }	// 01df
OP(bset_8_r_pd7)       { BITOP_DYNAMIC(SET, 8, PD7)            }	// 01e7

/*-----------------------------------------------------------------------------
  MOVEP    Move Peripheral Data
-----------------------------------------------------------------------------*/

// 0108
OP(movep_16_er)
{
	EA_DI(NA, Y)
	res  = READ_MEM_8(adr + 0);
	res <<= 8;
	res |= READ_MEM_8(adr + 2);
	EA_WRITE_RESULT(16, D, X)
	RET(16)
}

// 0148
OP(movep_32_er)
{
	EA_DI(NA, Y)
	res  = READ_MEM_8(adr + 0);
	res <<= 8;
	res |= READ_MEM_8(adr + 2);
	res <<= 8;
	res |= READ_MEM_8(adr + 4);
	res <<= 8;
	res |= READ_MEM_8(adr + 6);
	EA_WRITE_RESULT(32, D, X)
	RET(24)
}

// 0188
OP(movep_16_re)
{
	EA_READ_D(16, X, res)
	EA_DI(NA, Y)
	WRITE_MEM_8(adr + 2, res);
	res >>= 8;
	WRITE_MEM_8(adr + 0, res);
	RET(16)
}

// 01c8
OP(movep_32_re)
{
	EA_READ_D(32, X, res)
	EA_DI(NA, Y)
	WRITE_MEM_8(adr + 6, res);
	res >>= 8;
	WRITE_MEM_8(adr + 4, res);
	res >>= 8;
	WRITE_MEM_8(adr + 2, res);
	res >>= 8;
	WRITE_MEM_8(adr + 0, res);
	RET(24)
}

/******************************************************************************
	OPCODE $1xxx
******************************************************************************/

/*-----------------------------------------------------------------------------
  MOVE    Move Data from Source to Destination
-----------------------------------------------------------------------------*/

OP(move_8_d_d)         { MOVE(8, D,   D)                       }	// 1000
OP(move_8_ai_d)        { MOVE(8, AI,  D)                       }	// 1080
OP(move_8_pi_d)        { MOVE(8, PI,  D)                       }	// 10c0
OP(move_8_pd_d)        { MOVE(8, PD,  D)                       }	// 1100
OP(move_8_di_d)        { MOVE(8, DI,  D)                       }	// 1140
OP(move_8_ix_d)        { MOVE(8, IX,  D)                       }	// 1180
OP(move_8_aw_d)        { MOVE(8, AW,  D)                       }	// 11c0
OP(move_8_al_d)        { MOVE(8, AL,  D)                       }	// 13c0
OP(move_8_pi7_d)       { MOVE(8, PI7, D)                       }	// 1ec0
OP(move_8_pd7_d)       { MOVE(8, PD7, D)                       }	// 1f00

OP(move_8_d_ai)        { MOVE(8, D,   AI)                      }	// 1010
OP(move_8_ai_ai)       { MOVE(8, AI,  AI)                      }	// 1090
OP(move_8_pi_ai)       { MOVE(8, PI,  AI)                      }	// 10d0
OP(move_8_pd_ai)       { MOVE(8, PD,  AI)                      }	// 1110
OP(move_8_di_ai)       { MOVE(8, DI,  AI)                      }	// 1150
OP(move_8_ix_ai)       { MOVE(8, IX,  AI)                      }	// 1190
OP(move_8_aw_ai)       { MOVE(8, AW,  AI)                      }	// 11d0
OP(move_8_al_ai)       { MOVE(8, AL,  AI)                      }	// 13d0
OP(move_8_pi7_ai)      { MOVE(8, PI7, AI)                      }	// 1ed0
OP(move_8_pd7_ai)      { MOVE(8, PD7, AI)                      }	// 1f10

OP(move_8_d_pi)        { MOVE(8, D,   PI)                      }	// 1018
OP(move_8_ai_pi)       { MOVE(8, AI,  PI)                      }	// 1098
OP(move_8_pi_pi)       { MOVE(8, PI,  PI)                      }	// 10d8
OP(move_8_pd_pi)       { MOVE(8, PD,  PI)                      }	// 1118
OP(move_8_di_pi)       { MOVE(8, DI,  PI)                      }	// 1158
OP(move_8_ix_pi)       { MOVE(8, IX,  PI)                      }	// 1198
OP(move_8_aw_pi)       { MOVE(8, AW,  PI)                      }	// 11d8
OP(move_8_al_pi)       { MOVE(8, AL,  PI)                      }	// 13d8
OP(move_8_pi7_pi)      { MOVE(8, PI7, PI)                      }	// 1ed8
OP(move_8_pd7_pi)      { MOVE(8, PD7, PI)                      }	// 1f18

OP(move_8_d_pd)        { MOVE(8, D,   PD)                      }	// 1020
OP(move_8_ai_pd)       { MOVE(8, AI,  PD)                      }	// 10a0
OP(move_8_pi_pd)       { MOVE(8, PI,  PD)                      }	// 10e0
OP(move_8_pd_pd)       { MOVE(8, PD,  PD)                      }	// 1120
OP(move_8_di_pd)       { MOVE(8, DI,  PD)                      }	// 1160
OP(move_8_ix_pd)       { MOVE(8, IX,  PD)                      }	// 11a0
OP(move_8_aw_pd)       { MOVE(8, AW,  PD)                      }	// 11e0
OP(move_8_al_pd)       { MOVE(8, AL,  PD)                      }	// 13e0
OP(move_8_pi7_pd)      { MOVE(8, PI7, PD)                      }	// 1ee0
OP(move_8_pd7_pd)      { MOVE(8, PD7, PD)                      }	// 1f20

OP(move_8_d_di)        { MOVE(8, D,   DI)                      }	// 1028
OP(move_8_ai_di)       { MOVE(8, AI,  DI)                      }	// 10a8
OP(move_8_pi_di)       { MOVE(8, PI,  DI)                      }	// 10e8
OP(move_8_pd_di)       { MOVE(8, PD,  DI)                      }	// 1128
OP(move_8_di_di)       { MOVE(8, DI,  DI)                      }	// 1168
OP(move_8_ix_di)       { MOVE(8, IX,  DI)                      }	// 11a8
OP(move_8_aw_di)       { MOVE(8, AW,  DI)                      }	// 11e8
OP(move_8_al_di)       { MOVE(8, AL,  DI)                      }	// 13e8
OP(move_8_pi7_di)      { MOVE(8, PI7, DI)                      }	// 1ee8
OP(move_8_pd7_di)      { MOVE(8, PD7, DI)                      }	// 1f28

OP(move_8_d_ix)        { MOVE(8, D,   IX)                      }	// 1030
OP(move_8_ai_ix)       { MOVE(8, AI,  IX)                      }	// 10b0
OP(move_8_pi_ix)       { MOVE(8, PI,  IX)                      }	// 10f0
OP(move_8_pd_ix)       { MOVE(8, PD,  IX)                      }	// 1130
OP(move_8_di_ix)       { MOVE(8, DI,  IX)                      }	// 1170
OP(move_8_ix_ix)       { MOVE(8, IX,  IX)                      }	// 11b0
OP(move_8_aw_ix)       { MOVE(8, AW,  IX)                      }	// 11f0
OP(move_8_al_ix)       { MOVE(8, AL,  IX)                      }	// 13f0
OP(move_8_pi7_ix)      { MOVE(8, PI7, IX)                      }	// 1ef0
OP(move_8_pd7_ix)      { MOVE(8, PD7, IX)                      }	// 1f30

OP(move_8_d_aw)        { MOVE(8, D,   AW)                      }	// 1038
OP(move_8_ai_aw)       { MOVE(8, AI,  AW)                      }	// 10b8
OP(move_8_pi_aw)       { MOVE(8, PI,  AW)                      }	// 10f8
OP(move_8_pd_aw)       { MOVE(8, PD,  AW)                      }	// 1138
OP(move_8_di_aw)       { MOVE(8, DI,  AW)                      }	// 1178
OP(move_8_ix_aw)       { MOVE(8, IX,  AW)                      }	// 11b8
OP(move_8_aw_aw)       { MOVE(8, AW,  AW)                      }	// 11f8
OP(move_8_al_aw)       { MOVE(8, AL,  AW)                      }	// 13f8
OP(move_8_pi7_aw)      { MOVE(8, PI7, AW)                      }	// 1ef8
OP(move_8_pd7_aw)      { MOVE(8, PD7, AW)                      }	// 1f38

OP(move_8_d_al)        { MOVE(8, D,   AL)                      }	// 1039
OP(move_8_ai_al)       { MOVE(8, AI,  AL)                      }	// 10b9
OP(move_8_pi_al)       { MOVE(8, PI,  AL)                      }	// 10f9
OP(move_8_pd_al)       { MOVE(8, PD,  AL)                      }	// 1139
OP(move_8_di_al)       { MOVE(8, DI,  AL)                      }	// 1179
OP(move_8_ix_al)       { MOVE(8, IX,  AL)                      }	// 11b9
OP(move_8_aw_al)       { MOVE(8, AW,  AL)                      }	// 11f9
OP(move_8_al_al)       { MOVE(8, AL,  AL)                      }	// 13f9
OP(move_8_pi7_al)      { MOVE(8, PI7, AL)                      }	// 1ef9
OP(move_8_pd7_al)      { MOVE(8, PD7, AL)                      }	// 1f39

OP(move_8_d_pcdi)      { MOVE(8, D,   PCDI)                    }	// 103a
OP(move_8_ai_pcdi)     { MOVE(8, AI,  PCDI)                    }	// 10ba
OP(move_8_pi_pcdi)     { MOVE(8, PI,  PCDI)                    }	// 10fa
OP(move_8_pd_pcdi)     { MOVE(8, PD,  PCDI)                    }	// 113a
OP(move_8_di_pcdi)     { MOVE(8, DI,  PCDI)                    }	// 117a
OP(move_8_ix_pcdi)     { MOVE(8, IX,  PCDI)                    }	// 11ba
OP(move_8_aw_pcdi)     { MOVE(8, AW,  PCDI)                    }	// 11fa
OP(move_8_al_pcdi)     { MOVE(8, AL,  PCDI)                    }	// 13fa
OP(move_8_pi7_pcdi)    { MOVE(8, PI7, PCDI)                    }	// 1efa
OP(move_8_pd7_pcdi)    { MOVE(8, PD7, PCDI)                    }	// 1f3a

OP(move_8_d_pcix)      { MOVE(8, D,   PCIX)                    }	// 103b
OP(move_8_ai_pcix)     { MOVE(8, AI,  PCIX)                    }	// 10bb
OP(move_8_pi_pcix)     { MOVE(8, PI,  PCIX)                    }	// 10fb
OP(move_8_pd_pcix)     { MOVE(8, PD,  PCIX)                    }	// 113b
OP(move_8_di_pcix)     { MOVE(8, DI,  PCIX)                    }	// 117b
OP(move_8_ix_pcix)     { MOVE(8, IX,  PCIX)                    }	// 11bb
OP(move_8_aw_pcix)     { MOVE(8, AW,  PCIX)                    }	// 11fb
OP(move_8_al_pcix)     { MOVE(8, AL,  PCIX)                    }	// 13fb
OP(move_8_pi7_pcix)    { MOVE(8, PI7, PCIX)                    }	// 1efb
OP(move_8_pd7_pcix)    { MOVE(8, PD7, PCIX)                    }	// 1f3b

OP(move_8_d_i)         { MOVE(8, D,   I)                       }	// 103c
OP(move_8_ai_i)        { MOVE(8, AI,  I)                       }	// 10bc
OP(move_8_pi_i)        { MOVE(8, PI,  I)                       }	// 10fc
OP(move_8_pd_i)        { MOVE(8, PD,  I)                       }	// 113c
OP(move_8_di_i)        { MOVE(8, DI,  I)                       }	// 117c
OP(move_8_ix_i)        { MOVE(8, IX,  I)                       }	// 11bc
OP(move_8_aw_i)        { MOVE(8, AW,  I)                       }	// 11fc
OP(move_8_al_i)        { MOVE(8, AL,  I)                       }	// 13fc
OP(move_8_pi7_i)       { MOVE(8, PI7, I)                       }	// 1efc
OP(move_8_pd7_i)       { MOVE(8, PD7, I)                       }	// 1f3c

OP(move_8_d_pi7)       { MOVE(8, D,   PI7)                     }	// 101f
OP(move_8_ai_pi7)      { MOVE(8, AI,  PI7)                     }	// 109f
OP(move_8_pi_pi7)      { MOVE(8, PI,  PI7)                     }	// 10df
OP(move_8_pd_pi7)      { MOVE(8, PD,  PI7)                     }	// 111f
OP(move_8_di_pi7)      { MOVE(8, DI,  PI7)                     }	// 115f
OP(move_8_ix_pi7)      { MOVE(8, IX,  PI7)                     }	// 119f
OP(move_8_aw_pi7)      { MOVE(8, AW,  PI7)                     }	// 11df
OP(move_8_al_pi7)      { MOVE(8, AL,  PI7)                     }	// 13df
OP(move_8_pi7_pi7)     { MOVE(8, PI7, PI7)                     }	// 1edf
OP(move_8_pd7_pi7)     { MOVE(8, PD7, PI7)                     }	// 1f1f

OP(move_8_d_pd7)       { MOVE(8, D,   PD7)                     }	// 1027
OP(move_8_ai_pd7)      { MOVE(8, AI,  PD7)                     }	// 10a7
OP(move_8_pi_pd7)      { MOVE(8, PI,  PD7)                     }	// 10e7
OP(move_8_pd_pd7)      { MOVE(8, PD,  PD7)                     }	// 1127
OP(move_8_di_pd7)      { MOVE(8, DI,  PD7)                     }	// 1167
OP(move_8_ix_pd7)      { MOVE(8, IX,  PD7)                     }	// 11a7
OP(move_8_aw_pd7)      { MOVE(8, AW,  PD7)                     }	// 11e7
OP(move_8_al_pd7)      { MOVE(8, AL,  PD7)                     }	// 13e7
OP(move_8_pi7_pd7)     { MOVE(8, PI7, PD7)                     }	// 1ee7
OP(move_8_pd7_pd7)     { MOVE(8, PD7, PD7)                     }	// 1f27

/******************************************************************************
	OPCODE $2xxx
******************************************************************************/

/*-----------------------------------------------------------------------------
  MOVE    Move Data from Source to Destination
-----------------------------------------------------------------------------*/

OP(move_32_d_d)        { MOVE(32, D,   D)                      }	// 2000
OP(move_32_ai_d)       { MOVE(32, AI,  D)                      }	// 2080
OP(move_32_pi_d)       { MOVE(32, PI,  D)                      }	// 20c0
OP(move_32_pd_d)       { MOVE(32, PD,  D)                      }	// 2100
OP(move_32_di_d)       { MOVE(32, DI,  D)                      }	// 2140
OP(move_32_ix_d)       { MOVE(32, IX,  D)                      }	// 2180
OP(move_32_aw_d)       { MOVE(32, AW,  D)                      }	// 21c0
OP(move_32_al_d)       { MOVE(32, AL,  D)                      }	// 23c0

OP(move_32_d_a)        { MOVE(32, D,   A)                      }	// 2008
OP(move_32_ai_a)       { MOVE(32, AI,  A)                      }	// 2088
OP(move_32_pi_a)       { MOVE(32, PI,  A)                      }	// 20c8
OP(move_32_pd_a)       { MOVE(32, PD,  A)                      }	// 2108
OP(move_32_di_a)       { MOVE(32, DI,  A)                      }	// 2148
OP(move_32_ix_a)       { MOVE(32, IX,  A)                      }	// 2188
OP(move_32_aw_a)       { MOVE(32, AW,  A)                      }	// 21c8
OP(move_32_al_a)       { MOVE(32, AL,  A)                      }	// 23c8

OP(move_32_d_ai)       { MOVE(32, D,   AI)                     }	// 2010
OP(move_32_ai_ai)      { MOVE(32, AI,  AI)                     }	// 2090
OP(move_32_pi_ai)      { MOVE(32, PI,  AI)                     }	// 20d0
OP(move_32_pd_ai)      { MOVE(32, PD,  AI)                     }	// 2110
OP(move_32_di_ai)      { MOVE(32, DI,  AI)                     }	// 2150
OP(move_32_ix_ai)      { MOVE(32, IX,  AI)                     }	// 2190
OP(move_32_aw_ai)      { MOVE(32, AW,  AI)                     }	// 21d0
OP(move_32_al_ai)      { MOVE(32, AL,  AI)                     }	// 23d0

OP(move_32_d_pi)       { MOVE(32, D,   PI)                     }	// 2018
OP(move_32_ai_pi)      { MOVE(32, AI,  PI)                     }	// 2098
OP(move_32_pi_pi)      { MOVE(32, PI,  PI)                     }	// 20d8
OP(move_32_pd_pi)      { MOVE(32, PD,  PI)                     }	// 2118
OP(move_32_di_pi)      { MOVE(32, DI,  PI)                     }	// 2158
OP(move_32_ix_pi)      { MOVE(32, IX,  PI)                     }	// 2198
OP(move_32_aw_pi)      { MOVE(32, AW,  PI)                     }	// 21d8
OP(move_32_al_pi)      { MOVE(32, AL,  PI)                     }	// 23d8

OP(move_32_d_pd)       { MOVE(32, D,   PD)                     }	// 2020
OP(move_32_ai_pd)      { MOVE(32, AI,  PD)                     }	// 20a0
OP(move_32_pi_pd)      { MOVE(32, PI,  PD)                     }	// 20e0
OP(move_32_pd_pd)      { MOVE(32, PD,  PD)                     }	// 2120
OP(move_32_di_pd)      { MOVE(32, DI,  PD)                     }	// 2160
OP(move_32_ix_pd)      { MOVE(32, IX,  PD)                     }	// 21a0
OP(move_32_aw_pd)      { MOVE(32, AW,  PD)                     }	// 21e0
OP(move_32_al_pd)      { MOVE(32, AL,  PD)                     }	// 23e0

OP(move_32_d_di)       { MOVE(32, D,   DI)                     }	// 2028
OP(move_32_ai_di)      { MOVE(32, AI,  DI)                     }	// 20a8
OP(move_32_pi_di)      { MOVE(32, PI,  DI)                     }	// 20e8
OP(move_32_pd_di)      { MOVE(32, PD,  DI)                     }	// 2128
OP(move_32_di_di)      { MOVE(32, DI,  DI)                     }	// 2168
OP(move_32_ix_di)      { MOVE(32, IX,  DI)                     }	// 21a8
OP(move_32_aw_di)      { MOVE(32, AW,  DI)                     }	// 21e8
OP(move_32_al_di)      { MOVE(32, AL,  DI)                     }	// 23e8

OP(move_32_d_ix)       { MOVE(32, D,   IX)                     }	// 2030
OP(move_32_ai_ix)      { MOVE(32, AI,  IX)                     }	// 20b0
OP(move_32_pi_ix)      { MOVE(32, PI,  IX)                     }	// 20f0
OP(move_32_pd_ix)      { MOVE(32, PD,  IX)                     }	// 2130
OP(move_32_di_ix)      { MOVE(32, DI,  IX)                     }	// 2170
OP(move_32_ix_ix)      { MOVE(32, IX,  IX)                     }	// 21b0
OP(move_32_aw_ix)      { MOVE(32, AW,  IX)                     }	// 21f0
OP(move_32_al_ix)      { MOVE(32, AL,  IX)                     }	// 23f0

OP(move_32_d_aw)       { MOVE(32, D,   AW)                     }	// 2038
OP(move_32_ai_aw)      { MOVE(32, AI,  AW)                     }	// 20b8
OP(move_32_pi_aw)      { MOVE(32, PI,  AW)                     }	// 20f8
OP(move_32_pd_aw)      { MOVE(32, PD,  AW)                     }	// 2138
OP(move_32_di_aw)      { MOVE(32, DI,  AW)                     }	// 2178
OP(move_32_ix_aw)      { MOVE(32, IX,  AW)                     }	// 21b8
OP(move_32_aw_aw)      { MOVE(32, AW,  AW)                     }	// 21f8
OP(move_32_al_aw)      { MOVE(32, AL,  AW)                     }	// 23f8

OP(move_32_d_al)       { MOVE(32, D,   AL)                     }	// 2039
OP(move_32_ai_al)      { MOVE(32, AI,  AL)                     }	// 20b9
OP(move_32_pi_al)      { MOVE(32, PI,  AL)                     }	// 20f9
OP(move_32_pd_al)      { MOVE(32, PD,  AL)                     }	// 2139
OP(move_32_di_al)      { MOVE(32, DI,  AL)                     }	// 2179
OP(move_32_ix_al)      { MOVE(32, IX,  AL)                     }	// 21b9
OP(move_32_aw_al)      { MOVE(32, AW,  AL)                     }	// 21f9
OP(move_32_al_al)      { MOVE(32, AL,  AL)                     }	// 23f9

OP(move_32_d_pcdi)     { MOVE(32, D,   PCDI)                   }	// 203a
OP(move_32_ai_pcdi)    { MOVE(32, AI,  PCDI)                   }	// 20ba
OP(move_32_pi_pcdi)    { MOVE(32, PI,  PCDI)                   }	// 20fa
OP(move_32_pd_pcdi)    { MOVE(32, PD,  PCDI)                   }	// 213a
OP(move_32_di_pcdi)    { MOVE(32, DI,  PCDI)                   }	// 217a
OP(move_32_ix_pcdi)    { MOVE(32, IX,  PCDI)                   }	// 21ba
OP(move_32_aw_pcdi)    { MOVE(32, AW,  PCDI)                   }	// 21fa
OP(move_32_al_pcdi)    { MOVE(32, AL,  PCDI)                   }	// 23fa

OP(move_32_d_pcix)     { MOVE(32, D,   PCIX)                   }	// 203b
OP(move_32_ai_pcix)    { MOVE(32, AI,  PCIX)                   }	// 20bb
OP(move_32_pi_pcix)    { MOVE(32, PI,  PCIX)                   }	// 20fb
OP(move_32_pd_pcix)    { MOVE(32, PD,  PCIX)                   }	// 213b
OP(move_32_di_pcix)    { MOVE(32, DI,  PCIX)                   }	// 217b
OP(move_32_ix_pcix)    { MOVE(32, IX,  PCIX)                   }	// 21bb
OP(move_32_aw_pcix)    { MOVE(32, AW,  PCIX)                   }	// 21fb
OP(move_32_al_pcix)    { MOVE(32, AL,  PCIX)                   }	// 23fb

OP(move_32_d_i)        { MOVE(32, D,   I)                      }	// 203c
OP(move_32_ai_i)       { MOVE(32, AI,  I)                      }	// 20bc
OP(move_32_pi_i)       { MOVE(32, PI,  I)                      }	// 20fc
OP(move_32_pd_i)       { MOVE(32, PD,  I)                      }	// 213c
OP(move_32_di_i)       { MOVE(32, DI,  I)                      }	// 217c
OP(move_32_ix_i)       { MOVE(32, IX,  I)                      }	// 21bc
OP(move_32_aw_i)       { MOVE(32, AW,  I)                      }	// 21fc
OP(move_32_al_i)       { MOVE(32, AL,  I)                      }	// 23fc

/*-----------------------------------------------------------------------------
  MOVEA    Move Address
-----------------------------------------------------------------------------*/

OP(movea_32_d)         { MOVEA(32, D)                          }	// 2040
OP(movea_32_a)         { MOVEA(32, A)                          }	// 2048
OP(movea_32_ai)        { MOVEA(32, AI)                         }	// 2050
OP(movea_32_pi)        { MOVEA(32, PI)                         }	// 2058
OP(movea_32_pd)        { MOVEA(32, PD)                         }	// 2060
OP(movea_32_di)        { MOVEA(32, DI)                         }	// 2068
OP(movea_32_ix)        { MOVEA(32, IX)                         }	// 2070
OP(movea_32_aw)        { MOVEA(32, AW)                         }	// 2078
OP(movea_32_al)        { MOVEA(32, AL)                         }	// 2079
OP(movea_32_pcdi)      { MOVEA(32, PCDI)                       }	// 207a
OP(movea_32_pcix)      { MOVEA(32, PCIX)                       }	// 207b
OP(movea_32_i)         { MOVEA(32, I)                          }	// 207c

/******************************************************************************
	OPCODE $3xxx
******************************************************************************/

/*-----------------------------------------------------------------------------
  MOVE    Move Data from Source to Destination
-----------------------------------------------------------------------------*/

OP(move_16_d_d)        { MOVE(16, D,   D)                      }	// 3000
OP(move_16_ai_d)       { MOVE(16, AI,  D)                      }	// 3080
OP(move_16_pi_d)       { MOVE(16, PI,  D)                      }	// 30c0
OP(move_16_pd_d)       { MOVE(16, PD,  D)                      }	// 3100
OP(move_16_di_d)       { MOVE(16, DI,  D)                      }	// 3140
OP(move_16_ix_d)       { MOVE(16, IX,  D)                      }	// 3180
OP(move_16_aw_d)       { MOVE(16, AW,  D)                      }	// 31c0
OP(move_16_al_d)       { MOVE(16, AL,  D)                      }	// 33c0

OP(move_16_d_a)        { MOVE(16, D,   A)                      }	// 3008
OP(move_16_ai_a)       { MOVE(16, AI,  A)                      }	// 3088
OP(move_16_pi_a)       { MOVE(16, PI,  A)                      }	// 30c8
OP(move_16_pd_a)       { MOVE(16, PD,  A)                      }	// 3108
OP(move_16_di_a)       { MOVE(16, DI,  A)                      }	// 3148
OP(move_16_ix_a)       { MOVE(16, IX,  A)                      }	// 3188
OP(move_16_aw_a)       { MOVE(16, AW,  A)                      }	// 31c8
OP(move_16_al_a)       { MOVE(16, AL,  A)                      }	// 33c8

OP(move_16_d_ai)       { MOVE(16, D,   AI)                     }	// 3010
OP(move_16_ai_ai)      { MOVE(16, AI,  AI)                     }	// 3090
OP(move_16_pi_ai)      { MOVE(16, PI,  AI)                     }	// 30d0
OP(move_16_pd_ai)      { MOVE(16, PD,  AI)                     }	// 3110
OP(move_16_di_ai)      { MOVE(16, DI,  AI)                     }	// 3150
OP(move_16_ix_ai)      { MOVE(16, IX,  AI)                     }	// 3190
OP(move_16_aw_ai)      { MOVE(16, AW,  AI)                     }	// 31d0
OP(move_16_al_ai)      { MOVE(16, AL,  AI)                     }	// 33d0

OP(move_16_d_pi)       { MOVE(16, D,   PI)                     }	// 3018
OP(move_16_ai_pi)      { MOVE(16, AI,  PI)                     }	// 3098
OP(move_16_pi_pi)      { MOVE(16, PI,  PI)                     }	// 30d8
OP(move_16_pd_pi)      { MOVE(16, PD,  PI)                     }	// 3118
OP(move_16_di_pi)      { MOVE(16, DI,  PI)                     }	// 3158
OP(move_16_ix_pi)      { MOVE(16, IX,  PI)                     }	// 3198
OP(move_16_aw_pi)      { MOVE(16, AW,  PI)                     }	// 31d8
OP(move_16_al_pi)      { MOVE(16, AL,  PI)                     }	// 33d8

OP(move_16_d_pd)       { MOVE(16, D,   PD)                     }	// 3020
OP(move_16_ai_pd)      { MOVE(16, AI,  PD)                     }	// 30a0
OP(move_16_pi_pd)      { MOVE(16, PI,  PD)                     }	// 30e0
OP(move_16_pd_pd)      { MOVE(16, PD,  PD)                     }	// 3120
OP(move_16_di_pd)      { MOVE(16, DI,  PD)                     }	// 3160
OP(move_16_ix_pd)      { MOVE(16, IX,  PD)                     }	// 31a0
OP(move_16_aw_pd)      { MOVE(16, AW,  PD)                     }	// 31e0
OP(move_16_al_pd)      { MOVE(16, AL,  PD)                     }	// 33e0

OP(move_16_d_di)       { MOVE(16, D,   DI)                     }	// 3028
OP(move_16_ai_di)      { MOVE(16, AI,  DI)                     }	// 30a8
OP(move_16_pi_di)      { MOVE(16, PI,  DI)                     }	// 30e8
OP(move_16_pd_di)      { MOVE(16, PD,  DI)                     }	// 3128
OP(move_16_di_di)      { MOVE(16, DI,  DI)                     }	// 3168
OP(move_16_ix_di)      { MOVE(16, IX,  DI)                     }	// 31a8
OP(move_16_aw_di)      { MOVE(16, AW,  DI)                     }	// 31e8
OP(move_16_al_di)      { MOVE(16, AL,  DI)                     }	// 33e8

OP(move_16_d_ix)       { MOVE(16, D,   IX)                     }	// 3030
OP(move_16_ai_ix)      { MOVE(16, AI,  IX)                     }	// 30b0
OP(move_16_pi_ix)      { MOVE(16, PI,  IX)                     }	// 30f0
OP(move_16_pd_ix)      { MOVE(16, PD,  IX)                     }	// 3130
OP(move_16_di_ix)      { MOVE(16, DI,  IX)                     }	// 3170
OP(move_16_ix_ix)      { MOVE(16, IX,  IX)                     }	// 31b0
OP(move_16_aw_ix)      { MOVE(16, AW,  IX)                     }	// 31f0
OP(move_16_al_ix)      { MOVE(16, AL,  IX)                     }	// 33f0

OP(move_16_d_aw)       { MOVE(16, D,   AW)                     }	// 3038
OP(move_16_ai_aw)      { MOVE(16, AI,  AW)                     }	// 30b8
OP(move_16_pi_aw)      { MOVE(16, PI,  AW)                     }	// 30f8
OP(move_16_pd_aw)      { MOVE(16, PD,  AW)                     }	// 3138
OP(move_16_di_aw)      { MOVE(16, DI,  AW)                     }	// 3178
OP(move_16_ix_aw)      { MOVE(16, IX,  AW)                     }	// 31b8
OP(move_16_aw_aw)      { MOVE(16, AW,  AW)                     }	// 31f8
OP(move_16_al_aw)      { MOVE(16, AL,  AW)                     }	// 33f8

OP(move_16_d_al)       { MOVE(16, D,   AL)                     }	// 3039
OP(move_16_ai_al)      { MOVE(16, AI,  AL)                     }	// 30b9
OP(move_16_pi_al)      { MOVE(16, PI,  AL)                     }	// 30f9
OP(move_16_pd_al)      { MOVE(16, PD,  AL)                     }	// 3139
OP(move_16_di_al)      { MOVE(16, DI,  AL)                     }	// 3179
OP(move_16_ix_al)      { MOVE(16, IX,  AL)                     }	// 31b9
OP(move_16_aw_al)      { MOVE(16, AW,  AL)                     }	// 31f9
OP(move_16_al_al)      { MOVE(16, AL,  AL)                     }	// 33f9

OP(move_16_d_pcdi)     { MOVE(16, D,   PCDI)                   }	// 303a
OP(move_16_ai_pcdi)    { MOVE(16, AI,  PCDI)                   }	// 30ba
OP(move_16_pi_pcdi)    { MOVE(16, PI,  PCDI)                   }	// 30fa
OP(move_16_pd_pcdi)    { MOVE(16, PD,  PCDI)                   }	// 313a
OP(move_16_di_pcdi)    { MOVE(16, DI,  PCDI)                   }	// 317a
OP(move_16_ix_pcdi)    { MOVE(16, IX,  PCDI)                   }	// 31ba
OP(move_16_aw_pcdi)    { MOVE(16, AW,  PCDI)                   }	// 31fa
OP(move_16_al_pcdi)    { MOVE(16, AL,  PCDI)                   }	// 33fa

OP(move_16_d_pcix)     { MOVE(16, D,   PCIX)                   }	// 303b
OP(move_16_ai_pcix)    { MOVE(16, AI,  PCIX)                   }	// 30bb
OP(move_16_pi_pcix)    { MOVE(16, PI,  PCIX)                   }	// 30fb
OP(move_16_pd_pcix)    { MOVE(16, PD,  PCIX)                   }	// 313b
OP(move_16_di_pcix)    { MOVE(16, DI,  PCIX)                   }	// 317b
OP(move_16_ix_pcix)    { MOVE(16, IX,  PCIX)                   }	// 31bb
OP(move_16_aw_pcix)    { MOVE(16, AW,  PCIX)                   }	// 31fb
OP(move_16_al_pcix)    { MOVE(16, AL,  PCIX)                   }	// 33fb

OP(move_16_d_i)        { MOVE(16, D,   I)                      }	// 303c
OP(move_16_ai_i)       { MOVE(16, AI,  I)                      }	// 30bc
OP(move_16_pi_i)       { MOVE(16, PI,  I)                      }	// 30fc
OP(move_16_pd_i)       { MOVE(16, PD,  I)                      }	// 313c
OP(move_16_di_i)       { MOVE(16, DI,  I)                      }	// 317c
OP(move_16_ix_i)       { MOVE(16, IX,  I)                      }	// 31bc
OP(move_16_aw_i)       { MOVE(16, AW,  I)                      }	// 31fc
OP(move_16_al_i)       { MOVE(16, AL,  I)                      }	// 33fc

/*-----------------------------------------------------------------------------
  MOVEA    Move Address
-----------------------------------------------------------------------------*/

OP(movea_16_d)         { MOVEA(16, D)                          }	// 3040
OP(movea_16_a)         { MOVEA(16, A)                          }	// 3048
OP(movea_16_ai)        { MOVEA(16, AI)                         }	// 3050
OP(movea_16_pi)        { MOVEA(16, PI)                         }	// 3058
OP(movea_16_pd)        { MOVEA(16, PD)                         }	// 3060
OP(movea_16_di)        { MOVEA(16, DI)                         }	// 3068
OP(movea_16_ix)        { MOVEA(16, IX)                         }	// 3070
OP(movea_16_aw)        { MOVEA(16, AW)                         }	// 3078
OP(movea_16_al)        { MOVEA(16, AL)                         }	// 3079
OP(movea_16_pcdi)      { MOVEA(16, PCDI)                       }	// 307a
OP(movea_16_pcix)      { MOVEA(16, PCIX)                       }	// 307b
OP(movea_16_i)         { MOVEA(16, I)                          }	// 307c

/******************************************************************************
	OPCODE $4xxx
******************************************************************************/

/*-----------------------------------------------------------------------------
  NEGX    Negate with Extend
-----------------------------------------------------------------------------*/

OP(negx_8_d)           { NEGX(8, D, D)                         }	// 4000
OP(negx_8_ai)          { NEGX(8, M, AI)                        }	// 4010
OP(negx_8_pi)          { NEGX(8, M, PI)                        }	// 4018
OP(negx_8_pd)          { NEGX(8, M, PD)                        }	// 4020
OP(negx_8_di)          { NEGX(8, M, DI)                        }	// 4028
OP(negx_8_ix)          { NEGX(8, M, IX)                        }	// 4030
OP(negx_8_aw)          { NEGX(8, M, AW)                        }	// 4038
OP(negx_8_al)          { NEGX(8, M, AL)                        }	// 4039
OP(negx_8_pi7)         { NEGX(8, M, PI7)                       }	// 401f
OP(negx_8_pd7)         { NEGX(8, M, PD7)                       }	// 4027

OP(negx_16_d)          { NEGX(16, D, D)                        }	// 4040
OP(negx_16_ai)         { NEGX(16, M, AI)                       }	// 4050
OP(negx_16_pi)         { NEGX(16, M, PI)                       }	// 4058
OP(negx_16_pd)         { NEGX(16, M, PD)                       }	// 4060
OP(negx_16_di)         { NEGX(16, M, DI)                       }	// 4068
OP(negx_16_ix)         { NEGX(16, M, IX)                       }	// 4070
OP(negx_16_aw)         { NEGX(16, M, AW)                       }	// 4078
OP(negx_16_al)         { NEGX(16, M, AL)                       }	// 4079

OP(negx_32_d)          { NEGX(32, D, D)                        }	// 4080
OP(negx_32_ai)         { NEGX(32, M, AI)                       }	// 4090
OP(negx_32_pi)         { NEGX(32, M, PI)                       }	// 4098
OP(negx_32_pd)         { NEGX(32, M, PD)                       }	// 40a0
OP(negx_32_di)         { NEGX(32, M, DI)                       }	// 40a8
OP(negx_32_ix)         { NEGX(32, M, IX)                       }	// 40b0
OP(negx_32_aw)         { NEGX(32, M, AW)                       }	// 40b8
OP(negx_32_al)         { NEGX(32, M, AL)                       }	// 40b9

/*-----------------------------------------------------------------------------
  CLR    Clear an Operand
-----------------------------------------------------------------------------*/

OP(clr_8_d)            { CLR(8, D, D)                          }	// 4200
OP(clr_8_ai)           { CLR(8, M, AI)                         }	// 4210
OP(clr_8_pi)           { CLR(8, M, PI)                         }	// 4218
OP(clr_8_pd)           { CLR(8, M, PD)                         }	// 4220
OP(clr_8_di)           { CLR(8, M, DI)                         }	// 4228
OP(clr_8_ix)           { CLR(8, M, IX)                         }	// 4230
OP(clr_8_aw)           { CLR(8, M, AW)                         }	// 4238
OP(clr_8_al)           { CLR(8, M, AL)                         }	// 4239
OP(clr_8_pi7)          { CLR(8, M, PI7)                        }	// 421f
OP(clr_8_pd7)          { CLR(8, M, PD7)                        }	// 4227

OP(clr_16_d)           { CLR(16, D, D)                         }	// 4240
OP(clr_16_ai)          { CLR(16, M, AI)                        }	// 4250
OP(clr_16_pi)          { CLR(16, M, PI)                        }	// 4258
OP(clr_16_pd)          { CLR(16, M, PD)                        }	// 4260
OP(clr_16_di)          { CLR(16, M, DI)                        }	// 4268
OP(clr_16_ix)          { CLR(16, M, IX)                        }	// 4270
OP(clr_16_aw)          { CLR(16, M, AW)                        }	// 4278
OP(clr_16_al)          { CLR(16, M, AL)                        }	// 4279

OP(clr_32_d)           { CLR(32, D, D)                         }	// 4280
OP(clr_32_ai)          { CLR(32, M, AI)                        }	// 4290
OP(clr_32_pi)          { CLR(32, M, PI)                        }	// 4298
OP(clr_32_pd)          { CLR(32, M, PD)                        }	// 42a0
OP(clr_32_di)          { CLR(32, M, DI)                        }	// 42a8
OP(clr_32_ix)          { CLR(32, M, IX)                        }	// 42b0
OP(clr_32_aw)          { CLR(32, M, AW)                        }	// 42b8
OP(clr_32_al)          { CLR(32, M, AL)                        }	// 42b9

/*-----------------------------------------------------------------------------
  NEG    Negate
-----------------------------------------------------------------------------*/

OP(neg_8_d)            { NEG(8, D, D)                          }	// 4400
OP(neg_8_ai)           { NEG(8, M, AI)                         }	// 4410
OP(neg_8_pi)           { NEG(8, M, PI)                         }	// 4418
OP(neg_8_pd)           { NEG(8, M, PD)                         }	// 4420
OP(neg_8_di)           { NEG(8, M, DI)                         }	// 4428
OP(neg_8_ix)           { NEG(8, M, IX)                         }	// 4430
OP(neg_8_aw)           { NEG(8, M, AW)                         }	// 4438
OP(neg_8_al)           { NEG(8, M, AL)                         }	// 4439
OP(neg_8_pi7)          { NEG(8, M, PI7)                        }	// 441f
OP(neg_8_pd7)          { NEG(8, M, PD7)                        }	// 4427

OP(neg_16_d)           { NEG(16, D, D)                         }	// 4440
OP(neg_16_ai)          { NEG(16, M, AI)                        }	// 4450
OP(neg_16_pi)          { NEG(16, M, PI)                        }	// 4458
OP(neg_16_pd)          { NEG(16, M, PD)                        }	// 4460
OP(neg_16_di)          { NEG(16, M, DI)                        }	// 4468
OP(neg_16_ix)          { NEG(16, M, IX)                        }	// 4470
OP(neg_16_aw)          { NEG(16, M, AW)                        }	// 4478
OP(neg_16_al)          { NEG(16, M, AL)                        }	// 4479

OP(neg_32_d)           { NEG(32, D, D)                         }	// 4480
OP(neg_32_ai)          { NEG(32, M, AI)                        }	// 4490
OP(neg_32_pi)          { NEG(32, M, PI)                        }	// 4498
OP(neg_32_pd)          { NEG(32, M, PD)                        }	// 44a0
OP(neg_32_di)          { NEG(32, M, DI)                        }	// 44a8
OP(neg_32_ix)          { NEG(32, M, IX)                        }	// 44b0
OP(neg_32_aw)          { NEG(32, M, AW)                        }	// 44b8
OP(neg_32_al)          { NEG(32, M, AL)                        }	// 44b9

/*-----------------------------------------------------------------------------
  NOT    Logical Complement
-----------------------------------------------------------------------------*/

OP(not_8_d)            { NOT(8, D, D)                          }	// 4600
OP(not_8_ai)           { NOT(8, M, AI)                         }	// 4610
OP(not_8_pi)           { NOT(8, M, PI)                         }	// 4618
OP(not_8_pd)           { NOT(8, M, PD)                         }	// 4620
OP(not_8_di)           { NOT(8, M, DI)                         }	// 4628
OP(not_8_ix)           { NOT(8, M, IX)                         }	// 4630
OP(not_8_aw)           { NOT(8, M, AW)                         }	// 4638
OP(not_8_al)           { NOT(8, M, AL)                         }	// 4639
OP(not_8_pi7)          { NOT(8, M, PI7)                        }	// 461f
OP(not_8_pd7)          { NOT(8, M, PD7)                        }	// 4627

OP(not_16_d)           { NOT(16, D, D)                         }	// 4640
OP(not_16_ai)          { NOT(16, M, AI)                        }	// 4650
OP(not_16_pi)          { NOT(16, M, PI)                        }	// 4658
OP(not_16_pd)          { NOT(16, M, PD)                        }	// 4660
OP(not_16_di)          { NOT(16, M, DI)                        }	// 4668
OP(not_16_ix)          { NOT(16, M, IX)                        }	// 4670
OP(not_16_aw)          { NOT(16, M, AW)                        }	// 4678
OP(not_16_al)          { NOT(16, M, AL)                        }	// 4679

OP(not_32_d)           { NOT(32, D, D)                         }	// 4680
OP(not_32_ai)          { NOT(32, M, AI)                        }	// 4690
OP(not_32_pi)          { NOT(32, M, PI)                        }	// 4698
OP(not_32_pd)          { NOT(32, M, PD)                        }	// 46a0
OP(not_32_di)          { NOT(32, M, DI)                        }	// 46a8
OP(not_32_ix)          { NOT(32, M, IX)                        }	// 46b0
OP(not_32_aw)          { NOT(32, M, AW)                        }	// 46b8
OP(not_32_al)          { NOT(32, M, AL)                        }	// 46b9

/*-----------------------------------------------------------------------------
  MOVE from SR    Move from the Status Register
-----------------------------------------------------------------------------*/

OP(move_16_frs_d)      { MOVE_FRS(D, D)                        }	// 40c0
OP(move_16_frs_ai)     { MOVE_FRS(M, AI)                       }	// 40d0
OP(move_16_frs_pi)     { MOVE_FRS(M, PI)                       }	// 40d8
OP(move_16_frs_pd)     { MOVE_FRS(M, PD)                       }	// 40e0
OP(move_16_frs_di)     { MOVE_FRS(M, DI)                       }	// 40e8
OP(move_16_frs_ix)     { MOVE_FRS(M, IX)                       }	// 40f0
OP(move_16_frs_aw)     { MOVE_FRS(M, AW)                       }	// 40f8
OP(move_16_frs_al)     { MOVE_FRS(M, AL)                       }	// 40f9

/*-----------------------------------------------------------------------------
  MOVE to CCR    Move to Condition Code Register
-----------------------------------------------------------------------------*/

OP(move_16_toc_d)      { MOVE_TOC(D)                           }	// 44c0
OP(move_16_toc_ai)     { MOVE_TOC(AI)                          }	// 44d0
OP(move_16_toc_pi)     { MOVE_TOC(PI)                          }	// 44d8
OP(move_16_toc_pd)     { MOVE_TOC(PD)                          }	// 44e0
OP(move_16_toc_di)     { MOVE_TOC(DI)                          }	// 44e8
OP(move_16_toc_ix)     { MOVE_TOC(IX)                          }	// 44f0
OP(move_16_toc_aw)     { MOVE_TOC(AW)                          }	// 44f8
OP(move_16_toc_al)     { MOVE_TOC(AL)                          }	// 44f9
OP(move_16_toc_pcdi)   { MOVE_TOC(PCDI)                        }	// 44fa
OP(move_16_toc_pcix)   { MOVE_TOC(PCIX)                        }	// 44fb
OP(move_16_toc_i)      { MOVE_TOC(I)                           }	// 44fc

/*-----------------------------------------------------------------------------
  MOVE to SR    Move to the Status Register
-----------------------------------------------------------------------------*/

OP(move_16_tos_d)      { MOVE_TOS(D)                           }	// 46c0
OP(move_16_tos_ai)     { MOVE_TOS(AI)                          }	// 46d0
OP(move_16_tos_pi)     { MOVE_TOS(PI)                          }	// 46d8
OP(move_16_tos_pd)     { MOVE_TOS(PD)                          }	// 46e0
OP(move_16_tos_di)     { MOVE_TOS(DI)                          }	// 46e8
OP(move_16_tos_ix)     { MOVE_TOS(IX)                          }	// 46f0
OP(move_16_tos_aw)     { MOVE_TOS(AW)                          }	// 46f8
OP(move_16_tos_al)     { MOVE_TOS(AL)                          }	// 46f9
OP(move_16_tos_pcdi)   { MOVE_TOS(PCDI)                        }	// 46fa
OP(move_16_tos_pcix)   { MOVE_TOS(PCIX)                        }	// 46fb
OP(move_16_tos_i)      { MOVE_TOS(I)                           }	// 46fc

/*-----------------------------------------------------------------------------
  NBCD    Negate Decimal with Extend
-----------------------------------------------------------------------------*/

OP(nbcd_8_d)           { NBCD(D, D)                            }	// 4800
OP(nbcd_8_ai)          { NBCD(M, AI)                           }	// 4810
OP(nbcd_8_pi)          { NBCD(M, PI)                           }	// 4818
OP(nbcd_8_pd)          { NBCD(M, PD)                           }	// 4820
OP(nbcd_8_di)          { NBCD(M, DI)                           }	// 4828
OP(nbcd_8_ix)          { NBCD(M, IX)                           }	// 4830
OP(nbcd_8_aw)          { NBCD(M, AW)                           }	// 4838
OP(nbcd_8_al)          { NBCD(M, AL)                           }	// 4839
OP(nbcd_8_pi7)         { NBCD(M, PI7)                          }	// 481f
OP(nbcd_8_pd7)         { NBCD(M, PD7)                          }	// 4827

/*-----------------------------------------------------------------------------
  PEA    Push Effective Address
-----------------------------------------------------------------------------*/

OP(pea_32_ai)          { PEA(AI)                               }	// 4850
OP(pea_32_di)          { PEA(DI)                               }	// 4868
OP(pea_32_ix)          { PEA(IX)                               }	// 4870
OP(pea_32_aw)          { PEA(AW)                               }	// 4878
OP(pea_32_al)          { PEA(AL)                               }	// 4879
OP(pea_32_pcdi)        { PEA(PCDI)                             }	// 487a
OP(pea_32_pcix)        { PEA(PCIX)                             }	// 487b

/*-----------------------------------------------------------------------------
  SWAP    Swap Register Halves
-----------------------------------------------------------------------------*/

// 4840
OP(swap_32)
{
	EA_READ_D(32, Y, res);
	res = (res >> 16) | (res << 16);
	FLAGS(32)
	EA_WRITE_RESULT(32, D, Y)
	RET(4)
}

/*-----------------------------------------------------------------------------
  MOVEM    Move from Multiple Registers
-----------------------------------------------------------------------------*/

OP(movem_16_re_ai)     { MOVEM_RE(16, AI)                      }	// 4890
OP(movem_16_re_pd)     { MOVEM_RE_PD(16, Y)                    }	// 48a0
OP(movem_16_re_di)     { MOVEM_RE(16, DI)                      }	// 48a8
OP(movem_16_re_ix)     { MOVEM_RE(16, IX)                      }	// 48b0
OP(movem_16_re_aw)     { MOVEM_RE(16, AW)                      }	// 48b8
OP(movem_16_re_al)     { MOVEM_RE(16, AL)                      }	// 48b9

OP(movem_32_re_ai)     { MOVEM_RE(32, AI)                      }	// 48d0
OP(movem_32_re_pd)     { MOVEM_RE_PD(32, Y)                    }	// 48e0
OP(movem_32_re_di)     { MOVEM_RE(32, DI)                      }	// 48e8
OP(movem_32_re_ix)     { MOVEM_RE(32, IX)                      }	// 48f0
OP(movem_32_re_aw)     { MOVEM_RE(32, AW)                      }	// 48f8
OP(movem_32_re_al)     { MOVEM_RE(32, AL)                      }	// 48f9

/*-----------------------------------------------------------------------------
  EXT    Sign-Extend
-----------------------------------------------------------------------------*/

OP(ext_16)             { EXT(8, 16)                            }	// 4880
OP(ext_32)             { EXT(16, 32)                           }	// 48c0

/*-----------------------------------------------------------------------------
  TST    Test an Operand
-----------------------------------------------------------------------------*/

OP(tst_8_d)            { TST(8, D)                             }	// 4a00
OP(tst_8_ai)           { TST(8, AI)                            }	// 4a10
OP(tst_8_pi)           { TST(8, PI)                            }	// 4a18
OP(tst_8_pd)           { TST(8, PD)                            }	// 4a20
OP(tst_8_di)           { TST(8, DI)                            }	// 4a28
OP(tst_8_ix)           { TST(8, IX)                            }	// 4a30
OP(tst_8_aw)           { TST(8, AW)                            }	// 4a38
OP(tst_8_al)           { TST(8, AL)                            }	// 4a39
OP(tst_8_pi7)          { TST(8, PI7)                           }	// 4a1f
OP(tst_8_pd7)          { TST(8, PD7)                           }	// 4a27

OP(tst_16_d)           { TST(16, D)                            }	// 4a40
OP(tst_16_ai)          { TST(16, AI)                           }	// 4a50
OP(tst_16_pi)          { TST(16, PI)                           }	// 4a58
OP(tst_16_pd)          { TST(16, PD)                           }	// 4a60
OP(tst_16_di)          { TST(16, DI)                           }	// 4a68
OP(tst_16_ix)          { TST(16, IX)                           }	// 4a70
OP(tst_16_aw)          { TST(16, AW)                           }	// 4a78
OP(tst_16_al)          { TST(16, AL)                           }	// 4a79

OP(tst_32_d)           { TST(32, D)                            }	// 4a80
OP(tst_32_ai)          { TST(32, AI)                           }	// 4a90
OP(tst_32_pi)          { TST(32, PI)                           }	// 4a98
OP(tst_32_pd)          { TST(32, PD)                           }	// 4aa0
OP(tst_32_di)          { TST(32, DI)                           }	// 4aa8
OP(tst_32_ix)          { TST(32, IX)                           }	// 4ab0
OP(tst_32_aw)          { TST(32, AW)                           }	// 4ab8
OP(tst_32_al)          { TST(32, AL)                           }	// 4ab9

/*-----------------------------------------------------------------------------
  TAS    Test and Set an Operand
-----------------------------------------------------------------------------*/

OP(tas_8_d)            { TAS(D, D)                             }	// 4ac0
OP(tas_8_ai)           { TAS(M, AI)                            }	// 4ad0
OP(tas_8_pi)           { TAS(M, PI)                            }	// 4ad8
OP(tas_8_pd)           { TAS(M, PD)                            }	// 4ae0
OP(tas_8_di)           { TAS(M, DI)                            }	// 4ae8
OP(tas_8_ix)           { TAS(M, IX)                            }	// 4af0
OP(tas_8_aw)           { TAS(M, AW)                            }	// 4af8
OP(tas_8_al)           { TAS(M, AL)                            }	// 4af9
OP(tas_8_pi7)          { TAS(M, PI7)                           }	// 4adf
OP(tas_8_pd7)          { TAS(M, PD7)                           }	// 4ae7

/*-----------------------------------------------------------------------------
  ILLEGAL    Take Illegal Instruction Trap
-----------------------------------------------------------------------------*/

// 4afc
OP(illegal)
{
	SWAP_SP()
	EXCEPTION(C68K_ILLEGAL_INSTRUCTION_EX)
	RET(34)
}

/*-----------------------------------------------------------------------------
  MOVEM    Move to Multiple Registers
-----------------------------------------------------------------------------*/

OP(movem_16_er_ai)     { MOVEM_ER(16, AI)                      }	// 4c90
OP(movem_16_er_pi)     { MOVEM_ER_PI(16, Y)                    }	// 4c98
OP(movem_16_er_di)     { MOVEM_ER(16, DI)                      }	// 4ca8
OP(movem_16_er_ix)     { MOVEM_ER(16, IX)                      }	// 4cb0
OP(movem_16_er_aw)     { MOVEM_ER(16, AW)                      }	// 4cb8
OP(movem_16_er_al)     { MOVEM_ER(16, AL)                      }	// 4cb9
OP(movem_16_er_pcdi)   { MOVEM_ER(16, PCDI)                    }	// 4cba
OP(movem_16_er_pcix)   { MOVEM_ER(16, PCIX)                    }	// 4cbb

OP(movem_32_er_ai)     { MOVEM_ER(32, AI)                      }	// 4cd0
OP(movem_32_er_pi)     { MOVEM_ER_PI(32, Y)                    }	// 4cd8
OP(movem_32_er_di)     { MOVEM_ER(32, DI)                      }	// 4ce8
OP(movem_32_er_ix)     { MOVEM_ER(32, IX)                      }	// 4cf0
OP(movem_32_er_aw)     { MOVEM_ER(32, AW)                      }	// 4cf8
OP(movem_32_er_al)     { MOVEM_ER(32, AL)                      }	// 4cf9
OP(movem_32_er_pcdi)   { MOVEM_ER(32, PCDI)                    }	// 4cfa
OP(movem_32_er_pcix)   { MOVEM_ER(32, PCIX)                    }	// 4cfb

/*-----------------------------------------------------------------------------
  TRAP    Take Illegal Instruction Trap
-----------------------------------------------------------------------------*/

// 4e40
OP(trap)
{
	SWAP_SP()
	res = C68K_TRAP_BASE_EX + (Opcode & 0x0f);
	EXCEPTION(res)
	RET(34)
}

/*-----------------------------------------------------------------------------
  LINK    Link and Allocate
-----------------------------------------------------------------------------*/

// 4e50
OP(link_16)
{
	EA_AI(32, Y)
	PUSH_32_F(adr)
	AY = A7;
	A7 += READSX_I(16, NA);
	RET(16)
}

// 4e57
OP(link_16_a7)
{
	A7 -= 4;
	WRITE_MEM_32PD(A7, A7);
	A7 += READSX_I(16, NA);
	RET(16)
}

/*-----------------------------------------------------------------------------
  UNLK    Unlink
-----------------------------------------------------------------------------*/

// 4e58
OP(unlk_32)
{
	EA_AI(32, Y)
	A7 = adr + 4;
	AY = READ_MEM_32(adr);
	RET(12)
}

// 4e5f
OP(unlk_32_a7)
{
	A7 = READ_MEM_32(A7);
	RET(12)
}

/*-----------------------------------------------------------------------------
  MOVE USP    Move User Stack Pointer
-----------------------------------------------------------------------------*/

OP(move_32_tou)        { MOVEUSP(CPU->USP, AY)                 }	// 4e60
OP(move_32_fru)        { MOVEUSP(AY, CPU->USP)                 }	// 4e68

/*-----------------------------------------------------------------------------
  RESET    Reset External Devices
-----------------------------------------------------------------------------*/

// 4e70
OP(reset)
{
	if (FLAG_S)
	{
		CPU->Reset_CallBack();
		RET(132)
	}
	SWAP_SP_NOCHECK()
	EXCEPTION(C68K_PRIVILEGE_VIOLATION_EX)
	RET(40)
}

/*-----------------------------------------------------------------------------
  NOP    No Operation
-----------------------------------------------------------------------------*/

OP(nop)                { RET(4)                                }	// 4e71

/*-----------------------------------------------------------------------------
  STOP    Load Status Register and Stop
-----------------------------------------------------------------------------*/

// 4e72
OP(stop)
{
	if (FLAG_S)
	{
		EA_READ_I(16, NA, res)
		res &= C68K_SR_MASK;
		SET_SR(res)
		SWAP_SP()
		RET_HALT()
	}
	PC += 2;
	SWAP_SP_NOCHECK()
	EXCEPTION(C68K_PRIVILEGE_VIOLATION_EX)
	RET(40)
}

/*-----------------------------------------------------------------------------
  RTE    Return from Exception
-----------------------------------------------------------------------------*/

// 4e73
OP(rte_32)
{
	if (FLAG_S)
	{
		POP_16_F(res)
		SET_SR(res)
		POP_32_F(res)
		SET_PC(res)
		SWAP_SP()
		RET_INT(20)
	}
	SWAP_SP_NOCHECK()
	EXCEPTION(C68K_PRIVILEGE_VIOLATION_EX)
	RET(34)
}

/*-----------------------------------------------------------------------------
  RTS    Return from Subroutine
-----------------------------------------------------------------------------*/

// 4e75
OP(rts_32)
{
	POP_32_F(res)
	SET_PC(res)
	RET(16)
}

/*-----------------------------------------------------------------------------
  TRAPV    Trap on Overflow
-----------------------------------------------------------------------------*/

// 4e76
OP(trapv)
{
	if (COND_VS())
	{
		SWAP_SP()
		EXCEPTION(C68K_TRAPV_EX);
		RET(34)
	}
	RET(4)
}

/*-----------------------------------------------------------------------------
  RTR    Return and Restore Condition Codes
-----------------------------------------------------------------------------*/

// 4e77
OP(rtr_32)
{
	POP_16_F(res)
	SET_CCR(res)
	POP_32_F(res)
	SET_PC(res)
	RET(20)
}

/*-----------------------------------------------------------------------------
  JSR    Jump to Subroutine
-----------------------------------------------------------------------------*/

OP(jsr_32_ai)          { JSR(AI)                               }	// 4e90
OP(jsr_32_di)          { JSR(DI)                               }	// 4ea8
OP(jsr_32_ix)          { JSR(IX)                               }	// 4eb0
OP(jsr_32_aw)          { JSR(AW)                               }	// 4eb8
OP(jsr_32_al)          { JSR(AL)                               }	// 4eb9
OP(jsr_32_pcdi)        { JSR(PCDI)                             }	// 4eba
OP(jsr_32_pcix)        { JSR(PCIX)                             }	// 4ebb

/*-----------------------------------------------------------------------------
  JMP    Jump
-----------------------------------------------------------------------------*/

OP(jmp_32_ai)          { JMP(AI)                               }	// 4ed0
OP(jmp_32_di)          { JMP(DI)                               }	// 4ee8
OP(jmp_32_ix)          { JMP(IX)                               }	// 4ef0
OP(jmp_32_aw)          { JMP(AW)                               }	// 4ef8
OP(jmp_32_al)          { JMP(AL)                               }	// 4ef9
OP(jmp_32_pcdi)        { JMP(PCDI)                             }	// 4efa
OP(jmp_32_pcix)        { JMP(PCIX)                             }	// 4efb

/*-----------------------------------------------------------------------------
  CHK    Check Register Against Bounds
-----------------------------------------------------------------------------*/

OP(chk_16_d)           { CHK(D)                                }	// 4180
OP(chk_16_ai)          { CHK(AI)                               }	// 4190
OP(chk_16_pi)          { CHK(PI)                               }	// 4198
OP(chk_16_pd)          { CHK(PD)                               }	// 41a0
OP(chk_16_di)          { CHK(DI)                               }	// 41a8
OP(chk_16_ix)          { CHK(IX)                               }	// 41b0
OP(chk_16_aw)          { CHK(AW)                               }	// 41b8
OP(chk_16_al)          { CHK(AL)                               }	// 41b9
OP(chk_16_pcdi)        { CHK(PCDI)                             }	// 41ba
OP(chk_16_pcix)        { CHK(PCIX)                             }	// 41bb
OP(chk_16_i)           { CHK(I)                                }	// 41bc

/*-----------------------------------------------------------------------------
  LEA    Load Effective Address
-----------------------------------------------------------------------------*/

OP(lea_32_ai)          { LEA(AI)                               }	// 41d0
OP(lea_32_di)          { LEA(DI)                               }	// 41e8
OP(lea_32_ix)          { LEA(IX)                               }	// 41f0
OP(lea_32_aw)          { LEA(AW)                               }	// 41f8
OP(lea_32_al)          { LEA(AL)                               }	// 41f9
OP(lea_32_pcdi)        { LEA(PCDI)                             }	// 41fa
OP(lea_32_pcix)        { LEA(PCIX)                             }	// 41fb

/******************************************************************************
	OPCODE $5xxx
******************************************************************************/

/*-----------------------------------------------------------------------------
  Scc    Set According to Condition
-----------------------------------------------------------------------------*/

OP(st_8_d)             { ST_D()                                }	// 50c0
OP(sf_8_d)             { SF_D()                                }	// 51c0
OP(shi_8_d)            { Scc_D(HI)                             }	// 52c0
OP(sls_8_d)            { Scc_D(LS)                             }	// 53c0
OP(scc_8_d)            { Scc_D(CC)                             }	// 54c0
OP(scs_8_d)            { Scc_D(CS)                             }	// 55c0
OP(sne_8_d)            { Scc_D(NE)                             }	// 56c0
OP(seq_8_d)            { Scc_D(EQ)                             }	// 57c0
OP(svc_8_d)            { Scc_D(VC)                             }	// 58c0
OP(svs_8_d)            { Scc_D(VS)                             }	// 59c0
OP(spl_8_d)            { Scc_D(PL)                             }	// 5ac0
OP(smi_8_d)            { Scc_D(MI)                             }	// 5bc0
OP(sge_8_d)            { Scc_D(GE)                             }	// 5cc0
OP(slt_8_d)            { Scc_D(LT)                             }	// 5dc0
OP(sgt_8_d)            { Scc_D(GT)                             }	// 5ec0
OP(sle_8_d)            { Scc_D(LE)                             }	// 5fc0

OP(st_8_ai)            { ST(AI)                                }	// 50d0
OP(sf_8_ai)            { SF(AI)                                }	// 51d0
OP(shi_8_ai)           { Scc(AI, HI)                           }	// 52d0
OP(sls_8_ai)           { Scc(AI, LS)                           }	// 53d0
OP(scc_8_ai)           { Scc(AI, CC)                           }	// 54d0
OP(scs_8_ai)           { Scc(AI, CS)                           }	// 55d0
OP(sne_8_ai)           { Scc(AI, NE)                           }	// 56d0
OP(seq_8_ai)           { Scc(AI, EQ)                           }	// 57d0
OP(svc_8_ai)           { Scc(AI, VC)                           }	// 58d0
OP(svs_8_ai)           { Scc(AI, VS)                           }	// 59d0
OP(spl_8_ai)           { Scc(AI, PL)                           }	// 5ad0
OP(smi_8_ai)           { Scc(AI, MI)                           }	// 5bd0
OP(sge_8_ai)           { Scc(AI, GE)                           }	// 5cd0
OP(slt_8_ai)           { Scc(AI, LT)                           }	// 5dd0
OP(sgt_8_ai)           { Scc(AI, GT)                           }	// 5ed0
OP(sle_8_ai)           { Scc(AI, LE)                           }	// 5fd0

OP(st_8_pi)            { ST(PI)                                }	// 50d8
OP(sf_8_pi)            { SF(PI)                                }	// 51d8
OP(shi_8_pi)           { Scc(PI, HI)                           }	// 52d8
OP(sls_8_pi)           { Scc(PI, LS)                           }	// 53d8
OP(scc_8_pi)           { Scc(PI, CC)                           }	// 54d8
OP(scs_8_pi)           { Scc(PI, CS)                           }	// 55d8
OP(sne_8_pi)           { Scc(PI, NE)                           }	// 56d8
OP(seq_8_pi)           { Scc(PI, EQ)                           }	// 57d8
OP(svc_8_pi)           { Scc(PI, VC)                           }	// 58d8
OP(svs_8_pi)           { Scc(PI, VS)                           }	// 59d8
OP(spl_8_pi)           { Scc(PI, PL)                           }	// 5ad8
OP(smi_8_pi)           { Scc(PI, MI)                           }	// 5bd8
OP(sge_8_pi)           { Scc(PI, GE)                           }	// 5cd8
OP(slt_8_pi)           { Scc(PI, LT)                           }	// 5dd8
OP(sgt_8_pi)           { Scc(PI, GT)                           }	// 5ed8
OP(sle_8_pi)           { Scc(PI, LE)                           }	// 5fd8

OP(st_8_pd)            { ST(PD)                                }	// 50e0
OP(sf_8_pd)            { SF(PD)                                }	// 51e0
OP(shi_8_pd)           { Scc(PD, HI)                           }	// 52e0
OP(sls_8_pd)           { Scc(PD, LS)                           }	// 53e0
OP(scc_8_pd)           { Scc(PD, CC)                           }	// 54e0
OP(scs_8_pd)           { Scc(PD, CS)                           }	// 55e0
OP(sne_8_pd)           { Scc(PD, NE)                           }	// 56e0
OP(seq_8_pd)           { Scc(PD, EQ)                           }	// 57e0
OP(svc_8_pd)           { Scc(PD, VC)                           }	// 58e0
OP(svs_8_pd)           { Scc(PD, VS)                           }	// 59e0
OP(spl_8_pd)           { Scc(PD, PL)                           }	// 5ae0
OP(smi_8_pd)           { Scc(PD, MI)                           }	// 5be0
OP(sge_8_pd)           { Scc(PD, GE)                           }	// 5ce0
OP(slt_8_pd)           { Scc(PD, LT)                           }	// 5de0
OP(sgt_8_pd)           { Scc(PD, GT)                           }	// 5ee0
OP(sle_8_pd)           { Scc(PD, LE)                           }	// 5fe0

OP(st_8_di)            { ST(DI)                                }	// 50e8
OP(sf_8_di)            { SF(DI)                                }	// 51e8
OP(shi_8_di)           { Scc(DI, HI)                           }	// 52e8
OP(sls_8_di)           { Scc(DI, LS)                           }	// 53e8
OP(scc_8_di)           { Scc(DI, CC)                           }	// 54e8
OP(scs_8_di)           { Scc(DI, CS)                           }	// 55e8
OP(sne_8_di)           { Scc(DI, NE)                           }	// 56e8
OP(seq_8_di)           { Scc(DI, EQ)                           }	// 57e8
OP(svc_8_di)           { Scc(DI, VC)                           }	// 58e8
OP(svs_8_di)           { Scc(DI, VS)                           }	// 59e8
OP(spl_8_di)           { Scc(DI, PL)                           }	// 5ae8
OP(smi_8_di)           { Scc(DI, MI)                           }	// 5be8
OP(sge_8_di)           { Scc(DI, GE)                           }	// 5ce8
OP(slt_8_di)           { Scc(DI, LT)                           }	// 5de8
OP(sgt_8_di)           { Scc(DI, GT)                           }	// 5ee8
OP(sle_8_di)           { Scc(DI, LE)                           }	// 5fe8

OP(st_8_ix)            { ST(IX)                                }	// 50f0
OP(sf_8_ix)            { SF(IX)                                }	// 51f0
OP(shi_8_ix)           { Scc(IX, HI)                           }	// 52f0
OP(sls_8_ix)           { Scc(IX, LS)                           }	// 53f0
OP(scc_8_ix)           { Scc(IX, CC)                           }	// 54f0
OP(scs_8_ix)           { Scc(IX, CS)                           }	// 55f0
OP(sne_8_ix)           { Scc(IX, NE)                           }	// 56f0
OP(seq_8_ix)           { Scc(IX, EQ)                           }	// 57f0
OP(svc_8_ix)           { Scc(IX, VC)                           }	// 58f0
OP(svs_8_ix)           { Scc(IX, VS)                           }	// 59f0
OP(spl_8_ix)           { Scc(IX, PL)                           }	// 5af0
OP(smi_8_ix)           { Scc(IX, MI)                           }	// 5bf0
OP(sge_8_ix)           { Scc(IX, GE)                           }	// 5cf0
OP(slt_8_ix)           { Scc(IX, LT)                           }	// 5df0
OP(sgt_8_ix)           { Scc(IX, GT)                           }	// 5ef0
OP(sle_8_ix)           { Scc(IX, LE)                           }	// 5ff0

OP(st_8_aw)            { ST(AW)                                }	// 50f8
OP(sf_8_aw)            { SF(AW)                                }	// 51f8
OP(shi_8_aw)           { Scc(AW, HI)                           }	// 52f8
OP(sls_8_aw)           { Scc(AW, LS)                           }	// 53f8
OP(scc_8_aw)           { Scc(AW, CC)                           }	// 54f8
OP(scs_8_aw)           { Scc(AW, CS)                           }	// 55f8
OP(sne_8_aw)           { Scc(AW, NE)                           }	// 56f8
OP(seq_8_aw)           { Scc(AW, EQ)                           }	// 57f8
OP(svc_8_aw)           { Scc(AW, VC)                           }	// 58f8
OP(svs_8_aw)           { Scc(AW, VS)                           }	// 59f8
OP(spl_8_aw)           { Scc(AW, PL)                           }	// 5af8
OP(smi_8_aw)           { Scc(AW, MI)                           }	// 5bf8
OP(sge_8_aw)           { Scc(AW, GE)                           }	// 5cf8
OP(slt_8_aw)           { Scc(AW, LT)                           }	// 5df8
OP(sgt_8_aw)           { Scc(AW, GT)                           }	// 5ef8
OP(sle_8_aw)           { Scc(AW, LE)                           }	// 5ff8

OP(st_8_al)            { ST(AL)                                }	// 50f9
OP(sf_8_al)            { SF(AL)                                }	// 51f9
OP(shi_8_al)           { Scc(AL, HI)                           }	// 52f9
OP(sls_8_al)           { Scc(AL, LS)                           }	// 53f9
OP(scc_8_al)           { Scc(AL, CC)                           }	// 54f9
OP(scs_8_al)           { Scc(AL, CS)                           }	// 55f9
OP(sne_8_al)           { Scc(AL, NE)                           }	// 56f9
OP(seq_8_al)           { Scc(AL, EQ)                           }	// 57f9
OP(svc_8_al)           { Scc(AL, VC)                           }	// 58f9
OP(svs_8_al)           { Scc(AL, VS)                           }	// 59f9
OP(spl_8_al)           { Scc(AL, PL)                           }	// 5af9
OP(smi_8_al)           { Scc(AL, MI)                           }	// 5bf9
OP(sge_8_al)           { Scc(AL, GE)                           }	// 5cf9
OP(slt_8_al)           { Scc(AL, LT)                           }	// 5df9
OP(sgt_8_al)           { Scc(AL, GT)                           }	// 5ef9
OP(sle_8_al)           { Scc(AL, LE)                           }	// 5ff9

OP(st_8_pi7)           { ST(PI7)                               }	// 50df
OP(sf_8_pi7)           { SF(PI7)                               }	// 51df
OP(shi_8_pi7)          { Scc(PI7, HI)                          }	// 52df
OP(sls_8_pi7)          { Scc(PI7, LS)                          }	// 53df
OP(scc_8_pi7)          { Scc(PI7, CC)                          }	// 54df
OP(scs_8_pi7)          { Scc(PI7, CS)                          }	// 55df
OP(sne_8_pi7)          { Scc(PI7, NE)                          }	// 56df
OP(seq_8_pi7)          { Scc(PI7, EQ)                          }	// 57df
OP(svc_8_pi7)          { Scc(PI7, VC)                          }	// 58df
OP(svs_8_pi7)          { Scc(PI7, VS)                          }	// 59df
OP(spl_8_pi7)          { Scc(PI7, PL)                          }	// 5adf
OP(smi_8_pi7)          { Scc(PI7, MI)                          }	// 5bdf
OP(sge_8_pi7)          { Scc(PI7, GE)                          }	// 5cdf
OP(slt_8_pi7)          { Scc(PI7, LT)                          }	// 5ddf
OP(sgt_8_pi7)          { Scc(PI7, GT)                          }	// 5edf
OP(sle_8_pi7)          { Scc(PI7, LE)                          }	// 5fdf

OP(st_8_pd7)           { ST(PD7)                               }	// 50e7
OP(sf_8_pd7)           { SF(PD7)                               }	// 51e7
OP(shi_8_pd7)          { Scc(PD7, HI)                          }	// 52e7
OP(sls_8_pd7)          { Scc(PD7, LS)                          }	// 53e7
OP(scc_8_pd7)          { Scc(PD7, CC)                          }	// 54e7
OP(scs_8_pd7)          { Scc(PD7, CS)                          }	// 55e7
OP(sne_8_pd7)          { Scc(PD7, NE)                          }	// 56e7
OP(seq_8_pd7)          { Scc(PD7, EQ)                          }	// 57e7
OP(svc_8_pd7)          { Scc(PD7, VC)                          }	// 58e7
OP(svs_8_pd7)          { Scc(PD7, VS)                          }	// 59e7
OP(spl_8_pd7)          { Scc(PD7, PL)                          }	// 5ae7
OP(smi_8_pd7)          { Scc(PD7, MI)                          }	// 5be7
OP(sge_8_pd7)          { Scc(PD7, GE)                          }	// 5ce7
OP(slt_8_pd7)          { Scc(PD7, LT)                          }	// 5de7
OP(sgt_8_pd7)          { Scc(PD7, GT)                          }	// 5ee7
OP(sle_8_pd7)          { Scc(PD7, LE)                          }	// 5fe7

/*-----------------------------------------------------------------------------
  DBcc    Test Condition, Decrement, and Branch
-----------------------------------------------------------------------------*/

OP(dbt_16)             { DBT()                                 }	// 50c8
OP(dbf_16)             { DBF()                                 }	// 51c8
OP(dbhi_16)            { DBcc(HI)                              }	// 52c8
OP(dbls_16)            { DBcc(LS)                              }	// 53c8
OP(dbcc_16)            { DBcc(CC)                              }	// 54c8
OP(dbcs_16)            { DBcc(CS)                              }	// 55c8
OP(dbne_16)            { DBcc(NE)                              }	// 56c8
OP(dbeq_16)            { DBcc(EQ)                              }	// 57c8
OP(dbvc_16)            { DBcc(VC)                              }	// 58c8
OP(dbvs_16)            { DBcc(VS)                              }	// 59c8
OP(dbpl_16)            { DBcc(PL)                              }	// 5ac8
OP(dbmi_16)            { DBcc(MI)                              }	// 5bc8
OP(dbge_16)            { DBcc(GE)                              }	// 5cc8
OP(dblt_16)            { DBcc(LT)                              }	// 5dc8
OP(dbgt_16)            { DBcc(GT)                              }	// 5ec8
OP(dble_16)            { DBcc(LE)                              }	// 5fc8

/*-----------------------------------------------------------------------------
  ADDQ    Add Quick
-----------------------------------------------------------------------------*/

OP(addq_8_d)           { ADDQ(8, D, D)                         }	// 5000
OP(addq_8_ai)          { ADDQ(8, M, AI)                        }	// 5010
OP(addq_8_pi)          { ADDQ(8, M, PI)                        }	// 5018
OP(addq_8_pd)          { ADDQ(8, M, PD)                        }	// 5020
OP(addq_8_di)          { ADDQ(8, M, DI)                        }	// 5028
OP(addq_8_ix)          { ADDQ(8, M, IX)                        }	// 5030
OP(addq_8_aw)          { ADDQ(8, M, AW)                        }	// 5038
OP(addq_8_al)          { ADDQ(8, M, AL)                        }	// 5039
OP(addq_8_pi7)         { ADDQ(8, M, PI7)                       }	// 501f
OP(addq_8_pd7)         { ADDQ(8, M, PD7)                       }	// 5027

OP(addq_16_d)          { ADDQ(16, D, D)                        }	// 5040
OP(addq_16_a)          { ADDQ_A(16)                            }	// 5048
OP(addq_16_ai)         { ADDQ(16, M, AI)                       }	// 5050
OP(addq_16_pi)         { ADDQ(16, M, PI)                       }	// 5058
OP(addq_16_pd)         { ADDQ(16, M, PD)                       }	// 5060
OP(addq_16_di)         { ADDQ(16, M, DI)                       }	// 5068
OP(addq_16_ix)         { ADDQ(16, M, IX)                       }	// 5070
OP(addq_16_aw)         { ADDQ(16, M, AW)                       }	// 5078
OP(addq_16_al)         { ADDQ(16, M, AL)                       }	// 5079

OP(addq_32_d)          { ADDQ(32, D, D)                        }	// 5080
OP(addq_32_a)          { ADDQ_A(32)                            }	// 5088
OP(addq_32_ai)         { ADDQ(32, M, AI)                       }	// 5090
OP(addq_32_pi)         { ADDQ(32, M, PI)                       }	// 5098
OP(addq_32_pd)         { ADDQ(32, M, PD)                       }	// 50a0
OP(addq_32_di)         { ADDQ(32, M, DI)                       }	// 50a8
OP(addq_32_ix)         { ADDQ(32, M, IX)                       }	// 50b0
OP(addq_32_aw)         { ADDQ(32, M, AW)                       }	// 50b8
OP(addq_32_al)         { ADDQ(32, M, AL)                       }	// 50b9

/*-----------------------------------------------------------------------------
  SUBQ    Subtract Quick
-----------------------------------------------------------------------------*/

OP(subq_8_d)           { SUBQ(8, D, D)                         }	// 5100
OP(subq_8_ai)          { SUBQ(8, M, AI)                        }	// 5110
OP(subq_8_pi)          { SUBQ(8, M, PI)                        }	// 5118
OP(subq_8_pd)          { SUBQ(8, M, PD)                        }	// 5120
OP(subq_8_di)          { SUBQ(8, M, DI)                        }	// 5128
OP(subq_8_ix)          { SUBQ(8, M, IX)                        }	// 5130
OP(subq_8_aw)          { SUBQ(8, M, AW)                        }	// 5138
OP(subq_8_al)          { SUBQ(8, M, AL)                        }	// 5139
OP(subq_8_pi7)         { SUBQ(8, M, PI7)                       }	// 511f
OP(subq_8_pd7)         { SUBQ(8, M, PD7)                       }	// 5127

OP(subq_16_d)          { SUBQ(16, D, D)                        }	// 5140
OP(subq_16_a)          { SUBQ_A(16)                            }	// 5148
OP(subq_16_ai)         { SUBQ(16, M, AI)                       }	// 5150
OP(subq_16_pi)         { SUBQ(16, M, PI)                       }	// 5158
OP(subq_16_pd)         { SUBQ(16, M, PD)                       }	// 5160
OP(subq_16_di)         { SUBQ(16, M, DI)                       }	// 5168
OP(subq_16_ix)         { SUBQ(16, M, IX)                       }	// 5170
OP(subq_16_aw)         { SUBQ(16, M, AW)                       }	// 5178
OP(subq_16_al)         { SUBQ(16, M, AL)                       }	// 5179

OP(subq_32_d)          { SUBQ(32, D, D)                        }	// 5180
OP(subq_32_a)          { SUBQ_A(32)                            }	// 5188
OP(subq_32_ai)         { SUBQ(32, M, AI)                       }	// 5190
OP(subq_32_pi)         { SUBQ(32, M, PI)                       }	// 5198
OP(subq_32_pd)         { SUBQ(32, M, PD)                       }	// 51a0
OP(subq_32_di)         { SUBQ(32, M, DI)                       }	// 51a8
OP(subq_32_ix)         { SUBQ(32, M, IX)                       }	// 51b0
OP(subq_32_aw)         { SUBQ(32, M, AW)                       }	// 51b8
OP(subq_32_al)         { SUBQ(32, M, AL)                       }	// 51b9

/******************************************************************************
	OPCODE $6xxx
******************************************************************************/

/*-----------------------------------------------------------------------------
  Bcc    Branch Conditionally
-----------------------------------------------------------------------------*/

OP(bhi_8)              { Bcc_8(HI)                             }	// 6201
OP(bls_8)              { Bcc_8(LS)                             }	// 6301
OP(bcc_8)              { Bcc_8(CC)                             }	// 6401
OP(bcs_8)              { Bcc_8(CS)                             }	// 6501
OP(bne_8)              { Bcc_8(NE)                             }	// 6601
OP(beq_8)              { Bcc_8(EQ)                             }	// 6701
OP(bvc_8)              { Bcc_8(VC)                             }	// 6801
OP(bvs_8)              { Bcc_8(VS)                             }	// 6901
OP(bpl_8)              { Bcc_8(PL)                             }	// 6a01
OP(bmi_8)              { Bcc_8(MI)                             }	// 6b01
OP(bge_8)              { Bcc_8(GE)                             }	// 6c01
OP(blt_8)              { Bcc_8(LT)                             }	// 6d01
OP(bgt_8)              { Bcc_8(GT)                             }	// 6e01
OP(ble_8)              { Bcc_8(LE)                             }	// 6f01

OP(bhi_16)             { Bcc_16(HI)                            }	// 6200
OP(bls_16)             { Bcc_16(LS)                            }	// 6300
OP(bcc_16)             { Bcc_16(CC)                            }	// 6400
OP(bcs_16)             { Bcc_16(CS)                            }	// 6500
OP(bne_16)             { Bcc_16(NE)                            }	// 6600
OP(beq_16)             { Bcc_16(EQ)                            }	// 6700
OP(bvc_16)             { Bcc_16(VC)                            }	// 6800
OP(bvs_16)             { Bcc_16(VS)                            }	// 6900
OP(bpl_16)             { Bcc_16(PL)                            }	// 6a00
OP(bmi_16)             { Bcc_16(MI)                            }	// 6b00
OP(bge_16)             { Bcc_16(GE)                            }	// 6c00
OP(blt_16)             { Bcc_16(LT)                            }	// 6d00
OP(bgt_16)             { Bcc_16(GT)                            }	// 6e00
OP(ble_16)             { Bcc_16(LE)                            }	// 6f00

/*-----------------------------------------------------------------------------
  BRA    Branch Always
-----------------------------------------------------------------------------*/

// 6001
OP(bra_8)
{
	PC += (INT32)(INT8)Opcode;
	RET(10)
}

// 6000
OP(bra_16)
{
	PC += READSX_IMM_16();
	ADJUST_PC()
	SET_PC(PC)
	RET(10)
}

/*-----------------------------------------------------------------------------
  BSR    Branch to Subroutine
-----------------------------------------------------------------------------*/

// 6101
OP(bsr_8)
{
	res = GET_PC();
	PUSH_32_F(res)
	PC += MAKE_INT_8(Opcode);
	RET(18)
}

// 6100
OP(bsr_16)
{
	res = READSX_IMM_16();
	ADJUST_PC()
	PUSH_32_F(PC + 2)
	PC += res;
	SET_PC(PC)
	RET(18)
}

/******************************************************************************
	OPCODE $7xxx
******************************************************************************/

/*-----------------------------------------------------------------------------
  MOVEQ    Move Quick
-----------------------------------------------------------------------------*/

// 7000
OP(moveq_32)
{
	res = MAKE_INT_8(Opcode);
	FLAG_C = CFLAG_CLEAR;
	FLAG_V = VFLAG_CLEAR;
	FLAG_N = res;
	FLAG_Z = res;
	EA_WRITE_RESULT(32, D, X)
	RET(4)
}

/******************************************************************************
	OPCODE $8xxx
******************************************************************************/

/*-----------------------------------------------------------------------------
  OR    inclusive-OR logical
-----------------------------------------------------------------------------*/

OP(or_8_er_d)          { OR_ER(8, D)                           }	// 8000
OP(or_8_er_ai)         { OR_ER(8, AI)                          }	// 8010
OP(or_8_er_pi)         { OR_ER(8, PI)                          }	// 8018
OP(or_8_er_pd)         { OR_ER(8, PD)                          }	// 8020
OP(or_8_er_di)         { OR_ER(8, DI)                          }	// 8028
OP(or_8_er_ix)         { OR_ER(8, IX)                          }	// 8030
OP(or_8_er_aw)         { OR_ER(8, AW)                          }	// 8038
OP(or_8_er_al)         { OR_ER(8, AL)                          }	// 8039
OP(or_8_er_pcdi)       { OR_ER(8, PCDI)                        }	// 803a
OP(or_8_er_pcix)       { OR_ER(8, PCIX)                        }	// 803b
OP(or_8_er_i)          { OR_ER(8, I)                           }	// 803c
OP(or_8_er_pi7)        { OR_ER(8, PI7)                         }	// 801f
OP(or_8_er_pd7)        { OR_ER(8, PD7)                         }	// 8027

OP(or_16_er_d)         { OR_ER(16, D)                          }	// 8040
OP(or_16_er_ai)        { OR_ER(16, AI)                         }	// 8050
OP(or_16_er_pi)        { OR_ER(16, PI)                         }	// 8058
OP(or_16_er_pd)        { OR_ER(16, PD)                         }	// 8060
OP(or_16_er_di)        { OR_ER(16, DI)                         }	// 8068
OP(or_16_er_ix)        { OR_ER(16, IX)                         }	// 8070
OP(or_16_er_aw)        { OR_ER(16, AW)                         }	// 8078
OP(or_16_er_al)        { OR_ER(16, AL)                         }	// 8079
OP(or_16_er_pcdi)      { OR_ER(16, PCDI)                       }	// 807a
OP(or_16_er_pcix)      { OR_ER(16, PCIX)                       }	// 807b
OP(or_16_er_i)         { OR_ER(16, I)                          }	// 807c

OP(or_32_er_d)         { OR_ER_RI32(D)                         }	// 8080
OP(or_32_er_ai)        { OR_ER_RI32(AI)                        }	// 8090
OP(or_32_er_pi)        { OR_ER(32, PI)                         }	// 8098
OP(or_32_er_pd)        { OR_ER(32, PD)                         }	// 80a0
OP(or_32_er_di)        { OR_ER(32, DI)                         }	// 80a8
OP(or_32_er_ix)        { OR_ER(32, IX)                         }	// 80b0
OP(or_32_er_aw)        { OR_ER(32, AW)                         }	// 80b8
OP(or_32_er_al)        { OR_ER(32, AL)                         }	// 80b9
OP(or_32_er_pcdi)      { OR_ER(32, PCDI)                       }	// 80ba
OP(or_32_er_pcix)      { OR_ER(32, PCIX)                       }	// 80bb
OP(or_32_er_i)         { OR_ER_RI32(I)                         }	// 80bc

OP(or_8_re_ai)         { OR_RE(8, AI)                          }	// 8110
OP(or_8_re_pi)         { OR_RE(8, PI)                          }	// 8118
OP(or_8_re_pd)         { OR_RE(8, PD)                          }	// 8120
OP(or_8_re_di)         { OR_RE(8, DI)                          }	// 8128
OP(or_8_re_ix)         { OR_RE(8, IX)                          }	// 8130
OP(or_8_re_aw)         { OR_RE(8, AW)                          }	// 8138
OP(or_8_re_al)         { OR_RE(8, AL)                          }	// 8139
OP(or_8_re_pi7)        { OR_RE(8, PI7)                         }	// 811f
OP(or_8_re_pd7)        { OR_RE(8, PD7)                         }	// 8127

OP(or_16_re_ai)        { OR_RE(16, AI)                         }	// 8150
OP(or_16_re_pi)        { OR_RE(16, PI)                         }	// 8158
OP(or_16_re_pd)        { OR_RE(16, PD)                         }	// 8160
OP(or_16_re_di)        { OR_RE(16, DI)                         }	// 8168
OP(or_16_re_ix)        { OR_RE(16, IX)                         }	// 8170
OP(or_16_re_aw)        { OR_RE(16, AW)                         }	// 8178
OP(or_16_re_al)        { OR_RE(16, AL)                         }	// 8179

OP(or_32_re_ai)        { OR_RE(32, AI)                         }	// 8190
OP(or_32_re_pi)        { OR_RE(32, PI)                         }	// 8198
OP(or_32_re_pd)        { OR_RE(32, PD)                         }	// 81a0
OP(or_32_re_di)        { OR_RE(32, DI)                         }	// 81a8
OP(or_32_re_ix)        { OR_RE(32, IX)                         }	// 81b0
OP(or_32_re_aw)        { OR_RE(32, AW)                         }	// 81b8
OP(or_32_re_al)        { OR_RE(32, AL)                         }	// 81b9

/*-----------------------------------------------------------------------------
  SBCD    Subtract Decimal with Extend
-----------------------------------------------------------------------------*/

OP(sbcd_8_rr)          { SBCD(D, D,   D)                       }	// 8100
OP(sbcd_8_mm)          { SBCD(M, PD,  PD)                      }	// 8108
OP(sbcd_8_mm_ay7)      { SBCD(M, PD,  PD7)                     }	// 810f
OP(sbcd_8_mm_ax7)      { SBCD(M, PD7, PD)                      }	// 8f08
OP(sbcd_8_mm_axy7)     { SBCD(M, PD7, PD7)                     }	// 8f0f

/*-----------------------------------------------------------------------------
  DIVU    Unsigned Divide
-----------------------------------------------------------------------------*/

OP(divu_16_d)          { DIVU(D)                               }	// 80c0
OP(divu_16_ai)         { DIVU(AI)                              }	// 80d0
OP(divu_16_pi)         { DIVU(PI)                              }	// 80d8
OP(divu_16_pd)         { DIVU(PD)                              }	// 80e0
OP(divu_16_di)         { DIVU(DI)                              }	// 80e8
OP(divu_16_ix)         { DIVU(IX)                              }	// 80f0
OP(divu_16_aw)         { DIVU(AW)                              }	// 80f8
OP(divu_16_al)         { DIVU(AL)                              }	// 80f9
OP(divu_16_pcdi)       { DIVU(PCDI)                            }	// 80fa
OP(divu_16_pcix)       { DIVU(PCIX)                            }	// 80fb
OP(divu_16_i)          { DIVU(I)                               }	// 80fc

/*-----------------------------------------------------------------------------
  DIVS    Signed Divide
-----------------------------------------------------------------------------*/

OP(divs_16_d)          { DIVS(D)                               }	// 81c0
OP(divs_16_ai)         { DIVS(AI)                              }	// 81d0
OP(divs_16_pi)         { DIVS(PI)                              }	// 81d8
OP(divs_16_pd)         { DIVS(PD)                              }	// 81e0
OP(divs_16_di)         { DIVS(DI)                              }	// 81e8
OP(divs_16_ix)         { DIVS(IX)                              }	// 81f0
OP(divs_16_aw)         { DIVS(AW)                              }	// 81f8
OP(divs_16_al)         { DIVS(AL)                              }	// 81f9
OP(divs_16_pcdi)       { DIVS(PCDI)                            }	// 81fa
OP(divs_16_pcix)       { DIVS(PCIX)                            }	// 81fb
OP(divs_16_i)          { DIVS(I)                               }	// 81fc

/******************************************************************************
	OPCODE $9xxx
******************************************************************************/

/*-----------------------------------------------------------------------------
  SUB    Subtract
-----------------------------------------------------------------------------*/

OP(sub_8_er_d)         { SUB_ER(8, D)                          }	// 9000
OP(sub_8_er_ai)        { SUB_ER(8, AI)                         }	// 9010
OP(sub_8_er_pi)        { SUB_ER(8, PI)                         }	// 9018
OP(sub_8_er_pd)        { SUB_ER(8, PD)                         }	// 9020
OP(sub_8_er_di)        { SUB_ER(8, DI)                         }	// 9028
OP(sub_8_er_ix)        { SUB_ER(8, IX)                         }	// 9030
OP(sub_8_er_aw)        { SUB_ER(8, AW)                         }	// 9038
OP(sub_8_er_al)        { SUB_ER(8, AL)                         }	// 9039
OP(sub_8_er_pcdi)      { SUB_ER(8, PCDI)                       }	// 903a
OP(sub_8_er_pcix)      { SUB_ER(8, PCIX)                       }	// 903b
OP(sub_8_er_i)         { SUB_ER(8, I)                          }	// 903c
OP(sub_8_er_pi7)       { SUB_ER(8, PI7)                        }	// 901f
OP(sub_8_er_pd7)       { SUB_ER(8, PD7)                        }	// 9027

OP(sub_16_er_d)        { SUB_ER(16, D)                         }	// 9040
OP(sub_16_er_a)        { SUB_ER(16, A)                         }	// 9048
OP(sub_16_er_ai)       { SUB_ER(16, AI)                        }	// 9050
OP(sub_16_er_pi)       { SUB_ER(16, PI)                        }	// 9058
OP(sub_16_er_pd)       { SUB_ER(16, PD)                        }	// 9060
OP(sub_16_er_di)       { SUB_ER(16, DI)                        }	// 9068
OP(sub_16_er_ix)       { SUB_ER(16, IX)                        }	// 9070
OP(sub_16_er_aw)       { SUB_ER(16, AW)                        }	// 9078
OP(sub_16_er_al)       { SUB_ER(16, AL)                        }	// 9079
OP(sub_16_er_pcdi)     { SUB_ER(16, PCDI)                      }	// 907a
OP(sub_16_er_pcix)     { SUB_ER(16, PCIX)                      }	// 907b
OP(sub_16_er_i)        { SUB_ER(16, I)                         }	// 907c

OP(sub_32_er_d)        { SUB_ER_RI32(D)                        }	// 9080
OP(sub_32_er_a)        { SUB_ER_RI32(A)                        }	// 9088
OP(sub_32_er_ai)       { SUB_ER(32, AI)                        }	// 9090
OP(sub_32_er_pi)       { SUB_ER(32, PI)                        }	// 9098
OP(sub_32_er_pd)       { SUB_ER(32, PD)                        }	// 90a0
OP(sub_32_er_di)       { SUB_ER(32, DI)                        }	// 90a8
OP(sub_32_er_ix)       { SUB_ER(32, IX)                        }	// 90b0
OP(sub_32_er_aw)       { SUB_ER(32, AW)                        }	// 90b8
OP(sub_32_er_al)       { SUB_ER(32, AL)                        }	// 90b9
OP(sub_32_er_pcdi)     { SUB_ER(32, PCDI)                      }	// 90ba
OP(sub_32_er_pcix)     { SUB_ER(32, PCIX)                      }	// 90bb
OP(sub_32_er_i)        { SUB_ER_RI32(I)                        }	// 90bc

OP(sub_8_re_ai)        { SUB_RE(8, AI)                         }	// 9110
OP(sub_8_re_pi)        { SUB_RE(8, PI)                         }	// 9118
OP(sub_8_re_pd)        { SUB_RE(8, PD)                         }	// 9120
OP(sub_8_re_di)        { SUB_RE(8, DI)                         }	// 9128
OP(sub_8_re_ix)        { SUB_RE(8, IX)                         }	// 9130
OP(sub_8_re_aw)        { SUB_RE(8, AW)                         }	// 9138
OP(sub_8_re_al)        { SUB_RE(8, AL)                         }	// 9139
OP(sub_8_re_pi7)       { SUB_RE(8, PI7)                        }	// 911f
OP(sub_8_re_pd7)       { SUB_RE(8, PD7)                        }	// 9127

OP(sub_16_re_ai)       { SUB_RE(16, AI)                        }	// 9150
OP(sub_16_re_pi)       { SUB_RE(16, PI)                        }	// 9158
OP(sub_16_re_pd)       { SUB_RE(16, PD)                        }	// 9160
OP(sub_16_re_di)       { SUB_RE(16, DI)                        }	// 9168
OP(sub_16_re_ix)       { SUB_RE(16, IX)                        }	// 9170
OP(sub_16_re_aw)       { SUB_RE(16, AW)                        }	// 9178
OP(sub_16_re_al)       { SUB_RE(16, AL)                        }	// 9179

OP(sub_32_re_ai)       { SUB_RE(32, AI)                        }	// 9190
OP(sub_32_re_pi)       { SUB_RE(32, PI)                        }	// 9198
OP(sub_32_re_pd)       { SUB_RE(32, PD)                        }	// 91a0
OP(sub_32_re_di)       { SUB_RE(32, DI)                        }	// 91a8
OP(sub_32_re_ix)       { SUB_RE(32, IX)                        }	// 91b0
OP(sub_32_re_aw)       { SUB_RE(32, AW)                        }	// 91b8
OP(sub_32_re_al)       { SUB_RE(32, AL)                        }	// 91b9

/*-----------------------------------------------------------------------------
  SUBX    Subtract with Extend
-----------------------------------------------------------------------------*/

OP(subx_8_rr)          { SUBX(8,  D, D, D)                     }	// 9100
OP(subx_16_rr)         { SUBX(16, D, D, D)                     }	// 9140
OP(subx_32_rr)         { SUBX(32, D, D, D)                     }	// 9180

OP(subx_8_mm)          { SUBX(8,  M, PD, PD)                   }	// 9108
OP(subx_16_mm)         { SUBX(16, M, PD, PD)                   }	// 9148
OP(subx_32_mm)         { SUBX(32, M, PD, PD)                   }	// 9188

OP(subx_8_mm_ay7)      { SUBX(8,  M, PD, PD7)                  }	// 910f
OP(subx_8_mm_ax7)      { SUBX(8,  M, PD7, PD)                  }	// 9f08
OP(subx_8_mm_axy7)     { SUBX(8,  M, PD7, PD7)                 }	// 9f0f

/*-----------------------------------------------------------------------------
  SUBA    Subtract Address
-----------------------------------------------------------------------------*/

OP(suba_16_d)          { SUBA(16, D)                           }	// 0440
OP(suba_16_a)          { SUBA(16, A)                           }	// 0448
OP(suba_16_ai)         { SUBA(16, AI)                          }	// 0450
OP(suba_16_pi)         { SUBA(16, PI)                          }	// 0458
OP(suba_16_pd)         { SUBA(16, PD)                          }	// 0460
OP(suba_16_di)         { SUBA(16, DI)                          }	// 0468
OP(suba_16_ix)         { SUBA(16, IX)                          }	// 0470
OP(suba_16_aw)         { SUBA(16, AW)                          }	// 0478
OP(suba_16_al)         { SUBA(16, AL)                          }	// 0479
OP(suba_16_pcdi)       { SUBA(16, PCDI)                        }	// 047a
OP(suba_16_pcix)       { SUBA(16, PCIX)                        }	// 047b
OP(suba_16_i)          { SUBA(16, I)                           }	// 047c

OP(suba_32_d)          { SUBA_RI32(D)                          }	// 0480
OP(suba_32_a)          { SUBA_RI32(A)                          }	// 0488
OP(suba_32_ai)         { SUBA(32, AI)                          }	// 0490
OP(suba_32_pi)         { SUBA(32, PI)                          }	// 0498
OP(suba_32_pd)         { SUBA(32, PD)                          }	// 04a0
OP(suba_32_di)         { SUBA(32, DI)                          }	// 04a8
OP(suba_32_ix)         { SUBA(32, IX)                          }	// 04b0
OP(suba_32_aw)         { SUBA(32, AW)                          }	// 04b8
OP(suba_32_al)         { SUBA(32, AL)                          }	// 04b9
OP(suba_32_pcdi)       { SUBA(32, PCDI)                        }	// 04ba
OP(suba_32_pcix)       { SUBA(32, PCIX)                        }	// 04bb
OP(suba_32_i)          { SUBA_RI32(I)                          }	// 04bc

/******************************************************************************
	OPCODE $Axxx
******************************************************************************/

/*-----------------------------------------------------------------------------
  Line-A
-----------------------------------------------------------------------------*/

// a000
OP(1010)
{
	SWAP_SP()
	EXCEPTION(C68K_1010_EX)
	RET(34)
}

/******************************************************************************
	OPCODE $Bxxx
******************************************************************************/

/*-----------------------------------------------------------------------------
  CMP    Compare
-----------------------------------------------------------------------------*/

OP(cmp_8_d)            { CMP(8, D)                             }	// b000
OP(cmp_8_ai)           { CMP(8, AI)                            }	// b010
OP(cmp_8_pi)           { CMP(8, PI)                            }	// b018
OP(cmp_8_pd)           { CMP(8, PD)                            }	// b020
OP(cmp_8_di)           { CMP(8, DI)                            }	// b028
OP(cmp_8_ix)           { CMP(8, IX)                            }	// b030
OP(cmp_8_aw)           { CMP(8, AW)                            }	// b038
OP(cmp_8_al)           { CMP(8, AL)                            }	// b039
OP(cmp_8_pcdi)         { CMP(8, PCDI)                          }	// b03a
OP(cmp_8_pcix)         { CMP(8, PCIX)                          }	// b03b
OP(cmp_8_i)            { CMP(8, I)                             }	// b03c
OP(cmp_8_pi7)          { CMP(8, PI7)                           }	// b01f
OP(cmp_8_pd7)          { CMP(8, PD7)                           }	// b027

OP(cmp_16_d)           { CMP(16, D)                            }	// b040
OP(cmp_16_a)           { CMP(16, A)                            }	// b048
OP(cmp_16_ai)          { CMP(16, AI)                           }	// b050
OP(cmp_16_pi)          { CMP(16, PI)                           }	// b058
OP(cmp_16_pd)          { CMP(16, PD)                           }	// b060
OP(cmp_16_di)          { CMP(16, DI)                           }	// b068
OP(cmp_16_ix)          { CMP(16, IX)                           }	// b070
OP(cmp_16_aw)          { CMP(16, AW)                           }	// b078
OP(cmp_16_al)          { CMP(16, AL)                           }	// b079
OP(cmp_16_pcdi)        { CMP(16, PCDI)                         }	// b07a
OP(cmp_16_pcix)        { CMP(16, PCIX)                         }	// b07b
OP(cmp_16_i)           { CMP(16, I)                            }	// b07c

OP(cmp_32_d)           { CMP(32, D)                            }	// b080
OP(cmp_32_a)           { CMP(32, A)                            }	// b088
OP(cmp_32_ai)          { CMP(32, AI)                           }	// b090
OP(cmp_32_pi)          { CMP(32, PI)                           }	// b098
OP(cmp_32_pd)          { CMP(32, PD)                           }	// b0a0
OP(cmp_32_di)          { CMP(32, DI)                           }	// b0a8
OP(cmp_32_ix)          { CMP(32, IX)                           }	// b0b0
OP(cmp_32_aw)          { CMP(32, AW)                           }	// b0b8
OP(cmp_32_al)          { CMP(32, AL)                           }	// b0b9
OP(cmp_32_pcdi)        { CMP(32, PCDI)                         }	// b0ba
OP(cmp_32_pcix)        { CMP(32, PCIX)                         }	// b0bb
OP(cmp_32_i)           { CMP(32, I)                            }	// b0bc

/*-----------------------------------------------------------------------------
  CMPM    Compare Memory
-----------------------------------------------------------------------------*/

OP(cmpm_8)             { CMPM(8,  PI, PI)                      }	// b108
OP(cmpm_16)            { CMPM(16, PI, PI)                      }	// b148
OP(cmpm_32)            { CMPM(32, PI, PI)                      }	// b188

OP(cmpm_8_ay7)         { CMPM(8,  PI, PI7)                     }	// b10f
OP(cmpm_8_ax7)         { CMPM(8,  PI7, PI)                     }	// bf08
OP(cmpm_8_axy7)        { CMPM(8,  PI7, PI7)                    }	// bf0f

/*-----------------------------------------------------------------------------
  EOR    Exclusive-OR logical
-----------------------------------------------------------------------------*/

OP(eor_8_d)            { EOR(8, D, D)                          }	// b100
OP(eor_8_ai)           { EOR(8, M, AI)                         }	// b110
OP(eor_8_pi)           { EOR(8, M, PI)                         }	// b118
OP(eor_8_pd)           { EOR(8, M, PD)                         }	// b120
OP(eor_8_di)           { EOR(8, M, DI)                         }	// b128
OP(eor_8_ix)           { EOR(8, M, IX)                         }	// b130
OP(eor_8_aw)           { EOR(8, M, AW)                         }	// b138
OP(eor_8_al)           { EOR(8, M, AL)                         }	// b139
OP(eor_8_pi7)          { EOR(8, M, PI7)                        }	// b11f
OP(eor_8_pd7)          { EOR(8, M, PD7)                        }	// b127

OP(eor_16_d)           { EOR(16, D, D)                         }	// b140
OP(eor_16_ai)          { EOR(16, M, AI)                        }	// b150
OP(eor_16_pi)          { EOR(16, M, PI)                        }	// b158
OP(eor_16_pd)          { EOR(16, M, PD)                        }	// b160
OP(eor_16_di)          { EOR(16, M, DI)                        }	// b168
OP(eor_16_ix)          { EOR(16, M, IX)                        }	// b170
OP(eor_16_aw)          { EOR(16, M, AW)                        }	// b178
OP(eor_16_al)          { EOR(16, M, AL)                        }	// b179

OP(eor_32_d)           { EOR(32, D, D)                         }	// b180
OP(eor_32_ai)          { EOR(32, M, AI)                        }	// b190
OP(eor_32_pi)          { EOR(32, M, PI)                        }	// b198
OP(eor_32_pd)          { EOR(32, M, PD)                        }	// b1a0
OP(eor_32_di)          { EOR(32, M, DI)                        }	// b1a8
OP(eor_32_ix)          { EOR(32, M, IX)                        }	// b1b0
OP(eor_32_aw)          { EOR(32, M, AW)                        }	// b1b8
OP(eor_32_al)          { EOR(32, M, AL)                        }	// b1b9

/*-----------------------------------------------------------------------------
  CMPA    Compare
-----------------------------------------------------------------------------*/

OP(cmpa_16_d)          { CMPA(16, D)                           }	// b0c0
OP(cmpa_16_a)          { CMPA(16, A)                           }	// b0c8
OP(cmpa_16_ai)         { CMPA(16, AI)                          }	// b0d0
OP(cmpa_16_pi)         { CMPA(16, PI)                          }	// b0d8
OP(cmpa_16_pd)         { CMPA(16, PD)                          }	// b0e0
OP(cmpa_16_di)         { CMPA(16, DI)                          }	// b0e8
OP(cmpa_16_ix)         { CMPA(16, IX)                          }	// b0f0
OP(cmpa_16_aw)         { CMPA(16, AW)                          }	// b0f8
OP(cmpa_16_al)         { CMPA(16, AL)                          }	// b0f9
OP(cmpa_16_pcdi)       { CMPA(16, PCDI)                        }	// b0fa
OP(cmpa_16_pcix)       { CMPA(16, PCIX)                        }	// b0fb
OP(cmpa_16_i)          { CMPA(16, I)                           }	// b0fc

OP(cmpa_32_d)          { CMPA(32, D)                           }	// b1c0
OP(cmpa_32_a)          { CMPA(32, A)                           }	// b1c8
OP(cmpa_32_ai)         { CMPA(32, AI)                          }	// b1d0
OP(cmpa_32_pi)         { CMPA(32, PI)                          }	// b1d8
OP(cmpa_32_pd)         { CMPA(32, PD)                          }	// b1e0
OP(cmpa_32_di)         { CMPA(32, DI)                          }	// b1e8
OP(cmpa_32_ix)         { CMPA(32, IX)                          }	// b1f0
OP(cmpa_32_aw)         { CMPA(32, AW)                          }	// b1f8
OP(cmpa_32_al)         { CMPA(32, AL)                          }	// b1f9
OP(cmpa_32_pcdi)       { CMPA(32, PCDI)                        }	// b1fa
OP(cmpa_32_pcix)       { CMPA(32, PCIX)                        }	// b1fb
OP(cmpa_32_i)          { CMPA(32, I)                           }	// b1fc

/******************************************************************************
	OPCODE $Cxxx
******************************************************************************/

/*-----------------------------------------------------------------------------
  AND    AND logical
-----------------------------------------------------------------------------*/

OP(and_8_er_d)         { AND_ER(8, D)                          }	// c000
OP(and_8_er_ai)        { AND_ER(8, AI)                         }	// c010
OP(and_8_er_pi)        { AND_ER(8, PI)                         }	// c018
OP(and_8_er_pd)        { AND_ER(8, PD)                         }	// c020
OP(and_8_er_di)        { AND_ER(8, DI)                         }	// c028
OP(and_8_er_ix)        { AND_ER(8, IX)                         }	// c030
OP(and_8_er_aw)        { AND_ER(8, AW)                         }	// c038
OP(and_8_er_al)        { AND_ER(8, AL)                         }	// c039
OP(and_8_er_pcdi)      { AND_ER(8, PCDI)                       }	// c03a
OP(and_8_er_pcix)      { AND_ER(8, PCIX)                       }	// c03b
OP(and_8_er_i)         { AND_ER(8, I)                          }	// c03c
OP(and_8_er_pi7)       { AND_ER(8, PI7)                        }	// c01f
OP(and_8_er_pd7)       { AND_ER(8, PD7)                        }	// c027

OP(and_16_er_d)        { AND_ER(16, D)                         }	// c040
OP(and_16_er_ai)       { AND_ER(16, AI)                        }	// c050
OP(and_16_er_pi)       { AND_ER(16, PI)                        }	// c058
OP(and_16_er_pd)       { AND_ER(16, PD)                        }	// c060
OP(and_16_er_di)       { AND_ER(16, DI)                        }	// c068
OP(and_16_er_ix)       { AND_ER(16, IX)                        }	// c070
OP(and_16_er_aw)       { AND_ER(16, AW)                        }	// c078
OP(and_16_er_al)       { AND_ER(16, AL)                        }	// c079
OP(and_16_er_pcdi)     { AND_ER(16, PCDI)                      }	// c07a
OP(and_16_er_pcix)     { AND_ER(16, PCIX)                      }	// c07b
OP(and_16_er_i)        { AND_ER(16, I)                         }	// c07c

OP(and_32_er_d)        { AND_ER_RI32(D)                        }	// c080
OP(and_32_er_ai)       { AND_ER(32, AI)                        }	// c090
OP(and_32_er_pi)       { AND_ER(32, PI)                        }	// c098
OP(and_32_er_pd)       { AND_ER(32, PD)                        }	// c0a0
OP(and_32_er_di)       { AND_ER(32, DI)                        }	// c0a8
OP(and_32_er_ix)       { AND_ER(32, IX)                        }	// c0b0
OP(and_32_er_aw)       { AND_ER(32, AW)                        }	// c0b8
OP(and_32_er_al)       { AND_ER(32, AL)                        }	// c0b9
OP(and_32_er_pcdi)     { AND_ER(32, PCDI)                      }	// c0ba
OP(and_32_er_pcix)     { AND_ER(32, PCIX)                      }	// c0bb
OP(and_32_er_i)        { AND_ER_RI32(I)                        }	// c0bc

OP(and_8_re_ai)        { AND_RE(8, AI)                         }	// c110
OP(and_8_re_pi)        { AND_RE(8, PI)                         }	// c118
OP(and_8_re_pd)        { AND_RE(8, PD)                         }	// c120
OP(and_8_re_di)        { AND_RE(8, DI)                         }	// c128
OP(and_8_re_ix)        { AND_RE(8, IX)                         }	// c130
OP(and_8_re_aw)        { AND_RE(8, AW)                         }	// c138
OP(and_8_re_al)        { AND_RE(8, AL)                         }	// c139
OP(and_8_re_pi7)       { AND_RE(8, PI7)                        }	// c11f
OP(and_8_re_pd7)       { AND_RE(8, PD7)                        }	// c127

OP(and_16_re_ai)       { AND_RE(16, AI)                        }	// c150
OP(and_16_re_pi)       { AND_RE(16, PI)                        }	// c158
OP(and_16_re_pd)       { AND_RE(16, PD)                        }	// c160
OP(and_16_re_di)       { AND_RE(16, DI)                        }	// c168
OP(and_16_re_ix)       { AND_RE(16, IX)                        }	// c170
OP(and_16_re_aw)       { AND_RE(16, AW)                        }	// c178
OP(and_16_re_al)       { AND_RE(16, AL)                        }	// c179

OP(and_32_re_ai)       { AND_RE(32, AI)                        }	// c190
OP(and_32_re_pi)       { AND_RE(32, PI)                        }	// c198
OP(and_32_re_pd)       { AND_RE(32, PD)                        }	// c1a0
OP(and_32_re_di)       { AND_RE(32, DI)                        }	// c1a8
OP(and_32_re_ix)       { AND_RE(32, IX)                        }	// c1b0
OP(and_32_re_aw)       { AND_RE(32, AW)                        }	// c1b8
OP(and_32_re_al)       { AND_RE(32, AL)                        }	// c1b9

/*-----------------------------------------------------------------------------
  ABCD    Add Decimal with Extend
-----------------------------------------------------------------------------*/

OP(abcd_8_rr)          { ABCD(D, D,   D)                       }	// c100
OP(abcd_8_mm)          { ABCD(M, PD,  PD)                      }	// c108
OP(abcd_8_mm_ay7)      { ABCD(M, PD,  PD7)                     }	// c10f
OP(abcd_8_mm_ax7)      { ABCD(M, PD7, PD)                      }	// cf08
OP(abcd_8_mm_axy7)     { ABCD(M, PD7, PD7)                     }	// cf0f

/*-----------------------------------------------------------------------------
  MULU    Unsigned Multiply
-----------------------------------------------------------------------------*/

OP(mulu_16_d)          { MULU(D)                               }	// c0c0
OP(mulu_16_ai)         { MULU(AI)                              }	// c0d0
OP(mulu_16_pi)         { MULU(PI)                              }	// c0d8
OP(mulu_16_pd)         { MULU(PD)                              }	// c0e0
OP(mulu_16_di)         { MULU(DI)                              }	// c0e8
OP(mulu_16_ix)         { MULU(IX)                              }	// c0f0
OP(mulu_16_aw)         { MULU(AW)                              }	// c0f8
OP(mulu_16_al)         { MULU(AL)                              }	// c0f9
OP(mulu_16_pcdi)       { MULU(PCDI)                            }	// c0fa
OP(mulu_16_pcix)       { MULU(PCIX)                            }	// c0fb
OP(mulu_16_i)          { MULU(I)                               }	// c0fc

/*-----------------------------------------------------------------------------
  MULS    Signed Multiply
-----------------------------------------------------------------------------*/

OP(muls_16_d)          { MULS(D)                               }	// c1c0
OP(muls_16_ai)         { MULS(AI)                              }	// c1d0
OP(muls_16_pi)         { MULS(PI)                              }	// c1d8
OP(muls_16_pd)         { MULS(PD)                              }	// c1e0
OP(muls_16_di)         { MULS(DI)                              }	// c1e8
OP(muls_16_ix)         { MULS(IX)                              }	// c1f0
OP(muls_16_aw)         { MULS(AW)                              }	// c1f8
OP(muls_16_al)         { MULS(AL)                              }	// c1f9
OP(muls_16_pcdi)       { MULS(PCDI)                            }	// c1fa
OP(muls_16_pcix)       { MULS(PCIX)                            }	// c1fb
OP(muls_16_i)          { MULS(I)                               }	// c1fc

/*-----------------------------------------------------------------------------
  EXG    Exchange Registers
-----------------------------------------------------------------------------*/

OP(exg_32_dd)          { EXG(D, D)                             }	// c140
OP(exg_32_aa)          { EXG(A, A)                             }	// c148
OP(exg_32_da)          { EXG(D, A)                             }	// c188

/******************************************************************************
	OPCODE $Dxxx
******************************************************************************/

/*-----------------------------------------------------------------------------
  ADD    Add
-----------------------------------------------------------------------------*/

OP(add_8_er_d)         { ADD_ER(8, D)                          }	// d000
OP(add_8_er_ai)        { ADD_ER(8, AI)                         }	// d010
OP(add_8_er_pi)        { ADD_ER(8, PI)                         }	// d018
OP(add_8_er_pd)        { ADD_ER(8, PD)                         }	// d020
OP(add_8_er_di)        { ADD_ER(8, DI)                         }	// d028
OP(add_8_er_ix)        { ADD_ER(8, IX)                         }	// d030
OP(add_8_er_aw)        { ADD_ER(8, AW)                         }	// d038
OP(add_8_er_al)        { ADD_ER(8, AL)                         }	// d039
OP(add_8_er_pcdi)      { ADD_ER(8, PCDI)                       }	// d03a
OP(add_8_er_pcix)      { ADD_ER(8, PCIX)                       }	// d03b
OP(add_8_er_i)         { ADD_ER(8, I)                          }	// d03c
OP(add_8_er_pi7)       { ADD_ER(8, PI7)                        }	// d01f
OP(add_8_er_pd7)       { ADD_ER(8, PD7)                        }	// d027

OP(add_16_er_d)        { ADD_ER(16, D)                         }	// d040
OP(add_16_er_a)        { ADD_ER(16, A)                         }	// d048
OP(add_16_er_ai)       { ADD_ER(16, AI)                        }	// d050
OP(add_16_er_pi)       { ADD_ER(16, PI)                        }	// d058
OP(add_16_er_pd)       { ADD_ER(16, PD)                        }	// d060
OP(add_16_er_di)       { ADD_ER(16, DI)                        }	// d068
OP(add_16_er_ix)       { ADD_ER(16, IX)                        }	// d070
OP(add_16_er_aw)       { ADD_ER(16, AW)                        }	// d078
OP(add_16_er_al)       { ADD_ER(16, AL)                        }	// d079
OP(add_16_er_pcdi)     { ADD_ER(16, PCDI)                      }	// d07a
OP(add_16_er_pcix)     { ADD_ER(16, PCIX)                      }	// d07b
OP(add_16_er_i)        { ADD_ER(16, I)                         }	// d07c

OP(add_32_er_d)        { ADD_ER_RI32(D)                        }	// d080
OP(add_32_er_a)        { ADD_ER_RI32(A)                        }	// d088
OP(add_32_er_ai)       { ADD_ER(32, AI)                        }	// d090
OP(add_32_er_pi)       { ADD_ER(32, PI)                        }	// d098
OP(add_32_er_pd)       { ADD_ER(32, PD)                        }	// d0a0
OP(add_32_er_di)       { ADD_ER(32, DI)                        }	// d0a8
OP(add_32_er_ix)       { ADD_ER(32, IX)                        }	// d0b0
OP(add_32_er_aw)       { ADD_ER(32, AW)                        }	// d0b8
OP(add_32_er_al)       { ADD_ER(32, AL)                        }	// d0b9
OP(add_32_er_pcdi)     { ADD_ER(32, PCDI)                      }	// d0ba
OP(add_32_er_pcix)     { ADD_ER(32, PCIX)                      }	// d0bb
OP(add_32_er_i)        { ADD_ER_RI32(I)                        }	// d0bc

OP(add_8_re_ai)        { ADD_RE(8, AI)                         }	// d110
OP(add_8_re_pi)        { ADD_RE(8, PI)                         }	// d118
OP(add_8_re_pd)        { ADD_RE(8, PD)                         }	// d120
OP(add_8_re_di)        { ADD_RE(8, DI)                         }	// d128
OP(add_8_re_ix)        { ADD_RE(8, IX)                         }	// d130
OP(add_8_re_aw)        { ADD_RE(8, AW)                         }	// d138
OP(add_8_re_al)        { ADD_RE(8, AL)                         }	// d139
OP(add_8_re_pi7)       { ADD_RE(8, PI7)                        }	// d11f
OP(add_8_re_pd7)       { ADD_RE(8, PD7)                        }	// d127

OP(add_16_re_ai)       { ADD_RE(16, AI)                        }	// d150
OP(add_16_re_pi)       { ADD_RE(16, PI)                        }	// d158
OP(add_16_re_pd)       { ADD_RE(16, PD)                        }	// d160
OP(add_16_re_di)       { ADD_RE(16, DI)                        }	// d168
OP(add_16_re_ix)       { ADD_RE(16, IX)                        }	// d170
OP(add_16_re_aw)       { ADD_RE(16, AW)                        }	// d178
OP(add_16_re_al)       { ADD_RE(16, AL)                        }	// d179

OP(add_32_re_ai)       { ADD_RE(32, AI)                        }	// d190
OP(add_32_re_pi)       { ADD_RE(32, PI)                        }	// d198
OP(add_32_re_pd)       { ADD_RE(32, PD)                        }	// d1a0
OP(add_32_re_di)       { ADD_RE(32, DI)                        }	// d1a8
OP(add_32_re_ix)       { ADD_RE(32, IX)                        }	// d1b0
OP(add_32_re_aw)       { ADD_RE(32, AW)                        }	// d1b8
OP(add_32_re_al)       { ADD_RE(32, AL)                        }	// d1b9

/*-----------------------------------------------------------------------------
  ADDX    Subtract with Extend
-----------------------------------------------------------------------------*/

OP(addx_8_rr)          { ADDX(8,  D, D, D)                     }	// d100
OP(addx_16_rr)         { ADDX(16, D, D, D)                     }	// d140
OP(addx_32_rr)         { ADDX(32, D, D, D)                     }	// d180

OP(addx_8_mm)          { ADDX(8,  M, PD, PD)                   }	// d108
OP(addx_16_mm)         { ADDX(16, M, PD, PD)                   }	// d148
OP(addx_32_mm)         { ADDX(32, M, PD, PD)                   }	// d188

OP(addx_8_mm_ay7)      { ADDX(8,  M, PD, PD7)                  }	// d10f
OP(addx_8_mm_ax7)      { ADDX(8,  M, PD7, PD)                  }	// df08
OP(addx_8_mm_axy7)     { ADDX(8,  M, PD7, PD7)                 }	// df0f

/*-----------------------------------------------------------------------------
  ADDA    Add Address
-----------------------------------------------------------------------------*/

OP(adda_16_d)          { ADDA(16, D)                           }	// 0440
OP(adda_16_a)          { ADDA(16, A)                           }	// 0448
OP(adda_16_ai)         { ADDA(16, AI)                          }	// 0450
OP(adda_16_pi)         { ADDA(16, PI)                          }	// 0458
OP(adda_16_pd)         { ADDA(16, PD)                          }	// 0460
OP(adda_16_di)         { ADDA(16, DI)                          }	// 0468
OP(adda_16_ix)         { ADDA(16, IX)                          }	// 0470
OP(adda_16_aw)         { ADDA(16, AW)                          }	// 0478
OP(adda_16_al)         { ADDA(16, AL)                          }	// 0479
OP(adda_16_pcdi)       { ADDA(16, PCDI)                        }	// 047a
OP(adda_16_pcix)       { ADDA(16, PCIX)                        }	// 047b
OP(adda_16_i)          { ADDA(16, I)                           }	// 047c

OP(adda_32_d)          { ADDA_RI32(D)                          }	// 0480
OP(adda_32_a)          { ADDA_RI32(A)                          }	// 0488
OP(adda_32_ai)         { ADDA(32, AI)                          }	// 0490
OP(adda_32_pi)         { ADDA(32, PI)                          }	// 0498
OP(adda_32_pd)         { ADDA(32, PD)                          }	// 04a0
OP(adda_32_di)         { ADDA(32, DI)                          }	// 04a8
OP(adda_32_ix)         { ADDA(32, IX)                          }	// 04b0
OP(adda_32_aw)         { ADDA(32, AW)                          }	// 04b8
OP(adda_32_al)         { ADDA(32, AL)                          }	// 04b9
OP(adda_32_pcdi)       { ADDA(32, PCDI)                        }	// 04ba
OP(adda_32_pcix)       { ADDA(32, PCIX)                        }	// 04bb
OP(adda_32_i)          { ADDA_RI32(I)                          }	// 04bc

/******************************************************************************
	OPCODE $Exxx
******************************************************************************/

// e000
OP(asr_8_s)
{
	UINT32 sft;

	sft = (((Opcode >> 9) - 1) & 7) + 1;
	USE_CYCLES(sft << 1)
	src = (INT32)(INT8)DY;
	CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src << ((C68K_SR_C_SFT + 1) - sft);
	res = ((INT32)src) >> sft;
	CPU->flag_N = res >> 0;
	CPU->flag_Z = res;
	*(UINT8 *)(&DY) = res;
	RET(6)
}

// e040
OP(asr_16_s)
{
	UINT32 sft;

	sft = (((Opcode >> 9) - 1) & 7) + 1;
	USE_CYCLES(sft << 1)
	src = (INT32)(INT16)DY;
	CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src << ((C68K_SR_C_SFT + 1) - sft);
	res = ((INT32)src) >> sft;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res;
	*(UINT16 *)(&DY) = res;
	RET(6)
}

// e080
OP(asr_32_s)
{
	UINT32 sft;

	sft = (((Opcode >> 9) - 1) & 7) + 1;
	USE_CYCLES(sft << 1)
	src = (INT32)DY;
	CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src << ((C68K_SR_C_SFT + 1) - sft);
	res = ((INT32)src) >> sft;
	CPU->flag_N = res >> 24;
	CPU->flag_Z = res;
	*(UINT32 *)(&DY) = res;
	RET(8)
}

// e008
OP(lsr_8_s)
{
	UINT32 sft;

	sft = (((Opcode >> 9) - 1) & 7) + 1;
	USE_CYCLES(sft << 1)
	src = READ_REG_8(DY);
	CPU->flag_N = CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src << ((C68K_SR_C_SFT + 1) - sft);
	res = src >> sft;
	CPU->flag_Z = res;
	*(UINT8 *)(&DY) = res;
	RET(6)
}

// e048
OP(lsr_16_s)
{
	UINT32 sft;

	sft = (((Opcode >> 9) - 1) & 7) + 1;
	USE_CYCLES(sft << 1)
	src = READ_REG_16(DY);
	CPU->flag_N = CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src << ((C68K_SR_C_SFT + 1) - sft);
	res = src >> sft;
	CPU->flag_Z = res;
	*(UINT16 *)(&DY) = res;
	RET(6)
}

// e088
OP(lsr_32_s)
{
	UINT32 sft;

	sft = (((Opcode >> 9) - 1) & 7) + 1;
	USE_CYCLES(sft << 1)
	src = READ_REG_32(DY);
	CPU->flag_N = CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src << ((C68K_SR_C_SFT + 1) - sft);
	res = src >> sft;
	CPU->flag_Z = res;
	*(UINT32 *)(&DY) = res;
	RET(8)
}

// e010
OP(roxr_8_s)
{
	UINT32 sft;

	sft = (((Opcode >> 9) - 1) & 7) + 1;
	USE_CYCLES(sft << 1)
	src = READ_REG_8(DY);
	src |= (CPU->flag_X & C68K_SR_X) << 0;
	res = (src >> sft) | (src << (9 - sft));
	CPU->flag_X = CPU->flag_C = res >> 0;
	CPU->flag_V = 0;
	CPU->flag_N = res >> 0;
	CPU->flag_Z = res & 0xff;
	*(UINT8 *)(&DY) = res;
	RET(6)
}

// e050
OP(roxr_16_s)
{
	UINT32 sft;

	sft = (((Opcode >> 9) - 1) & 7) + 1;
	USE_CYCLES(sft << 1)
	src = READ_REG_16(DY);
	src |= (CPU->flag_X & C68K_SR_X) << 8;
	res = (src >> sft) | (src << (17 - sft));
	CPU->flag_X = CPU->flag_C = res >> 8;
	CPU->flag_V = 0;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	*(UINT16 *)(&DY) = res;
	RET(6)
}

// e090
OP(roxr_32_s)
{
	UINT32 sft;

	sft = (((Opcode >> 9) - 1) & 7) + 1;
	USE_CYCLES(sft << 1)
	src = READ_REG_32(DY);
	CPU->flag_C = src << ((C68K_SR_C_SFT + 1) - sft);
	if (sft == 1) res = (src >> 1) | ((CPU->flag_X & C68K_SR_X) << (32 - (C68K_SR_X_SFT + 1)));
	else res = (src >> sft) | (src << (33 - sft)) | ((CPU->flag_X & C68K_SR_X) << (32 - (C68K_SR_X_SFT + sft)));
	CPU->flag_X = CPU->flag_C;
	CPU->flag_V = 0;
	CPU->flag_N = res >> 24;
	CPU->flag_Z = res;
	*(UINT32 *)(&DY) = res;
	RET(8)
}

// e018
OP(ror_8_s)
{
	UINT32 sft;

	sft = (((Opcode >> 9) - 1) & 7) + 1;
	USE_CYCLES(sft << 1)
	src = READ_REG_8(DY);
	CPU->flag_V = 0;
	CPU->flag_C = src << ((C68K_SR_C_SFT + 1) - sft);
	res = (src >> sft) | (src << (8 - sft));
	CPU->flag_N = res >> 0;
	CPU->flag_Z = res & 0xff;
	*(UINT8 *)(&DY) = res;
	RET(6)
}

// e058
OP(ror_16_s)
{
	UINT32 sft;

	sft = (((Opcode >> 9) - 1) & 7) + 1;
	USE_CYCLES(sft << 1)
	src = READ_REG_16(DY);
	CPU->flag_V = 0;
	CPU->flag_C = src << ((C68K_SR_C_SFT + 1) - sft);
	res = (src >> sft) | (src << (16 - sft));
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	*(UINT16 *)(&DY) = res;
	RET(6)
}

// e098
OP(ror_32_s)
{
	UINT32 sft;

	sft = (((Opcode >> 9) - 1) & 7) + 1;
	USE_CYCLES(sft << 1)
	src = READ_REG_32(DY);
	CPU->flag_V = 0;
	CPU->flag_C = src << ((C68K_SR_C_SFT + 1) - sft);
	res = (src >> sft) | (src << (32 - sft));
	CPU->flag_N = res >> 24;
	CPU->flag_Z = res;
	*(UINT32 *)(&DY) = res;
	RET(8)
}

// e100
OP(asl_8_s)
{
	UINT32 sft;

	sft = (((Opcode >> 9) - 1) & 7) + 1;
	USE_CYCLES(sft << 1)
	src = READ_REG_8(DY);
	if (sft < 8)
	{
		CPU->flag_X = CPU->flag_C = src << (0 + sft);
		res = src << sft;
		CPU->flag_N = res >> 0;
		CPU->flag_Z = res & 0xff;
		*(UINT8 *)(&DY) = res;
		CPU->flag_V = 0;
		if ((sft > 7) && (src)) CPU->flag_V = C68K_SR_V;
		else
		{
			UINT32 msk = (((INT32)0x80000000) >> (sft + 24)) & 0xff;
			src &= msk;
			if ((src) && (src != msk)) CPU->flag_V = C68K_SR_V;
		}
		RET(6)
	}

	if (src) CPU->flag_V = C68K_SR_V;
	else CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src << C68K_SR_C_SFT;
	res = 0;
	*(UINT8 *)(&DY) = res;
	CPU->flag_N = 0;
	CPU->flag_Z = 0;
	RET(6)
}

// e140
OP(asl_16_s)
{
	UINT32 sft;

	sft = (((Opcode >> 9) - 1) & 7) + 1;
	USE_CYCLES(sft << 1)
	src = READ_REG_16(DY);
	CPU->flag_X = CPU->flag_C = src >> (8 - sft);
	res = src << sft;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	*(UINT16 *)(&DY) = res;
	CPU->flag_V = 0;
	{
		UINT32 msk = (((INT32)0x80000000) >> (sft + 16)) & 0xffff;
		src &= msk;
		if ((src) && (src != msk)) CPU->flag_V = C68K_SR_V;
	}
	RET(6)
}

// e180
OP(asl_32_s)
{
	UINT32 sft;

	sft = (((Opcode >> 9) - 1) & 7) + 1;
	USE_CYCLES(sft << 1)
	src = READ_REG_32(DY);
	CPU->flag_X = CPU->flag_C = src >> (24 - sft);
	res = src << sft;
	CPU->flag_N = res >> 24;
	CPU->flag_Z = res;
	*(UINT32 *)(&DY) = res;
	CPU->flag_V = 0;
	{
		UINT32 msk = (((INT32)0x80000000) >> (sft + 0));
		src &= msk;
		if ((src) && (src != msk)) CPU->flag_V = C68K_SR_V;
	}
	RET(8)
}

// e108
OP(lsl_8_s)
{
	UINT32 sft;

	sft = (((Opcode >> 9) - 1) & 7) + 1;
	USE_CYCLES(sft << 1)
	src = READ_REG_8(DY);
	CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src << (0 + sft);
	res = src << sft;
	CPU->flag_N = res >> 0;
	CPU->flag_Z = res & 0xff;
	*(UINT8 *)(&DY) = res;
	RET(6)
}

// e148
OP(lsl_16_s)
{
	UINT32 sft;

	sft = (((Opcode >> 9) - 1) & 7) + 1;
	USE_CYCLES(sft << 1)
	src = READ_REG_16(DY);
	CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src >> (8 - sft);
	res = src << sft;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	*(UINT16 *)(&DY) = res;
	RET(6)
}

// e188
OP(lsl_32_s)
{
	UINT32 sft;

	sft = (((Opcode >> 9) - 1) & 7) + 1;
	USE_CYCLES(sft << 1)
	src = READ_REG_32(DY);
	CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src >> (24 - sft);
	res = src << sft;
	CPU->flag_N = res >> 24;
	CPU->flag_Z = res;
	*(UINT32 *)(&DY) = res;
	RET(8)
}

// e110
OP(roxl_8_s)
{
	UINT32 sft;

	sft = (((Opcode >> 9) - 1) & 7) + 1;
	USE_CYCLES(sft << 1)
	src = READ_REG_8(DY);
	src |= (CPU->flag_X & C68K_SR_X) << 0;
	res = (src << sft) | (src >> (9 - sft));
	CPU->flag_X = CPU->flag_C = res >> 0;
	CPU->flag_V = 0;
	CPU->flag_N = res >> 0;
	CPU->flag_Z = res & 0xff;
	*(UINT8 *)(&DY) = res;
	RET(6)
}

// e150
OP(roxl_16_s)
{
	UINT32 sft;

	sft = (((Opcode >> 9) - 1) & 7) + 1;
	USE_CYCLES(sft << 1)
	src = READ_REG_16(DY);
	src |= (CPU->flag_X & C68K_SR_X) << 8;
	res = (src << sft) | (src >> (17 - sft));
	CPU->flag_X = CPU->flag_C = res >> 8;
	CPU->flag_V = 0;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	*(UINT16 *)(&DY) = res;
	RET(6)
}

// e190
OP(roxl_32_s)
{
	UINT32 sft;

	sft = (((Opcode >> 9) - 1) & 7) + 1;
	USE_CYCLES(sft << 1)
	src = READ_REG_32(DY);
	CPU->flag_C = src >> ((32 - C68K_SR_C_SFT) - sft);
	if (sft == 1) res = (src << 1) | ((CPU->flag_X & C68K_SR_X) >> ((C68K_SR_X_SFT + 1) - 1));
	else res = (src << sft) | (src >> (33 - sft)) | ((CPU->flag_X & C68K_SR_X) >> ((C68K_SR_X_SFT + 1) - sft));
	CPU->flag_X = CPU->flag_C;
	CPU->flag_V = 0;
	CPU->flag_N = res >> 24;
	CPU->flag_Z = res;
	*(UINT32 *)(&DY) = res;
	RET(8)
}

// e118
OP(rol_8_s)
{
	UINT32 sft;

	sft = (((Opcode >> 9) - 1) & 7) + 1;
	USE_CYCLES(sft << 1)
	src = READ_REG_8(DY);
	CPU->flag_V = 0;
	CPU->flag_C = src << (0 + sft);
	res = (src << sft) | (src >> (8 - sft));
	CPU->flag_N = res >> 0;
	CPU->flag_Z = res & 0xff;
	*(UINT8 *)(&DY) = res;
	RET(6)
}

// e158
OP(rol_16_s)
{
	UINT32 sft;

	sft = (((Opcode >> 9) - 1) & 7) + 1;
	USE_CYCLES(sft << 1)
	src = READ_REG_16(DY);
	CPU->flag_V = 0;
	CPU->flag_C = src >> (8 - sft);
	res = (src << sft) | (src >> (16 - sft));
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	*(UINT16 *)(&DY) = res;
	RET(6)
}

// e198
OP(rol_32_s)
{
	UINT32 sft;

	sft = (((Opcode >> 9) - 1) & 7) + 1;
	USE_CYCLES(sft << 1)
	src = READ_REG_32(DY);
	CPU->flag_V = 0;
	CPU->flag_C = src >> (24 - sft);
	res = (src << sft) | (src >> (32 - sft));
	CPU->flag_N = res >> 24;
	CPU->flag_Z = res;
	*(UINT32 *)(&DY) = res;
	RET(8)
}

// e020
OP(asr_8_r)
{
	UINT32 sft;

	sft = DX & 0x3f;
	src = (INT32)(INT8)DY;
	if (sft)
	{
		USE_CYCLES(sft << 1)
		if (sft < 8)
		{
			CPU->flag_V = 0;
			CPU->flag_X = CPU->flag_C = src << ((C68K_SR_C_SFT + 1) - sft);
			res = ((INT32)src) >> sft;
			CPU->flag_N = res >> 0;
			CPU->flag_Z = res;
			*(UINT8 *)(&DY) = res;
			RET(6)
		}

		if (src & (1 << 7))
		{
			CPU->flag_N = C68K_SR_N;
			CPU->flag_Z = 1;
			CPU->flag_V = 0;
			CPU->flag_C = C68K_SR_C;
			CPU->flag_X = C68K_SR_X;
			res = 0xff;
			*(UINT8 *)(&DY) = res;
			RET(6)
		}

		CPU->flag_N = 0;
		CPU->flag_Z = 0;
		CPU->flag_V = 0;
		CPU->flag_C = 0;
		CPU->flag_X = 0;
		res = 0;
		*(UINT8 *)(&DY) = res;
		RET(6)
	}

	CPU->flag_V = 0;
	CPU->flag_C = 0;
	CPU->flag_N = src >> 0;
	CPU->flag_Z = src;
	RET(6)
}

// e060
OP(asr_16_r)
{
	UINT32 sft;

	sft = DX & 0x3f;
	src = (INT32)(INT16)DY;
	if (sft)
	{
		USE_CYCLES(sft << 1)
		if (sft < 16)
		{
			CPU->flag_V = 0;
			CPU->flag_X = CPU->flag_C = (src >> (sft - 1)) << C68K_SR_C_SFT;
			res = ((INT32)src) >> sft;
			CPU->flag_N = res >> 8;
			CPU->flag_Z = res;
			*(UINT16 *)(&DY) = res;
			RET(6)
		}

		if (src & (1 << 15))
		{
			CPU->flag_N = C68K_SR_N;
			CPU->flag_Z = 1;
			CPU->flag_V = 0;
			CPU->flag_C = C68K_SR_C;
			CPU->flag_X = C68K_SR_X;
			res = 0xffff;
			*(UINT16 *)(&DY) = res;
			RET(6)
		}

		CPU->flag_N = 0;
		CPU->flag_Z = 0;
		CPU->flag_V = 0;
		CPU->flag_C = 0;
		CPU->flag_X = 0;
		res = 0;
		*(UINT16 *)(&DY) = res;
		RET(6)
	}

	CPU->flag_V = 0;
	CPU->flag_C = 0;
	CPU->flag_N = src >> 8;
	CPU->flag_Z = src;
	RET(6)
}

// e0a0
OP(asr_32_r)
{
	UINT32 sft;

	sft = DX & 0x3f;
	src = (INT32)DY;
	if (sft)
	{
		USE_CYCLES(sft << 1)
		if (sft < 32)
		{
			CPU->flag_V = 0;
			CPU->flag_X = CPU->flag_C = (src >> (sft - 1)) << C68K_SR_C_SFT;
			res = ((INT32)src) >> sft;
			CPU->flag_N = res >> 24;
			CPU->flag_Z = res;
			*(UINT32 *)(&DY) = res;
			RET(8)
		}

		if (src & (1 << 31))
		{
			CPU->flag_N = C68K_SR_N;
			CPU->flag_Z = 1;
			CPU->flag_V = 0;
			CPU->flag_C = C68K_SR_C;
			CPU->flag_X = C68K_SR_X;
			res = 0xffffffff;
			*(UINT32 *)(&DY) = res;
			RET(8)
		}

		CPU->flag_N = 0;
		CPU->flag_Z = 0;
		CPU->flag_V = 0;
		CPU->flag_C = 0;
		CPU->flag_X = 0;
		res = 0;
		*(UINT32 *)(&DY) = res;
		RET(8)
	}

	CPU->flag_V = 0;
	CPU->flag_C = 0;
	CPU->flag_N = src >> 24;
	CPU->flag_Z = src;
	RET(8)
}

// e028
OP(lsr_8_r)
{
	UINT32 sft;

	sft = DX & 0x3f;
	src = READ_REG_8(DY);
	if (sft)
	{
		USE_CYCLES(sft << 1)
		if (sft <= 8)
		{
			CPU->flag_N = CPU->flag_V = 0;
			CPU->flag_X = CPU->flag_C = src << ((C68K_SR_C_SFT + 1) - sft);
			res = src >> sft;
			CPU->flag_Z = res;
			*(UINT8 *)(&DY) = res;
			RET(6)
		}

		CPU->flag_X = CPU->flag_C = 0;
		CPU->flag_N = 0;
		CPU->flag_Z = 0;
		CPU->flag_V = 0;
		res = 0;
		*(UINT8 *)(&DY) = res;
		RET(6)
	}

	CPU->flag_V = 0;
	CPU->flag_C = 0;
	CPU->flag_N = src >> 0;
	CPU->flag_Z = src;
	RET(6)
}

// e068
OP(lsr_16_r)
{
	UINT32 sft;

	sft = DX & 0x3f;
	src = READ_REG_16(DY);
	if (sft)
	{
		USE_CYCLES(sft << 1)
		if (sft <= 16)
		{
			CPU->flag_N = CPU->flag_V = 0;
			CPU->flag_X = CPU->flag_C = (src >> (sft - 1)) << C68K_SR_C_SFT;
			res = src >> sft;
			CPU->flag_Z = res;
			*(UINT16 *)(&DY) = res;
			RET(6)
		}

		CPU->flag_X = CPU->flag_C = 0;
		CPU->flag_N = 0;
		CPU->flag_Z = 0;
		CPU->flag_V = 0;
		res = 0;
		*(UINT16 *)(&DY) = res;
		RET(6)
	}

	CPU->flag_V = 0;
	CPU->flag_C = 0;
	CPU->flag_N = src >> 8;
	CPU->flag_Z = src;
	RET(6)
}

// e0a8
OP(lsr_32_r)
{
	UINT32 sft;

	sft = DX & 0x3f;
	src = READ_REG_32(DY);
	if (sft)
	{
		USE_CYCLES(sft << 1)
		if (sft < 32)
		{
			CPU->flag_N = CPU->flag_V = 0;
			CPU->flag_X = CPU->flag_C = (src >> (sft - 1)) << C68K_SR_C_SFT;
			res = src >> sft;
			CPU->flag_Z = res;
			*(UINT32 *)(&DY) = res;
			RET(8)
		}

		if (sft == 32) CPU->flag_C = src >> (31 - C68K_SR_C_SFT);
		else CPU->flag_C = 0;
		CPU->flag_X = CPU->flag_C;
		CPU->flag_N = 0;
		CPU->flag_Z = 0;
		CPU->flag_V = 0;
		res = 0;
		*(UINT32 *)(&DY) = res;
		RET(8)
	}

	CPU->flag_V = 0;
	CPU->flag_C = 0;
	CPU->flag_N = src >> 24;
	CPU->flag_Z = src;
	RET(8)
}

// e030
OP(roxr_8_r)
{
	UINT32 sft;

	sft = DX & 0x3f;
	src = READ_REG_8(DY);
	if (sft)
	{
		USE_CYCLES(sft << 1)
		sft %= 9;

		src |= (CPU->flag_X & C68K_SR_X) << 0;
		res = (src >> sft) | (src << (9 - sft));
		CPU->flag_X = CPU->flag_C = res >> 0;
		CPU->flag_V = 0;
		CPU->flag_N = res >> 0;
		CPU->flag_Z = res & 0xff;
		*(UINT8 *)(&DY) = res;
		RET(6)
	}

	CPU->flag_V = 0;
	CPU->flag_C = CPU->flag_X;
	CPU->flag_N = src >> 0;
	CPU->flag_Z = src;
	RET(6)
}

// e070
OP(roxr_16_r)
{
	UINT32 sft;

	sft = DX & 0x3f;
	src = READ_REG_16(DY);
	if (sft)
	{
		USE_CYCLES(sft << 1)
		sft %= 17;

		src |= (CPU->flag_X & C68K_SR_X) << 8;
		res = (src >> sft) | (src << (17 - sft));
		CPU->flag_X = CPU->flag_C = res >> 8;
		CPU->flag_V = 0;
		CPU->flag_N = res >> 8;
		CPU->flag_Z = res & 0xffff;
		*(UINT16 *)(&DY) = res;
		RET(6)
	}

	CPU->flag_V = 0;
	CPU->flag_C = CPU->flag_X;
	CPU->flag_N = src >> 8;
	CPU->flag_Z = src;
	RET(6)
}

// e0b0
OP(roxr_32_r)
{
	UINT32 sft;

	sft = DX & 0x3f;
	src = READ_REG_32(DY);
	if (sft)
	{
		USE_CYCLES(sft << 1)
		sft %= 33;

		if (sft != 0)
		{
			if (sft == 1) res = (src >> 1) | ((CPU->flag_X & C68K_SR_X) << (32 - (C68K_SR_X_SFT + 1)));
			else res = (src >> sft) | (src << (33 - sft)) | (((CPU->flag_X & C68K_SR_X) << (32 - (C68K_SR_X_SFT + 1))) >> (sft - 1));
			CPU->flag_X = (src >> (32 - sft)) << C68K_SR_X_SFT;
		}
		else res = src;
		CPU->flag_C = CPU->flag_X;
		CPU->flag_V = 0;
		CPU->flag_N = res >> 24;
		CPU->flag_Z = res;
		*(UINT32 *)(&DY) = res;
		RET(8)
	}

	CPU->flag_V = 0;
	CPU->flag_C = CPU->flag_X;
	CPU->flag_N = src >> 24;
	CPU->flag_Z = src;
	RET(8)
}

// e038
OP(ror_8_r)
{
	UINT32 sft;

	sft = DX & 0x3f;
	src = READ_REG_8(DY);
	if (sft)
	{
		USE_CYCLES(sft << 1)
		sft &= 0x07;

		CPU->flag_C = src << (C68K_SR_C_SFT - ((sft - 1) & 7));
		res = (src >> sft) | (src << (8 - sft));
		CPU->flag_V = 0;
		CPU->flag_N = res >> 0;
		CPU->flag_Z = res & 0xff;
		*(UINT8 *)(&DY) = res;
		RET(6)
	}

	CPU->flag_V = 0;
	CPU->flag_C = 0;
	CPU->flag_N = src >> 0;
	CPU->flag_Z = src;
	RET(6)
}

// e078
OP(ror_16_r)
{
	UINT32 sft;

	sft = DX & 0x3f;
	src = READ_REG_16(DY);
	if (sft)
	{
		USE_CYCLES(sft << 1)
		sft &= 0x0f;

		CPU->flag_C = (src >> ((sft - 1) & 15)) << C68K_SR_C_SFT;
		res = (src >> sft) | (src << (16 - sft));
		CPU->flag_V = 0;
		CPU->flag_N = res >> 8;
		CPU->flag_Z = res & 0xffff;
		*(UINT16 *)(&DY) = res;
		RET(6)
	}

	CPU->flag_V = 0;
	CPU->flag_C = 0;
	CPU->flag_N = src >> 8;
	CPU->flag_Z = src;
	RET(6)
}

// e0b8
OP(ror_32_r)
{
	UINT32 sft;

	sft = DX & 0x3f;
	src = READ_REG_32(DY);
	if (sft)
	{
		USE_CYCLES(sft << 1)
		sft &= 0x1f;

		CPU->flag_C = (src >> ((sft - 1) & 31)) << C68K_SR_C_SFT;
		res = (src >> sft) | (src << (32 - sft));
		CPU->flag_V = 0;
		CPU->flag_N = res >> 24;
		CPU->flag_Z = res;
		*(UINT32 *)(&DY) = res;
		RET(8)
	}

	CPU->flag_V = 0;
	CPU->flag_C = 0;
	CPU->flag_N = src >> 24;
	CPU->flag_Z = src;
	RET(8)
}

// e120
OP(asl_8_r)
{
	UINT32 sft;

	sft = DX & 0x3f;
	src = READ_REG_8(DY);
	if (sft)
	{
		USE_CYCLES(sft << 1)
		if (sft < 8)
		{
			CPU->flag_X = CPU->flag_C = (src << sft) >> 0;
			res = (src << sft) & 0xff;
			CPU->flag_N = res >> 0;
			CPU->flag_Z = res;
			*(UINT8 *)(&DY) = res;
			CPU->flag_V = 0;
			{
				UINT32 msk = (((INT32)0x80000000) >> (sft + 24)) & 0xff;
				src &= msk;
				if ((src) && (src != msk)) CPU->flag_V = C68K_SR_V;
			}
			RET(6)
		}

		if (sft == 256) CPU->flag_C = src << C68K_SR_C_SFT;
		else CPU->flag_C = 0;
		CPU->flag_X = CPU->flag_C;
		CPU->flag_V = (src) ? C68K_SR_V : 0;
		*(UINT8 *)(&DY) = 0;
		CPU->flag_N = 0;
		CPU->flag_Z = 0;
		RET(6)
	}

	CPU->flag_V = 0;
	CPU->flag_C = 0;
	CPU->flag_N = src >> 0;
	CPU->flag_Z = src;
	RET(6)
}

// e160
OP(asl_16_r)
{
	UINT32 sft;

	sft = DX & 0x3f;
	src = READ_REG_16(DY);
	if (sft)
	{
		USE_CYCLES(sft << 1)
		if (sft < 16)
		{
			CPU->flag_X = CPU->flag_C = (src << sft) >> 8;
			res = (src << sft) & 0xffff;
			CPU->flag_N = res >> 8;
			CPU->flag_Z = res;
			*(UINT16 *)(&DY) = res;
			CPU->flag_V = 0;
			{
				UINT32 msk = (((INT32)0x80000000) >> (sft + 16)) & 0xffff;
				src &= msk;
				if ((src) && (src != msk)) CPU->flag_V = C68K_SR_V;
			}
			RET(6)
		}

		if (sft == 65536) CPU->flag_C = src << C68K_SR_C_SFT;
		else CPU->flag_C = 0;
		CPU->flag_X = CPU->flag_C;
		CPU->flag_V = (src) ? C68K_SR_V : 0;
		*(UINT16 *)(&DY) = 0;
		CPU->flag_N = 0;
		CPU->flag_Z = 0;
		RET(6)
	}

	CPU->flag_V = 0;
	CPU->flag_C = 0;
	CPU->flag_N = src >> 8;
	CPU->flag_Z = src;
	RET(6)
}

// e1a0
OP(asl_32_r)
{
	UINT32 sft;

	sft = DX & 0x3f;
	src = READ_REG_32(DY);
	if (sft)
	{
		USE_CYCLES(sft << 1)
		if (sft < 32)
		{
			CPU->flag_X = CPU->flag_C = (src >> (32 - sft)) << C68K_SR_C_SFT;
			res = src << sft;
			CPU->flag_N = res >> 24;
			CPU->flag_Z = res;
			*(UINT32 *)(&DY) = res;
			CPU->flag_V = 0;
			{
				UINT32 msk = (((INT32)0x80000000) >> (sft + 0));
				src &= msk;
				if ((src) && (src != msk)) CPU->flag_V = C68K_SR_V;
			}
			RET(8)
		}

		if (sft == 0) CPU->flag_C = src << C68K_SR_C_SFT;
		else CPU->flag_C = 0;
		CPU->flag_X = CPU->flag_C;
		CPU->flag_V = (src) ? C68K_SR_V : 0;
		*(UINT32 *)(&DY) = 0;
		CPU->flag_N = 0;
		CPU->flag_Z = 0;
		RET(8)
	}

	CPU->flag_V = 0;
	CPU->flag_C = 0;
	CPU->flag_N = src >> 24;
	CPU->flag_Z = src;
	RET(8)
}

// e128
OP(lsl_8_r)
{
	UINT32 sft;

	sft = DX & 0x3f;
	src = READ_REG_8(DY);
	if (sft)
	{
		USE_CYCLES(sft << 1)
		if (sft <= 8)
		{
			CPU->flag_X = CPU->flag_C = (src << sft) >> 0;
			res = (src << sft) & 0xff;
			CPU->flag_V = 0;
			CPU->flag_N = res >> 0;
			CPU->flag_Z = res;
			*(UINT8 *)(&DY) = res;
			RET(6)
		}

		CPU->flag_X = CPU->flag_C = 0;
		CPU->flag_N = 0;
		CPU->flag_Z = 0;
		CPU->flag_V = 0;
		*(UINT8 *)(&DY) = 0;
		RET(6)
	}

	CPU->flag_V = 0;
	CPU->flag_C = 0;
	CPU->flag_N = src >> 0;
	CPU->flag_Z = src;
	RET(6)
}

// e168
OP(lsl_16_r)
{
	UINT32 sft;

	sft = DX & 0x3f;
	src = READ_REG_16(DY);
	if (sft)
	{
		USE_CYCLES(sft << 1)
		if (sft <= 16)
		{
			CPU->flag_X = CPU->flag_C = (src << sft) >> 8;
			res = (src << sft) & 0xffff;
			CPU->flag_V = 0;
			CPU->flag_N = res >> 8;
			CPU->flag_Z = res;
			*(UINT16 *)(&DY) = res;
			RET(6)
		}

		CPU->flag_X = CPU->flag_C = 0;
		CPU->flag_N = 0;
		CPU->flag_Z = 0;
		CPU->flag_V = 0;
		*(UINT16 *)(&DY) = 0;
		RET(6)
	}

	CPU->flag_V = 0;
	CPU->flag_C = 0;
	CPU->flag_N = src >> 8;
	CPU->flag_Z = src;
	RET(6)
}

// e1a8
OP(lsl_32_r)
{
	UINT32 sft;

	sft = DX & 0x3f;
	src = READ_REG_32(DY);
	if (sft)
	{
		USE_CYCLES(sft << 1)
		if (sft < 32)
		{
			CPU->flag_X = CPU->flag_C = (src >> (32 - sft)) << C68K_SR_C_SFT;
			res = src << sft;
			CPU->flag_V = 0;
			CPU->flag_N = res >> 24;
			CPU->flag_Z = res;
			*(UINT32 *)(&DY) = res;
			RET(8)
		}

		if (sft == 32) CPU->flag_C = src << C68K_SR_C_SFT;
		else CPU->flag_C = 0;
		CPU->flag_X = CPU->flag_C;
		CPU->flag_N = 0;
		CPU->flag_Z = 0;
		CPU->flag_V = 0;
		*(UINT32 *)(&DY) = 0;
		RET(8)
	}

	CPU->flag_V = 0;
	CPU->flag_C = 0;
	CPU->flag_N = src >> 24;
	CPU->flag_Z = src;
	RET(8)
}

// e130
OP(roxl_8_r)
{
	UINT32 sft;

	sft = DX & 0x3f;
	src = READ_REG_8(DY);
	if (sft)
	{
		USE_CYCLES(sft << 1)
		sft %= 9;

		src |= (CPU->flag_X & C68K_SR_X) << 0;
		res = (src << sft) | (src >> (9 - sft));
		CPU->flag_X = CPU->flag_C = res >> 0;
		CPU->flag_V = 0;
		CPU->flag_N = res >> 0;
		CPU->flag_Z = res & 0xff;
		*(UINT8 *)(&DY) = res;
		RET(6)
	}

	CPU->flag_V = 0;
	CPU->flag_C = CPU->flag_X;
	CPU->flag_N = src >> 0;
	CPU->flag_Z = src;
	RET(6)
}

// e170
OP(roxl_16_r)
{
	UINT32 sft;

	sft = DX & 0x3f;
	src = READ_REG_16(DY);
	if (sft)
	{
		USE_CYCLES(sft << 1)
		sft %= 17;

		src |= (CPU->flag_X & C68K_SR_X) << 8;
		res = (src << sft) | (src >> (17 - sft));
		CPU->flag_X = CPU->flag_C = res >> 8;
		CPU->flag_V = 0;
		CPU->flag_N = res >> 8;
		CPU->flag_Z = res & 0xffff;
		*(UINT16 *)(&DY) = res;
		RET(6)
	}

	CPU->flag_V = 0;
	CPU->flag_C = CPU->flag_X;
	CPU->flag_N = src >> 8;
	CPU->flag_Z = src;
	RET(6)
}

// e1b0
OP(roxl_32_r)
{
	UINT32 sft;

	sft = DX & 0x3f;
	src = READ_REG_32(DY);
	if (sft)
	{
		USE_CYCLES(sft << 1)
		sft %= 33;

		if (sft != 0)
		{
			if (sft == 1) res = (src << 1) | ((CPU->flag_X >> ((C68K_SR_X_SFT + 1) - 1)) & 1);
			else res = (src << sft) | (src >> (33 - sft)) | (((CPU->flag_X >> ((C68K_SR_X_SFT + 1) - 1)) & 1) << (sft - 1));
			CPU->flag_X = (src >> (32 - sft)) << C68K_SR_X_SFT;
		}
		else res = src;
		CPU->flag_C = CPU->flag_X;
		CPU->flag_V = 0;
		CPU->flag_N = res >> 24;
		CPU->flag_Z = res;
		*(UINT32 *)(&DY) = res;
		RET(8)
	}

	CPU->flag_V = 0;
	CPU->flag_C = CPU->flag_X;
	CPU->flag_N = src >> 24;
	CPU->flag_Z = src;
	RET(8)
}

// e138
OP(rol_8_r)
{
	UINT32 sft;

	sft = DX & 0x3f;
	src = READ_REG_8(DY);
	if (sft)
	{
		USE_CYCLES(sft << 1)
		if (sft &= 0x07)
		{
			CPU->flag_C = (src << sft) >> 0;
			res = ((src << sft) | (src >> (8 - sft))) & 0xff;
			CPU->flag_V = 0;
			CPU->flag_N = res >> 0;
			CPU->flag_Z = res;
			*(UINT8 *)(&DY) = res;
			RET(6)
		}

		CPU->flag_V = 0;
		CPU->flag_C = src << C68K_SR_C_SFT;
		CPU->flag_N = src >> 0;
		CPU->flag_Z = src;
		RET(6)
	}

	CPU->flag_V = 0;
	CPU->flag_C = 0;
	CPU->flag_N = src >> 0;
	CPU->flag_Z = src;
	RET(6)
}

// e178
OP(rol_16_r)
{
	UINT32 sft;

	sft = DX & 0x3f;
	src = READ_REG_16(DY);
	if (sft)
	{
		USE_CYCLES(sft << 1)
		if (sft &= 0x0f)
		{
			CPU->flag_C = (src << sft) >> 8;
			res = ((src << sft) | (src >> (16 - sft))) & 0xffff;
			CPU->flag_V = 0;
			CPU->flag_N = res >> 8;
			CPU->flag_Z = res;
			*(UINT16 *)(&DY) = res;
			RET(6)
		}

		CPU->flag_V = 0;
		CPU->flag_C = src << C68K_SR_C_SFT;
		CPU->flag_N = src >> 8;
		CPU->flag_Z = src;
		RET(6)
	}

	CPU->flag_V = 0;
	CPU->flag_C = 0;
	CPU->flag_N = src >> 8;
	CPU->flag_Z = src;
	RET(6)
}

// e1b8
OP(rol_32_r)
{
	UINT32 sft;

	sft = DX & 0x3f;
	src = READ_REG_32(DY);
	if (sft)
	{
		USE_CYCLES(sft << 1)
		if (sft &= 0x1f)
		{
			CPU->flag_C = (src >> (32 - sft)) << C68K_SR_C_SFT;
			res = (src << sft) | (src >> (32 - sft));
			CPU->flag_V = 0;
			CPU->flag_N = res >> 24;
			CPU->flag_Z = res;
			*(UINT32 *)(&DY) = res;
			RET(8)
		}

		CPU->flag_V = 0;
		CPU->flag_C = src << C68K_SR_C_SFT;
		CPU->flag_N = src >> 24;
		CPU->flag_Z = src;
		RET(8)
	}

	CPU->flag_V = 0;
	CPU->flag_C = 0;
	CPU->flag_N = src >> 24;
	CPU->flag_Z = src;
	RET(8)
}

#if 0

/*-----------------------------------------------------------------------------
  ASR (EA)   Arithmetic Shift Right
-----------------------------------------------------------------------------*/

OP(asr_16_ai)          { ASR_EA(AI)                            }	// e0d0
OP(asr_16_pi)          { ASR_EA(PI)                            }	// e0d8
OP(asr_16_pd)          { ASR_EA(PD)                            }	// e0e0
OP(asr_16_di)          { ASR_EA(DI)                            }	// e0e8
OP(asr_16_ix)          { ASR_EA(IX)                            }	// e0f0
OP(asr_16_aw)          { ASR_EA(AW)                            }	// e0f8
OP(asr_16_al)          { ASR_EA(AL)                            }	// e0f9

/*-----------------------------------------------------------------------------
  LSR (EA)   Logical Shift Right
-----------------------------------------------------------------------------*/

OP(lsr_16_ai)          { LSR_EA(AI)                            }	// e2d0
OP(lsr_16_pi)          { LSR_EA(PI)                            }	// e2d8
OP(lsr_16_pd)          { LSR_EA(PD)                            }	// e2e0
OP(lsr_16_di)          { LSR_EA(DI)                            }	// e2e8
OP(lsr_16_ix)          { LSR_EA(IX)                            }	// e2f0
OP(lsr_16_aw)          { LSR_EA(AW)                            }	// e2f8
OP(lsr_16_al)          { LSR_EA(AL)                            }	// e2f9

/*-----------------------------------------------------------------------------
  ROXR (EA)   Rotate Right with Extend
-----------------------------------------------------------------------------*/

OP(roxr_16_ai)         { ROXR_EA(AI)                           }	// e4d0
OP(roxr_16_pi)         { ROXR_EA(PI)                           }	// e4d8
OP(roxr_16_pd)         { ROXR_EA(PD)                           }	// e4e0
OP(roxr_16_di)         { ROXR_EA(DI)                           }	// e4e8
OP(roxr_16_ix)         { ROXR_EA(IX)                           }	// e4f0
OP(roxr_16_aw)         { ROXR_EA(AW)                           }	// e4f8
OP(roxr_16_al)         { ROXR_EA(AL)                           }	// e4f9

/*-----------------------------------------------------------------------------
  ROR (EA)   Rotate Right
-----------------------------------------------------------------------------*/

OP(ror_16_ai)          { ROR_EA(AI)                            }	// e6d0
OP(ror_16_pi)          { ROR_EA(PI)                            }	// e6d8
OP(ror_16_pd)          { ROR_EA(PD)                            }	// e6e0
OP(ror_16_di)          { ROR_EA(DI)                            }	// e6e8
OP(ror_16_ix)          { ROR_EA(IX)                            }	// e6f0
OP(ror_16_aw)          { ROR_EA(AW)                            }	// e6f8
OP(ror_16_al)          { ROR_EA(AL)                            }	// e6f9

/*-----------------------------------------------------------------------------
  ASL (EA)   Arithmetic Shift Left
-----------------------------------------------------------------------------*/

OP(asl_16_ai)          { ASL_EA(AI)                            }	// e1d0
OP(asl_16_pi)          { ASL_EA(PI)                            }	// e1d8
OP(asl_16_pd)          { ASL_EA(PD)                            }	// e1e0
OP(asl_16_di)          { ASL_EA(DI)                            }	// e1e8
OP(asl_16_ix)          { ASL_EA(IX)                            }	// e1f0
OP(asl_16_aw)          { ASL_EA(AW)                            }	// e1f8
OP(asl_16_al)          { ASL_EA(AL)                            }	// e1f9

/*-----------------------------------------------------------------------------
  LSL (EA)   Logical Shift Left
-----------------------------------------------------------------------------*/

OP(lsl_16_ai)          { LSL_EA(AI)                            }	// e3d0
OP(lsl_16_pi)          { LSL_EA(PI)                            }	// e3d8
OP(lsl_16_pd)          { LSL_EA(PD)                            }	// e3e0
OP(lsl_16_di)          { LSL_EA(DI)                            }	// e3e8
OP(lsl_16_ix)          { LSL_EA(IX)                            }	// e3f0
OP(lsl_16_aw)          { LSL_EA(AW)                            }	// e3f8
OP(lsl_16_al)          { LSL_EA(AL)                            }	// e3f9

/*-----------------------------------------------------------------------------
  ROXL (EA)   Rotate Right with Extend
-----------------------------------------------------------------------------*/

OP(roxl_16_ai)         { ROXL_EA(AI)                           }	// e5d0
OP(roxl_16_pi)         { ROXL_EA(PI)                           }	// e5d8
OP(roxl_16_pd)         { ROXL_EA(PD)                           }	// e5e0
OP(roxl_16_di)         { ROXL_EA(DI)                           }	// e5e8
OP(roxl_16_ix)         { ROXL_EA(IX)                           }	// e5f0
OP(roxl_16_aw)         { ROXL_EA(AW)                           }	// e5f8
OP(roxl_16_al)         { ROXL_EA(AL)                           }	// e5f9

/*-----------------------------------------------------------------------------
  ROL (EA)   Rotate Left
-----------------------------------------------------------------------------*/

OP(rol_16_ai)          { ROL_EA(AI)                            }	// e7d0
OP(rol_16_pi)          { ROL_EA(PI)                            }	// e7d8
OP(rol_16_pd)          { ROL_EA(PD)                            }	// e7e0
OP(rol_16_di)          { ROL_EA(DI)                            }	// e7e8
OP(rol_16_ix)          { ROL_EA(IX)                            }	// e7f0
OP(rol_16_aw)          { ROL_EA(AW)                            }	// e7f8
OP(rol_16_al)          { ROL_EA(AL)                            }	// e7f9

#else

// e0d0
OP(asr_16_ai)
{
	adr = AY;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src << C68K_SR_C_SFT;
	res = (src >> 1) | (src & (1 << 15));
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res;
	WRITE_MEM_16(adr, res);
	RET(12)
}

// e0d8
OP(asr_16_pi)
{
	adr = AY;
	AY += 2;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src << C68K_SR_C_SFT;
	res = (src >> 1) | (src & (1 << 15));
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res;
	WRITE_MEM_16(adr, res);
	RET(12)
}

// e0e0
OP(asr_16_pd)
{
	adr = AY - 2;
	AY = adr;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src << C68K_SR_C_SFT;
	res = (src >> 1) | (src & (1 << 15));
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res;
	WRITE_MEM_16(adr, res);
	RET(14)
}

// e0e8
OP(asr_16_di)
{
	adr = AY + READSX_IMM_16();
	PC += 2;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src << C68K_SR_C_SFT;
	res = (src >> 1) | (src & (1 << 15));
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res;
	WRITE_MEM_16(adr, res);
	RET(16)
}

// e0f0
OP(asr_16_ix)
{
	adr = AY;
	DECODE_EXT_WORD
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src << C68K_SR_C_SFT;
	res = (src >> 1) | (src & (1 << 15));
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res;
	WRITE_MEM_16(adr, res);
	RET(18)
}

// e0f8
OP(asr_16_aw)
{
	adr = READSX_IMM_16();
	PC += 2;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src << C68K_SR_C_SFT;
	res = (src >> 1) | (src & (1 << 15));
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res;
	WRITE_MEM_16(adr, res);
	RET(16)
}

// e0f9
OP(asr_16_al)
{
	adr = READSX_IMM_32();
	PC += 4;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src << C68K_SR_C_SFT;
	res = (src >> 1) | (src & (1 << 15));
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res;
	WRITE_MEM_16(adr, res);
	RET(20)
}

// e2d0
OP(lsr_16_ai)
{
	adr = AY;
	src = READ_MEM_16(adr);
	CPU->flag_N = CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src << C68K_SR_C_SFT;
	res = src >> 1;
	CPU->flag_Z = res;
	WRITE_MEM_16(adr, res);
	RET(12)
}

// e2d8
OP(lsr_16_pi)
{
	adr = AY;
	AY += 2;
	src = READ_MEM_16(adr);
	CPU->flag_N = CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src << C68K_SR_C_SFT;
	res = src >> 1;
	CPU->flag_Z = res;
	WRITE_MEM_16(adr, res);
	RET(12)
}

// e2e0
OP(lsr_16_pd)
{
	adr = AY - 2;
	AY = adr;
	src = READ_MEM_16(adr);
	CPU->flag_N = CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src << C68K_SR_C_SFT;
	res = src >> 1;
	CPU->flag_Z = res;
	WRITE_MEM_16(adr, res);
	RET(14)
}

// e2e8
OP(lsr_16_di)
{
	adr = AY + READSX_IMM_16();
	PC += 2;
	src = READ_MEM_16(adr);
	CPU->flag_N = CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src << C68K_SR_C_SFT;
	res = src >> 1;
	CPU->flag_Z = res;
	WRITE_MEM_16(adr, res);
	RET(16)
}

// e2f0
OP(lsr_16_ix)
{
	adr = AY;
	DECODE_EXT_WORD
	src = READ_MEM_16(adr);
	CPU->flag_N = CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src << C68K_SR_C_SFT;
	res = src >> 1;
	CPU->flag_Z = res;
	WRITE_MEM_16(adr, res);
	RET(18)
}

// e2f8
OP(lsr_16_aw)
{
	adr = READSX_IMM_16();
	PC += 2;
	src = READ_MEM_16(adr);
	CPU->flag_N = CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src << C68K_SR_C_SFT;
	res = src >> 1;
	CPU->flag_Z = res;
	WRITE_MEM_16(adr, res);
	RET(16)
}

// e2f9
OP(lsr_16_al)
{
	adr = READSX_IMM_32();
	PC += 4;
	src = READ_MEM_16(adr);
	CPU->flag_N = CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src << C68K_SR_C_SFT;
	res = src >> 1;
	CPU->flag_Z = res;
	WRITE_MEM_16(adr, res);
	RET(20)
}

// e4d0
OP(roxr_16_ai)
{
	adr = AY;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	res = (src >> 1) | ((CPU->flag_X & C68K_SR_X) << 7);
	CPU->flag_C = CPU->flag_X = src << C68K_SR_C_SFT;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res;
	WRITE_MEM_16(adr, res);
	RET(12)
}

// e4d8
OP(roxr_16_pi)
{
	adr = AY;
	AY += 2;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	res = (src >> 1) | ((CPU->flag_X & C68K_SR_X) << 7);
	CPU->flag_C = CPU->flag_X = src << C68K_SR_C_SFT;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res;
	WRITE_MEM_16(adr, res);
	RET(12)
}

// e4e0
OP(roxr_16_pd)
{
	adr = AY - 2;
	AY = adr;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	res = (src >> 1) | ((CPU->flag_X & C68K_SR_X) << 7);
	CPU->flag_C = CPU->flag_X = src << C68K_SR_C_SFT;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res;
	WRITE_MEM_16(adr, res);
	RET(14)
}

// e4e8
OP(roxr_16_di)
{
	adr = AY + READSX_IMM_16();
	PC += 2;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	res = (src >> 1) | ((CPU->flag_X & C68K_SR_X) << 7);
	CPU->flag_C = CPU->flag_X = src << C68K_SR_C_SFT;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res;
	WRITE_MEM_16(adr, res);
	RET(16)
}

// e4f0
OP(roxr_16_ix)
{
	adr = AY;
	DECODE_EXT_WORD
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	res = (src >> 1) | ((CPU->flag_X & C68K_SR_X) << 7);
	CPU->flag_C = CPU->flag_X = src << C68K_SR_C_SFT;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res;
	WRITE_MEM_16(adr, res);
	RET(18)
}

// e4f8
OP(roxr_16_aw)
{
	adr = READSX_IMM_16();
	PC += 2;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	res = (src >> 1) | ((CPU->flag_X & C68K_SR_X) << 7);
	CPU->flag_C = CPU->flag_X = src << C68K_SR_C_SFT;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res;
	WRITE_MEM_16(adr, res);
	RET(16)
}

// e4f9
OP(roxr_16_al)
{
	adr = READSX_IMM_32();
	PC += 4;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	res = (src >> 1) | ((CPU->flag_X & C68K_SR_X) << 7);
	CPU->flag_C = CPU->flag_X = src << C68K_SR_C_SFT;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res;
	WRITE_MEM_16(adr, res);
	RET(20)
}

// e6d0
OP(ror_16_ai)
{
	adr = AY;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	CPU->flag_C = src << C68K_SR_C_SFT;
	res = (src >> 1) | (src << 15);
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(12)
}

// e6d8
OP(ror_16_pi)
{
	adr = AY;
	AY += 2;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	CPU->flag_C = src << C68K_SR_C_SFT;
	res = (src >> 1) | (src << 15);
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(12)
}

// e6e0
OP(ror_16_pd)
{
	adr = AY - 2;
	AY = adr;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	CPU->flag_C = src << C68K_SR_C_SFT;
	res = (src >> 1) | (src << 15);
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(14)
}

// e6e8
OP(ror_16_di)
{
	adr = AY + READSX_IMM_16();
	PC += 2;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	CPU->flag_C = src << C68K_SR_C_SFT;
	res = (src >> 1) | (src << 15);
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(16)
}

// e6f0
OP(ror_16_ix)
{
	adr = AY;
	DECODE_EXT_WORD
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	CPU->flag_C = src << C68K_SR_C_SFT;
	res = (src >> 1) | (src << 15);
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(18)
}

// e6f8
OP(ror_16_aw)
{
	adr = READSX_IMM_16();
	PC += 2;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	CPU->flag_C = src << C68K_SR_C_SFT;
	res = (src >> 1) | (src << 15);
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(16)
}

// e6f9
OP(ror_16_al)
{
	adr = READSX_IMM_32();
	PC += 4;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	CPU->flag_C = src << C68K_SR_C_SFT;
	res = (src >> 1) | (src << 15);
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(20)
}

// e1d0
OP(asl_16_ai)
{
	adr = AY;
	src = READ_MEM_16(adr);
	CPU->flag_X = CPU->flag_C = src >> 7;
	res = src << 1;
	CPU->flag_V = (src ^ res) >> 8;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(12)
}

// e1d8
OP(asl_16_pi)
{
	adr = AY;
	AY += 2;
	src = READ_MEM_16(adr);
	CPU->flag_X = CPU->flag_C = src >> 7;
	res = src << 1;
	CPU->flag_V = (src ^ res) >> 8;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(12)
}

// e1e0
OP(asl_16_pd)
{
	adr = AY - 2;
	AY = adr;
	src = READ_MEM_16(adr);
	CPU->flag_X = CPU->flag_C = src >> 7;
	res = src << 1;
	CPU->flag_V = (src ^ res) >> 8;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(14)
}

// e1e8
OP(asl_16_di)
{
	adr = AY + READSX_IMM_16();
	PC += 2;
	src = READ_MEM_16(adr);
	CPU->flag_X = CPU->flag_C = src >> 7;
	res = src << 1;
	CPU->flag_V = (src ^ res) >> 8;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(16)
}

// e1f0
OP(asl_16_ix)
{
	adr = AY;
	DECODE_EXT_WORD
	src = READ_MEM_16(adr);
	CPU->flag_X = CPU->flag_C = src >> 7;
	res = src << 1;
	CPU->flag_V = (src ^ res) >> 8;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(18)
}

// e1f8
OP(asl_16_aw)
{
	adr = READSX_IMM_16();
	PC += 2;
	src = READ_MEM_16(adr);
	CPU->flag_X = CPU->flag_C = src >> 7;
	res = src << 1;
	CPU->flag_V = (src ^ res) >> 8;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(16)
}

// e1f9
OP(asl_16_al)
{
	adr = READSX_IMM_32();
	PC += 4;
	src = READ_MEM_16(adr);
	CPU->flag_X = CPU->flag_C = src >> 7;
	res = src << 1;
	CPU->flag_V = (src ^ res) >> 8;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(20)
}

// e3d0
OP(lsl_16_ai)
{
	adr = AY;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src >> 7;
	res = src << 1;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(12)
}

// e3d8
OP(lsl_16_pi)
{
	adr = AY;
	AY += 2;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src >> 7;
	res = src << 1;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(12)
}

// e3e0
OP(lsl_16_pd)
{
	adr = AY - 2;
	AY = adr;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src >> 7;
	res = src << 1;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(14)
}

// e3e8
OP(lsl_16_di)
{
	adr = AY + READSX_IMM_16();
	PC += 2;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src >> 7;
	res = src << 1;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(16)
}

// e3f0
OP(lsl_16_ix)
{
	adr = AY;
	DECODE_EXT_WORD
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src >> 7;
	res = src << 1;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(18)
}

// e3f8
OP(lsl_16_aw)
{
	adr = READSX_IMM_16();
	PC += 2;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src >> 7;
	res = src << 1;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(16)
}

// e3f9
OP(lsl_16_al)
{
	adr = READSX_IMM_32();
	PC += 4;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	CPU->flag_X = CPU->flag_C = src >> 7;
	res = src << 1;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(20)
}

// e5d0
OP(roxl_16_ai)
{
	adr = AY;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	res = (src << 1) | ((CPU->flag_X & C68K_SR_X) >> 8);
	CPU->flag_X = CPU->flag_C = src >> 7;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(12)
}

// e5d8
OP(roxl_16_pi)
{
	adr = AY;
	AY += 2;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	res = (src << 1) | ((CPU->flag_X & C68K_SR_X) >> 8);
	CPU->flag_X = CPU->flag_C = src >> 7;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(12)
}

// e5e0
OP(roxl_16_pd)
{
	adr = AY - 2;
	AY = adr;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	res = (src << 1) | ((CPU->flag_X & C68K_SR_X) >> 8);
	CPU->flag_X = CPU->flag_C = src >> 7;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(14)
}

// e5e8
OP(roxl_16_di)
{
	adr = AY + READSX_IMM_16();
	PC += 2;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	res = (src << 1) | ((CPU->flag_X & C68K_SR_X) >> 8);
	CPU->flag_X = CPU->flag_C = src >> 7;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(16)
}

// e5f0
OP(roxl_16_ix)
{
	adr = AY;
	DECODE_EXT_WORD
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	res = (src << 1) | ((CPU->flag_X & C68K_SR_X) >> 8);
	CPU->flag_X = CPU->flag_C = src >> 7;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(18)
}

// e5f8
OP(roxl_16_aw)
{
	adr = READSX_IMM_16();
	PC += 2;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	res = (src << 1) | ((CPU->flag_X & C68K_SR_X) >> 8);
	CPU->flag_X = CPU->flag_C = src >> 7;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(16)
}

// e5f9
OP(roxl_16_al)
{
	adr = READSX_IMM_32();
	PC += 4;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	res = (src << 1) | ((CPU->flag_X & C68K_SR_X) >> 8);
	CPU->flag_X = CPU->flag_C = src >> 7;
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(20)
}

// e7d0
OP(rol_16_ai)
{
	adr = AY;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	CPU->flag_C = src >> 7;
	res = (src << 1) | (src >> 15);
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(12)
}

// e7d8
OP(rol_16_pi)
{
	adr = AY;
	AY += 2;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	CPU->flag_C = src >> 7;
	res = (src << 1) | (src >> 15);
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(12)
}

// e7e0
OP(rol_16_pd)
{
	adr = AY - 2;
	AY = adr;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	CPU->flag_C = src >> 7;
	res = (src << 1) | (src >> 15);
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(14)
}

// e7e8
OP(rol_16_di)
{
	adr = AY + READSX_IMM_16();
	PC += 2;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	CPU->flag_C = src >> 7;
	res = (src << 1) | (src >> 15);
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(16)
}

// e7f0
OP(rol_16_ix)
{
	adr = AY;
	DECODE_EXT_WORD
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	CPU->flag_C = src >> 7;
	res = (src << 1) | (src >> 15);
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(18)
}

// e7f8
OP(rol_16_aw)
{
	adr = READSX_IMM_16();
	PC += 2;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	CPU->flag_C = src >> 7;
	res = (src << 1) | (src >> 15);
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(16)
}

// e7f9
OP(rol_16_al)
{
	adr = READSX_IMM_32();
	PC += 4;
	src = READ_MEM_16(adr);
	CPU->flag_V = 0;
	CPU->flag_C = src >> 7;
	res = (src << 1) | (src >> 15);
	CPU->flag_N = res >> 8;
	CPU->flag_Z = res & 0xffff;
	WRITE_MEM_16(adr, res);
	RET(20)
}

#endif

/******************************************************************************
	OPCODE $Fxxx
******************************************************************************/

/*-----------------------------------------------------------------------------
  Line-F
-----------------------------------------------------------------------------*/

// f000
OP(1111)
{
	SWAP_SP()
	EXCEPTION(C68K_1111_EX)
	RET(34)
}

#ifdef BUILD_NCDZPSP

OP(FAC0)
{
	extern void cdrom_load_files(void);

	cdrom_load_files();
	RET(400)
}

OP(FAC1)
{
	extern void neogeo_cdda_control(void);

	neogeo_cdda_control();
	RET(200)
}

OP(FAC2)
{
	extern void neogeo_bios_exit(void);

	neogeo_bios_exit();
	RET(200)
}

#endif /* BUILD_NCDZPSP */
