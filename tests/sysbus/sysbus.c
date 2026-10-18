#include "sysbus.h"
#include "helper.h"
#include "unity.h"
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

void test_sysbus_memory_map_iodev(){
    {
        struct sysbus self = {0};
        struct io_dev *first = calloc(1, sizeof(*first));
        struct io_dev *second = calloc(1, sizeof(*second));
        int err = 0;
        first->meta_data.mapped_base_addr = 0;
        first->meta_data.exposed_memory_length = 100;
        first->meta_data.mapped_base_addr = 101;
        first->meta_data.exposed_memory_length = 200;

        err = sysbus_memory_map_iodev(&self, &first);
        TEST_ASSERT_NULL(first);
        TEST_ASSERT_EQUAL(0, err);

        err = sysbus_memory_map_iodev(&self, &second);

        TEST_ASSERT_NULL(second);
        TEST_ASSERT_EQUAL(0, err);
    }
}

// void io_devs_step(struct sysbus *_Nonnull self);
// void sysbus_step(struct sysbus *_Nonnull self);

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_sysbus_memory_map_iodev);

    return UNITY_END();
}

// NOLINTEND(readability-magic-numbers)
