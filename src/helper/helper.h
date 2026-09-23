void freep(void *ptr);

#define _cleanup_free_ __attribute__((__cleanup__(freep)))

#define MOVE(ptr_to_ptr)                                                       \
    ({                                                                         \
        void *__tmp_move_var = *(ptr_to_ptr);                                  \
        *(ptr_to_ptr)        = NULL;                                           \
        __tmp_move_var;                                                        \
    })
