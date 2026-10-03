#pragma once

/**
 * This file contains func3 and func7 aliases, extracted from risc-v unprivilege manual by notebooklm (although i probably should've used a script for determinism).
 */

// ==========================================
// func3 aliases
// ==========================================

// Jumps & Branches
#define JALR_FUNC3 0b000
#define BEQ_FUNC3  0b000
#define BNE_FUNC3  0b001
#define BLT_FUNC3  0b100
#define BGE_FUNC3  0b101
#define BLTU_FUNC3 0b110
#define BGEU_FUNC3 0b111

// Integer Loads
#define LB_FUNC3  0b000
#define LH_FUNC3  0b001
#define LW_FUNC3  0b010
#define LBU_FUNC3 0b100
#define LHU_FUNC3 0b101
#define LWU_FUNC3 0b110
#define LD_FUNC3  0b011

// Integer Stores
#define SB_FUNC3 0b000
#define SH_FUNC3 0b001
#define SW_FUNC3 0b010
#define SD_FUNC3 0b011

// OP-IMM Immediate Arithmetic & Shifts
#define ADDI_FUNC3  0b000
#define SLTI_FUNC3  0b010
#define SLTIU_FUNC3 0b011
#define XORI_FUNC3  0b100
#define ORI_FUNC3   0b110
#define ANDI_FUNC3  0b111
#define SLLI_FUNC3  0b001
#define SRLI_FUNC3  0b101
#define SRAI_FUNC3  0b101
#define ADDIW_FUNC3 0b000
#define SLLIW_FUNC3 0b001
#define SRLIW_FUNC3 0b101
#define SRAIW_FUNC3 0b101

// OP Register-Register Arithmetic
#define ADD_FUNC3  0b000
#define SUB_FUNC3  0b000
#define SLL_FUNC3  0b001
#define SLT_FUNC3  0b010
#define SLTU_FUNC3 0b011
#define XOR_FUNC3  0b100
#define SRL_FUNC3  0b101
#define SRA_FUNC3  0b101
#define OR_FUNC3   0b110
#define AND_FUNC3  0b111
#define ADDW_FUNC3 0b000
#define SUBW_FUNC3 0b000
#define SLLW_FUNC3 0b001
#define SRLW_FUNC3 0b101
#define SRAW_FUNC3 0b101

// System & Fence Instructions
#define FENCE_FUNC3   0b000
#define FENCE_I_FUNC3 0b001
#define ECALL_FUNC3   0b000
#define EBREAK_FUNC3  0b000

// Zicsr CSR Instructions
#define CSRRW_FUNC3  0b001
#define CSRRS_FUNC3  0b010
#define CSRRC_FUNC3  0b011
#define CSRRWI_FUNC3 0b101
#define CSRRSI_FUNC3 0b110
#define CSRRCI_FUNC3 0b111

// RV32M / RV64M Multiplication & Division
#define MUL_FUNC3    0b000
#define MULH_FUNC3   0b001
#define MULHSU_FUNC3 0b010
#define MULHU_FUNC3  0b011
#define DIV_FUNC3    0b100
#define DIVU_FUNC3   0b101
#define REM_FUNC3    0b110
#define REMU_FUNC3   0b111
#define MULW_FUNC3   0b000
#define DIVW_FUNC3   0b100
#define DIVUW_FUNC3  0b101
#define REMW_FUNC3   0b110
#define REMUW_FUNC3  0b111

// Atomic Extensions (A)
#define LR_W_FUNC3     0b010
#define SC_W_FUNC3     0b010
#define AMOADD_W_FUNC3 0b010
#define LR_D_FUNC3     0b011
#define SC_D_FUNC3     0b011
#define AMOADD_D_FUNC3 0b011

