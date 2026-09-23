#include "cpu.h"
#include "helper.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int default_decoder_handler(
    rv64_instruction_t /*unused*/,
    void *_Nullable *_Nonnull /*unused*/,
    uint7b_t /*unused*/)
{
    return ENOTSUP;
}

int (*ins_decode_handlers[UINT7B_MAX])(
    rv64_instruction_t ins,
    void *_Nullable *_Nonnull ret_decoded,
    uint7b_t opcode) = {[0 ... UINT7B_MAX - 1] = default_decoder_handler};

void __attribute__((constructor())) init_ins_decode_handlers(void)
{
    ins_decode_handlers[LOAD]      = decode_ins_i;
    ins_decode_handlers[STORE]     = decode_ins_s;
    ins_decode_handlers[OP_IMM]    = decode_ins_i;
    ins_decode_handlers[LUI]       = decode_ins_u;
    ins_decode_handlers[OP]        = decode_ins_r;
    ins_decode_handlers[JAL]       = decode_ins_j;
    ins_decode_handlers[JALR]      = decode_ins_i;
    ins_decode_handlers[MISC_MEM]  = decode_ins_i;
    ins_decode_handlers[SYSTEM]    = decode_ins_i;
    ins_decode_handlers[OP_IMM_32] = decode_ins_i;
    ins_decode_handlers[AUIPC]     = decode_ins_u;
    ins_decode_handlers[OP_32]     = decode_ins_r;
}

uint64_t bitmask_from_bit_size(uint64_t bit_size)
{
    uint64_t ret = 0;

    for(int i = 0; i < bit_size; i++) {
        ret |= (1 << i);
    }

    return ret;
}

uint64_t
extract_bits_from_uint64(uint64_t num, uint64_t start, uint64_t bitmask)
{
    uint64_t ret = 0;

    ret = num >> start;

    ret &= bitmask;

    return ret;
}

uint64_t repeat_bit_in_num(uint64_t num, uint8_t start, uint8_t bit)
{
    for(int i = start; i < XLEN; i++) {
        num |= ((uint64_t)bit << i);
    }

    return num;
}

uint64_t decode_imm_i(rv64_instruction_t ins)
{
    uint64_t ret     = 0;
    uint8_t sign_bit = ins >> (RV64_INS_LEN - 1);

    ret = ins >> 20;

    ret = repeat_bit_in_num(ret, IMM_I_SIZE_BITS, sign_bit);

    return ret;
}

static const uint8_t s_imm1_size_bits = 5;
static const uint8_t s_imm1_bitpos    = 7;

static const uint8_t s_imm2_size_bits = 7;
static const uint8_t s_imm2_bitpos    = 25;
uint64_t decode_imm_s(rv64_instruction_t ins)
{
    const uint64_t s_imm1_bitmask = bitmask_from_bit_size(s_imm1_size_bits);
    const uint64_t s_imm2_bitmask = bitmask_from_bit_size(s_imm2_size_bits);
    uint64_t ret                  = 0;
    uint8_t sign_bit              = ins >> (RV64_INS_LEN - 1);
    uint64_t imm1 =
        extract_bits_from_uint64(ins, s_imm1_bitpos, s_imm1_bitmask);
    uint64_t imm2 =
        extract_bits_from_uint64(ins, s_imm2_bitpos, s_imm2_bitmask);

    ret = imm1 + (imm2 << s_imm1_size_bits);

    ret = repeat_bit_in_num(ret, IMM_I_SIZE_BITS, sign_bit);

    return ret;
}

static const uint8_t b_imm1_size_bits      = 1;
static const uint8_t b_imm1_encoded_bitpos = 7;
static const uint8_t b_imm1_real_bitpos    = 11;

static const uint8_t b_imm2_size_bits      = 4;
static const uint8_t b_imm2_encoded_bitpos = 8;
static const uint8_t b_imm2_real_bitpos    = 1;

static const uint8_t b_imm3_size_bits      = 6;
static const uint8_t b_imm3_encoded_bitpos = 25;
static const uint8_t b_imm3_real_bitpos    = 5;

