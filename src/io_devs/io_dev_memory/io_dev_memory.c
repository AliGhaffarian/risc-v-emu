#include "io_dev_memory.h"
#include <assert.h>
#include <stdlib.h>

struct io_dev_memory *
new_io_dev_memory(struct io_dev_metadata *_Nonnull *_Nonnull metadata)
{
    assert(metadata);
    assert(*metadata);
    struct io_dev_memory *ret = calloc(1, sizeof(*ret));
    ret->metadata             = **metadata;

    ret->metadata.debug_dump = NULL;
    ret->metadata.step       = NULL;

    return ret;
}