// Floating-Point Loads & Stores (F / D / Q / Zfh)
#define FLH_FUNC3 0b001
#define FSH_FUNC3 0b001
#define FLW_FUNC3 0b010
#define FSW_FUNC3 0b010
#define FLD_FUNC3 0b011
#define FSD_FUNC3 0b011
#define FLQ_FUNC3 0b100
#define FSQ_FUNC3 0b100

// ==========================================
// func7 aliases
// ==========================================

// Base R-Type & Shift Immediates
#define SLLI_FUNC7 0b0000000
#define SRLI_FUNC7 0b0000000
#define SRAI_FUNC7 0b0100000
#define ADD_FUNC7  0b0000000
#define SUB_FUNC7  0b0100000
#define SLL_FUNC7  0b0000000
#define SLT_FUNC7  0b0000000
#define SLTU_FUNC7 0b0000000
#define XOR_FUNC7  0b0000000
#define SRL_FUNC7  0b0000000
#define SRA_FUNC7  0b0100000
#define OR_FUNC7   0b0000000
#define AND_FUNC7  0b0000000

#define SLLIW_FUNC7 0b0000000
#define SRLIW_FUNC7 0b0000000
#define SRAIW_FUNC7 0b0100000
#define ADDW_FUNC7  0b0000000
#define SUBW_FUNC7  0b0100000
#define SLLW_FUNC7  0b0000000
#define SRLW_FUNC7  0b0000000
#define SRAW_FUNC7  0b0100000

// RV32M / RV64M Standard Extension
#define MUL_FUNC7    0b0000001
#define MULH_FUNC7   0b0000001
#define MULHSU_FUNC7 0b0000001
#define MULHU_FUNC7  0b0000001
#define DIV_FUNC7    0b0000001
#define DIVU_FUNC7   0b0000001
#define REM_FUNC7    0b0000001
#define REMU_FUNC7   0b0000001
#define MULW_FUNC7   0b0000001
#define DIVW_FUNC7   0b0000001
#define DIVUW_FUNC7  0b0000001
#define REMW_FUNC7   0b0000001
#define REMUW_FUNC7  0b0000001

// RV32F / RV64F Single-Precision Floating-Point
#define FADD_S_FUNC7   0b0000000
#define FSUB_S_FUNC7   0b0000100
#define FMUL_S_FUNC7   0b0001000
#define FDIV_S_FUNC7   0b0001100
#define FSQRT_S_FUNC7  0b0101100
#define FSGNJ_S_FUNC7  0b0010000
#define FMIN_S_FUNC7   0b0010100
#define FCVT_W_S_FUNC7 0b1100000
#define FMV_X_W_FUNC7  0b1110000
#define FEQ_S_FUNC7    0b1010000
#define FCVT_S_W_FUNC7 0b1101000
#define FMV_W_X_FUNC7  0b1111000

// RV32D / RV64D Double-Precision Floating-Point
#define FADD_D_FUNC7  0b0000001
#define FSUB_D_FUNC7  0b0000101
#define FMUL_D_FUNC7  0b0001001
#define FDIV_D_FUNC7  0b0001101
#define FSQRT_D_FUNC7 0b0101101
#define FSGNJ_D_FUNC7 0b0010001
#define FMIN_D_FUNC7  0b0010101
#define FEQ_D_FUNC7   0b1010001
#define FMV_X_D_FUNC7 0b1110001
#define FMV_D_X_FUNC7 0b1111001

// RV32Q / RV64Q Quad-Precision Floating-Point
#define FADD_Q_FUNC7  0b0000011
#define FSUB_Q_FUNC7  0b0000111
#define FMUL_Q_FUNC7  0b0001011
#define FDIV_Q_FUNC7  0b0001111
#define FSQRT_Q_FUNC7 0b0101111

// RV32Zfh Half-Precision Floating-Point
#define FADD_H_FUNC7  0b0000010
#define FSUB_H_FUNC7  0b0000110
#define FMUL_H_FUNC7  0b0001010
#define FDIV_H_FUNC7  0b0001110
#define FSQRT_H_FUNC7 0b0101110
