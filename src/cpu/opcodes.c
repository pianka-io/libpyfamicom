#include <stdio.h>
#include <stdlib.h>

#include "com/globals.h"
#include "cpu/opcodes.h"
#include "cpu/memory.h"
#include "cpu/stack.h"

void cpu_handle_instruction(struct cpu_t* cpu) {
    word pc = cpu->registers.pc;
    byte opcode = cpu_memory_read_byte(cpu, pc);
    cpu->registers.pc += 1;

    switch (opcode) {
        case 0x10: bpl_10(cpu); break;
        case 0x18: clc_18(cpu); break;
        case 0x20: jsr_20(cpu); break;
        case 0x29: and_29(cpu); break;
        case 0x2C: bit_2c(cpu); break;
        case 0x38: sec_38(cpu); break;
        case 0x40: rti_40(cpu); break;
        case 0x48: pha_48(cpu); break;
        case 0x49: eor_49(cpu); break;
        case 0x4C: jmp_4c(cpu); break;
        case 0x60: rts_60(cpu); break;
        case 0x68: pla_68(cpu); break;
        case 0x69: adc_69(cpu); break;
        case 0x78: sei_78(cpu); break;
        case 0x84: sty_84(cpu); break;
        case 0x85: sta_85(cpu); break;
        case 0x86: stx_86(cpu); break;
        case 0x88: dey_88(cpu); break;
        case 0x8A: txa_8a(cpu); break;
        case 0x8D: sta_8d(cpu); break;
        case 0x8E: stx_8e(cpu); break;
        case 0x90: bcc_90(cpu); break;
        case 0x98: tya_98(cpu); break;
        case 0xA0: ldy_a0(cpu); break;
        case 0xA2: ldx_a2(cpu); break;
        case 0xA5: lda_a5(cpu); break;
        case 0xA8: tay_a8(cpu); break;
        case 0xA9: lda_a9(cpu); break;
        case 0xAA: tax_aa(cpu); break;
        case 0xAD: lda_ad(cpu); break;
        case 0xB0: bcs_b0(cpu); break;
        case 0xB1: lda_b1(cpu); break;
        case 0xBD: lda_bd(cpu); break;
        case 0xC5: cmp_c5(cpu); break;
        case 0xC9: cmp_c9(cpu); break;
        case 0xC6: dec_c6(cpu); break;
        case 0xC8: iny_c8(cpu); break;
        case 0xCA: dex_ca(cpu); break;
        case 0xCD: cmp_cd(cpu); break;
        case 0xD0: bne_d0(cpu); break;
        case 0xF0: beq_f0(cpu); break;
        case 0xE6: inc_e6(cpu); break;
        case 0xE8: inx_e8(cpu); break;
        case 0xEA: nop_ea(cpu); break;
        case 0xEE: inc_ee(cpu); break;
        default:
            printf("[$%04x] unknown opcode $%02x\n", pc, opcode);
            exit(0);
    }
}

/* helpers */
static inline void set_n(struct cpu_t* cpu, byte value) {
    byte p = cpu->registers.p;
    p = (p & ~CPU_STATUS_NEGATIVE) | (value & CPU_STATUS_NEGATIVE);
    cpu->registers.p = p;
}

static inline void set_z(struct cpu_t* cpu, byte value) {
    cpu->registers.p = (cpu->registers.p & ~CPU_STATUS_ZERO) | ((value == 0) ? CPU_STATUS_ZERO : 0);
}

static inline void set_c(struct cpu_t* cpu, word value) {
    cpu->registers.p = (cpu->registers.p & ~CPU_STATUS_CARRY) | ((value > 0xFF) ? CPU_STATUS_CARRY : 0);
}

