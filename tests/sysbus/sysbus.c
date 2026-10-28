#include "sysbus.h"
#include "helper.h"
#include "io_dev.h"
#include "logger.h"
#include "unity.h"

#include <asm-generic/errno-base.h>
#include <stdlib.h>
#include <string.h>

// NOLINTBEGIN(readability-magic-numbers)

static struct io_dev *new_test_io_dev(uint64_t base_addr, uint64_t length)
{
    _cleanup_freep_io_dev_ struct io_dev *dev = calloc(1, sizeof(*dev));
    if(dev == NULL) {
        return NULL;
    }

    int err = new_metadata_with_mem(&dev->meta_data, NULL, length);
    if(err != 0) {
        return NULL;
    }

    dev->meta_data->mapped_base_addr      = base_addr;
    dev->meta_data->exposed_memory_length = length;

    return MOVE(&dev);
}

void setUp(void) {}

void tearDown(void) {}

void test_sysbus_memory_map_iodev(void)
{
    {
        _cleanup_free_sysbus_ struct sysbus self    = {0};
        _cleanup_freep_io_dev_ struct io_dev *first = new_test_io_dev(0, 100);
        _cleanup_freep_io_dev_ struct io_dev *second =
            new_test_io_dev(101, 100);

        TEST_ASSERT_NOT_NULL(first);
        TEST_ASSERT_NOT_NULL(second);

        int err = sysbus_memory_map_iodev(&self, &first);
        TEST_ASSERT_EQUAL(0, err);
        TEST_ASSERT_NULL(first);

        err = sysbus_memory_map_iodev(&self, &second);
        TEST_ASSERT_EQUAL(0, err);
        TEST_ASSERT_NULL(second);
    }

    {
        _cleanup_free_sysbus_ struct sysbus self    = {0};
        _cleanup_freep_io_dev_ struct io_dev *first = new_test_io_dev(0, 100);
        _cleanup_freep_io_dev_ struct io_dev *second =
            new_test_io_dev(100, 100);

        TEST_ASSERT_NOT_NULL(first);
        TEST_ASSERT_NOT_NULL(second);

        int err = sysbus_memory_map_iodev(&self, &first);
        TEST_ASSERT_EQUAL(0, err);
        TEST_ASSERT_NULL(first);

        err = sysbus_memory_map_iodev(&self, &second);
        TEST_ASSERT_EQUAL(ENOMEM, err);
        TEST_ASSERT_NOT_NULL(second);
    }

    {
        _cleanup_free_sysbus_ struct sysbus self     = {0};
        _cleanup_freep_io_dev_ struct io_dev *first  = new_test_io_dev(50, 50);
        _cleanup_freep_io_dev_ struct io_dev *second = new_test_io_dev(5, 45);

        TEST_ASSERT_NOT_NULL(first);
        TEST_ASSERT_NOT_NULL(second);

        int err = sysbus_memory_map_iodev(&self, &first);
        TEST_ASSERT_EQUAL(0, err);
        TEST_ASSERT_NULL(first);

        err = sysbus_memory_map_iodev(&self, &second);
        TEST_ASSERT_EQUAL(ENOMEM, err);
        TEST_ASSERT_NOT_NULL(second);
    }
}

void test_sysbus_step(void)
{
    {
        _cleanup_free_sysbus_ struct sysbus self = {0};

        _cleanup_freep_io_dev_ struct io_dev *first  = new_test_io_dev(0, 100);
        _cleanup_freep_io_dev_ struct io_dev *second = new_test_io_dev(101, 99);
        _cleanup_freep_io_dev_ struct io_dev *third = new_test_io_dev(201, 100);

        TEST_ASSERT_NOT_NULL(first);
        TEST_ASSERT_NOT_NULL(second);
        TEST_ASSERT_NOT_NULL(third);

        /*
         * Keep aliases for assertions after ownership transfers to self.
         * Do not free these aliases independently.
         */
        struct io_dev *first_bak  = first;
        struct io_dev *second_bak = second;
        struct io_dev *third_bak  = third;

        current_log_level = LOG_DEBUG;

        int err = sysbus_memory_map_iodev(&self, &first);
        TEST_ASSERT_EQUAL(0, err);
        TEST_ASSERT_NULL(first);

        err = sysbus_memory_map_iodev(&self, &second);
        TEST_ASSERT_EQUAL(0, err);
        TEST_ASSERT_NULL(second);

        err = sysbus_memory_map_iodev(&self, &third);
        TEST_ASSERT_EQUAL(0, err);
        TEST_ASSERT_NULL(third);

        self.address   = 0;
        self.memwrite  = true;
        self.writedata = 10;

        sysbus_step(&self);
        current_log_level = LOG_INFO;

        TEST_ASSERT_EQUAL(10, first_bak->meta_data->exposed_memory[0]);
        TEST_ASSERT_EQUAL(0, first_bak->meta_data->exposed_memory[1]);
        TEST_ASSERT_EQUAL(0, first_bak->meta_data->exposed_memory[2]);
        TEST_ASSERT_EQUAL(10, self.readdata);

        self.address   = 1;
        self.memwrite  = false;
        self.writedata = 20;

        sysbus_step(&self);

        TEST_ASSERT_EQUAL(10, first_bak->meta_data->exposed_memory[0]);
        TEST_ASSERT_EQUAL(0, first_bak->meta_data->exposed_memory[1]);
        TEST_ASSERT_EQUAL(0, first_bak->meta_data->exposed_memory[2]);
        TEST_ASSERT_EQUAL(0, self.readdata);

        self.address   = 101;
        self.memwrite  = true;
        self.writedata = 30;

        sysbus_step(&self);

        TEST_ASSERT_EQUAL(10, first_bak->meta_data->exposed_memory[0]);
        TEST_ASSERT_EQUAL(0, first_bak->meta_data->exposed_memory[1]);
        TEST_ASSERT_EQUAL(0, first_bak->meta_data->exposed_memory[2]);

        TEST_ASSERT_EQUAL(30, second_bak->meta_data->exposed_memory[0]);
        TEST_ASSERT_EQUAL(0, second_bak->meta_data->exposed_memory[1]);
        TEST_ASSERT_EQUAL(30, self.readdata);

        self.address   = 101;
        self.memwrite  = false;
        self.writedata = 40;

        sysbus_step(&self);

        TEST_ASSERT_EQUAL(10, first_bak->meta_data->exposed_memory[0]);
        TEST_ASSERT_EQUAL(0, first_bak->meta_data->exposed_memory[1]);
        TEST_ASSERT_EQUAL(0, first_bak->meta_data->exposed_memory[2]);

        TEST_ASSERT_EQUAL(30, second_bak->meta_data->exposed_memory[0]);
        TEST_ASSERT_EQUAL(0, second_bak->meta_data->exposed_memory[1]);
        TEST_ASSERT_EQUAL(30, self.readdata);
    }
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_sysbus_memory_map_iodev);
    RUN_TEST(test_sysbus_step);

    return UNITY_END();
}

// NOLINTEND(readability-magic-numbers)