static const uint8_t b_imm4_size_bits      = 1;
static const uint8_t b_imm4_encoded_bitpos = 31;
static const uint8_t b_imm4_real_bitpos    = 12;
uint64_t decode_imm_b(rv64_instruction_t ins)
{
    const uint64_t b_imm1_bitmask = bitmask_from_bit_size(b_imm1_size_bits);
    const uint64_t b_imm2_bitmask = bitmask_from_bit_size(b_imm2_size_bits);
    const uint64_t b_imm3_bitmask = bitmask_from_bit_size(b_imm3_size_bits);
    const uint64_t b_imm4_bitmask = bitmask_from_bit_size(b_imm4_size_bits);

    uint64_t ret     = 0;
    uint8_t sign_bit = ins >> (RV64_INS_LEN - 1);

    uint64_t imm1 =
        extract_bits_from_uint64(ins, b_imm1_encoded_bitpos, b_imm1_bitmask);
    uint64_t imm2 =
        extract_bits_from_uint64(ins, b_imm2_encoded_bitpos, b_imm2_bitmask);
    uint64_t imm3 =
        extract_bits_from_uint64(ins, b_imm3_encoded_bitpos, b_imm3_bitmask);
    uint64_t imm4 =
        extract_bits_from_uint64(ins, b_imm4_encoded_bitpos, b_imm4_bitmask);

    ret = (imm1 << b_imm1_real_bitpos) + (imm2 << b_imm2_real_bitpos) +
          (imm3 << b_imm3_real_bitpos) + (imm4 << b_imm4_real_bitpos);

    ret = repeat_bit_in_num(ret, IMM_B_SIZE_BITS, sign_bit);

    return ret;
}

static const uint8_t u_imm1_size_bits      = 20;
static const uint8_t u_imm1_encoded_bitpos = 12;
static const uint8_t u_imm1_real_bitpos    = 12;
uint64_t decode_imm_u(rv64_instruction_t ins)
{
    const uint64_t u_imm1_bitmask = bitmask_from_bit_size(u_imm1_size_bits);

    uint64_t ret = 0;

    uint64_t imm1 =
        extract_bits_from_uint64(ins, u_imm1_encoded_bitpos, u_imm1_bitmask);

    ret = (imm1 << u_imm1_real_bitpos);

    return ret;
}

static const uint8_t j_imm1_size_bits      = 8;
static const uint8_t j_imm1_encoded_bitpos = 12;
static const uint8_t j_imm1_real_bitpos    = 12;

static const uint8_t j_imm2_size_bits      = 1;
static const uint8_t j_imm2_encoded_bitpos = 20;
static const uint8_t j_imm2_real_bitpos    = 11;

static const uint8_t j_imm3_size_bits      = 10;
static const uint8_t j_imm3_encoded_bitpos = 21;
static const uint8_t j_imm3_real_bitpos    = 1;

static const uint8_t j_imm4_size_bits      = 1;
static const uint8_t j_imm4_encoded_bitpos = 31;
static const uint8_t j_imm4_real_bitpos    = 20;
uint64_t decode_imm_j(rv64_instruction_t ins)
{
    const uint64_t j_imm1_bitmask = bitmask_from_bit_size(j_imm1_size_bits);
    const uint64_t j_imm2_bitmask = bitmask_from_bit_size(j_imm2_size_bits);
    const uint64_t j_imm3_bitmask = bitmask_from_bit_size(j_imm3_size_bits);
    const uint64_t j_imm4_bitmask = bitmask_from_bit_size(j_imm4_size_bits);

    uint64_t ret     = 0;
    uint8_t sign_bit = ins >> (RV64_INS_LEN - 1);

    uint64_t imm1 =
        extract_bits_from_uint64(ins, j_imm1_encoded_bitpos, j_imm1_bitmask);
    uint64_t imm2 =
        extract_bits_from_uint64(ins, j_imm2_encoded_bitpos, j_imm2_bitmask);
    uint64_t imm3 =
        extract_bits_from_uint64(ins, j_imm3_encoded_bitpos, j_imm3_bitmask);
    uint64_t imm4 =
        extract_bits_from_uint64(ins, j_imm4_encoded_bitpos, j_imm4_bitmask);

    ret = (imm1 << j_imm1_real_bitpos) + (imm2 << j_imm2_real_bitpos) +
          (imm3 << j_imm3_real_bitpos) + (imm4 << j_imm4_real_bitpos);

    ret = repeat_bit_in_num(ret, IMM_J_SIZE_BITS, sign_bit);

    return ret;
}

const uint64_t rd_ind_size_bits  = 5;
const uint64_t rd_encoded_bitpos = 7;
uint5b_t decode_rd(rv64_instruction_t ins)
{
    uint64_t bitmask = bitmask_from_bit_size(rd_ind_size_bits);
    return extract_bits_from_uint64(ins, rd_encoded_bitpos, bitmask);
}

