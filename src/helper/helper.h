#include <stdint.h>

void freep(void *ptr);

#define _cleanup_free_ __attribute__((__cleanup__(freep)))

#define GET_SIGN_BIT(num) ({ (num) >> ((sizeof(typeof(num)) * 8) - 1); })

#define MOVE(ptr_to_ptr)                                                       \
    ({                                                                         \
        void *__tmp_move_var = *(ptr_to_ptr);                                  \
        *(ptr_to_ptr)        = NULL;                                           \
        __tmp_move_var;                                                        \
    })

uint64_t sign_extend_u32_to_u64(uint32_t num);

uint64_t bitmask_from_bit_size(uint64_t bit_size);
uint64_t
extract_bits_from_uint64(uint64_t num, uint64_t start, uint64_t bitmask);
uint64_t repeat_bit_in_num(uint64_t num, uint8_t start, uint8_t bit);
