#include "io_dev.h"

struct io_dev_memory {
    struct io_dev_metadata *metadata;
};

void io_dev_freep_memory(struct io_dev_memory *_Nullable *_Nonnull mem);
#define _cleanup_freep_io_dev_memory_                                          \
    __attribute__((__cleanup__(io_dev_freep_memory)))

struct io_dev_memory *
new_io_dev_memory(struct io_dev_metadata *_Nonnull *_Nonnull metadata);
