#pragma once
#include "cpu.h"
#include "sysbus.h"
#include <assert.h>

void stall_and_fetch(
    struct rv64_cpu *_Nonnull cpu,
    struct sysbus *_Nullable sbus,
    rv64_instruction_t *_Nonnull current_instruction);
void machine_mainloop(
    struct rv64_cpu *_Nonnull cpu, struct sysbus *_Nullable sbus);
