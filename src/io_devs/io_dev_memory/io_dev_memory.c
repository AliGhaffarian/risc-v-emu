#include "io_dev_memory.h"
#include <stdlib.h>

struct io_dev_memory *
new_io_dev_memory(const struct io_dev_metadata *_Nonnull metadata)
{
    struct io_dev_memory *ret = calloc(1, sizeof(*ret));
    ret->metadata             = *metadata;

    ret->metadata.debug_dump = NULL;
    ret->metadata.step       = NULL;

    return ret;
}
