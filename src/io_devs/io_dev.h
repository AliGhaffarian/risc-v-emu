#pragma once

#include <stdint.h>

struct io_dev;
/**
 * @brief fields every io_dev share
 */
struct io_dev_metadata {
    void (*_Nullable step)(struct io_dev *_Nonnull self);
    struct sysbus *_Nullable parent;
    uint8_t *_Nonnull exposed_memory;
    uint64_t exposed_memory_length;
    uint64_t mapped_base_addr;
    void (*_Nullable free)(
        struct io_dev *_Nonnull self); /** free dev_specific_data only **/
    void (*_Nullable debug_dump)(
        const struct io_dev *_Nonnull self); /** dump dev_specific_data **/
};

void freep_io_dev_metadata(void *_Nonnull io_dev_metadata_voidpp);
#define _cleanup_free_io_dev_meta_data_                                        \
    __attribute__((__cleanup__(freep_io_dev_metadata)))

/**
 * @brief abstract io_dev, each io_dev should start with the following fields
 */
struct io_dev {
    struct io_dev_metadata meta_data;
    char dev_specific_data[];
};

// dump struct of io_dev, call io_dev->meta_data.debug_dump
void debug_io_dev_dump(const struct io_dev *_Nonnull dev);

// free meta data, call io_dev->meta_data.free
void io_dev_freep(struct io_dev *_Nullable *_Nonnull dev);

// free meta data, call io_dev->meta_data.free
void io_dev_free(struct io_dev *_Nullable dev);

// free meta data, call io_dev->meta_data.free
void io_dev_free_void(void *_Nullable dev);

int new_metadata_with_mem(
    struct io_dev_metadata *_Nullable *_Nonnull ret_metadata,
    const uint8_t *_Nullable mem_buf,
    uint64_t buf_len);
