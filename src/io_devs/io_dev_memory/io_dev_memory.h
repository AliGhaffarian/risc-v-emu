#include "io_dev.h"

struct io_dev_memory {
    struct io_dev_metadata metadata;
};

struct io_dev_memory *
new_io_dev_memory(const struct io_dev_metadata *_Nonnull metadata);
