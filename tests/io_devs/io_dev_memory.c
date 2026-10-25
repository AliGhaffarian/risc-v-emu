#include "io_dev_memory.h"
#include "io_dev.h"
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

void test_new_io_dev_memory()
{
    struct io_dev_metadata metadata = {0};
    struct io_dev_memory *got       = NULL;
    metadata.exposed_memory         = calloc(1, 1028);
    metadata.exposed_memory_length  = 1028;

    memcpy(metadata.exposed_memory, "hello!", sizeof("hello!"));

    got = new_io_dev_memory(&metadata);

    TEST_ASSERT_NOT_NULL(got);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(
        "hello!", got->metadata.exposed_memory, sizeof("hello!"));
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_new_io_dev_memory);

    return UNITY_END();
}

// NOLINTEND(readability-magic-numbers)