const uint64_t rs1_ind_size_bits  = 5;
const uint64_t rs1_encoded_bitpos = 15;
uint5b_t decode_rs1(rv64_instruction_t ins)
{
    uint64_t bitmask = bitmask_from_bit_size(rs1_ind_size_bits);
    return extract_bits_from_uint64(ins, rs1_encoded_bitpos, bitmask);
}

const uint64_t rs2_ind_size_bits  = 5;
const uint64_t rs2_encoded_bitpos = 20;
uint5b_t decode_rs2(rv64_instruction_t ins)
{
    uint64_t bitmask = bitmask_from_bit_size(rs2_ind_size_bits);
    return extract_bits_from_uint64(ins, rs2_encoded_bitpos, bitmask);
}

const uint64_t func3_size_bits       = 3;
const uint64_t func3_encoded_bit_pos = 12;
uint3b_t decode_func3(rv64_instruction_t ins)
{
    uint64_t bitmask = bitmask_from_bit_size(func3_size_bits);
    return extract_bits_from_uint64(ins, func3_encoded_bit_pos, bitmask);
}

const uint64_t func7_size_bits       = 7;
const uint64_t func7_encoded_bit_pos = 25;
uint3b_t decode_func7(rv64_instruction_t ins)
{
    uint64_t bitmask = bitmask_from_bit_size(func7_size_bits);
    return extract_bits_from_uint64(ins, func7_encoded_bit_pos, bitmask);
}

int decode_ins_r(
    rv64_instruction_t ins,
    void *_Nullable *_Nonnull vret_decoded,
    uint7b_t opcode)
{
    assert(vret_decoded);

    struct decoded_rv64_base_ins_r **ret_decoded =
        (struct decoded_rv64_base_ins_r **)vret_decoded;

    if(!(*ret_decoded)) {
        *ret_decoded = malloc(sizeof(struct decoded_rv64_base_ins_r));
    }
    if(!(*ret_decoded)) {
        return ENOMEM;
    }

    (*ret_decoded)->opcode = opcode;
    (*ret_decoded)->rd     = decode_rd(ins);
    (*ret_decoded)->rs1    = decode_rs1(ins);
    (*ret_decoded)->rs2    = decode_rs2(ins);
    (*ret_decoded)->func3  = decode_func3(ins);
    (*ret_decoded)->func7  = decode_func7(ins);

    return 0;
}
int decode_ins_i(
    rv64_instruction_t ins,
    void *_Nullable *_Nonnull vret_decoded,
    uint7b_t opcode)
{
    assert(vret_decoded);

    struct decoded_rv64_base_ins_i **ret_decoded =
        (struct decoded_rv64_base_ins_i **)vret_decoded;

    if(!(*ret_decoded)) {
        *ret_decoded = malloc(sizeof(struct decoded_rv64_base_ins_i));
    }
    if(!(*ret_decoded)) {
        return ENOMEM;
    }

    (*ret_decoded)->opcode = opcode;
    (*ret_decoded)->rd     = decode_rd(ins);
    (*ret_decoded)->rs1    = decode_rs1(ins);
    (*ret_decoded)->func3  = decode_func3(ins);
    (*ret_decoded)->imm    = decode_imm_i(ins);

    return 0;
}
int decode_ins_s(
    rv64_instruction_t ins,
    void *_Nullable *_Nonnull vret_decoded,
    uint7b_t opcode)
{
    assert(vret_decoded);

    struct decoded_rv64_base_ins_s **ret_decoded =
        (struct decoded_rv64_base_ins_s **)vret_decoded;

    if(!(*ret_decoded)) {
        *ret_decoded = malloc(sizeof(struct decoded_rv64_base_ins_i));
    }
    if(!(*ret_decoded)) {
        return ENOMEM;
    }

    (*ret_decoded)->opcode = opcode;
    (*ret_decoded)->rs1    = decode_rs1(ins);
    (*ret_decoded)->rs2    = decode_rs2(ins);
    (*ret_decoded)->func3  = decode_func3(ins);
    (*ret_decoded)->imm    = decode_imm_i(ins);

    return 0;
}
int decode_ins_b(
    rv64_instruction_t ins,
    void *_Nullable *_Nonnull vret_decoded,
    uint7b_t opcode)
{
    assert(vret_decoded);

    struct decoded_rv64_base_ins_b **ret_decoded =
        (struct decoded_rv64_base_ins_b **)vret_decoded;

    if(!(*ret_decoded)) {
        *ret_decoded = malloc(sizeof(struct decoded_rv64_base_ins_b));
    }
    if(!(*ret_decoded)) {
        return ENOMEM;
    }

    (*ret_decoded)->opcode = opcode;
    (*ret_decoded)->rs1    = decode_rs1(ins);
    (*ret_decoded)->rs2    = decode_rs2(ins);
    (*ret_decoded)->func3  = decode_func3(ins);
    (*ret_decoded)->imm    = decode_imm_b(ins);

    return 0;
}
int decode_ins_u(
    rv64_instruction_t ins,
    void *_Nullable *_Nonnull vret_decoded,
    uint7b_t opcode)
{
    assert(vret_decoded);

    struct decoded_rv64_base_ins_u **ret_decoded =
        (struct decoded_rv64_base_ins_u **)vret_decoded;

    if(!(*ret_decoded)) {
        *ret_decoded = malloc(sizeof(struct decoded_rv64_base_ins_b));
    }
    if(!(*ret_decoded)) {
        return ENOMEM;
    }

    (*ret_decoded)->opcode = opcode;
    (*ret_decoded)->rd     = decode_rd(ins);
    (*ret_decoded)->imm    = decode_imm_u(ins);

    return 0;
}
int decode_ins_j(
    rv64_instruction_t ins,
    void *_Nullable *_Nonnull vret_decoded,
    uint7b_t opcode)
{
    assert(vret_decoded);

    struct decoded_rv64_base_ins_j **ret_decoded =
        (struct decoded_rv64_base_ins_j **)vret_decoded;

    if(!(*ret_decoded)) {
        *ret_decoded = malloc(sizeof(struct decoded_rv64_base_ins_b));
    }
    if(!(*ret_decoded)) {
        return ENOMEM;
    }

    (*ret_decoded)->opcode = opcode;
    (*ret_decoded)->rd     = decode_rd(ins);
    (*ret_decoded)->imm    = decode_imm_j(ins);

    return 0;
}

