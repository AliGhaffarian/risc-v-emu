#include "io_dev_memory.h"
#include "helper.h"
#include <assert.h>
#include <stdlib.h>

struct io_dev_memory *
new_io_dev_memory(struct io_dev_metadata *_Nullable *_Nonnull metadata)
{
    assert(metadata);

    if(!metadata) {
        *metadata = calloc(1, sizeof(struct io_dev_metadata));
        (*metadata)->exposed_memory =
            calloc(1, IO_DEV_MEMORY_DEFAULT_MEM_LENGTH);
    }
    if(!metadata) {
        return NULL;
        if(!(*metadata)->exposed_memory) {
            freep((void *)metadata);
            return NULL;
        }
    }

    struct io_dev_memory *ret = calloc(1, sizeof(*ret));
    ret->metadata             = MOVE(metadata);
    ret->metadata->read       = io_dev_memory_read;
    ret->metadata->write      = io_dev_memory_write;

    ret->metadata->debug_dump = NULL;
    ret->metadata->step       = NULL;

    return ret;
}

void io_dev_freep_memory(struct io_dev_memory *_Nullable *_Nonnull mem)
{
    io_dev_freep((struct io_dev **)mem);
}

void io_dev_memory_write(
    struct io_dev *_Nonnull self, uint64_t address, uint8_t value)
{
    assert(self);
    self->meta_data->exposed_memory[address] = value;
}
void io_dev_memory_read(
    struct io_dev *_Nonnull self, uint64_t address, uint8_t *_Nonnull value)
{
    assert(self);
    assert(value);
    *value = self->meta_data->exposed_memory[address];
}
