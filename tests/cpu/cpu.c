#include "cpu.h"
#include "generated_aliases.h"
#include "helper.h"
#include "io_dev.h"
#include "io_dev_memory.h"
#include "logger.h"
#include "unity.h"
#include <stdlib.h>
#include <string.h>

// NOLINTBEGIN(readability-magic-numbers)

struct sysbus *
simple_setup_sysbus_with_mem(const char *_Nullable mem_buf, uint64_t buf_len)
{
    struct sysbus *ret = calloc(1, sizeof(*ret));
    _cleanup_free_io_dev_meta_data_ struct io_dev_metadata *metadata = NULL;
    struct io_dev_memory *mem                                        = NULL;

    new_metadata_with_mem(&metadata, (uint8_t *)mem_buf, buf_len);
    metadata->parent = ret;
    mem              = new_io_dev_memory(&metadata);
    sysbus_memory_map_iodev(ret, (struct io_dev **)&mem);

    return ret;
}

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
    TEST_ASSERT_EQUAL_HEX64(0xff'ff'ff'ff'f1'f1'a0'00, got);
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

    uint64_t got = decode_imm_i(ins);

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

    init_rv64_cpu(&cpu, NULL, NULL);

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

    init_rv64_cpu(&cpu, NULL, NULL);

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

    init_rv64_cpu(&cpu, NULL, NULL);

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

    init_rv64_cpu(&cpu, NULL, NULL);

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

    init_rv64_cpu(&cpu, NULL, NULL);

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

    init_rv64_cpu(&cpu, NULL, NULL);

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

        init_rv64_cpu(&cpu, NULL, NULL);

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

        init_rv64_cpu(&cpu, NULL, NULL);

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

    init_rv64_cpu(&cpu, NULL, NULL);

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

    init_rv64_cpu(&cpu, NULL, NULL);

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

    init_rv64_cpu(&cpu, NULL, NULL);

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

    init_rv64_cpu(&cpu, NULL, NULL);

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

    init_rv64_cpu(&cpu, NULL, NULL);

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

        init_rv64_cpu(&cpu, NULL, NULL);

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

        init_rv64_cpu(&cpu, NULL, NULL);

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

    init_rv64_cpu(&cpu, NULL, NULL);

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

    init_rv64_cpu(&cpu, NULL, NULL);

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

    init_rv64_cpu(&cpu, NULL, NULL);

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

    init_rv64_cpu(&cpu, NULL, NULL);

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

    init_rv64_cpu(&cpu, NULL, NULL);
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

    init_rv64_cpu(&cpu, NULL, NULL);
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

    init_rv64_cpu(&cpu, NULL, NULL);

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

    init_rv64_cpu(&cpu, NULL, NULL);

    cpu.regs[REG_INX_PC] = 0xff;
    cpu.regs[5]          = 5;

    execute_jal(&cpu, (void **)&ins);

    TEST_ASSERT_EQUAL_HEX64(0xff + 4, cpu.regs[5]);
    TEST_ASSERT_EQUAL_HEX64(0xaa + 0xff, cpu.regs[REG_INX_PC]);
    TEST_ASSERT_EQUAL(1, cpu.i_flags.pc_updated);
    TEST_ASSERT_NULL(ins);
}

