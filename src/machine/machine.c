#include "machine.h"
void stall_and_fetch(
    struct rv64_cpu *_Nonnull cpu,
    struct sysbus *_Nullable sbus,
    rv64_instruction_t *_Nonnull current_instruction)
{
    assert(current_instruction);
    while(1) {
        fetch(cpu, current_instruction);
        if(sbus) {
            sysbus_step(sbus);
        }
        if(!cpu->i_flags.stall) {
            break;
        }
    }
}
void machine_mainloop(
    struct rv64_cpu *_Nonnull cpu, struct sysbus *_Nullable sbus)
{
    rv64_instruction_t current_instruction = 0;
    while(1) {
        if(!cpu->i_flags.stall) {
            stall_and_fetch(cpu, sbus, &current_instruction);
        }

        if(current_instruction == HALT_MAGIC) {
            break;
        }

        step_rv64_cpu(cpu, current_instruction);

        if(sbus) {
            sysbus_step(sbus);
        }
    }
}