int init_rv64_cpu(struct rv64_cpu *_Nonnull cpu)
{
    cpu->regs = calloc(1, BASE_REGS_NUM);
    if(!cpu->regs) {
        return ENOMEM;
    }

    cpu->mem = calloc(1, BASE_MEM_SIZE);
    if(!cpu->mem) {
        return ENOMEM;
    }

    return 0;
}

void execute_addi(
    struct rv64_cpu *_Nonnull cpu, void *_Nonnull *_Nonnull vdecoded_ins_i)
{
    assert(cpu);
    assert(vdecoded_ins_i);
    assert(*vdecoded_ins_i);

    _cleanup_free_ struct decoded_rv64_base_ins_i *decoded_ins =
        MOVE(vdecoded_ins_i);

    cpu->regs[decoded_ins->rd] = cpu->regs[decoded_ins->rs1] + decoded_ins->imm;
}

void execute_slti(
    struct rv64_cpu *_Nonnull cpu, void *_Nonnull *_Nonnull vdecoded_ins_i)
{
    assert(cpu);
    assert(vdecoded_ins_i);
    assert(*vdecoded_ins_i);

    _cleanup_free_ struct decoded_rv64_base_ins_i *decoded_ins =
        MOVE(vdecoded_ins_i);

    cpu->regs[decoded_ins->rd] =
        (int64_t)cpu->regs[decoded_ins->rs1] < (int64_t)decoded_ins->imm ? 1
                                                                         : 0;
}

void execute_sltiu(
    struct rv64_cpu *_Nonnull cpu, void *_Nonnull *_Nonnull vdecoded_ins_i)
{
    assert(cpu);
    assert(vdecoded_ins_i);
    assert(*vdecoded_ins_i);

    _cleanup_free_ struct decoded_rv64_base_ins_i *decoded_ins =
        MOVE(vdecoded_ins_i);

    cpu->regs[decoded_ins->rd] =
        cpu->regs[decoded_ins->rs1] < decoded_ins->imm ? 1 : 0;
}

void execute_andi(
    struct rv64_cpu *_Nonnull cpu, void *_Nonnull *_Nonnull vdecoded_ins_i)
{
    assert(cpu);
    assert(vdecoded_ins_i);
    assert(*vdecoded_ins_i);

    _cleanup_free_ struct decoded_rv64_base_ins_i *decoded_ins =
        MOVE(vdecoded_ins_i);

    cpu->regs[decoded_ins->rd] = cpu->regs[decoded_ins->rs1] & decoded_ins->imm;
}

