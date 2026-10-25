#include "io_dev.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

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
