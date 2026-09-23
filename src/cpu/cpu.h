#pragma once

#include <stdint.h>

/**
 * This file holds the core cpu and base RV64I, plus some bindings
 *   intended to simplify the compliance with the doc.
 */

#define RV64_INS_LEN 32
#define XLEN         64

#define IMM_I_SIZE_BITS 12
#define IMM_B_SIZE_BITS 12
#define IMM_S_SIZE_BITS                                                        \
    IMM_B_SIZE_BITS /** S is a variant of B risc-v unpriviledge 2.3 */
#define IMM_J_SIZE_BITS 20
#define IMM_U_SIZE_BITS                                                        \
    IMM_J_SIZE_BITS /** U is a variant of J risc-v unpriviledge 2.3 */

#define SHIFT_SHAMT_BIT_SIZE            6
#define SHIFT_RIGHT_TYPE_BIT_POS_IN_IMM 10
#define SHIFTW_SHAMT_BIT_SIZE           5

typedef uint32_t rv64_instruction_t;

typedef uint8_t uint1b_t;
typedef uint8_t uint3b_t;
typedef uint8_t uint4b_t;
typedef uint8_t uint5b_t;
typedef uint8_t uint6b_t;
typedef uint8_t uint7b_t;
typedef uint16_t uint10b_t;
typedef uint16_t uint12b_t;
typedef uint32_t uint20b_t;

#define UINT7B_MAX 0b0111'1111
#define UINT3B_MAX 0b111

#define OPCODE_MAX UINT7B_MAX
#define FUNC3_MAX  UINT3B_MAX
#define FUNC7_MAX  UINT7B_MAX

/**
 * Rules for decode handlers:
 *  if *ret_decoded is NULL, allocate the memory for it
 */
extern int (*_Nonnull ins_decode_handlers[OPCODE_MAX])(
    rv64_instruction_t ins,
    void *_Nullable *_Nonnull ret_decoded,
    uint7b_t opcode);

/** single quotes are placed to represent the rv64 unpriviledge
 * chapter 36 opcode table. non base instruction are also included
 * for the sake of fully representing the table
 */
enum RV64_OPCODE {
    LOAD       = 0b00'000'11,
    LOAD_FP    = 0b00'001'11,
    CUSTOM_0   = 0b00'010'11,
    MISC_MEM   = 0b00'011'11,
    OP_IMM     = 0b00'100'11,
    AUIPC      = 0b00'101'11,
    OP_IMM_32  = 0b00'110'11,
    RESERVED_1 = 0b00'111'11,

    STORE      = 0b01'000'11,
    STORE_FP   = 0b01'001'11,
    CUSTOM_1   = 0b01'010'11,
    AMO        = 0b01'011'11,
    OP         = 0b01'100'11,
    LUI        = 0b01'101'11,
    OP_32      = 0b01'110'11,
    RESERVED_2 = 0b01'111'11,

    MADD       = 0b10'000'11,
    MSUB       = 0b10'001'11,
    NMSUB      = 0b10'010'11,
    NMADD      = 0b10'011'11,
    OP_FP      = 0b10'100'11,
    OP_V       = 0b10'101'11,
    CUSTOM_2   = 0b10'110'11,
    RESERVED_3 = 0b10'111'11,

    BRANCH     = 0b11'000'11,
    JALR       = 0b11'001'11,
    RESERVED_4 = 0b11'010'11,
    JAL        = 0b11'011'11,
    SYSTEM     = 0b11'100'11,
    OP_VE      = 0b11'101'11,
    CUSTOM_3   = 0b11'110'11,
    RESERVED_5 = 0b11'111'11,
};

#define BASE_REGS_NUM (32 + 1) // 32 x regs + pc
#define BASE_MEM_SIZE 1024
#define REG_INX_PC    32
struct rv64_cpu {
    uint64_t *_Nonnull regs;
    void *_Nonnull mem;
};

// TODO:
// Rules for execution handlers:
//  must steal decoded_ins
extern void (*_Nonnull execution_handlers[OPCODE_MAX][FUNC3_MAX][FUNC7_MAX])(
    struct rv64_cpu *_Nonnull cpu, void *_Nonnull *_Nonnull vdecoded_ins);

int init_rv64_cpu(struct rv64_cpu *_Nonnull cpu);

