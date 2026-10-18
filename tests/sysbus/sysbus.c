#include "sysbus.h"
#include "helper.h"
#include "io_dev.h"
#include "logger.h"
#include "unity.h"
#include <asm-generic/errno-base.h>
#include <stdlib.h>
#include <string.h>

// NOLINTBEGIN(readability-magic-numbers)

void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    // clean stuff up here
}

void test_sysbus_memory_map_iodev()
{
    {
        struct sysbus self                      = {0};
        struct io_dev *first                    = calloc(1, sizeof(*first));
        struct io_dev *second                   = calloc(1, sizeof(*second));
        int err                                 = 0;
        first->meta_data.mapped_base_addr       = 0;
        first->meta_data.exposed_memory_length  = 100;
        second->meta_data.mapped_base_addr      = 101;
        second->meta_data.exposed_memory_length = 100;

        err = sysbus_memory_map_iodev(&self, &first);
        TEST_ASSERT_EQUAL(0, err);
        TEST_ASSERT_NULL(first);

        err = sysbus_memory_map_iodev(&self, &second);

        TEST_ASSERT_EQUAL(0, err);
        TEST_ASSERT_NULL(second);
    }
    {
        struct sysbus self                      = {0};
        struct io_dev *first                    = calloc(1, sizeof(*first));
        struct io_dev *second                   = calloc(1, sizeof(*second));
        int err                                 = 0;
        first->meta_data.mapped_base_addr       = 0;
        first->meta_data.exposed_memory_length  = 100;
        second->meta_data.mapped_base_addr      = 100;
        second->meta_data.exposed_memory_length = 100;

        err = sysbus_memory_map_iodev(&self, &first);
        TEST_ASSERT_EQUAL(0, err);
        TEST_ASSERT_NULL(first);

        err = sysbus_memory_map_iodev(&self, &second);

        TEST_ASSERT_EQUAL(ENOMEM, err);
        TEST_ASSERT_NOT_NULL(second);
    }
    {
        struct sysbus self                      = {0};
        struct io_dev *first                    = calloc(1, sizeof(*first));
        struct io_dev *second                   = calloc(1, sizeof(*second));
        int err                                 = 0;
        first->meta_data.mapped_base_addr       = 50;
        first->meta_data.exposed_memory_length  = 50;
        second->meta_data.mapped_base_addr      = 5;
        second->meta_data.exposed_memory_length = 45;

        err = sysbus_memory_map_iodev(&self, &first);
        TEST_ASSERT_EQUAL(0, err);
        TEST_ASSERT_NULL(first);

        err = sysbus_memory_map_iodev(&self, &second);

        TEST_ASSERT_EQUAL(ENOMEM, err);
        TEST_ASSERT_NOT_NULL(second);
    }
}

void test_sysbus_step()
{

    {
        struct sysbus self    = {0};
        struct io_dev *first  = calloc(1, sizeof(*first));
        struct io_dev *second = calloc(1, sizeof(*second));
        struct io_dev *third  = calloc(1, sizeof(*third));
        // HACK: we back these pointers up to be able to do assertions easily, don't do this in the real code
        struct io_dev *first_bak  = first;
        struct io_dev *second_bak = second;
        struct io_dev *third_bak  = third;
        int err                   = 0;

        first->meta_data.mapped_base_addr       = 0;
        first->meta_data.exposed_memory_length  = 100;
        first->meta_data.exposed_memory         = calloc(1, 100);
        second->meta_data.mapped_base_addr      = 101;
        second->meta_data.exposed_memory_length = 100;
        second->meta_data.exposed_memory        = calloc(1, 100);
        third->meta_data.mapped_base_addr       = 201;
        third->meta_data.exposed_memory_length  = 100;
        third->meta_data.exposed_memory         = calloc(1, 100);

        current_log_level = LOG_DEBUG;
        sysbus_memory_map_iodev(&self, &first_bak);
        sysbus_memory_map_iodev(&self, &second_bak);
        sysbus_memory_map_iodev(&self, &third_bak);

        self.address   = 0;
        self.memwrite  = true;
        self.writedata = 10;

        sysbus_step(&self);
        current_log_level = LOG_INFO;

        TEST_ASSERT_EQUAL(10, first->meta_data.exposed_memory[0]);
        TEST_ASSERT_EQUAL(0, first->meta_data.exposed_memory[1]);
        TEST_ASSERT_EQUAL(0, first->meta_data.exposed_memory[2]);
        TEST_ASSERT_EQUAL(0, self.readdata);

        self.address   = 1;
        self.memwrite  = false;
        self.writedata = 20;

        sysbus_step(&self);

        TEST_ASSERT_EQUAL(10, first->meta_data.exposed_memory[0]);
        TEST_ASSERT_EQUAL(0, first->meta_data.exposed_memory[1]);
        TEST_ASSERT_EQUAL(0, first->meta_data.exposed_memory[2]);
        TEST_ASSERT_EQUAL(0, self.readdata);

        self.address   = 101;
        self.memwrite  = true;
        self.writedata = 30;

        sysbus_step(&self);

        TEST_ASSERT_EQUAL(10, first->meta_data.exposed_memory[0]);
        TEST_ASSERT_EQUAL(0, first->meta_data.exposed_memory[1]);
        TEST_ASSERT_EQUAL(0, first->meta_data.exposed_memory[2]);

        TEST_ASSERT_EQUAL(30, second->meta_data.exposed_memory[0]);
        TEST_ASSERT_EQUAL(0, second->meta_data.exposed_memory[1]);
        TEST_ASSERT_EQUAL(0, self.readdata);

        self.address   = 101;
        self.memwrite  = false;
        self.writedata = 40;

        sysbus_step(&self);

        TEST_ASSERT_EQUAL(10, first->meta_data.exposed_memory[0]);
        TEST_ASSERT_EQUAL(0, first->meta_data.exposed_memory[1]);
        TEST_ASSERT_EQUAL(0, first->meta_data.exposed_memory[2]);

        TEST_ASSERT_EQUAL(30, second->meta_data.exposed_memory[0]);
        TEST_ASSERT_EQUAL(0, second->meta_data.exposed_memory[1]);
        TEST_ASSERT_EQUAL(30, self.readdata);
    }
}

// void io_devs_step(struct sysbus *_Nonnull self);
// void sysbus_step(struct sysbus *_Nonnull self);

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_sysbus_memory_map_iodev);
    RUN_TEST(test_sysbus_step);

    return UNITY_END();
}

// NOLINTEND(readability-magic-numbers)