/* opcodes */
void adc_69(struct cpu_t* cpu) {
    sbyte arg = (sbyte)cpu_memory_read_byte(cpu, cpu->registers.pc);
    //printf("[$%04x] adc #%+d\n", cpu->registers.pc, arg);
    cpu->registers.pc += 1;
    bool carry = is_flag_set(cpu->registers.p, CPU_STATUS_CARRY);
    word result = (word)cpu->registers.a + arg + (word)carry;
    cpu->registers.a = result & 0xFF;
    set_n(cpu, cpu->registers.a);
    set_z(cpu, cpu->registers.a);
    set_c(cpu, result);
    if (((cpu->registers.a ^ result) & (arg ^ result) & 0x80) != 0) {
        cpu->registers.p |= CPU_STATUS_OVERFLOW;
    } else {
        cpu->registers.p &= ~CPU_STATUS_OVERFLOW;
    }
    cpu->clock->cpu_cycles += 2;
}

void and_29(struct cpu_t* cpu) {
    byte arg = cpu_memory_read_byte(cpu, cpu->registers.pc);
    //printf("[$%04x] and #$%02x\n", cpu->registers.pc, arg);
    cpu->registers.pc += 1;
    cpu->registers.a = cpu->registers.a & arg;
    set_n(cpu, cpu->registers.a);
    set_z(cpu, cpu->registers.a);
    cpu->clock->cpu_cycles += 2;
}

void bcc_90(struct cpu_t* cpu) {
    sbyte arg = (sbyte)cpu_memory_read_byte(cpu, cpu->registers.pc);
    word original_pc = cpu->registers.pc + 1;
    //printf("[$%04x] bcc *%+d\n", cpu->registers.pc, arg);
    cpu->registers.pc += 1;
    if (!is_flag_set(cpu->registers.p, CPU_STATUS_CARRY)) {
        word new_pc = cpu->registers.pc + arg;
        cpu->clock->cpu_cycles += 1;
        if ((original_pc & 0xFF00) != (new_pc & 0xFF00)) {
            cpu->clock->cpu_cycles += 1;
        }
        cpu->registers.pc = new_pc;
    }
    cpu->clock->cpu_cycles += 2;
}

void bcs_b0(struct cpu_t* cpu) {
    sbyte arg = (sbyte)cpu_memory_read_byte(cpu, cpu->registers.pc);
    word original_pc = cpu->registers.pc + 1;
    //printf("[$%04x] bcs *%+d\n", cpu->registers.pc, arg);
    cpu->registers.pc += 1;
    if (is_flag_set(cpu->registers.p, CPU_STATUS_CARRY)) {
        word new_pc = cpu->registers.pc + arg;
        cpu->clock->cpu_cycles += 1;
        if ((original_pc & 0xFF00) != (new_pc & 0xFF00)) {
            cpu->clock->cpu_cycles += 1;
        }
        cpu->registers.pc = new_pc;
    }
    cpu->clock->cpu_cycles += 2;
}

void beq_f0(struct cpu_t* cpu) {
    sbyte arg = (sbyte)cpu_memory_read_byte(cpu, cpu->registers.pc);
    word original_pc = cpu->registers.pc + 1;
    //printf("[$%04x] beq *%+d\n", cpu->registers.pc, arg);
    cpu->registers.pc += 1;
    if (is_flag_set(cpu->registers.p, CPU_STATUS_ZERO)) {
        word new_pc = cpu->registers.pc + arg;
        cpu->clock->cpu_cycles += 1;
        if ((original_pc & 0xFF00) != (new_pc & 0xFF00)) {
            cpu->clock->cpu_cycles += 1;
        }
        cpu->registers.pc = new_pc;
    }
    cpu->clock->cpu_cycles += 2;
}

void bit_2c(struct cpu_t* cpu) {
    word arg = cpu_memory_read_word(cpu, cpu->registers.pc);
    //printf("[$%04x] bit $%04x\n", cpu->registers.pc, arg);
    cpu->registers.pc += 2;
    byte value = cpu_memory_read_byte(cpu, arg);
    byte nv = value & (CPU_STATUS_NEGATIVE | CPU_STATUS_OVERFLOW);
    byte p = cpu->registers.p & ~(CPU_STATUS_NEGATIVE | CPU_STATUS_OVERFLOW);
    cpu->registers.p = p | nv;
    byte result = cpu->registers.a & value;
    set_z(cpu, result);
    cpu->clock->cpu_cycles += 4;
}

