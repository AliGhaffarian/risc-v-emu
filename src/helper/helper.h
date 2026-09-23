void freep(void *ptr);

#define _cleanup_free_ __attribute__((__cleanup__(freep)))

#define GET_SIGN_BIT(num) ({ (num) >> ((sizeof(typeof(num)) * 8) - 1); })

#define MOVE(ptr_to_ptr)                                                       \
    ({                                                                         \
        void *__tmp_move_var = *(ptr_to_ptr);                                  \
        *(ptr_to_ptr)        = NULL;                                           \
        __tmp_move_var;                                                        \
    })
