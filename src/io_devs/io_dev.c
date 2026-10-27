#include "io_dev.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// dump struct of io_dev, call io_dev->meta_data.debug_dump
void debug_io_dev_dump(const struct io_dev *_Nonnull dev)
{
    assert(dev);
    __builtin_dump_struct(dev, printf);

    if(dev->meta_data.debug_dump) {
        dev->meta_data.debug_dump(dev);
    }
}

// free meta data, call io_dev->meta_data.free
void io_dev_free(struct io_dev *_Nullable dev)
{

    if(dev->meta_data.free) {
        dev->meta_data.free(dev);
    }

    free(dev->meta_data.exposed_memory);
    free(dev);
}

// free meta data, call io_dev->meta_data.free
void io_dev_freep(struct io_dev *_Nullable *_Nonnull dev)
{
    if(!(*dev)) {
        return;
    }
    io_dev_free(*dev);
    (*dev) = NULL;
}

// free meta data, call io_dev->meta_data.free
void io_dev_free_void(void *_Nullable dev) { io_dev_free(dev); }

int new_metadata_with_mem(
    struct io_dev_metadata *_Nullable *_Nonnull ret_metadata,
    const uint8_t *_Nullable mem_buf,
    uint64_t buf_len)
{
    assert(ret_metadata);

    if(!(*ret_metadata)) {
        (*ret_metadata) = calloc(1, sizeof(struct io_dev_metadata));
        if(!(*ret_metadata)) {
            return ENOMEM;
        }
    }

    (*ret_metadata)->exposed_memory = calloc(1, buf_len);
    if(mem_buf) {
        memcpy((*ret_metadata)->exposed_memory, mem_buf, buf_len);
    }
    (*ret_metadata)->exposed_memory_length = buf_len;

    return 0;
}

void freep_io_dev_metadata(void *_Nonnull io_dev_metadata_voidpp)
{
    assert(io_dev_metadata_voidpp);

    struct io_dev_metadata **to_freep =
        (struct io_dev_metadata **)io_dev_metadata_voidpp;
    struct io_dev_metadata *to_free = *to_freep;

    free(to_free->exposed_memory);
    free(to_free);

    *to_freep = NULL;
}
