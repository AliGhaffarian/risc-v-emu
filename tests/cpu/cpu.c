#include "cpu.h"
#include "unity.h"

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
    return UNITY_END();
}

// NOLINTEND(readability-magic-numbers)
