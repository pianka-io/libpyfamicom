#include "cpu/stack.h"
#include "memory.h"
#include "com/globals.h"

void stack_push_byte(struct cpu_t* cpu, byte value) {
//    printf("  [%+d] push %02x\n", (sbyte)cpu->registers.sp, value);
    byte sp = cpu->registers.sp;
    cpu_memory_write_byte(cpu, STACK_OFFSET + sp, value);
    cpu->registers.sp -= 1;
}

byte stack_pull_byte(struct cpu_t* cpu) {
    cpu->registers.sp += 1;
    byte value = cpu_memory_read_byte(cpu, STACK_OFFSET + cpu->registers.sp);
//    printf("  [%+d] pull %02x\n", (sbyte)cpu->registers.sp, value);
    return value;
}

void stack_push_word(struct cpu_t* cpu, word value) {
//    printf("  [%+d] push %04x\n", (sbyte)cpu->registers.sp, value);
    byte sp = cpu->registers.sp;
    cpu_memory_write_byte(cpu, STACK_OFFSET + sp, value >> 8);
    sp -= 1;
    cpu_memory_write_byte(cpu, STACK_OFFSET + sp, value);
    sp -= 1;
    cpu->registers.sp = sp;
}

word stack_pull_word(struct cpu_t* cpu) {
    cpu->registers.sp += 1;
    uint8_t low_byte = cpu_memory_read_byte(cpu, STACK_OFFSET + cpu->registers.sp);
    cpu->registers.sp += 1;
    uint8_t high_byte = cpu_memory_read_byte(cpu, STACK_OFFSET + cpu->registers.sp);
    word value = (uint16_t)(high_byte << 8 | low_byte);
//    printf("  [%+d] pull %04x\n", (sbyte)cpu->registers.sp, value);
    return value;
}