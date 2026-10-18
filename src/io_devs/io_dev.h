#pragma once

#include <stdint.h>

struct io_dev;
/**
 * @brief fields every io_dev share
 */
struct io_dev_metadata {
    void (*step)(struct io_dev *self);
    struct sysbus *parent;
    uint8_t *exposed_memory;
    uint64_t exposed_memory_length;
    uint64_t mapped_base_addr;
    void (*free)(void * _Nonnull self);
    void (*_Nullable debug_dump)(void *_Nonnull self);
};

/**
 * @brief abstract io_dev, each io_dev should start with the following fields
 */
struct io_dev {
    struct io_dev_metadata meta_data;
    char dev_specific_data[];
};
