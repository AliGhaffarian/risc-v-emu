#pragma once

#include "linkedlist.h"
#include <stdint.h>

struct io_dev;

struct sysbus {
    struct linked_list_node *_Nullable io_devs;
    bool memwrite;
    uint64_t address;
    uint64_t writedata;
    uint64_t readdata;
};

void io_devs_step(struct sysbus *_Nonnull self);
void sysbus_step(struct sysbus *_Nonnull self);
int sysbus_memory_map_iodev(
    struct sysbus *_Nonnull self, struct io_dev *_Nonnull *_Nonnull dev);
