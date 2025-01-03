#include <stdlib.h>
#include <string.h>

#include "com/globals.h"
#include "cpu/memory.h"
#include "ppu/memory.h"

word translate_cpu_address(struct cpu_t*, word);

struct cpu_memory_t* cpu_memory_create(struct rom_t* rom) {
    struct cpu_memory_t* memory = (struct cpu_memory_t*)calloc(1, sizeof(struct cpu_memory_t));
    memcpy(memory->data + PRG_ROM_OFFSET, rom->prg_rom, rom->prg_rom_size);

    memory->mirrored = rom->header.prg_rom == 1;
    memory->ppuscroll_read = 1;
    memory->ppuscroll_write = 1;
    memory->ppuaddr_read = 1;
    memory->ppuaddr_write = 1;

    return memory;
}

void cpu_memory_destroy(struct cpu_memory_t* memory) {
    free(memory);
}

byte cpu_memory_read_byte(struct cpu_t* cpu, word address) {
    address = translate_cpu_address(cpu, address);
    byte status;

    switch (address) {
        case PPU_REGISTER_PPUCTRL:
            return cpu->ppu->registers.ppuctrl;
        case PPU_REGISTER_PPUMASK:
            return cpu->ppu->registers.ppumask;
        case PPU_REGISTER_PPUSTATUS:
            status = cpu->ppu->registers.ppustatus;
            cpu->ppu->registers.ppustatus = clear_flag(cpu->ppu->registers.ppustatus, PPUSTATUS_VBLANK);
            return status;
        case PPU_REGISTER_OAMADDR:
            return cpu->ppu->registers.oamaddr;
        case PPU_REGISTER_OAMDATA:
            return cpu->ppu->registers.oamdata;
        case PPU_REGISTER_PPUSCROLL:
            return (cpu->memory->ppuscroll_read ^= 1) ?
                   (cpu->ppu->registers.ppuscroll & 0xFF) :
                   (cpu->ppu->registers.ppuscroll >> 8);
        case PPU_REGISTER_PPUADDR:
            return (cpu->memory->ppuaddr_read ^= 1) ?
                   (cpu->ppu->registers.ppuaddr & 0xFF) :
                   (cpu->ppu->registers.ppuaddr >> 8);
        case PPU_REGISTER_PPUDATA:
            return cpu->ppu->registers.ppudata;
        case PPU_REGISTER_OAMDMA:
            return cpu->ppu->registers.oamdma;
        default:
            return cpu->memory->data[address];
    }
}

word cpu_memory_read_word(struct cpu_t* cpu, word address) {
    address = translate_cpu_address(cpu, address);
    byte low_byte = cpu->memory->data[address];
    byte high_byte = cpu->memory->data[address + 1];
    return (word)(high_byte << 8 | low_byte);
}

void cpu_memory_write_byte(struct cpu_t* cpu, word address, byte value) {
    address = translate_cpu_address(cpu, address);

    switch (address) {
        case PPU_REGISTER_PPUCTRL:
            if (cpu->clock->ppu_cycles < 29658) {
                return;
            }
            cpu->ppu->registers.ppuctrl = value;
            // name table
            byte name_table = cpu->ppu->registers.ppuctrl & PPUCTRL_BASE_NAME_TABLE;
            cpu->ppu->state.name_table_offset = NAME_TABLE_OFFSETS[name_table];
            // sprite pattern table
            cpu->ppu->state.sprite_pattern_table = PATTERN_TABLE_OFFSETS[is_flag_set(
                cpu->ppu->registers.ppuctrl,
                PPUCTRL_SPRITE_PATTERN_TABLE
            )];
            // background pattern table
            cpu->ppu->state.background_pattern_table = PATTERN_TABLE_OFFSETS[is_flag_set(
                cpu->ppu->registers.ppuctrl,
                PPUCTRL_BACKGROUND_PATTERN_TABLE
            )];
            // vram increment
            cpu->ppu->state.vram_increment = is_flag_set(
                    cpu->ppu->registers.ppuctrl,
                    PPUCTRL_VRAM_INCREMENT
            ) ? 32 : 1;
            break;
        case PPU_REGISTER_PPUMASK:
            cpu->ppu->registers.ppumask = value;
            break;
        case PPU_REGISTER_PPUSTATUS:
            cpu->ppu->registers.ppustatus = value;
            break;
        case PPU_REGISTER_OAMADDR:
            cpu->ppu->registers.oamaddr = value;
            break;
        case PPU_REGISTER_OAMDATA:
            cpu->ppu->registers.oamdata = value;
            break;
        case PPU_REGISTER_PPUSCROLL:
            cpu->ppu->registers.ppuscroll &= ~(0xFF << (cpu->memory->ppuscroll_write * 8));
            cpu->ppu->registers.ppuscroll |= (value << (cpu->memory->ppuscroll_write * 8));
            cpu->memory->ppuscroll_write ^= 1;
            break;
        case PPU_REGISTER_PPUADDR:
            cpu->ppu->registers.ppuaddr &= ~(0xFF << (cpu->memory->ppuaddr_write * 8));
            cpu->ppu->registers.ppuaddr |= (value << (cpu->memory->ppuaddr_write * 8));
            cpu->memory->ppuaddr_write ^= 1;
            break;
        case PPU_REGISTER_PPUDATA:
            cpu->ppu->registers.ppudata = value;
            ppu_memory_write_byte(
                cpu->ppu,
                cpu->ppu->registers.ppuaddr,
                cpu->ppu->registers.ppudata
            );
            cpu->ppu->registers.ppuaddr = (cpu->ppu->registers.ppuaddr + cpu->ppu->state.vram_increment) & 0x3FFF;
            break;
        case PPU_REGISTER_OAMDMA:
            cpu->ppu->registers.oamdma = value;
            break;
        default:
            cpu->memory->data[address] = value;
            break;
    }
}

word translate_cpu_address(struct cpu_t* cpu, word address) {
    if (address >= 0x2000 && address <= 0x3FFF) {
        return 0x2000 + (address & 0x0007);
    }
    if (cpu->memory->mirrored && address >= PRG_ROM_OFFSET + PRG_RAM_BANK_SIZE) {
        return address - PRG_RAM_BANK_SIZE;
    }
    return address;
}