void execute_ori(
    struct rv64_cpu *_Nonnull cpu, void *_Nonnull *_Nonnull vdecoded_ins_i)
{
    assert(cpu);
    assert(vdecoded_ins_i);
    assert(*vdecoded_ins_i);

    _cleanup_free_ struct decoded_rv64_base_ins_i *decoded_ins =
        MOVE(vdecoded_ins_i);

    cpu->regs[decoded_ins->rd] = cpu->regs[decoded_ins->rs1] | decoded_ins->imm;
}

void execute_xori(
    struct rv64_cpu *_Nonnull cpu, void *_Nonnull *_Nonnull vdecoded_ins_i)

{
    assert(cpu);
    assert(vdecoded_ins_i);
    assert(*vdecoded_ins_i);

    _cleanup_free_ struct decoded_rv64_base_ins_i *decoded_ins =
        MOVE(vdecoded_ins_i);

    cpu->regs[decoded_ins->rd] = cpu->regs[decoded_ins->rs1] ^ decoded_ins->imm;
}

void execute_slli(
    struct rv64_cpu *_Nonnull cpu, void *_Nonnull *_Nonnull vdecoded_ins_i)
{
    assert(cpu);
    assert(vdecoded_ins_i);
    assert(*vdecoded_ins_i);

    _cleanup_free_ struct decoded_rv64_base_ins_i *decoded_ins =
        MOVE(vdecoded_ins_i);
    uint8_t shamt =
        decoded_ins->imm & bitmask_from_bit_size(SHIFT_SHAMT_BIT_SIZE);

    cpu->regs[decoded_ins->rd] = cpu->regs[decoded_ins->rs1] << shamt;
}

void execute_srlai(
    struct rv64_cpu *_Nonnull cpu, void *_Nonnull *_Nonnull vdecoded_ins_i)
{
    assert(cpu);
    assert(vdecoded_ins_i);
    assert(*vdecoded_ins_i);

    _cleanup_free_ struct decoded_rv64_base_ins_i *decoded_ins =
        MOVE(vdecoded_ins_i);

    uint64_t shift_type_bit_mask = 1 << SHIFT_RIGHT_TYPE_BIT_POS_IN_IMM;
    uint8_t do_arithmatic_shift =
        decoded_ins->imm & shift_type_bit_mask ? 1 : 0;

    if(do_arithmatic_shift) {
        execute_srai(cpu, decoded_ins);
    } else {
        execute_srli(cpu, decoded_ins);
    }
}

void execute_srli(
    struct rv64_cpu *_Nonnull cpu,
    struct decoded_rv64_base_ins_i *_Nonnull decoded_ins)
{
    uint8_t shamt =
        decoded_ins->imm & bitmask_from_bit_size(SHIFT_SHAMT_BIT_SIZE);
    cpu->regs[decoded_ins->rd] = cpu->regs[decoded_ins->rs1] >> shamt;
}

void execute_srai(
    struct rv64_cpu *_Nonnull cpu,
    struct decoded_rv64_base_ins_i *_Nonnull decoded_ins)
{
    uint8_t shamt =
        decoded_ins->imm & bitmask_from_bit_size(SHIFT_SHAMT_BIT_SIZE);

    uint64_t sign_bit =
        ((uint64_t)1 << (XLEN - 1)) & cpu->regs[decoded_ins->rs1];

    cpu->regs[decoded_ins->rd] = cpu->regs[decoded_ins->rs1];
    for(int i = 0; i < shamt; i++) {
        cpu->regs[decoded_ins->rd] = cpu->regs[decoded_ins->rd] >> 1;
        cpu->regs[decoded_ins->rd] |= sign_bit;
    }
}

void execute_lui(
    struct rv64_cpu *_Nonnull cpu, void *_Nonnull *_Nonnull vdecoded_ins_u)
{
    assert(cpu);
    assert(vdecoded_ins_u);
    assert(*vdecoded_ins_u);

    _cleanup_free_ struct decoded_rv64_base_ins_u *decoded_ins =
        MOVE(vdecoded_ins_u);

    cpu->regs[decoded_ins->rd] = decoded_ins->imm;
}

void execute_auipc(
    struct rv64_cpu *_Nonnull cpu, void *_Nonnull *_Nonnull vdecoded_ins_u)
{
    assert(cpu);
    assert(vdecoded_ins_u);
    assert(*vdecoded_ins_u);

    _cleanup_free_ struct decoded_rv64_base_ins_u *decoded_ins =
        MOVE(vdecoded_ins_u);

    cpu->regs[decoded_ins->rd] = decoded_ins->imm + cpu->regs[REG_INX_PC];
}
