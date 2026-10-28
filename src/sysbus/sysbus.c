#include "sysbus.h"
#include "io_dev.h"
#include "linkedlist.h"
#include "logger.h"
#include "stddef.h"
#include <assert.h>
#include <errno.h>
#include <stdlib.h>

void io_devs_step(struct sysbus *_Nonnull self)
{
    assert(self);
    struct linked_list_node *current_node = NULL;
    struct linked_list_node *next_node    = NULL;
    struct io_dev *current_io_dev         = NULL;

    LINKED_LIST_FOREACH(current_node, next_node, self->io_devs)
    {
        current_io_dev = current_node->data;
        current_io_dev->meta_data->step(current_io_dev);
    }
}

bool is_address_inbounds_exclusive(
    uint64_t lowerbound, uint64_t upperbound, uint64_t address)
{
    if(lowerbound < address && address < upperbound) {
        return true;
    }
    if(lowerbound == address) {
        return true;
    }
    return false;
}

bool is_address_inbounds_of_io_dev_exclusive(
    const struct io_dev *_Nonnull dev, uint64_t address)
{
    assert(dev);

    uint64_t dev_base_addr = dev->meta_data->mapped_base_addr;
    uint64_t dev_max_addr =
        dev->meta_data->exposed_memory_length + dev_base_addr;

    return is_address_inbounds_exclusive(dev_base_addr, dev_max_addr, address);
}

void sysbus_do_io_with_dev(
    struct sysbus *_Nonnull self, struct io_dev *_Nonnull dev)
{
    assert(self);
    assert(dev);

    uint64_t current_io_dev_base_addr = dev->meta_data->mapped_base_addr;
    uint64_t current_io_dev_max_addr =
        dev->meta_data->exposed_memory_length + current_io_dev_base_addr;
    uint64_t exposed_memory_offset = 0;

    exposed_memory_offset = self->address - current_io_dev_base_addr;

    if(self->memwrite) {
        logger(LOG_DEBUG, stdout, "writing to the following io dev:\n");
        if(current_log_level >= LOG_DEBUG) {
            __builtin_dump_struct(dev, printf);
        }
        if(dev->meta_data->write) {
            dev->meta_data->write(dev, exposed_memory_offset, self->writedata);
        }
    }
    logger(
        LOG_DEBUG,
        stdout,
        "reading the following value: %hhu\n",
        self->readdata);
    if(dev->meta_data->read) {
        dev->meta_data->read(dev, exposed_memory_offset, &self->readdata);
    }
}

void sysbus_step(struct sysbus *_Nonnull self)
{
    assert(self);
    logger(LOG_DEBUG, stdout, "stepping with the following self:\n");
    if(current_log_level >= LOG_DEBUG) {
        __builtin_dump_struct(self, printf);
    }

    struct linked_list_node *current_node = NULL;
    struct linked_list_node *next_node    = NULL;
    struct io_dev *current_io_dev         = NULL;

    LINKED_LIST_FOREACH(current_node, next_node, self->io_devs)
    {
        current_io_dev = current_node->data;

        logger(LOG_DEBUG, stdout, "matching self->address to io dev:\n");
        if(current_log_level >= LOG_DEBUG) {
            debug_io_dev_dump(current_io_dev);
        }

        if(is_address_inbounds_of_io_dev_exclusive(
               current_io_dev, self->address)) {

            logger(LOG_DEBUG, stdout, "matched\n");
            sysbus_do_io_with_dev(self, current_io_dev);
            goto end_of_step;
        }
    }

    logger(LOG_DEBUG, stdout, "no matched devices for self->address\n");
end_of_step:
    logger(LOG_DEBUG, stdout, "end of step self:\n");
    if(current_log_level >= LOG_DEBUG) {
        __builtin_dump_struct(self, printf);
    }
}

int sysbus_memory_map_iodev(
    struct sysbus *_Nonnull self, struct io_dev *_Nonnull *_Nonnull dev)
{
    assert(self);
    assert(dev);
    assert(*dev);

    uint64_t current_io_dev_max_addr  = 0;
    uint64_t current_io_dev_base_addr = 0;
    uint64_t to_map_io_dev_base_addr  = (*dev)->meta_data->mapped_base_addr;
    uint64_t to_map_io_dev_max_addr =
        to_map_io_dev_base_addr + (*dev)->meta_data->exposed_memory_length;
    struct linked_list_node *dev_node_to_map = NULL;

    struct linked_list_node *current_node = NULL;
    struct linked_list_node *next_node    = NULL;
    struct io_dev *current_io_dev         = NULL;

    // TODO: append in order to prevent needing to go through the whole list to check for conflicts
    // depends on linked_list_append_after_node, which is not implemented yet
    LINKED_LIST_FOREACH(current_node, next_node, self->io_devs)
    {
        current_io_dev           = current_node->data;
        current_io_dev_base_addr = current_io_dev->meta_data->mapped_base_addr;
        current_io_dev_max_addr =
            current_io_dev->meta_data->exposed_memory_length +
            current_io_dev_base_addr;

        if(current_io_dev_base_addr <= to_map_io_dev_base_addr &&
           to_map_io_dev_base_addr <= current_io_dev_max_addr) {
            return ENOMEM;
        }

        if(current_io_dev_base_addr <= to_map_io_dev_max_addr &&
           to_map_io_dev_max_addr <= current_io_dev_max_addr) {
            return ENOMEM;
        }
    }

    dev_node_to_map = linked_list_make_node((void **)dev, io_dev_free_void);
    if(!dev_node_to_map) {
        return ENOMEM;
    }

    linked_list_append(&self->io_devs, &dev_node_to_map);

    return 0;
}

void freep_sysbus(void *_Nonnull sysbus_voidpp)
{
    assert(sysbus_voidpp);
    struct sysbus **sysbuspp = (struct sysbus **)sysbus_voidpp;
    struct sysbus *sysbus    = *sysbuspp;
    if(!sysbus) {
        goto finish;
    }

    free_sysbus(sysbus);
    free(sysbus);

finish:
    *sysbuspp = NULL;
}

void free_sysbus(void *_Nullable sysbus)
{
    struct sysbus *sbus = sysbus;
    if(!sysbus) {
        return;
    }
    linked_list_free(&sbus->io_devs);
}
