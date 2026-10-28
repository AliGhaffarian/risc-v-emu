#include "helper.h"
#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>

void freep(void *_Nonnull ptr)
{
    void **ptrptr = (void **)ptr;
    free(*ptrptr);
    *ptrptr = NULL;
}

uint64_t sign_extend_u32_to_u64(uint32_t num)
{
    // NOLINTNEXTLINE(readability-magic-numbers)
    return repeat_bit_in_num(num, 32, GET_SIGN_BIT(num));
}

uint64_t sign_extend_u16_to_u64(uint32_t num)
{
    // NOLINTNEXTLINE(readability-magic-numbers)
    return repeat_bit_in_num(num, 16, GET_SIGN_BIT(num));
}

uint64_t sign_extend_u8_to_u64(uint32_t num)
{
    // NOLINTNEXTLINE(readability-magic-numbers)
    return repeat_bit_in_num(num, 8, GET_SIGN_BIT(num));
}

uint64_t bitmask_from_bit_size(uint64_t bit_size)
{
    uint64_t ret = 0;

    for(int i = 0; i < bit_size; i++) {
        ret |= (1 << i);
    }

    return ret;
}

// NOLINTBEGIN(bugprone-easily-swappable-parameters)
uint64_t
extract_bits_from_uint64(uint64_t num, uint64_t start, uint64_t bitmask)
// NOLINTEND(bugprone-easily-swappable-parameters)
{

    uint64_t ret = 0;

    ret = num >> start;

    ret &= bitmask;

    return ret;
}

// NOLINTBEGIN(bugprone-easily-swappable-parameters)
uint64_t repeat_bit_in_num(uint64_t num, uint8_t start, uint8_t bit)
// NOLINTEND(bugprone-easily-swappable-parameters)
{

    // NOLINTNEXTLINE(readability-magic-numbers)
    for(int i = start; i < 64; i++) {
        num |= ((uint64_t)bit << i);
    }

    return num;
}

void debug_dump_cpu(const struct rv64_cpu *_Nonnull cpu)
{
    __builtin_dump_struct(cpu, printf);
    for(int i = 0; i < cpu->opt.regs; i += 2) {
        printf("x%d: %lx\t\t", i, cpu->regs[i]);
        if(i + 1 == cpu->opt.regs) {
            puts("");
            break;
        }
        printf("x%d: %lx\n", i + 1, cpu->regs[i + 1]);
    }
}

void debug_dump_memory(const char *mem, uint64_t len, FILE *dst)
{
    puts("mem:");
    int i = 0;
    for(i = 0; i < len - 8; i += 8) {
        (void)fprintf(dst, "%hhx: ", i);
        (void)fprintf(dst, "%hhx, ", mem[i]);
        (void)fprintf(dst, "%hhx, ", mem[i + 1]);
        (void)fprintf(dst, "%hhx, ", mem[i + 2]);
        (void)fprintf(dst, "%hhx, ", mem[i + 3]);
        (void)fprintf(dst, "%hhx, ", mem[i + 4]);
        (void)fprintf(dst, "%hhx, ", mem[i + 5]);
        (void)fprintf(dst, "%hhx, ", mem[i + 6]);
        (void)fprintf(dst, "%hhx\n", mem[i + 7]);
    }

    if(i < len) {
        (void)fprintf(dst, "%hhx: ", i);
        for(; i < len; i++) {
            (void)fprintf(dst, "%hhx, ", mem[i]);
        }
        (void)fprintf(dst, "\n");
    }
}
