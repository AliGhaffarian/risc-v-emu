#include "cpu.h"
#include "unity.h"
#include <stdlib.h>

// NOLINTBEGIN(readability-magic-numbers)

void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    // clean stuff up here
}

void test_decode_imm_u(void)
{
    uint64_t got = decode_imm_u(0xf1'f1'af'ff);
    TEST_ASSERT_EQUAL_HEX64(0xf1'f1'a0'00, got);
}

void test_decode_imm_j(void)
{
    uint64_t expected_imm1             = 0xff;
    uint64_t expected_imm2             = 0b0;
    uint64_t expected_imm3             = 0b10'0101'1010;
    uint64_t expected_imm4             = 0b1;
    uint64_t expected_extentended_bits = 0xff'ff'ff'ff'ff'f0'00'00;

    uint64_t ins = (expected_imm1 << 12) | (expected_imm2 << 20) |
                   (expected_imm3 << 21) | (expected_imm4 << 31);

    uint64_t got = decode_imm_j(ins);

    uint64_t got_imm1 = (ins >> 12) & 0xff;
    uint64_t got_imm2 = (ins >> 20) & 0b1;
    uint64_t got_imm3 = (ins >> 21) & 0b11'1111'1111;
    uint64_t got_imm4 = (ins >> 31) & 0b1;

    TEST_ASSERT_EQUAL_HEX64(expected_imm1, got_imm1);
    TEST_ASSERT_EQUAL_HEX64(expected_imm2, got_imm2);
    TEST_ASSERT_EQUAL_HEX64(expected_imm3, got_imm3);
    TEST_ASSERT_EQUAL_HEX64(expected_imm4, got_imm4);
    TEST_ASSERT_EQUAL_HEX64(
        expected_extentended_bits, got & expected_extentended_bits);
}

void test_decode_imm_b(void)
{
    uint64_t expected_imm1             = 0b1;
    uint64_t expected_imm2             = 0b1011;
    uint64_t expected_imm3             = 0b100101;
    uint64_t expected_imm4             = 0b1;
    uint64_t expected_extentended_bits = 0xff'ff'ff'ff'ff'f0'00'00;

    uint64_t ins = (expected_imm1 << 7) | (expected_imm2 << 8) |
                   (expected_imm3 << 25) | (expected_imm4 << 31);

    uint64_t got = decode_imm_b(ins);

    uint64_t got_imm0 = got & 0b1;
    uint64_t got_imm1 = (got >> 11) & 0b1;
    uint64_t got_imm2 = (got >> 1) & 0b1111;
    uint64_t got_imm3 = (got >> 5) & 0b111111;
    uint64_t got_imm4 = (got >> 12) & 0b1;

    TEST_ASSERT_EQUAL_HEX64(0, got_imm0);
    TEST_ASSERT_EQUAL_HEX64(expected_imm1, got_imm1);
    TEST_ASSERT_EQUAL_HEX64(expected_imm2, got_imm2);
    TEST_ASSERT_EQUAL_HEX64(expected_imm3, got_imm3);
    TEST_ASSERT_EQUAL_HEX64(expected_imm4, got_imm4);
    TEST_ASSERT_EQUAL_HEX64(
        expected_extentended_bits, got & expected_extentended_bits);
}

void test_decode_imm_s(void)
{
    uint64_t expected_imm1             = 0b11001;
    uint64_t expected_imm2             = 0b101'0001;
    uint64_t expected_extentended_bits = 0xff'ff'ff'ff'ff'f0'00'00;

    uint64_t ins = (expected_imm1 << 7) | (expected_imm2 << 25);

    uint64_t got = decode_imm_s(ins);

    uint64_t got_imm1 = (got >> 0) & 0b1'1111;
    uint64_t got_imm2 = (got >> 5) & 0b111'1111;

    TEST_ASSERT_EQUAL_HEX64(expected_imm1, got_imm1);
    TEST_ASSERT_EQUAL_HEX64(expected_imm2, got_imm2);
    TEST_ASSERT_EQUAL_HEX64(
        expected_extentended_bits, got & expected_extentended_bits);
}

void test_decode_imm_i(void)
{
    uint64_t expected_imm1             = 0b1010'1111'1001;
    uint64_t expected_extentended_bits = 0xff'ff'ff'ff'ff'f0'00'00;

    uint64_t ins = (expected_imm1 << 20);

    uint64_t got = decode_imm_s(ins);

    uint64_t got_imm1 = (got >> 0) & 0b1111'1111'1111;

    TEST_ASSERT_EQUAL_HEX64(expected_imm1, got_imm1);
    TEST_ASSERT_EQUAL_HEX64(
        expected_extentended_bits, got & expected_extentended_bits);
}

void test_decode_rd()
{
    uint64_t expected_rd = 2;
    uint32_t ins         = (expected_rd << 7);

    uint7b_t got = decode_rd(ins);

    TEST_ASSERT_EQUAL_INT8(expected_rd, got);
}

void test_decode_rs1()
{
    uint64_t expected_rs1 = 2;
    uint32_t ins          = (expected_rs1 << 15);

    uint7b_t got = decode_rs1(ins);

    TEST_ASSERT_EQUAL_INT8(expected_rs1, got);
}

void test_decode_rs2()
{
    uint64_t expected_rs2 = 2;
    uint32_t ins          = (expected_rs2 << 20);

    uint7b_t got = decode_rs2(ins);

    TEST_ASSERT_EQUAL_INT8(expected_rs2, got);
}
void test_decode_func3(void)
{
    uint64_t expected_func3 = 0b111;
    uint32_t ins            = (expected_func3 << 12);

    uint3b_t got = decode_func3(ins);

    TEST_ASSERT_EQUAL_INT8(expected_func3, got);
}

void test_decode_func7(void)
{
    uint64_t expected_func7 = 0b11111;
    uint32_t ins            = (expected_func7 << 25);

    uint3b_t got = decode_func7(ins);

    TEST_ASSERT_EQUAL_INT8(expected_func7, got);
}

void test_execute_addi(void)
{
    struct decoded_rv64_base_ins_i *ins = calloc(1, sizeof(*ins));
    _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

    ins->rd  = 5;
    ins->rs1 = 7;
    ins->imm = 0xff;

    init_rv64_cpu(&cpu);

    cpu.regs[7] = 5;
    cpu.regs[5] = 2;

    execute_addi(&cpu, (void **)&ins);

    TEST_ASSERT_EQUAL_INT64((0xff + 5), cpu.regs[5]);
    TEST_ASSERT_NULL(ins);
}

void test_execute_slti(void)
{
    struct decoded_rv64_base_ins_i *ins = calloc(1, sizeof(*ins));
    _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

    ins->rd  = 5;
    ins->rs1 = 7;
    ins->imm = ((uint64_t)-1);

    init_rv64_cpu(&cpu);

    cpu.regs[7] = 5;
    cpu.regs[5] = 2;

    execute_slti(&cpu, (void **)&ins);

    TEST_ASSERT_EQUAL_INT64(0, cpu.regs[5]);
    TEST_ASSERT_NULL(ins);
}
void test_execute_sltiu(void)
{
    struct decoded_rv64_base_ins_i *ins = calloc(1, sizeof(*ins));
    _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

    ins->rd  = 5;
    ins->rs1 = 7;
    ins->imm = ((uint64_t)-1);

    init_rv64_cpu(&cpu);

    cpu.regs[7] = 5;
    cpu.regs[5] = 2;

    execute_sltiu(&cpu, (void **)&ins);

    TEST_ASSERT_EQUAL_INT64(1, cpu.regs[5]);
    TEST_ASSERT_NULL(ins);
}

void test_execute_andi(void)
{
    struct decoded_rv64_base_ins_i *ins = calloc(1, sizeof(*ins));
    _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

    ins->rd  = 5;
    ins->rs1 = 7;
    ins->imm = 0xff;

    init_rv64_cpu(&cpu);

    cpu.regs[7] = 5;
    cpu.regs[5] = 2;

    execute_andi(&cpu, (void **)&ins);

    TEST_ASSERT_EQUAL_HEX64(0xff & 5, cpu.regs[5]);
    TEST_ASSERT_NULL(ins);
}

void test_execute_ori(void)
{
    struct decoded_rv64_base_ins_i *ins = calloc(1, sizeof(*ins));
    _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

    ins->rd  = 5;
    ins->rs1 = 7;
    ins->imm = 0xff;

    init_rv64_cpu(&cpu);

    cpu.regs[7] = 5;
    cpu.regs[5] = 2;

    execute_ori(&cpu, (void **)&ins);

    TEST_ASSERT_EQUAL_HEX64(0xff | 5, cpu.regs[5]);
    TEST_ASSERT_NULL(ins);
}

void test_execute_xori(void)
{
    struct decoded_rv64_base_ins_i *ins = calloc(1, sizeof(*ins));
    _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

    ins->rd  = 5;
    ins->rs1 = 7;
    ins->imm = 0xff;

    init_rv64_cpu(&cpu);

    cpu.regs[7] = 5;
    cpu.regs[5] = 2;

    execute_xori(&cpu, (void **)&ins);

    TEST_ASSERT_EQUAL_HEX64(0xff ^ 5, cpu.regs[5]);
    TEST_ASSERT_NULL(ins);
}

void test_execute_srlai(void)
{
    // arithmetic
    {
        struct decoded_rv64_base_ins_i *ins = calloc(1, sizeof(*ins));
        _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

        ins->rd  = 5;
        ins->rs1 = 7;

        uint64_t shamt      = 32;
        uint64_t shift_type = 1 << (5 + 5);

        ins->imm = shamt + shift_type;

        init_rv64_cpu(&cpu);

        cpu.regs[7] = 0x80'aa'bb'cc'dd'ee'ff'11;
        cpu.regs[5] = 2;

        execute_srlai(&cpu, (void **)&ins);

        TEST_ASSERT_EQUAL_HEX64(0xff'ff'ff'ff'80'aa'bb'cc, cpu.regs[5]);
        TEST_ASSERT_NULL(ins);
    }

    // logical
    {
        struct decoded_rv64_base_ins_i *ins = calloc(1, sizeof(*ins));
        _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

        ins->rd  = 5;
        ins->rs1 = 7;

        uint64_t shamt      = 32;
        uint64_t shift_type = 0 << (5 + 5);

        ins->imm = shamt + shift_type;

        init_rv64_cpu(&cpu);

        cpu.regs[7] = 0x80'aa'bb'cc'dd'ee'ff'11;
        cpu.regs[5] = 2;

        execute_srlai(&cpu, (void **)&ins);

        TEST_ASSERT_EQUAL_HEX64(0x00'00'00'00'80'aa'bb'cc, cpu.regs[5]);
        TEST_ASSERT_NULL(ins);
    }
}

void test_execute_lui(void)
{
    struct decoded_rv64_base_ins_u *ins = calloc(1, sizeof(*ins));
    _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

    ins->rd  = 5;
    ins->imm = 0xff'00'0;

    init_rv64_cpu(&cpu);

    cpu.regs[5]          = -1;
    cpu.regs[REG_INX_PC] = 0xff'00;

    execute_lui(&cpu, (void **)&ins);

    TEST_ASSERT_EQUAL_HEX64(0xff'00'0, cpu.regs[5]);
    TEST_ASSERT_NULL(ins);
}

void test_execute_auipc(void)
{
    struct decoded_rv64_base_ins_u *ins = calloc(1, sizeof(*ins));
    _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

    ins->rd  = 5;
    ins->imm = 0xff'00'0;

    init_rv64_cpu(&cpu);

    cpu.regs[REG_INX_PC] = 0x0'ff'0;
    cpu.regs[5]          = -1;

    execute_auipc(&cpu, (void **)&ins);

    TEST_ASSERT_EQUAL_HEX64(0xff'ff'0, cpu.regs[5]);
    TEST_ASSERT_NULL(ins);
}

void test_execute_addiw(void)
{
    struct decoded_rv64_base_ins_i *ins = calloc(1, sizeof(*ins));
    _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

    ins->rd  = 5;
    ins->rs1 = 7;
    ins->imm = -2;

    init_rv64_cpu(&cpu);

    cpu.regs[7] = 1;
    cpu.regs[5] = 2;

    execute_addiw(&cpu, (void **)&ins);

    TEST_ASSERT_EQUAL_INT64(0xff'ff'ff'ff'ff'ff'ff'ff, cpu.regs[5]);
    TEST_ASSERT_NULL(ins);
}

void test_execute_slli(void)
{
    struct decoded_rv64_base_ins_i *ins = calloc(1, sizeof(*ins));
    _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

    ins->rd  = 5;
    ins->rs1 = 7;

    uint64_t shamt      = 16;
    uint64_t shift_type = 0 << (5 + 5);

    ins->imm = shamt + shift_type;

    init_rv64_cpu(&cpu);

    cpu.regs[7] = 0x80'aa'bb'cc'8d'ee'ff'11;
    cpu.regs[5] = 2;

    execute_slli(&cpu, (void **)&ins);

    TEST_ASSERT_EQUAL_HEX64(0xbb'cc'8d'ee'ff'11'00'00, cpu.regs[5]);
    TEST_ASSERT_NULL(ins);
}

void test_execute_slliw(void)
{
    struct decoded_rv64_base_ins_i *ins = calloc(1, sizeof(*ins));
    _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

    ins->rd  = 5;
    ins->rs1 = 7;

    uint64_t shamt      = 16;
    uint64_t shift_type = 0 << (5 + 5);

    ins->imm = shamt + shift_type;

    init_rv64_cpu(&cpu);

    cpu.regs[7] = 0x80'aa'bb'cc'8d'ee'ff'11;
    cpu.regs[5] = 2;

    execute_slliw(&cpu, (void **)&ins);

    TEST_ASSERT_EQUAL_HEX64(0xff'ff'ff'ff'ff'11'00'00, cpu.regs[5]);
    TEST_ASSERT_NULL(ins);
}

void test_execute_srlaiw(void)
{
    // arithmetic
    {
        struct decoded_rv64_base_ins_i *ins = calloc(1, sizeof(*ins));
        _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

        ins->rd  = 5;
        ins->rs1 = 7;

        uint64_t shamt      = 16;
        uint64_t shift_type = 1 << (5 + 5);

        ins->imm = shamt + shift_type;

        init_rv64_cpu(&cpu);

        cpu.regs[7] = 0x80'aa'bb'cc'8d'ee'ff'11;
        cpu.regs[5] = 2;

        execute_srlaiw(&cpu, (void **)&ins);

        TEST_ASSERT_EQUAL_HEX64(0xff'ff'ff'ff'ff'ff'8d'ee, cpu.regs[5]);
        TEST_ASSERT_NULL(ins);
    }

    // logical
    {
        struct decoded_rv64_base_ins_i *ins = calloc(1, sizeof(*ins));
        _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

        ins->rd  = 5;
        ins->rs1 = 7;

        uint64_t shamt      = 16;
        uint64_t shift_type = 0 << (5 + 5);

        ins->imm = shamt + shift_type;

        init_rv64_cpu(&cpu);

        cpu.regs[7] = 0x80'aa'bb'cc'8d'ee'ff'11;
        cpu.regs[5] = 2;

        execute_srlaiw(&cpu, (void **)&ins);

        TEST_ASSERT_EQUAL_HEX64(0x00'00'00'00'00'00'8d'ee, cpu.regs[5]);
        TEST_ASSERT_NULL(ins);
    }
}

void test_execute_add(void)
{
    struct decoded_rv64_base_ins_r *ins = calloc(1, sizeof(*ins));
    _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

    ins->rd  = 5;
    ins->rs1 = 7;
    ins->rs2 = 8;

    init_rv64_cpu(&cpu);

    cpu.regs[7] = 5;
    cpu.regs[8] = 50;
    cpu.regs[5] = 2;

    execute_add(&cpu, (void **)&ins);

    TEST_ASSERT_EQUAL_INT64(55, cpu.regs[5]);
    TEST_ASSERT_NULL(ins);
}

void test_execute_sub(void)
{
    struct decoded_rv64_base_ins_r *ins = calloc(1, sizeof(*ins));
    _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

    ins->rd  = 5;
    ins->rs1 = 7;
    ins->rs2 = 8;

    init_rv64_cpu(&cpu);

    cpu.regs[7] = 50;
    cpu.regs[8] = 5;
    cpu.regs[5] = 2;

    execute_sub(&cpu, (void **)&ins);

    TEST_ASSERT_EQUAL_INT64(45, cpu.regs[5]);
    TEST_ASSERT_NULL(ins);
}

void test_execute_slt(void)
{
    struct decoded_rv64_base_ins_r *ins = calloc(1, sizeof(*ins));
    _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

    ins->rd  = 5;
    ins->rs1 = 7;
    ins->rs2 = 3;

    init_rv64_cpu(&cpu);

    cpu.regs[7] = 5;
    cpu.regs[3] = -1;
    cpu.regs[5] = 2;

    execute_slt(&cpu, (void **)&ins);

    TEST_ASSERT_EQUAL_INT64(0, cpu.regs[5]);
    TEST_ASSERT_NULL(ins);
}
void test_execute_sltu(void)
{
    struct decoded_rv64_base_ins_r *ins = calloc(1, sizeof(*ins));
    _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

    ins->rd  = 5;
    ins->rs1 = 7;
    ins->rs2 = 3;

    init_rv64_cpu(&cpu);

    cpu.regs[7] = 5;
    cpu.regs[3] = -1;
    cpu.regs[5] = 2;

    execute_sltu(&cpu, (void **)&ins);

    TEST_ASSERT_EQUAL_INT64(1, cpu.regs[5]);
    TEST_ASSERT_NULL(ins);
}

void test_execute_and(void)
{
    struct decoded_rv64_base_ins_r *ins = calloc(1, sizeof(*ins));
    _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

    ins->rs2 = 3;
    ins->rs1 = 7;
    ins->rd  = 5;

    init_rv64_cpu(&cpu);
    cpu.regs[3] = 0xff;
    cpu.regs[7] = 5;
    cpu.regs[5] = 2;

    execute_and(&cpu, (void **)&ins);

    TEST_ASSERT_EQUAL_HEX64(0xff & 5, cpu.regs[5]);
    TEST_ASSERT_NULL(ins);
}

void test_execute_or(void)
{
    struct decoded_rv64_base_ins_r *ins = calloc(1, sizeof(*ins));
    _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

    ins->rd  = 5;
    ins->rs1 = 7;
    ins->rs2 = 3;

    init_rv64_cpu(&cpu);
    cpu.regs[3] = 0xff;
    cpu.regs[7] = 5;
    cpu.regs[5] = 2;

    execute_or(&cpu, (void **)&ins);

    TEST_ASSERT_EQUAL_HEX64(0xff | 5, cpu.regs[5]);
    TEST_ASSERT_NULL(ins);
}

void test_execute_xor(void)
{
    struct decoded_rv64_base_ins_r *ins = calloc(1, sizeof(*ins));
    _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

    ins->rd  = 5;
    ins->rs1 = 7;
    ins->rs2 = 3;

    init_rv64_cpu(&cpu);

    cpu.regs[3] = 0xff;
    cpu.regs[7] = 5;
    cpu.regs[5] = 2;

    execute_xor(&cpu, (void **)&ins);

    TEST_ASSERT_EQUAL_HEX64(0xff ^ 5, cpu.regs[5]);
    TEST_ASSERT_NULL(ins);
}

void test_execute_jal(void)
{
    struct decoded_rv64_base_ins_j *ins = calloc(1, sizeof(*ins));
    _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

    ins->rd  = 5;
    ins->imm = 0xaa;

    init_rv64_cpu(&cpu);

    cpu.regs[REG_INX_PC] = 0xff;
    cpu.regs[5]          = 5;

    execute_jal(&cpu, (void **)&ins);

    TEST_ASSERT_EQUAL_HEX64(0xff + 4, cpu.regs[5]);
    TEST_ASSERT_EQUAL_HEX64(0xaa + 0xff, cpu.regs[REG_INX_PC]);
    TEST_ASSERT_NULL(ins);
}

void test_execute_jalr(void)
{
    struct decoded_rv64_base_ins_i *ins = calloc(1, sizeof(*ins));
    _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

    ins->rd  = 5;
    ins->rs1 = 3;
    ins->imm = 0xaa;

    init_rv64_cpu(&cpu);

    cpu.regs[REG_INX_PC] = 0xff;
    cpu.regs[3]          = 0x11;
    cpu.regs[5]          = 5;

    execute_jalr(&cpu, (void **)&ins);

    TEST_ASSERT_EQUAL_HEX64(0xff + 4, cpu.regs[5]);
    TEST_ASSERT_EQUAL_HEX64(0xaa + 0x10, cpu.regs[REG_INX_PC]);
    TEST_ASSERT_NULL(ins);
}

void test_execute_sll(void)
{
    struct decoded_rv64_base_ins_r *ins = calloc(1, sizeof(*ins));
    _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

    ins->rd  = 5;
    ins->rs1 = 7;
    ins->rs2 = 2;

    uint64_t shamt      = 16;
    uint64_t shift_type = 0 << 5;

    ins->func7 = shift_type;

    init_rv64_cpu(&cpu);

    cpu.regs[2] = shamt;
    cpu.regs[7] = 0x80'aa'bb'cc'8d'ee'ff'11;
    cpu.regs[5] = 2;

    execute_sll(&cpu, (void **)&ins);

    TEST_ASSERT_EQUAL_HEX64(0xbb'cc'8d'ee'ff'11'00'00, cpu.regs[5]);
    TEST_ASSERT_NULL(ins);
}

void test_execute_srla(void)
{
    // arithmetic
    {
        struct decoded_rv64_base_ins_r *ins = calloc(1, sizeof(*ins));
        _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

        ins->rd  = 5;
        ins->rs1 = 7;
        ins->rs2 = 2;

        uint64_t shamt      = 32;
        uint64_t shift_type = 1 << 5;

        ins->func7 = shift_type;

        init_rv64_cpu(&cpu);

        cpu.regs[2] = shamt;
        cpu.regs[7] = 0x80'aa'bb'cc'dd'ee'ff'11;
        cpu.regs[5] = 2;

        execute_srla(&cpu, (void **)&ins);

        TEST_ASSERT_EQUAL_HEX64(0xff'ff'ff'ff'80'aa'bb'cc, cpu.regs[5]);
        TEST_ASSERT_NULL(ins);
    }

    // logical
    {
        struct decoded_rv64_base_ins_r *ins = calloc(1, sizeof(*ins));
        _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

        ins->rd  = 5;
        ins->rs1 = 7;
        ins->rs2 = 2;

        uint64_t shamt      = 32;
        uint64_t shift_type = 0 << 5;

        ins->func7 = shift_type;

        init_rv64_cpu(&cpu);

        cpu.regs[2] = shamt;
        cpu.regs[7] = 0x80'aa'bb'cc'dd'ee'ff'11;
        cpu.regs[5] = 2;

        execute_srla(&cpu, (void **)&ins);

        TEST_ASSERT_EQUAL_HEX64(0x00'00'00'00'80'aa'bb'cc, cpu.regs[5]);
        TEST_ASSERT_NULL(ins);
    }
}

void test_execute_addw(void)
{
    struct decoded_rv64_base_ins_r *ins = calloc(1, sizeof(*ins));
    _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

    ins->rd  = 5;
    ins->rs1 = 7;
    ins->rs2 = 2;

    init_rv64_cpu(&cpu);

    cpu.regs[2] = -2;
    cpu.regs[7] = 1;
    cpu.regs[5] = 2;

    execute_addw(&cpu, (void **)&ins);

    TEST_ASSERT_EQUAL_INT64(0xff'ff'ff'ff'ff'ff'ff'ff, cpu.regs[5]);
    TEST_ASSERT_NULL(ins);
}

void test_execute_sllw(void)
{
    struct decoded_rv64_base_ins_r *ins = calloc(1, sizeof(*ins));
    _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

    ins->rd  = 5;
    ins->rs1 = 7;
    ins->rs2 = 2;

    uint64_t shamt      = 16;
    uint64_t shift_type = 0 << 5;

    ins->func7 = shift_type;

    init_rv64_cpu(&cpu);

    cpu.regs[2] = shamt;
    cpu.regs[7] = 0x80'aa'bb'cc'8d'ee'ff'11;
    cpu.regs[5] = 2;

    execute_sllw(&cpu, (void **)&ins);

    TEST_ASSERT_EQUAL_HEX64(0xff'ff'ff'ff'ff'11'00'00, cpu.regs[5]);
    TEST_ASSERT_NULL(ins);
}

void test_execute_srlaw(void)
{
    // arithmetic
    {
        struct decoded_rv64_base_ins_r *ins = calloc(1, sizeof(*ins));
        _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

        ins->rd  = 5;
        ins->rs1 = 7;
        ins->rs2 = 2;

        uint64_t shamt      = 16;
        uint64_t shift_type = 1 << 5;

        ins->func7 = shift_type;

        init_rv64_cpu(&cpu);

        cpu.regs[2] = shamt;
        cpu.regs[7] = 0x80'aa'bb'cc'8d'ee'ff'11;
        cpu.regs[5] = 2;

        execute_srlaw(&cpu, (void **)&ins);

        TEST_ASSERT_EQUAL_HEX64(0xff'ff'ff'ff'ff'ff'8d'ee, cpu.regs[5]);
        TEST_ASSERT_NULL(ins);
    }

    // logical
    {
        struct decoded_rv64_base_ins_r *ins = calloc(1, sizeof(*ins));
        _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

        ins->rd  = 5;
        ins->rs1 = 7;
        ins->rs2 = 2;

        uint64_t shamt      = 16;
        uint64_t shift_type = 0 << 5;

        ins->func7 = shift_type;

        init_rv64_cpu(&cpu);

        cpu.regs[2] = shamt;
        cpu.regs[7] = 0x80'aa'bb'cc'8d'ee'ff'11;
        cpu.regs[5] = 2;

        execute_srlaw(&cpu, (void **)&ins);

        TEST_ASSERT_EQUAL_HEX64(0x00'00'00'00'00'00'8d'ee, cpu.regs[5]);
        TEST_ASSERT_NULL(ins);
    }
}

void test_execute_subw(void)
{
    struct decoded_rv64_base_ins_r *ins = calloc(1, sizeof(*ins));
    _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

    ins->rd  = 5;
    ins->rs1 = 7;
    ins->rs2 = 8;

    init_rv64_cpu(&cpu);

    cpu.regs[7] = 0xaa'bb'cc'dd'ff'aa'bb'fa;
    cpu.regs[8] = 0xcc'ee'ff'aa'00'00'00'f1;
    cpu.regs[5] = 2;

    execute_subw(&cpu, (void **)&ins);

    TEST_ASSERT_EQUAL_HEX64(0xff'ff'ff'ff'ff'aa'bb'09, cpu.regs[5]);
    TEST_ASSERT_NULL(ins);
}
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_decode_imm_b);
    RUN_TEST(test_decode_imm_u);
    RUN_TEST(test_decode_imm_j);
    RUN_TEST(test_decode_imm_s);

    RUN_TEST(test_decode_rd);
    RUN_TEST(test_decode_rs1);
    RUN_TEST(test_decode_rs2);
    RUN_TEST(test_decode_func3);
    RUN_TEST(test_decode_func7);

    RUN_TEST(test_execute_addi);
    RUN_TEST(test_execute_addiw);
    RUN_TEST(test_execute_slti);
    RUN_TEST(test_execute_sltiu);
    RUN_TEST(test_execute_andi);
    RUN_TEST(test_execute_ori);
    RUN_TEST(test_execute_xori);
    RUN_TEST(test_execute_slli);
    RUN_TEST(test_execute_slliw);
    RUN_TEST(test_execute_srlai);
    RUN_TEST(test_execute_srlaiw);
    RUN_TEST(test_execute_lui);
    RUN_TEST(test_execute_auipc);
    RUN_TEST(test_execute_add);
    RUN_TEST(test_execute_sub);
    RUN_TEST(test_execute_slt);
    RUN_TEST(test_execute_slti);
    RUN_TEST(test_execute_and);
    RUN_TEST(test_execute_or);
    RUN_TEST(test_execute_xor);
    RUN_TEST(test_execute_jal);
    RUN_TEST(test_execute_jalr);
    RUN_TEST(test_execute_sll);
    RUN_TEST(test_execute_srla);
    RUN_TEST(test_execute_addw);
    RUN_TEST(test_execute_sllw);
    RUN_TEST(test_execute_srlaw);
    RUN_TEST(test_execute_subw);
    return UNITY_END();
}

// NOLINTEND(readability-magic-numbers)