uint64_t bitmask_from_bit_size(uint64_t bit_size);
uint64_t
extract_bits_from_uint64(uint64_t num, uint64_t start, uint64_t bitmask);
uint64_t repeat_bit_in_num(uint64_t num, uint8_t start, uint8_t bit);

struct decoded_rv64_base_ins_r {
    uint7b_t opcode;
    uint5b_t rd;
    uint3b_t func3;
    uint5b_t rs1;
    uint5b_t rs2;
    uint7b_t func7;
};

struct decoded_rv64_base_ins_i {
    uint7b_t opcode;
    uint5b_t rd;
    uint3b_t func3;
    uint5b_t rs1;
    uint64_t imm;
};

struct decoded_rv64_base_ins_s {
    uint7b_t opcode;
    uint3b_t func3;
    uint5b_t rs1;
    uint5b_t rs2;
    uint64_t imm;
};

struct decoded_rv64_base_ins_b {
    uint7b_t opcode;
    uint3b_t func3;
    uint5b_t rs1;
    uint5b_t rs2;
    uint64_t
        imm; /** decoder must shift the decoded imm to left before inserting in this field [risc-v unpriviledge 2.3] */
};

struct decoded_rv64_base_ins_u {
    uint7b_t opcode;
    uint5b_t rd;
    uint64_t imm;
};

struct decoded_rv64_base_ins_j {
    uint7b_t opcode;
    uint5b_t rd;
    uint64_t imm;
};

uint5b_t decode_rd(rv64_instruction_t ins);
uint5b_t decode_rs1(rv64_instruction_t ins);
uint5b_t decode_rs2(rv64_instruction_t ins);

uint3b_t decode_func3(rv64_instruction_t ins);
uint3b_t decode_func7(rv64_instruction_t ins);

/** we require the opcode argument because the caller already has it, reuseing the opcode lifts the need for a recomputation
 *  thus less room for error */
int decode_ins_r(
    rv64_instruction_t ins,
    void *_Nullable *_Nonnull vret_decoded,
    uint7b_t opcode);
int decode_ins_i(
    rv64_instruction_t ins,
    void *_Nullable *_Nonnull vret_decoded,
    uint7b_t opcode);
int decode_ins_s(
    rv64_instruction_t ins,
    void *_Nullable *_Nonnull vret_decoded,
    uint7b_t opcode);
int decode_ins_b(
    rv64_instruction_t ins,
    void *_Nullable *_Nonnull vret_decoded,
    uint7b_t opcode);
int decode_ins_u(
    rv64_instruction_t ins,
    void *_Nullable *_Nonnull vret_decoded,
    uint7b_t opcode);
int decode_ins_j(
    rv64_instruction_t ins,
    void *_Nullable *_Nonnull vret_decoded,
    uint7b_t opcode);

uint64_t decode_imm_i(rv64_instruction_t ins);
uint64_t decode_imm_s(rv64_instruction_t ins);
uint64_t decode_imm_b(rv64_instruction_t ins);
uint64_t decode_imm_u(rv64_instruction_t ins);
uint64_t decode_imm_j(rv64_instruction_t ins);

void execute_addi(
    struct rv64_cpu *_Nonnull cpu, void *_Nonnull *_Nonnull vdecoded_ins_i);

void execute_slti(
    struct rv64_cpu *_Nonnull cpu, void *_Nonnull *_Nonnull vdecoded_ins_i);

void execute_sltiu(
    struct rv64_cpu *_Nonnull cpu, void *_Nonnull *_Nonnull vdecoded_ins_i);

void execute_andi(
    struct rv64_cpu *_Nonnull cpu, void *_Nonnull *_Nonnull vdecoded_ins_i);

void execute_ori(
    struct rv64_cpu *_Nonnull cpu, void *_Nonnull *_Nonnull vdecoded_ins_i);

void execute_xori(
    struct rv64_cpu *_Nonnull cpu, void *_Nonnull *_Nonnull vdecoded_ins_i);

void execute_slli(
    struct rv64_cpu *_Nonnull cpu, void *_Nonnull *_Nonnull vdecoded_ins_i);

void execute_srlai(
    struct rv64_cpu *_Nonnull cpu, void *_Nonnull *_Nonnull vdecoded_ins_i);

void execute_srli(
    struct rv64_cpu *_Nonnull cpu,
    struct decoded_rv64_base_ins_i *_Nonnull decoded_ins);

void execute_srai(
    struct rv64_cpu *_Nonnull cpu,
    struct decoded_rv64_base_ins_i *_Nonnull decoded_ins);