void bne_d0(struct cpu_t* cpu) {
    sbyte arg = (sbyte)cpu_memory_read_byte(cpu, cpu->registers.pc);
    word original_pc = cpu->registers.pc + 1;
    //printf("[$%04x] bne *%+d\n", cpu->registers.pc, arg);
    cpu->registers.pc += 1;
    if (!is_flag_set(cpu->registers.p, CPU_STATUS_ZERO)) {
        word new_pc = cpu->registers.pc + arg;
        cpu->clock->cpu_cycles += 1;
        if ((original_pc & 0xFF00) != (new_pc & 0xFF00)) {
            cpu->clock->cpu_cycles += 1;
        }
        cpu->registers.pc = new_pc;
    }
    cpu->clock->cpu_cycles += 2;
}

void bpl_10(struct cpu_t* cpu) {
    sbyte arg = (sbyte)cpu_memory_read_byte(cpu, cpu->registers.pc);
    word original_pc = cpu->registers.pc + 1;
    //printf("[$%04x] bpl *%+d\n", cpu->registers.pc, arg);
    cpu->registers.pc += 1;
    if (!is_flag_set(cpu->registers.p, CPU_STATUS_NEGATIVE)) {
        word new_pc = cpu->registers.pc + arg;
        cpu->clock->cpu_cycles += 1;
        if ((original_pc & 0xFF00) != (new_pc & 0xFF00)) {
            cpu->clock->cpu_cycles += 1;
        }
        cpu->registers.pc = new_pc;
    }
    cpu->clock->cpu_cycles += 2;
}

void clc_18(struct cpu_t* cpu) {
    //printf("[$%04x] clc\n", cpu->registers.pc);
    byte value = clear_flag(cpu->registers.p, CPU_STATUS_CARRY);
    cpu->registers.p = value;
    cpu->clock->cpu_cycles += 2;
}

void cmp_c5(struct cpu_t* cpu) {
    byte arg = cpu_memory_read_byte(cpu, cpu->registers.pc);
    //printf("[$%04x] cmp $%02x\n", cpu->registers.pc, arg);
    cpu->registers.pc += 1;
    byte value = cpu_memory_read_byte(cpu, arg);
    word result = (word)cpu->registers.a - (word)value;
    set_n(cpu, result & 0xFF);
    set_z(cpu, result & 0xFF);
    if (cpu->registers.a >= arg) {
        cpu->registers.p |= CPU_STATUS_CARRY;
    } else {
        cpu->registers.p &= ~CPU_STATUS_CARRY;
    }
    cpu->clock->cpu_cycles += 3;
}

void cmp_c9(struct cpu_t* cpu) {
    byte arg = cpu_memory_read_byte(cpu, cpu->registers.pc);
    //printf("[$%04x] cmp #$%02x\n", cpu->registers.pc, arg);
    cpu->registers.pc += 1;
    word result = (word)cpu->registers.a - (word)arg;
    set_n(cpu, result & 0xFF);
    set_z(cpu, result & 0xFF);
    if (cpu->registers.a >= arg) {
        cpu->registers.p |= CPU_STATUS_CARRY;
    } else {
        cpu->registers.p &= ~CPU_STATUS_CARRY;
    }
    cpu->clock->cpu_cycles += 2;
}

void cmp_cd(struct cpu_t* cpu) {
    word arg = cpu_memory_read_word(cpu, cpu->registers.pc);
    //printf("[$%04x] cmp $%04x\n", cpu->registers.pc, arg);
    cpu->registers.pc += 2;
    word value = cpu_memory_read_byte(cpu, arg);
    word result = (word)cpu->registers.a - (word)value;
    set_n(cpu, result & 0xFF);
    set_z(cpu, result & 0xFF);
    if (cpu->registers.a >= arg) {
        cpu->registers.p |= CPU_STATUS_CARRY;
    } else {
        cpu->registers.p &= ~CPU_STATUS_CARRY;
    }
    cpu->clock->cpu_cycles += 4;
}

