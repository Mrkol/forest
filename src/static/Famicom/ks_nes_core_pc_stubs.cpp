#include "Famicom/ks_nes.h"
#include "types.h"

#define KSNES_STUB(name) void name() {}

void ksNesLinecntIrqDefault(void) {}
void ksNesLinecntIrqQD(void) {}
KSNES_STUB(ksNesInit01)
KSNES_STUB(ksNesInit03)
KSNES_STUB(ksNesInit04)
KSNES_STUB(ksNesInit05)
KSNES_STUB(ksNesInit07)
KSNES_STUB(ksNesInit09)
KSNES_STUB(ksNesInit12)
KSNES_STUB(ksNesInit13)
KSNES_STUB(ksNesInit15)
KSNES_STUB(ksNesInit18)
KSNES_STUB(ksNesInit1a)
KSNES_STUB(ksNesInit42)
KSNES_STUB(ksNesInit43)
KSNES_STUB(ksNesInit45)
KSNES_STUB(ksNesInit49)
KSNES_STUB(ksNesInit56)
KSNES_STUB(ksNesMainLoop2)
KSNES_STUB(ksNesInst_load16_imm)
KSNES_STUB(ksNesInst_load8_abs)
KSNES_STUB(ksNesInst_load8_absx)
KSNES_STUB(ksNesInst_load8_absy)
KSNES_STUB(ksNesInst_load8_zerop)
KSNES_STUB(ksNesInst_load8_dx)
KSNES_STUB(ksNesInst_load8_dxi)
KSNES_STUB(ksNesInst_load8_dyi)
KSNES_STUB(ksNesInst_lda_a1)
KSNES_STUB(ksNesInst_ldx_b6)
KSNES_STUB(ksNesInst_ldx_a2)
KSNES_STUB(ksNesInst_ldy_a0)
KSNES_STUB(ksNesInst_sta_85)
KSNES_STUB(ksNesInst_stx_86)
KSNES_STUB(ksNesInst_sty_84)
KSNES_STUB(ksNesInst_sta_95)
KSNES_STUB(ksNesInst_stx_96)
KSNES_STUB(ksNesInst_sty_94)
KSNES_STUB(ksNesInst_sta_8d)
KSNES_STUB(ksNesInst_stx_8e)
KSNES_STUB(ksNesInst_sty_8c)
KSNES_STUB(ksNesInst_sta_9d)
KSNES_STUB(ksNesInst_sta_99)
KSNES_STUB(ksNesInst_sta_81)
KSNES_STUB(ksNesInst_sta_91)
KSNES_STUB(ksNesInst_clc_18)
KSNES_STUB(ksNesInst_sec_38)
KSNES_STUB(ksNesInst_dex_ca)
KSNES_STUB(ksNesInst_inx_e8)
KSNES_STUB(ksNesInst_dey_88)
KSNES_STUB(ksNesInst_iny_c8)
KSNES_STUB(ksNesInst_txa_8a)
KSNES_STUB(ksNesInst_tya_98)
KSNES_STUB(ksNesInst_tax_aa)
KSNES_STUB(ksNesInst_tay_a8)
KSNES_STUB(ksNesInst_cmp_c1)
KSNES_STUB(ksNesInst_cpx_e0)
KSNES_STUB(ksNesInst_cpy_c0)
KSNES_STUB(ksNesInst_adc_61)
KSNES_STUB(ksNesInst_sbc_e1)
KSNES_STUB(ksNesInst_ora_01)
KSNES_STUB(ksNesInst_and_21)
KSNES_STUB(ksNesInst_bit_24)
KSNES_STUB(ksNesInst_eor_41)
KSNES_STUB(ksNesInst_inc_e6)
KSNES_STUB(ksNesInst_dec_c6)
KSNES_STUB(ksNesInst_asl_0a)
KSNES_STUB(ksNesInst_asl_06)
KSNES_STUB(ksNesInst_lsr_4a)
KSNES_STUB(ksNesInst_lsr_46)
KSNES_STUB(ksNesInst_rol_2a)
KSNES_STUB(ksNesInst_rol_26)
KSNES_STUB(ksNesInst_ror_6a)
KSNES_STUB(ksNesInst_ror_66)
KSNES_STUB(ksNesInst_bpl_10)
KSNES_STUB(ksNesInst_bmi_30)
KSNES_STUB(ksNesInst_bvc_50)
KSNES_STUB(ksNesInst_bvs_70)
KSNES_STUB(ksNesInst_bcc_90)
KSNES_STUB(ksNesInst_bcs_b0)
KSNES_STUB(ksNesInst_bne_d0)
KSNES_STUB(ksNesInst_beq_f0)
KSNES_STUB(ksNesInst_jsr_20)
KSNES_STUB(ksNesPush16_a1)
KSNES_STUB(ksNesInst_pha_48)
KSNES_STUB(ksNesInst_pla_68)
KSNES_STUB(ksNesInst_rts_60)
KSNES_STUB(ksNesPopPC)
KSNES_STUB(ksNesInst_jmp_4c)
KSNES_STUB(ksNesInst_jmp_6c)
KSNES_STUB(ksNesInst_brk_00)
KSNES_STUB(ksNesActivateIntrIRQ)
KSNES_STUB(ksNesActivateIntr)
KSNES_STUB(ksNesInst_php_08)
KSNES_STUB(ksNesInst_plp_28)
KSNES_STUB(ksNesInst_rti_40)
KSNES_STUB(ksNesInst_rti_40_2)
KSNES_STUB(ksNesInst_cli_58)
KSNES_STUB(ksNesInst_sei_78)
KSNES_STUB(ksNesInst_txs_9a)
KSNES_STUB(ksNesInst_tsx_ba)
KSNES_STUB(ksNesInst_cld_d8)
KSNES_STUB(ksNesInst_sed_f8)
KSNES_STUB(ksNesInst_clv_b8)
KSNES_STUB(ksNesInst_wdm_42)
KSNES_STUB(ksNesLinecntIrq04)
KSNES_STUB(ksNesLinecntIrq05Timer)
KSNES_STUB(ksNesLinecntIrq05Vcount)
KSNES_STUB(ksNesLinecntIrq49)
KSNES_STUB(ksNesLinecntIrq18)
KSNES_STUB(ksNesLinecntIrq19)
KSNES_STUB(ksNesLinecntIrq43)