void test_execute_jalr(void)
{
    struct decoded_rv64_base_ins_i *ins = calloc(1, sizeof(*ins));
    _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

    ins->rd  = 5;
    ins->rs1 = 3;
    ins->imm = 0xaa;

    init_rv64_cpu(&cpu, NULL, NULL);

    cpu.regs[REG_INX_PC] = 0xff;
    cpu.regs[3]          = 0x11;
    cpu.regs[5]          = 5;

    execute_jalr(&cpu, (void **)&ins);

    TEST_ASSERT_EQUAL_HEX64(0xff + 4, cpu.regs[5]);
    TEST_ASSERT_EQUAL_HEX64(0xaa + 0x10, cpu.regs[REG_INX_PC]);
    TEST_ASSERT_EQUAL(1, cpu.i_flags.pc_updated);
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

    init_rv64_cpu(&cpu, NULL, NULL);

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

        init_rv64_cpu(&cpu, NULL, NULL);

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

        init_rv64_cpu(&cpu, NULL, NULL);

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

    init_rv64_cpu(&cpu, NULL, NULL);

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

    init_rv64_cpu(&cpu, NULL, NULL);

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

        init_rv64_cpu(&cpu, NULL, NULL);

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

        init_rv64_cpu(&cpu, NULL, NULL);

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

    init_rv64_cpu(&cpu, NULL, NULL);

    cpu.regs[7] = 0xaa'bb'cc'dd'ff'aa'bb'fa;
    cpu.regs[8] = 0xcc'ee'ff'aa'00'00'00'f1;
    cpu.regs[5] = 2;

    execute_subw(&cpu, (void **)&ins);

    TEST_ASSERT_EQUAL_HEX64(0xff'ff'ff'ff'ff'aa'bb'09, cpu.regs[5]);
    TEST_ASSERT_NULL(ins);
}

void test_execute_beq(void)
{
    // take the branch
    {
        struct decoded_rv64_base_ins_b *ins = calloc(1, sizeof(*ins));
        _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

        ins->rs1 = 7;
        ins->rs2 = 8;
        ins->imm = 0xf'00'00;

        init_rv64_cpu(&cpu, NULL, NULL);

        cpu.regs[REG_INX_PC] = 0xff'ff;
        cpu.regs[7]          = 1;
        cpu.regs[8]          = 1;

        execute_beq(&cpu, (void **)&ins);

        TEST_ASSERT_EQUAL_HEX64(0x0f'ff'ff, cpu.regs[REG_INX_PC]);
        TEST_ASSERT_EQUAL(1, cpu.i_flags.pc_updated);
        TEST_ASSERT_NULL(ins);
    }

    // don't take the branch
    {
        struct decoded_rv64_base_ins_b *ins = calloc(1, sizeof(*ins));
        _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

        ins->rs1 = 7;
        ins->rs2 = 8;
        ins->imm = 0xf'00'00;

        init_rv64_cpu(&cpu, NULL, NULL);

        cpu.regs[REG_INX_PC] = 0xff'ff;
        cpu.regs[7]          = 1;
        cpu.regs[8]          = -1;

        execute_beq(&cpu, (void **)&ins);

        TEST_ASSERT_EQUAL_HEX64(0x00'ff'ff, cpu.regs[REG_INX_PC]);
        TEST_ASSERT_EQUAL(0, cpu.i_flags.pc_updated);
        TEST_ASSERT_NULL(ins);
    }
}

void test_execute_bne(void)
{
    // take the branch
    {
        struct decoded_rv64_base_ins_b *ins = calloc(1, sizeof(*ins));
        _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

        ins->rs1 = 7;
        ins->rs2 = 8;
        ins->imm = 0xf'00'00;

        init_rv64_cpu(&cpu, NULL, NULL);

        cpu.regs[REG_INX_PC] = 0xff'ff;
        cpu.regs[7]          = 1;
        cpu.regs[8]          = -1;

        execute_bne(&cpu, (void **)&ins);

        TEST_ASSERT_EQUAL_HEX64(0x0f'ff'ff, cpu.regs[REG_INX_PC]);
        TEST_ASSERT_EQUAL(1, cpu.i_flags.pc_updated);
        TEST_ASSERT_NULL(ins);
    }

    // don't take the branch
    {
        struct decoded_rv64_base_ins_b *ins = calloc(1, sizeof(*ins));
        _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

        ins->rs1 = 7;
        ins->rs2 = 8;
        ins->imm = 0xf'00'00;

        init_rv64_cpu(&cpu, NULL, NULL);

        cpu.regs[REG_INX_PC] = 0xff'ff;
        cpu.regs[7]          = 1;
        cpu.regs[8]          = 1;

        execute_bne(&cpu, (void **)&ins);

        TEST_ASSERT_EQUAL_HEX64(0x00'ff'ff, cpu.regs[REG_INX_PC]);
        TEST_ASSERT_EQUAL(0, cpu.i_flags.pc_updated);
        TEST_ASSERT_NULL(ins);
    }
}

void test_execute_blt(void)
{
    // take the branch
    {
        struct decoded_rv64_base_ins_b *ins = calloc(1, sizeof(*ins));
        _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

        ins->rs1 = 7;
        ins->rs2 = 8;
        ins->imm = 0xf'00'00;

        init_rv64_cpu(&cpu, NULL, NULL);

        cpu.regs[REG_INX_PC] = 0xff'ff;
        cpu.regs[7]          = -1;
        cpu.regs[8]          = 1;

        execute_blt(&cpu, (void **)&ins);

        TEST_ASSERT_EQUAL_HEX64(0x0f'ff'ff, cpu.regs[REG_INX_PC]);
        TEST_ASSERT_EQUAL(1, cpu.i_flags.pc_updated);
        TEST_ASSERT_NULL(ins);
    }

    // don't take the branch
    {
        struct decoded_rv64_base_ins_b *ins = calloc(1, sizeof(*ins));
        _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

        ins->rs1 = 7;
        ins->rs2 = 8;
        ins->imm = 0xf'00'00;

        init_rv64_cpu(&cpu, NULL, NULL);

        cpu.regs[REG_INX_PC] = 0xff'ff;
        cpu.regs[7]          = 1;
        cpu.regs[8]          = -1;

        execute_blt(&cpu, (void **)&ins);

        TEST_ASSERT_EQUAL_HEX64(0x00'ff'ff, cpu.regs[REG_INX_PC]);
        TEST_ASSERT_EQUAL(0, cpu.i_flags.pc_updated);
        TEST_ASSERT_NULL(ins);
    }
}

void test_execute_bltu(void)
{
    // take the branch
    {
        struct decoded_rv64_base_ins_b *ins = calloc(1, sizeof(*ins));
        _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

        ins->rs1 = 7;
        ins->rs2 = 8;
        ins->imm = 0xf'00'00;

        init_rv64_cpu(&cpu, NULL, NULL);

        cpu.regs[REG_INX_PC] = 0xff'ff;
        cpu.regs[7]          = 1;
        cpu.regs[8]          = -1;

        execute_bltu(&cpu, (void **)&ins);

        TEST_ASSERT_EQUAL_HEX64(0x0f'ff'ff, cpu.regs[REG_INX_PC]);
        TEST_ASSERT_EQUAL(1, cpu.i_flags.pc_updated);
        TEST_ASSERT_NULL(ins);
    }

    // don't take the branch
    {
        struct decoded_rv64_base_ins_b *ins = calloc(1, sizeof(*ins));
        _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

        ins->rs1 = 7;
        ins->rs2 = 8;
        ins->imm = 0xf'00'00;

        init_rv64_cpu(&cpu, NULL, NULL);

        cpu.regs[REG_INX_PC] = 0xff'ff;
        cpu.regs[7]          = -1;
        cpu.regs[8]          = 1;

        execute_bltu(&cpu, (void **)&ins);

        TEST_ASSERT_EQUAL_HEX64(0x00'ff'ff, cpu.regs[REG_INX_PC]);
        TEST_ASSERT_EQUAL(0, cpu.i_flags.pc_updated);
        TEST_ASSERT_NULL(ins);
    }
}

void test_execute_bge(void)
{
    // take the branch
    {
        struct decoded_rv64_base_ins_b *ins = calloc(1, sizeof(*ins));
        _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

        ins->rs1 = 7;
        ins->rs2 = 8;
        ins->imm = 0xf'00'00;

        init_rv64_cpu(&cpu, NULL, NULL);

        cpu.regs[REG_INX_PC] = 0xff'ff;
        cpu.regs[7]          = 1;
        cpu.regs[8]          = -1;

        execute_bge(&cpu, (void **)&ins);

        TEST_ASSERT_EQUAL_HEX64(0x0f'ff'ff, cpu.regs[REG_INX_PC]);
        TEST_ASSERT_EQUAL(1, cpu.i_flags.pc_updated);
        TEST_ASSERT_NULL(ins);
    }

    // don't take the branch
    {
        struct decoded_rv64_base_ins_b *ins = calloc(1, sizeof(*ins));
        _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

        ins->rs1 = 7;
        ins->rs2 = 8;
        ins->imm = 0xf'00'00;

        init_rv64_cpu(&cpu, NULL, NULL);

        cpu.regs[REG_INX_PC] = 0xff'ff;
        cpu.regs[7]          = -1;
        cpu.regs[8]          = 1;

        execute_bge(&cpu, (void **)&ins);

        TEST_ASSERT_EQUAL_HEX64(0x00'ff'ff, cpu.regs[REG_INX_PC]);
        TEST_ASSERT_EQUAL(0, cpu.i_flags.pc_updated);
        TEST_ASSERT_NULL(ins);
    }
}

void test_execute_bgeu(void)
{
    // take the branch
    {
        struct decoded_rv64_base_ins_b *ins = calloc(1, sizeof(*ins));
        _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

        ins->rs1 = 7;
        ins->rs2 = 8;
        ins->imm = 0xf'00'00;

        init_rv64_cpu(&cpu, NULL, NULL);

        cpu.regs[REG_INX_PC] = 0xff'ff;
        cpu.regs[7]          = -1;
        cpu.regs[8]          = 1;

        execute_bgeu(&cpu, (void **)&ins);

        TEST_ASSERT_EQUAL_HEX64(0x0f'ff'ff, cpu.regs[REG_INX_PC]);
        TEST_ASSERT_EQUAL(1, cpu.i_flags.pc_updated);
        TEST_ASSERT_NULL(ins);
    }

    // don't take the branch
    {
        struct decoded_rv64_base_ins_b *ins = calloc(1, sizeof(*ins));
        _cleanup_free_rv64_cpu_ struct rv64_cpu cpu;

        ins->rs1 = 7;
        ins->rs2 = 8;
        ins->imm = 0xf'00'00;

        init_rv64_cpu(&cpu, NULL, NULL);

        cpu.regs[REG_INX_PC] = 0xff'ff;
        cpu.regs[7]          = 1;
        cpu.regs[8]          = -1;

        execute_bgeu(&cpu, (void **)&ins);

        TEST_ASSERT_EQUAL_HEX64(0x00'ff'ff, cpu.regs[REG_INX_PC]);
        TEST_ASSERT_EQUAL(0, cpu.i_flags.pc_updated);
        TEST_ASSERT_NULL(ins);
    }
}

void test_read_mem_rw()
{
    {
        _cleanup_free_rv64_cpu_ struct rv64_cpu cpu = {0};
        _cleanup_freep_sysbus_ struct sysbus *sbus  = NULL;
        char data[16]                               = {0};
        uint8_t expected_byte1                      = 0x11;
        uint8_t expected_byte2                      = 0x22;
        uint8_t expected_byte3                      = 0x33;
        uint64_t read_address                       = sizeof(data) - 6;
        uint64_t read_ret                           = 0;
        uint64_t read_err                           = 0;

        sbus = simple_setup_sysbus_with_mem(data, sizeof(data));
        init_rv64_cpu(&cpu, sbus, NULL);

        {
            struct io_dev *mem_dev = sbus->io_devs->data;
            uint8_t *mem           = mem_dev->meta_data->exposed_memory;

            mem[read_address]     = expected_byte1;
            mem[read_address + 1] = expected_byte2;
            mem[read_address + 2] = expected_byte3;

            /* Bytes beyond the requested length must not be included. */
            mem[read_address + 3] = expected_byte3;
            mem[read_address + 4] = expected_byte3;
            mem[read_address + 5] = expected_byte3;
        }

        read_err = read_mem(&cpu, read_address, &read_ret, 3);
        TEST_ASSERT_EQUAL(0, read_err);
        sysbus_step(cpu.ref_sysbus);

        for(int i = 0; i < 3; i++) {
            read_err = read_mem(&cpu, read_address, &read_ret, 3);
            TEST_ASSERT_EQUAL(0, read_err);

            sysbus_step(cpu.ref_sysbus);
        }

        TEST_ASSERT_EQUAL_HEX64(0x33'22'11, read_ret);
        TEST_ASSERT_EQUAL(0, cpu.i_flags.stall);
    }
}
void test_fetch()
{
    {
        _cleanup_free_rv64_cpu_ struct rv64_cpu cpu = {0};
        _cleanup_freep_sysbus_ struct sysbus *sbus  = NULL;
        uint32_t ins1                               = 0xff'11'22'33;
        uint32_t ins2                               = UINT32_MAX;
        uint32_t ins3                               = 0x00'11'22'33;
        uint64_t start_address;
        uint32_t ret_ins = 0;

        char data[64] = {0};

        sbus = simple_setup_sysbus_with_mem(data, sizeof(data));
        init_rv64_cpu(&cpu, sbus, NULL);

        start_address = ((sizeof(data) / 4) * 4) - 16;

        {
            struct io_dev *mem_dev = sbus->io_devs->data;
            uint8_t *mem           = mem_dev->meta_data->exposed_memory;

            /* Populate memory explicitly in little-endian order. */
            for(int i = 0; i < 4; i++) {
                mem[start_address - 8 + i]  = (uint8_t)(UINT32_MAX >> (i * 8));
                mem[start_address - 4 + i]  = 0;
                mem[start_address + i]      = (uint8_t)(ins1 >> (i * 8));
                mem[start_address + 4 + i]  = (uint8_t)(ins2 >> (i * 8));
                mem[start_address + 8 + i]  = (uint8_t)(ins3 >> (i * 8));
                mem[start_address + 12 + i] = 0;
            }
        }

        cpu.regs[REG_INX_PC] = start_address - 4;

        const uint32_t expected[] = {0, ins1, ins2, ins3, 0};

        for(size_t n = 0; n < sizeof(expected) / sizeof(expected[0]); n++) {
            ret_ins = 0;

            fetch(&cpu, &ret_ins);
            sysbus_step(cpu.ref_sysbus);
            for(int i = 0; i < 4; i++) {
                fetch(&cpu, &ret_ins);
                sysbus_step(cpu.ref_sysbus);
            }

            TEST_ASSERT_EQUAL_HEX32(expected[n], ret_ins);
            advance_pc_or_update_i_flags(&cpu);
        }
    }
}

void test_execution_handlers_table()
{
    {
        //sll
        struct decoded_rv64_base_ins_r *ins         = calloc(1, sizeof(*ins));
        _cleanup_free_rv64_cpu_ struct rv64_cpu cpu = {0};
        init_rv64_cpu(&cpu, NULL, NULL);

        ins->func3  = SLL_FUNC3;
        ins->func7  = SLL_FUNC7;
        ins->opcode = OP;
        ins->rd     = 3;
        ins->rs1    = 1;
        ins->rs2    = 2;

        cpu.regs[ins->rs1] = 2;
        cpu.regs[ins->rs2] = 10;
        cpu.regs[ins->rd]  = -1;

        execution_handlers[ins->opcode][ins->func3][ins->func7](
            &cpu, (void **)&ins);

        TEST_ASSERT_EQUAL(2 << 10, cpu.regs[3]);
        TEST_ASSERT_NULL(ins);
        TEST_ASSERT_EQUAL(0, cpu.i_flags.pc_updated);
    }
}

void test_execute_ld()
{
    {
        _cleanup_free_ struct decoded_rv64_base_ins_i *ins =
            calloc(1, sizeof(*ins));
        struct decoded_rv64_base_ins_i *copy_ins      = NULL;
        _cleanup_freep_rv64_cpu_ struct rv64_cpu *cpu = calloc(1, sizeof(*cpu));
        _cleanup_freep_sysbus_ struct sysbus *sbus    = NULL;
        char data[] =
            "\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff\x00";
        char *mem          = NULL;
        uint64_t base_addr = 3;
        uint64_t offset    = 1;

        sbus = simple_setup_sysbus_with_mem(data, sizeof(data));
        init_rv64_cpu(cpu, sbus, NULL);

        {
            struct io_dev *mem_dev = sbus->io_devs->data;
            mem                    = (char *)mem_dev->meta_data->exposed_memory;
        }

        ins->imm    = offset;
        ins->rd     = 1;
        ins->rs1    = 2;
        ins->func3  = LD_FUNC3;
        ins->opcode = LOAD;

        cpu->regs[ins->rs1] = base_addr;

        // first cycle, which no data is fetched and only address is given to the sysbus
        copy_ins = calloc(1, sizeof(*ins));
        memcpy(copy_ins, ins, sizeof(*ins));
        execute_ld(cpu, (void **)&copy_ins);
        sysbus_step(cpu->ref_sysbus);
        TEST_ASSERT_NULL(copy_ins);
        TEST_ASSERT_EQUAL(0, cpu->i_flags.pc_updated);
        TEST_ASSERT_EQUAL(true, cpu->i_flags.stall);

        for(int i = 0; i < 8; i++) {
            copy_ins = calloc(1, sizeof(*ins));
            memcpy(copy_ins, ins, sizeof(*ins));

            execute_ld(cpu, (void **)&copy_ins);
            sysbus_step(cpu->ref_sysbus);

            TEST_ASSERT_EQUAL_HEX64(
                *(uint8_t *)(mem + base_addr + offset + i),
                (cpu->regs[1] >> (i * 8)) & 0xff);

            TEST_ASSERT_NULL(copy_ins);
            TEST_ASSERT_EQUAL(0, cpu->i_flags.pc_updated);
            TEST_ASSERT_EQUAL(i < 7, cpu->i_flags.stall);
        }

        TEST_ASSERT_EQUAL_HEX64(
            *(uint64_t *)(mem + base_addr + offset), cpu->regs[1]);
    }
}

// Replaces test_execute_lw .. test_execute_sb in tests/cpu/cpu.c.
// Keep your existing test_execute_ld as it is.

void test_execute_lw()
{
    _cleanup_free_ struct decoded_rv64_base_ins_i *ins =
        calloc(1, sizeof(*ins));
    struct decoded_rv64_base_ins_i *copy_ins      = NULL;
    _cleanup_freep_rv64_cpu_ struct rv64_cpu *cpu = calloc(1, sizeof(*cpu));
    _cleanup_freep_sysbus_ struct sysbus *sbus    = NULL;
    char data[] =
        "\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff\x00";
    char *mem          = NULL;
    uint64_t base_addr = 10;
    uint64_t offset    = 1;

    sbus = simple_setup_sysbus_with_mem(data, sizeof(data));
    init_rv64_cpu(cpu, sbus, NULL);

    struct io_dev *mem_dev = sbus->io_devs->data;
    mem                    = (char *)mem_dev->meta_data->exposed_memory;

    ins->imm    = offset;
    ins->rd     = 1;
    ins->rs1    = 2;
    ins->func3  = LW_FUNC3;
    ins->opcode = LOAD;

    cpu->regs[ins->rs1] = base_addr;

    // first cycle, which no data is fetched and only address is given to the sysbus
    copy_ins = calloc(1, sizeof(*ins));
    memcpy(copy_ins, ins, sizeof(*ins));
    execute_lw(cpu, (void **)&copy_ins);
    sysbus_step(cpu->ref_sysbus);
    TEST_ASSERT_NULL(copy_ins);
    TEST_ASSERT_EQUAL(0, cpu->i_flags.pc_updated);
    TEST_ASSERT_EQUAL(true, cpu->i_flags.stall);

    for(int i = 0; i < 4; i++) {
        copy_ins = calloc(1, sizeof(*ins));
        memcpy(copy_ins, ins, sizeof(*ins));

        execute_lw(cpu, (void **)&copy_ins);
        sysbus_step(cpu->ref_sysbus);

        TEST_ASSERT_EQUAL_HEX8(
            (uint8_t)mem[base_addr + offset + i],
            (cpu->regs[1] >> (i * 8)) & 0xff);

        TEST_ASSERT_NULL(copy_ins);
        TEST_ASSERT_EQUAL(0, cpu->i_flags.pc_updated);
        TEST_ASSERT_EQUAL(i < 3, cpu->i_flags.stall);
    }

    uint32_t raw = 0;
    for(int i = 0; i < 4; i++) {
        raw |= (uint32_t)(uint8_t)mem[base_addr + offset + i] << (i * 8);
    }

    TEST_ASSERT_EQUAL_HEX64(sign_extend_u32_to_u64(raw), cpu->regs[1]);
}

void test_execute_lwu()
{
    _cleanup_free_ struct decoded_rv64_base_ins_i *ins =
        calloc(1, sizeof(*ins));
    struct decoded_rv64_base_ins_i *copy_ins      = NULL;
    _cleanup_freep_rv64_cpu_ struct rv64_cpu *cpu = calloc(1, sizeof(*cpu));
    _cleanup_freep_sysbus_ struct sysbus *sbus    = NULL;
    char data[] =
        "\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff\x00";
    char *mem          = NULL;
    uint64_t base_addr = 10;
    uint64_t offset    = 1;

    sbus = simple_setup_sysbus_with_mem(data, sizeof(data));
    init_rv64_cpu(cpu, sbus, NULL);

    struct io_dev *mem_dev = sbus->io_devs->data;
    mem                    = (char *)mem_dev->meta_data->exposed_memory;

    ins->imm    = offset;
    ins->rd     = 1;
    ins->rs1    = 2;
    ins->func3  = LWU_FUNC3;
    ins->opcode = LOAD;

    cpu->regs[ins->rs1] = base_addr;

    // first cycle, which no data is fetched and only address is given to the sysbus
    copy_ins = calloc(1, sizeof(*ins));
    memcpy(copy_ins, ins, sizeof(*ins));
    execute_lwu(cpu, (void **)&copy_ins);
    sysbus_step(cpu->ref_sysbus);
    TEST_ASSERT_NULL(copy_ins);
    TEST_ASSERT_EQUAL(0, cpu->i_flags.pc_updated);
    TEST_ASSERT_EQUAL(true, cpu->i_flags.stall);

    for(int i = 0; i < 4; i++) {
        copy_ins = calloc(1, sizeof(*ins));
        memcpy(copy_ins, ins, sizeof(*ins));

        execute_lwu(cpu, (void **)&copy_ins);
        sysbus_step(cpu->ref_sysbus);

        TEST_ASSERT_EQUAL_HEX8(
            (uint8_t)mem[base_addr + offset + i],
            (cpu->regs[1] >> (i * 8)) & 0xff);

        TEST_ASSERT_NULL(copy_ins);
        TEST_ASSERT_EQUAL(0, cpu->i_flags.pc_updated);
        TEST_ASSERT_EQUAL(i < 3, cpu->i_flags.stall);
    }

    uint32_t raw = 0;
    for(int i = 0; i < 4; i++) {
        raw |= (uint32_t)(uint8_t)mem[base_addr + offset + i] << (i * 8);
    }

    TEST_ASSERT_EQUAL_HEX64((uint64_t)raw, cpu->regs[1]);
}

void test_execute_lh()
{
    _cleanup_free_ struct decoded_rv64_base_ins_i *ins =
        calloc(1, sizeof(*ins));
    struct decoded_rv64_base_ins_i *copy_ins      = NULL;
    _cleanup_freep_rv64_cpu_ struct rv64_cpu *cpu = calloc(1, sizeof(*cpu));
    _cleanup_freep_sysbus_ struct sysbus *sbus    = NULL;
    char data[] =
        "\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff\x00";
    char *mem          = NULL;
    uint64_t base_addr = 10;
    uint64_t offset    = 3;

    sbus = simple_setup_sysbus_with_mem(data, sizeof(data));
    init_rv64_cpu(cpu, sbus, NULL);

    struct io_dev *mem_dev = sbus->io_devs->data;
    mem                    = (char *)mem_dev->meta_data->exposed_memory;

    ins->imm    = offset;
    ins->rd     = 1;
    ins->rs1    = 2;
    ins->func3  = LH_FUNC3;
    ins->opcode = LOAD;

    cpu->regs[ins->rs1] = base_addr;

    // first cycle, which no data is fetched and only address is given to the sysbus
    copy_ins = calloc(1, sizeof(*ins));
    memcpy(copy_ins, ins, sizeof(*ins));
    execute_lh(cpu, (void **)&copy_ins);
    sysbus_step(cpu->ref_sysbus);
    TEST_ASSERT_NULL(copy_ins);
    TEST_ASSERT_EQUAL(0, cpu->i_flags.pc_updated);
    TEST_ASSERT_EQUAL(true, cpu->i_flags.stall);

    for(int i = 0; i < 2; i++) {
        copy_ins = calloc(1, sizeof(*ins));
        memcpy(copy_ins, ins, sizeof(*ins));

        execute_lh(cpu, (void **)&copy_ins);
        sysbus_step(cpu->ref_sysbus);

        TEST_ASSERT_EQUAL_HEX8(
            (uint8_t)mem[base_addr + offset + i],
            (cpu->regs[1] >> (i * 8)) & 0xff);

        TEST_ASSERT_NULL(copy_ins);
        TEST_ASSERT_EQUAL(0, cpu->i_flags.pc_updated);
        TEST_ASSERT_EQUAL(i < 1, cpu->i_flags.stall);
    }

    uint16_t raw = (uint16_t)(uint8_t)mem[base_addr + offset] |
                   ((uint16_t)(uint8_t)mem[base_addr + offset + 1] << 8);

    TEST_ASSERT_EQUAL_HEX64(sign_extend_u16_to_u64(raw), cpu->regs[1]);
}

void test_execute_lhu()
{
    _cleanup_free_ struct decoded_rv64_base_ins_i *ins =
        calloc(1, sizeof(*ins));
    struct decoded_rv64_base_ins_i *copy_ins      = NULL;
    _cleanup_freep_rv64_cpu_ struct rv64_cpu *cpu = calloc(1, sizeof(*cpu));
    _cleanup_freep_sysbus_ struct sysbus *sbus    = NULL;
    char data[] =
        "\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff\x00";
    char *mem          = NULL;
    uint64_t base_addr = 10;
    uint64_t offset    = 3;

    sbus = simple_setup_sysbus_with_mem(data, sizeof(data));
    init_rv64_cpu(cpu, sbus, NULL);

    struct io_dev *mem_dev = sbus->io_devs->data;
    mem                    = (char *)mem_dev->meta_data->exposed_memory;

    ins->imm    = offset;
    ins->rd     = 1;
    ins->rs1    = 2;
    ins->func3  = LHU_FUNC3;
    ins->opcode = LOAD;

    cpu->regs[ins->rs1] = base_addr;

    // first cycle, which no data is fetched and only address is given to the sysbus
    copy_ins = calloc(1, sizeof(*ins));
    memcpy(copy_ins, ins, sizeof(*ins));
    execute_lhu(cpu, (void **)&copy_ins);
    sysbus_step(cpu->ref_sysbus);
    TEST_ASSERT_NULL(copy_ins);
    TEST_ASSERT_EQUAL(0, cpu->i_flags.pc_updated);
    TEST_ASSERT_EQUAL(true, cpu->i_flags.stall);

    for(int i = 0; i < 2; i++) {
        copy_ins = calloc(1, sizeof(*ins));
        memcpy(copy_ins, ins, sizeof(*ins));

        execute_lhu(cpu, (void **)&copy_ins);
        sysbus_step(cpu->ref_sysbus);

        TEST_ASSERT_EQUAL_HEX8(
            (uint8_t)mem[base_addr + offset + i],
            (cpu->regs[1] >> (i * 8)) & 0xff);

        TEST_ASSERT_NULL(copy_ins);
        TEST_ASSERT_EQUAL(0, cpu->i_flags.pc_updated);
        TEST_ASSERT_EQUAL(i < 1, cpu->i_flags.stall);
    }

    uint16_t expected = (uint16_t)(uint8_t)mem[base_addr + offset] |
                        ((uint16_t)(uint8_t)mem[base_addr + offset + 1] << 8);

    TEST_ASSERT_EQUAL_HEX64(expected, cpu->regs[1]);
}

void test_execute_lb()
{
    _cleanup_free_ struct decoded_rv64_base_ins_i *ins =
        calloc(1, sizeof(*ins));
    struct decoded_rv64_base_ins_i *copy_ins      = NULL;
    _cleanup_freep_rv64_cpu_ struct rv64_cpu *cpu = calloc(1, sizeof(*cpu));
    _cleanup_freep_sysbus_ struct sysbus *sbus    = NULL;
    char data[] =
        "\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff\x00";
    char *mem          = NULL;
    uint64_t base_addr = 10;
    uint64_t offset    = 4;

    sbus = simple_setup_sysbus_with_mem(data, sizeof(data));
    init_rv64_cpu(cpu, sbus, NULL);

    struct io_dev *mem_dev = sbus->io_devs->data;
    mem                    = (char *)mem_dev->meta_data->exposed_memory;

    ins->imm    = offset;
    ins->rd     = 1;
    ins->rs1    = 2;
    ins->func3  = LB_FUNC3;
    ins->opcode = LOAD;

    cpu->regs[ins->rs1] = base_addr;

    // first cycle, which no data is fetched and only address is given to the sysbus
    copy_ins = calloc(1, sizeof(*ins));
    memcpy(copy_ins, ins, sizeof(*ins));
    execute_lb(cpu, (void **)&copy_ins);
    sysbus_step(cpu->ref_sysbus);
    TEST_ASSERT_NULL(copy_ins);
    TEST_ASSERT_EQUAL(0, cpu->i_flags.pc_updated);
    TEST_ASSERT_EQUAL(true, cpu->i_flags.stall);

    // second cycle, the byte is consumed
    copy_ins = calloc(1, sizeof(*ins));
    memcpy(copy_ins, ins, sizeof(*ins));
    execute_lb(cpu, (void **)&copy_ins);
    sysbus_step(cpu->ref_sysbus);

    TEST_ASSERT_EQUAL_HEX64(
        sign_extend_u8_to_u64((uint8_t)mem[base_addr + offset]), cpu->regs[1]);
    TEST_ASSERT_NULL(copy_ins);
    TEST_ASSERT_EQUAL(0, cpu->i_flags.pc_updated);
    TEST_ASSERT_EQUAL(0, cpu->i_flags.stall);
}

void test_execute_lbu()
{
    _cleanup_free_ struct decoded_rv64_base_ins_i *ins =
        calloc(1, sizeof(*ins));
    struct decoded_rv64_base_ins_i *copy_ins      = NULL;
    _cleanup_freep_rv64_cpu_ struct rv64_cpu *cpu = calloc(1, sizeof(*cpu));
    _cleanup_freep_sysbus_ struct sysbus *sbus    = NULL;
    char data[] =
        "\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff\x00";
    char *mem          = NULL;
    uint64_t base_addr = 10;
    uint64_t offset    = 4;

    sbus = simple_setup_sysbus_with_mem(data, sizeof(data));
    init_rv64_cpu(cpu, sbus, NULL);

    struct io_dev *mem_dev = sbus->io_devs->data;
    mem                    = (char *)mem_dev->meta_data->exposed_memory;

    ins->imm    = offset;
    ins->rd     = 1;
    ins->rs1    = 2;
    ins->func3  = LBU_FUNC3;
    ins->opcode = LOAD;

    cpu->regs[ins->rs1] = base_addr;

    // first cycle, which no data is fetched and only address is given to the sysbus
    copy_ins = calloc(1, sizeof(*ins));
    memcpy(copy_ins, ins, sizeof(*ins));
    execute_lbu(cpu, (void **)&copy_ins);
    sysbus_step(cpu->ref_sysbus);
    TEST_ASSERT_NULL(copy_ins);
    TEST_ASSERT_EQUAL(0, cpu->i_flags.pc_updated);
    TEST_ASSERT_EQUAL(true, cpu->i_flags.stall);

    // second cycle, the byte is consumed
    copy_ins = calloc(1, sizeof(*ins));
    memcpy(copy_ins, ins, sizeof(*ins));
    execute_lbu(cpu, (void **)&copy_ins);
    sysbus_step(cpu->ref_sysbus);

    TEST_ASSERT_EQUAL_HEX64((uint8_t)mem[base_addr + offset], cpu->regs[1]);
    TEST_ASSERT_NULL(copy_ins);
    TEST_ASSERT_EQUAL(0, cpu->i_flags.pc_updated);
    TEST_ASSERT_EQUAL(0, cpu->i_flags.stall);
}

void test_execute_sd()
{
    {
        _cleanup_free_ struct decoded_rv64_base_ins_s *ins =
            calloc(1, sizeof(*ins));
        struct decoded_rv64_base_ins_s *copy_ins      = NULL;
        _cleanup_freep_rv64_cpu_ struct rv64_cpu *cpu = calloc(1, sizeof(*cpu));
        _cleanup_freep_sysbus_ struct sysbus *sbus    = NULL;

        char data[]        = "\x00\x00\x00\x00\x00\x00\x00\x00"
                             "\x00\x00\x00\x00\x00\x00\x00\x00";
        uint8_t *mem       = NULL;
        uint64_t base_addr = 4;
        uint64_t value     = 0x88'77'66'55'44'33'22'11;

        sbus = simple_setup_sysbus_with_mem(data, sizeof(data));
        init_rv64_cpu(cpu, sbus, NULL);

        {
            struct io_dev *mem_dev = sbus->io_devs->data;
            mem                    = mem_dev->meta_data->exposed_memory;
        }

        ins->imm    = 0;
        ins->rs1    = 1;
        ins->rs2    = 2;
        ins->func3  = SD_FUNC3;
        ins->opcode = STORE;

        cpu->regs[ins->rs1] = base_addr;
        cpu->regs[ins->rs2] = value;

        for(int i = 0; i < 8; i++) {
            copy_ins = calloc(1, sizeof(*ins));
            memcpy(copy_ins, ins, sizeof(*ins));

            execute_sd(cpu, (void **)&copy_ins);
            sysbus_step(cpu->ref_sysbus);

            TEST_ASSERT_EQUAL_HEX8(
                (value >> (i * 8)) & 0xff, mem[base_addr + i]);
            TEST_ASSERT_NULL(copy_ins);
            TEST_ASSERT_EQUAL(0, cpu->i_flags.pc_updated);
            TEST_ASSERT_EQUAL(i < 7, cpu->i_flags.stall);
        }

        uint64_t actual = 0;
        for(int i = 0; i < 8; i++) {
            actual |= (uint64_t)mem[base_addr + i] << (i * 8);
        }
        TEST_ASSERT_EQUAL_HEX64(value, actual);
    }
}

void test_execute_sw()
{
    {
        _cleanup_free_ struct decoded_rv64_base_ins_s *ins =
            calloc(1, sizeof(*ins));
        struct decoded_rv64_base_ins_s *copy_ins      = NULL;
        _cleanup_freep_rv64_cpu_ struct rv64_cpu *cpu = calloc(1, sizeof(*cpu));
        _cleanup_freep_sysbus_ struct sysbus *sbus    = NULL;

        char data[]        = "\x00\x00\x00\x00\x00\x00\x00\x00"
                             "\x00\x00\x00\x00\x00\x00\x00\x00";
        uint8_t *mem       = NULL;
        uint64_t base_addr = 2;
        uint32_t value     = UINT32_C(0x44332211);

        sbus = simple_setup_sysbus_with_mem(data, sizeof(data));
        init_rv64_cpu(cpu, sbus, NULL);

        {
            struct io_dev *mem_dev = sbus->io_devs->data;
            mem                    = mem_dev->meta_data->exposed_memory;
        }

        ins->imm    = 0;
        ins->rs1    = 1;
        ins->rs2    = 2;
        ins->func3  = SW_FUNC3;
        ins->opcode = STORE;

        cpu->regs[ins->rs1] = base_addr;
        cpu->regs[ins->rs2] = value;

        for(int i = 0; i < 4; i++) {
            copy_ins = calloc(1, sizeof(*ins));
            memcpy(copy_ins, ins, sizeof(*ins));

            execute_sw(cpu, (void **)&copy_ins);
            sysbus_step(cpu->ref_sysbus);

            TEST_ASSERT_EQUAL_HEX8(
                (value >> (i * 8)) & UINT32_C(0xff), mem[base_addr + i]);
            TEST_ASSERT_NULL(copy_ins);
            TEST_ASSERT_EQUAL(0, cpu->i_flags.pc_updated);
            TEST_ASSERT_EQUAL(i < 3, cpu->i_flags.stall);
        }

        uint32_t actual = 0;
        for(int i = 0; i < 4; i++) {
            actual |= (uint32_t)mem[base_addr + i] << (i * 8);
        }
        TEST_ASSERT_EQUAL_HEX32(value, actual);
    }
}

void test_execute_sh()
{
    {
        _cleanup_free_ struct decoded_rv64_base_ins_s *ins =
            calloc(1, sizeof(*ins));
        struct decoded_rv64_base_ins_s *copy_ins      = NULL;
        _cleanup_freep_rv64_cpu_ struct rv64_cpu *cpu = calloc(1, sizeof(*cpu));
        _cleanup_freep_sysbus_ struct sysbus *sbus    = NULL;

        char data[]        = "\x00\x00\x00\x00\x00\x00\x00\x00"
                             "\x00\x00\x00\x00\x00\x00\x00\x00";
        uint8_t *mem       = NULL;
        uint64_t base_addr = 2;
        uint16_t value     = UINT16_C(0x2211);

        sbus = simple_setup_sysbus_with_mem(data, sizeof(data));
        init_rv64_cpu(cpu, sbus, NULL);

        {
            struct io_dev *mem_dev = sbus->io_devs->data;
            mem                    = mem_dev->meta_data->exposed_memory;
        }

        ins->imm    = 0;
        ins->rs1    = 1;
        ins->rs2    = 2;
        ins->func3  = SH_FUNC3;
        ins->opcode = STORE;

        cpu->regs[ins->rs1] = base_addr;
        cpu->regs[ins->rs2] = value;

        for(int i = 0; i < 2; i++) {
            copy_ins = calloc(1, sizeof(*ins));
            memcpy(copy_ins, ins, sizeof(*ins));

            execute_sh(cpu, (void **)&copy_ins);
            sysbus_step(cpu->ref_sysbus);

            TEST_ASSERT_EQUAL_HEX8(
                (value >> (i * 8)) & UINT16_C(0xff), mem[base_addr + i]);
            TEST_ASSERT_NULL(copy_ins);
            TEST_ASSERT_EQUAL(0, cpu->i_flags.pc_updated);
            TEST_ASSERT_EQUAL(i < 1, cpu->i_flags.stall);
        }

        uint16_t actual = 0;
        for(int i = 0; i < 2; i++) {
            actual |= (uint16_t)mem[base_addr + i] << (i * 8);
        }
        TEST_ASSERT_EQUAL_HEX16(value, actual);
    }
}

void test_execute_sb()
{
    {
        _cleanup_free_ struct decoded_rv64_base_ins_s *ins =
            calloc(1, sizeof(*ins));
        struct decoded_rv64_base_ins_s *copy_ins      = NULL;
        _cleanup_freep_rv64_cpu_ struct rv64_cpu *cpu = calloc(1, sizeof(*cpu));
        _cleanup_freep_sysbus_ struct sysbus *sbus    = NULL;

        char data[]        = "\x00\x00\x00\x00\x00\x00\x00\x00"
                             "\x00\x00\x00\x00\x00\x00\x00\x00";
        uint8_t *mem       = NULL;
        uint64_t base_addr = 2;
        uint8_t value      = UINT8_C(0xaa);

        sbus = simple_setup_sysbus_with_mem(data, sizeof(data));
        init_rv64_cpu(cpu, sbus, NULL);

        {
            struct io_dev *mem_dev = sbus->io_devs->data;
            mem                    = mem_dev->meta_data->exposed_memory;
        }

        ins->imm    = 0;
        ins->rs1    = 1;
        ins->rs2    = 2;
        ins->func3  = SB_FUNC3;
        ins->opcode = STORE;

        cpu->regs[ins->rs1] = base_addr;
        cpu->regs[ins->rs2] = value;

        copy_ins = calloc(1, sizeof(*ins));
        memcpy(copy_ins, ins, sizeof(*ins));

        execute_sb(cpu, (void **)&copy_ins);
        sysbus_step(cpu->ref_sysbus);

        TEST_ASSERT_EQUAL_HEX8(value, mem[base_addr]);
        TEST_ASSERT_NULL(copy_ins);
        TEST_ASSERT_EQUAL(0, cpu->i_flags.pc_updated);
        TEST_ASSERT_EQUAL(0, cpu->i_flags.stall);
    }
}
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_decode_imm_b);
    RUN_TEST(test_decode_imm_u);
    RUN_TEST(test_decode_imm_j);
    RUN_TEST(test_decode_imm_s);
    RUN_TEST(test_decode_imm_i);

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
    RUN_TEST(test_execute_sltu);
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

    RUN_TEST(test_execute_beq);
    RUN_TEST(test_execute_bne);
    RUN_TEST(test_execute_blt);
    RUN_TEST(test_execute_bltu);
    RUN_TEST(test_execute_bge);
    RUN_TEST(test_execute_bgeu);

    RUN_TEST(test_execute_ld);
    RUN_TEST(test_execute_lw);
    RUN_TEST(test_execute_lwu);
    RUN_TEST(test_execute_lh);
    RUN_TEST(test_execute_lhu);
    RUN_TEST(test_execute_lb);
    RUN_TEST(test_execute_lbu);

    RUN_TEST(test_execute_sd);
    RUN_TEST(test_execute_sw);
    RUN_TEST(test_execute_sh);
    RUN_TEST(test_execute_sb);

    RUN_TEST(test_read_mem_rw);
    RUN_TEST(test_fetch);

    RUN_TEST(test_execution_handlers_table);

    return UNITY_END();
}

// NOLINTEND(readability-magic-numbers)