void dec_c6(struct cpu_t* cpu) {
    byte arg = cpu_memory_read_byte(cpu, cpu->registers.pc);
    //printf("[$%04x] dec $%02x\n", cpu->registers.pc, arg);
    cpu->registers.pc += 1;
    byte value = cpu_memory_read_byte(cpu, arg) - 1;
    cpu_memory_write_byte(cpu, arg, value);
    set_n(cpu, value);
    set_z(cpu, value);
    cpu->clock->cpu_cycles += 5;
}

void dex_ca(struct cpu_t* cpu) {
    //printf("[$%04x] dex\n", cpu->registers.pc);
    cpu->registers.x -= 1;
    set_n(cpu, cpu->registers.x);
    set_z(cpu, cpu->registers.x);
    cpu->clock->cpu_cycles += 2;
}

void dey_88(struct cpu_t* cpu) {
    //printf("[$%04x] dey\n", cpu->registers.pc);
    cpu->registers.y -= 1;
    set_n(cpu, cpu->registers.y);
    set_z(cpu, cpu->registers.y);
    cpu->clock->cpu_cycles += 2;
}

void eor_49(struct cpu_t* cpu) {
    byte arg = cpu_memory_read_byte(cpu, cpu->registers.pc);
    //printf("[$%04x] eor $%02x\n", cpu->registers.pc, arg);
    cpu->registers.pc += 1;
    cpu->registers.a = cpu->registers.a ^ arg;
    set_n(cpu, cpu->registers.a);
    set_z(cpu, cpu->registers.a);
    cpu->clock->cpu_cycles += 2;
}

void inc_e6(struct cpu_t* cpu) {
    byte arg = cpu_memory_read_byte(cpu, cpu->registers.pc);
    //printf("[$%04x] inc $%02x\n", cpu->registers.pc, arg);
    cpu->registers.pc += 1;
    byte value = cpu_memory_read_byte(cpu, arg) + 1;
    cpu_memory_write_byte(cpu, arg, value);
    set_n(cpu, value);
    set_z(cpu, value);
    cpu->clock->cpu_cycles += 5;
}

void inc_ee(struct cpu_t* cpu) {
    word arg = cpu_memory_read_word(cpu, cpu->registers.pc);
    //printf("[$%04x] inc $%04x\n", cpu->registers.pc, arg);
    cpu->registers.pc += 2;
    byte value = cpu_memory_read_byte(cpu, arg) + 1;
    cpu_memory_write_byte(cpu, arg, value);
    set_n(cpu, value);
    set_z(cpu, value);
    cpu->clock->cpu_cycles += 6;
}

void inx_e8(struct cpu_t* cpu) {
    //printf("[$%04x] inx\n", cpu->registers.pc);
    cpu->registers.x += 1;
    set_n(cpu, cpu->registers.x);
    set_z(cpu, cpu->registers.x);
    cpu->clock->cpu_cycles += 2;
}

void iny_c8(struct cpu_t* cpu) {
    //printf("[$%04x] iny\n", cpu->registers.pc);
    cpu->registers.y += 1;
    set_n(cpu, cpu->registers.y);
    set_z(cpu, cpu->registers.y);
    cpu->clock->cpu_cycles += 2;
}

void lda_a5(struct cpu_t* cpu) {
    byte arg = cpu_memory_read_byte(cpu, cpu->registers.pc);
    //printf("[$%04x] lda $%02x\n", cpu->registers.pc, arg);
    cpu->registers.pc += 1;
    byte value = cpu_memory_read_byte(cpu, arg);
    //printf("        value $%02x\n", value);
    cpu->registers.a = value;
    set_n(cpu, value);
    set_z(cpu, value);
    cpu->clock->cpu_cycles += 3;
}

void lda_a9(struct cpu_t* cpu) {
    byte arg = cpu_memory_read_byte(cpu, cpu->registers.pc);
    //printf("[$%04x] lda #$%02x\n", cpu->registers.pc, arg);
    cpu->registers.pc += 1;
    cpu->registers.a = arg;
    set_n(cpu, arg);
    set_z(cpu, arg);
    cpu->clock->cpu_cycles += 2;
}