void ksNesStoreWRAM() {}
void ksNesStoreBBRAM() {}
void ksNesLoadBBRAM() {}
void ksNesStorePPU() {}
void ksNesStoreIO() {}
void ksNesStore2000() {}
void ksNesStorePPURam() {}
void ksNesStore2004() {}
void ksNesStore2005() {}
void ksNesStore2006() {}
void ksNesStore2007ChrRom() {}
void ksNesStore4000() {}
void ksNesStore4003() {}
void ksNesStore4011() {}
void ksNesStore4014() {}
void ksNesStore4015() {}
void ksNesStore4016() {}
void ksNesStore4017() {}
void ksNesStoreInvalid() {}
void ksNesStoreQD_4020() {}
void ksNesStoreQD_4022() {}
void ksNesStoreQD_4023() {}
void ksNesStoreQD_4024() {}
void ksNesStoreQD_4025() {}
void ksNesStoreQD_4026() {}
void ksNesLoadInvalid() {}
void ksNesLoadIgnore() {}
void ksNesLoadWRAM() {}
void ksNesLoadPPU() {}
void ksNesLoadIO() {}
void ksNesLoad4015() {}
void ksNesLoad4017() {}
void ksNesLoad4016() {}
void ksNesLoad05_4000() {}
void ksNesLoad13_4000() {}
void ksNesStoreQDSound() {}
void ksNesStore01_8000() {}
void ksNesStore02_8000() {}
void ksNesStore03_6000() {}
void ksNesStore04_8000() {}
void ksNesStore04_a000() {}
void ksNesStore04_c000() {}
void ksNesStore04_e000() {}
void ksNesStore05_4000() {}
void ksNesStore05_5100() {}
void ksNesStore05_5101() {}
void ksNesStore05_5102() {}
void ksNesStore05_5104() {}
void ksNesStore05_5105() {}
void ksNesStore05_5106() {}
void ksNesStore05_5113() {}
void ksNesStore05_5120() {}
void ksNesStore05_5128() {}
void ksNesStore05_5130() {}
void ksNesStore07_8000() {}
void ksNesStore09_8000() {}
void ksNesStore09_a000() {}
void ksNesStore09_c000() {}
void ksNesStore09_e000() {}
void ksNesStore0a_8000() {}
void ksNesStore0a_a000() {}
void ksNesStore12_8000() {}
void ksNesStore12_a000() {}
void ksNesStore12_e000() {}
void ksNesStore13_4000() {}
void ksNesStore13_8000() {}
void ksNesStore13_c000() {}
void ksNesStore13_e000() {}
void ksNesStore16_8000() {}
void ksNesStore16_9000() {}
void ksNesStore16_a000() {}
void ksNesStore16_b000() {}
void ksNesStore17_a000() {}
void ksNesStore17_b000() {}
void ksNesStore17_e000() {}
void ksNesStore18_8000() {}
void ksNesStore18_a000() {}
void ksNesStore18_c000() {}
void ksNesStore18_e000() {}
void ksNesStore19_8000() {}
void ksNesStore19_a000() {}
void ksNesStore19_b000() {}
void ksNesStore19_e000() {}
void ksNesStore42_8000() {}
void ksNesStore43_c000() {}
void ksNesStore44_8000() {}
void ksNesStore44_c000() {}
void ksNesStore44_e000() {}
void ksNesStore45_8000() {}
void ksNesStore45_a000() {}
void ksNesStore46_8000() {}
void ksNesStore49_a000() {}
void ksNesStore49_c000() {}
void ksNesStore4b_8000() {}
void ksNesStore4b_a000() {}
void ksNesStore4b_e000() {}
void ksNesStore56_6000() {}
void ksNesStore57_6000() {}
void ksNesStore59_c000() {}
void ksNesStore5d_6000() {}
void ksNesStoreb8_6000() {}

u32 ksNesResetAsm(ksNesCommonWorkObj* work, ksNesStateObj* state)
{
    (void)work;
    (void)state;
    return 0;
}

void ksNesEmuFrameAsm(ksNesCommonWorkObj* work, ksNesStateObj* state)
{
    (void)work;
    (void)state;
}
