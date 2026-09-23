#include "helper.h"
#include <stdlib.h>

void freep(void *ptr)
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

    // NOLINTNEXTLINE(readability-magic-numbers)
    for(int i = start; i < 64; i++) {
        num |= ((uint64_t)bit << i);
    }

    return num;
}