void lda_ad(struct cpu_t* cpu) {
    word arg = cpu_memory_read_word(cpu, cpu->registers.pc);
    //printf("[$%04x] lda $%04x\n", cpu->registers.pc, arg);
    cpu->registers.pc += 2;
    word value = cpu_memory_read_byte(cpu, arg);
    cpu->registers.a = value;
    set_n(cpu, value);
    set_z(cpu, value);
    cpu->clock->cpu_cycles += 4;
}

void lda_b1(struct cpu_t* cpu) {
    byte arg = cpu_memory_read_byte(cpu, cpu->registers.pc);
    //printf("[$%04x] lda ($%02x),Y\n", cpu->registers.pc, arg);
    cpu->registers.pc += 1;
    word value = cpu_memory_read_word(cpu, arg);
    word address = value + cpu->registers.y;
    byte result = cpu_memory_read_byte(cpu, address);
    cpu->registers.a = result;
    set_n(cpu, result);
    set_z(cpu, result);
    if ((value & 0xFF00) != (address & 0xFF00)) {
        cpu->clock->cpu_cycles += 1;
    }
    cpu->clock->cpu_cycles += 5;
}

void lda_bd(struct cpu_t* cpu) {
    word arg = cpu_memory_read_word(cpu, cpu->registers.pc);
    //printf("[$%04x] lda $%04x,X\n", cpu->registers.pc, arg);
    cpu->registers.pc += 2;
    word address = cpu->registers.x + arg;
    byte value = cpu_memory_read_byte(cpu, address);
    cpu->registers.a = value;
    set_n(cpu, value);
    set_z(cpu, value);
    if ((value & 0xFF00) != (address & 0xFF00)) {
        cpu->clock->cpu_cycles += 1;
    }
    cpu->clock->cpu_cycles += 4;
}

void ldx_a2(struct cpu_t* cpu) {
    byte arg = cpu_memory_read_byte(cpu, cpu->registers.pc);
    //printf("[$%04x] ldx #$%02x\n", cpu->registers.pc, arg);
    cpu->registers.pc += 1;
    cpu->registers.x = arg;
    set_n(cpu, arg);
    set_z(cpu, arg);
    cpu->clock->cpu_cycles += 2;
}

void ldy_a0(struct cpu_t* cpu) {
    byte arg = cpu_memory_read_byte(cpu, cpu->registers.pc);
    //printf("[$%04x] ldy #$%02x\n", cpu->registers.pc, arg);
    cpu->registers.pc += 1;
    cpu->registers.y = arg;
    set_n(cpu, arg);
    set_z(cpu, arg);
    cpu->clock->cpu_cycles += 2;
}

void jmp_4c(struct cpu_t* cpu) {
    word arg = cpu_memory_read_word(cpu, cpu->registers.pc);
    if (arg != 0xe4f0) {
        //printf("[$%04x] jmp $%04x\n", cpu->registers.pc, arg);
    }
    cpu->registers.pc = arg;
    cpu->clock->cpu_cycles += 3;
}

void jsr_20(struct cpu_t* cpu) {
    word arg = cpu_memory_read_word(cpu, cpu->registers.pc);
    //printf("[$%04x] jsr $%04x\n", cpu->registers.pc, arg);
    cpu->registers.pc += 2;
    stack_push_word(cpu, cpu->registers.pc);
    cpu->registers.pc = arg;
    cpu->clock->cpu_cycles += 6;
}

void nop_ea(struct cpu_t* cpu) {
//    //printf("[$%04x] nop\n", cpu->registers.pc);
    cpu->clock->cpu_cycles += 2;
}

void pha_48(struct cpu_t* cpu) {
    //printf("[$%04x] pha\n", cpu->registers.pc);
    stack_push_byte(cpu, cpu->registers.a);
    cpu->clock->cpu_cycles += 3;
}

void pla_68(struct cpu_t* cpu) {
    //printf("[$%04x] pla\n", cpu->registers.pc);
    cpu->registers.a = stack_pull_byte(cpu);
    set_n(cpu, cpu->registers.a);
    set_z(cpu, cpu->registers.a);
    cpu->clock->cpu_cycles += 4;
}

void rti_40(struct cpu_t* cpu) {
    //printf("[$%04x] rti\n", cpu->registers.pc);
    cpu->registers.p = stack_pull_byte(cpu);
    cpu->registers.pc = stack_pull_word(cpu);
    cpu->clock->cpu_cycles += 6;
}

