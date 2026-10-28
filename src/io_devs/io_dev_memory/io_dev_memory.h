#include "io_dev.h"

struct io_dev_memory {
    struct io_dev_metadata *metadata;
};

#define IO_DEV_MEMORY_DEFAULT_MEM_LENGTH 128

void io_dev_freep_memory(struct io_dev_memory *_Nullable *_Nonnull mem);
#define _cleanup_freep_io_dev_memory_                                          \
    __attribute__((__cleanup__(io_dev_freep_memory)))

struct io_dev_memory *
new_io_dev_memory(struct io_dev_metadata *_Nonnull *_Nonnull metadata);

void io_dev_memory_write(
    struct io_dev *_Nonnull self, uint64_t address, uint8_t value);
void io_dev_memory_read(
    struct io_dev *_Nonnull self, uint64_t address, uint8_t *_Nonnull value);
