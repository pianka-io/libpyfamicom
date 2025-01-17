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
    memory->prg_rom_size = rom->prg_rom_size;
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
    return cpu_memory_read_byte_ghost(cpu, address, false);
}

byte cpu_memory_read_byte_ghost(struct cpu_t* cpu, word address, bool ghost) {
    address = translate_cpu_address(cpu, address);
    byte status;
    byte value;
    word vram_address;

    switch (address) {
        case PPU_REGISTER_PPUCTRL:
            return cpu->ppu->registers.ppuctrl;
        case PPU_REGISTER_PPUMASK:
            return cpu->ppu->registers.ppumask;
        case PPU_REGISTER_PPUSTATUS:
            status = cpu->ppu->registers.ppustatus;
            if (!ghost) {
                cpu->ppu->registers.ppustatus = clear_flag(cpu->ppu->registers.ppustatus, PPUSTATUS_VBLANK);
            }
            return status;
        case PPU_REGISTER_OAMADDR:
            printf("OAMADDR_READ: Value: 0x%02X\n", cpu->ppu->registers.oamaddr);
            return cpu->ppu->registers.oamaddr;
        case PPU_REGISTER_OAMDATA:
            printf("OAMDATA_READ: Value: 0x%02X\n", oam_memory_read_byte(cpu->ppu, cpu->ppu->registers.oamaddr));
            return oam_memory_read_byte(cpu->ppu, cpu->ppu->registers.oamaddr);
        case PPU_REGISTER_PPUSCROLL:
            value = (cpu->memory->ppuscroll_read ^ 1) ?
                    (cpu->ppu->registers.ppuscroll & 0xFF) :
                    (cpu->ppu->registers.ppuscroll >> 8);

            if (!ghost) {
                cpu->memory->ppuscroll_read ^= 1;
            }

            return value;
        case PPU_REGISTER_PPUADDR:
            value = (cpu->memory->ppuaddr_read ^ 1) ?
                    (cpu->ppu->registers.ppuaddr & 0xFF) :
                    (cpu->ppu->registers.ppuaddr >> 8);

            if (!ghost) {
                cpu->memory->ppuaddr_read ^= 1;
            }

            return value;
        case PPU_REGISTER_PPUDATA:
            vram_address = cpu->ppu->registers.ppuaddr & 0x3FFF;
            value = cpu->ppu->state.vram_read_buffer;
            cpu->ppu->state.vram_read_buffer = ppu_memory_read_byte(cpu->ppu, vram_address);
            cpu->ppu->registers.ppuaddr += cpu->ppu->state.vram_increment;

            if (vram_address >= 0x3F00) {
                cpu->ppu->state.vram_read_buffer = ppu_memory_read_byte(cpu->ppu, vram_address & 0x2FFF);
                return ppu_memory_read_byte(cpu->ppu, vram_address);
            } else {
                return value;
            }
        case INPUT_REGISTER_ONE:
            status = cpu->controller->shift_register & 1;
            if (!ghost) {
                *(byte *) &cpu->controller->shift_register >>= 1;
            }
            return (status | 0x40);
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
    byte* dma;
    byte start_address;

    switch (address) {
        case PPU_REGISTER_PPUCTRL:
            if (cpu->clock->ppu_cycles < 29658) {
                //printf("PPUCTRL_WRITE_BLOCKED\n");
                return;
            }
            cpu->ppu->registers.ppuctrl = value;
            //printf("PPUCTRL_WRITE: Value: 0x%02X, NameTable: %d, VRAM_Increment: %d\n",
//                   value,
//                   cpu->ppu->registers.ppuctrl & PPUCTRL_BASE_NAME_TABLE,
//                   is_flag_set(cpu->ppu->registers.ppuctrl, PPUCTRL_VRAM_INCREMENT) ? 32 : 1);
            cpu->ppu->state.name_table_offset = NAME_TABLE_OFFSETS[cpu->ppu->registers.ppuctrl & PPUCTRL_BASE_NAME_TABLE];
            cpu->ppu->state.sprite_pattern_table = PATTERN_TABLE_OFFSETS[is_flag_set(cpu->ppu->registers.ppuctrl, PPUCTRL_SPRITE_PATTERN_TABLE)];
            cpu->ppu->state.background_pattern_table = PATTERN_TABLE_OFFSETS[is_flag_set(cpu->ppu->registers.ppuctrl, PPUCTRL_BACKGROUND_PATTERN_TABLE)];
            cpu->ppu->state.vram_increment = is_flag_set(cpu->ppu->registers.ppuctrl, PPUCTRL_VRAM_INCREMENT) ? 32 : 1;
            cpu->ppu->state.sprite_size = is_flag_set(cpu->ppu->registers.ppuctrl, PPUCTRL_SPRITE_SIZE) ? 16 : 8;
            break;
        case PPU_REGISTER_PPUMASK:
            cpu->ppu->registers.ppumask = value;
            //printf("PPUMASK_WRITE: Value: 0x%02X\n", value);
            break;
        case PPU_REGISTER_PPUSTATUS:
            cpu->ppu->registers.ppustatus = value;
            //printf("PPUSTATUS_WRITE: Value: 0x%02X\n", value);
            break;
        case PPU_REGISTER_OAMADDR:
            cpu->ppu->registers.oamaddr = value;
            printf("OAMADDR_WRITE: Value: 0x%02X\n", value);
            break;
        case PPU_REGISTER_OAMDATA:
            printf("OAMDATA_WRITE: Value: 0x%02X\n", value);
            oam_memory_write_byte(cpu->ppu, cpu->ppu->registers.oamaddr, value);
            cpu->ppu->registers.oamaddr += 1;
            break;
        case PPU_REGISTER_PPUSCROLL:
            cpu->ppu->registers.ppuscroll &= ~(0xFF << (cpu->memory->ppuscroll_write * 8));
            cpu->ppu->registers.ppuscroll |= (value << (cpu->memory->ppuscroll_write * 8));
            printf("PPUSCROLL_WRITE: Value: $%02x Final $%04x, WritePhase: %d\n", value, cpu->ppu->registers.ppuscroll, cpu->memory->ppuscroll_write);
            cpu->memory->ppuscroll_write ^= 1;
            break;
        case PPU_REGISTER_PPUADDR:
            cpu->ppu->registers.ppuaddr &= ~(0xFF << (cpu->memory->ppuaddr_write * 8));
            cpu->ppu->registers.ppuaddr |= (value << (cpu->memory->ppuaddr_write * 8));
            //printf("PPUADDR_WRITE: Value: 0x%02X, Addr: 0x%04X, WritePhase: %d\n", value, cpu->ppu->registers.ppuaddr, cpu->memory->ppuaddr_write);
            cpu->memory->ppuaddr_write ^= 1;
            break;
        case PPU_REGISTER_PPUDATA:
            cpu->ppu->registers.ppudata = value;
            //printf("PPUDATA_WRITE: Addr: 0x%04X, Value: 0x%02X, VRAM_Increment: %d\n",
//                   cpu->ppu->registers.ppuaddr,
//                   cpu->ppu->registers.ppudata,
//                   cpu->ppu->state.vram_increment);
            ppu_memory_write_byte(cpu->ppu, cpu->ppu->registers.ppuaddr, cpu->ppu->registers.ppudata);
            cpu->ppu->registers.ppuaddr = (cpu->ppu->registers.ppuaddr + cpu->ppu->state.vram_increment) & 0x3FFF;
            break;
        case INPUT_REGISTER_ONE:
            if (value & 1) {
                cpu->controller->shift_register = cpu->controller->state;
//                printf("INPUT_WRITE: Strobe Set, ShiftRegister: 0x%02X\n", *(uint8_t*)&cpu->controller->shift_register);
            } else {
//                printf("INPUT_WRITE: Strobe Cleared\n");
            }
            break;
        case OAM_DMA:
            dma = &cpu->memory->data[value << 8];
            start_address = cpu->ppu->registers.oamaddr;

            for (int i = 0; i < 256; i++) {
                cpu->ppu->memory->oam[start_address] = dma[i];
                start_address = (start_address + 1) & 0xFF;
            }
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