void rts_60(struct cpu_t* cpu) {
    //printf("[$%04x] rts\n", cpu->registers.pc);
    cpu->registers.pc = stack_pull_word(cpu);
    cpu->clock->cpu_cycles += 6;
}

void sec_38(struct cpu_t* cpu) {
    //printf("[$%04x] sec\n", cpu->registers.pc);
    cpu->registers.p = set_flag(cpu->registers.p, CPU_STATUS_CARRY);
    cpu->clock->cpu_cycles += 2;
}

void sei_78(struct cpu_t* cpu) {
    //printf("[$%04x] sei\n", cpu->registers.pc);
    cpu->registers.p = set_flag(cpu->registers.p, CPU_STATUS_INTERRUPT);
    cpu->clock->cpu_cycles += 2;
}

void sta_85(struct cpu_t* cpu) {
    byte arg = cpu_memory_read_byte(cpu, cpu->registers.pc);
    //printf("[$%04x] sta $%02x\n", cpu->registers.pc, arg);
    cpu->registers.pc += 1;
    cpu_memory_write_byte(cpu, arg, cpu->registers.a);
    cpu->clock->cpu_cycles += 3;
}

void sta_8d(struct cpu_t* cpu) {
    word arg = cpu_memory_read_word(cpu, cpu->registers.pc);
    //printf("[$%04x] sta $%04x\n", cpu->registers.pc, arg);
    cpu->registers.pc += 2;
    cpu_memory_write_byte(cpu, arg, cpu->registers.a);
    cpu->clock->cpu_cycles += 4;
}

void stx_86(struct cpu_t* cpu) {
    byte arg = cpu_memory_read_byte(cpu, cpu->registers.pc);
    //printf("[$%04x] stx $%02x\n", cpu->registers.pc, arg);
    cpu->registers.pc += 1;
    cpu_memory_write_byte(cpu, arg, cpu->registers.x);
    cpu->clock->cpu_cycles += 3;
}

void stx_8e(struct cpu_t* cpu) {
    word arg = cpu_memory_read_word(cpu, cpu->registers.pc);
    //printf("[$%04x] stx $%04x\n", cpu->registers.pc, arg);
    cpu->registers.pc += 2;
    cpu_memory_write_byte(cpu, arg, cpu->registers.x);
    cpu->clock->cpu_cycles += 4;
}

void sty_84(struct cpu_t* cpu) {
    byte arg = cpu_memory_read_byte(cpu, cpu->registers.pc);
    //printf("[$%04x] sty $%02x\n", cpu->registers.pc, arg);
    cpu->registers.pc += 1;
    cpu_memory_write_byte(cpu, arg, cpu->registers.y);
    cpu->clock->cpu_cycles += 3;
}

void tax_aa(struct cpu_t* cpu) {
    //printf("[$%04x] tax\n", cpu->registers.pc);
    cpu->registers.x = cpu->registers.a;
    set_n(cpu, cpu->registers.x);
    set_z(cpu, cpu->registers.x);
    cpu->clock->cpu_cycles += 2;
}

void tay_a8(struct cpu_t* cpu) {
    //printf("[$%04x] tay\n", cpu->registers.pc);
    cpu->registers.y = cpu->registers.a;
    set_n(cpu, cpu->registers.y);
    set_z(cpu, cpu->registers.y);
    cpu->clock->cpu_cycles += 2;
}

void txa_8a(struct cpu_t* cpu) {
    //printf("[$%04x] txa\n", cpu->registers.pc);
    cpu->registers.a = cpu->registers.x;
    set_n(cpu, cpu->registers.a);
    set_z(cpu, cpu->registers.a);
    cpu->clock->cpu_cycles += 2;
}

void tya_98(struct cpu_t* cpu) {
    //printf("[$%04x] tya\n", cpu->registers.pc);
    cpu->registers.a = cpu->registers.y;
    set_n(cpu, cpu->registers.a);
    set_z(cpu, cpu->registers.a);
    cpu->clock->cpu_cycles += 2;
}