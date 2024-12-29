#ifndef pyfamicom_cpu_registers_h
#define pyfamicom_cpu_registers_h

#include <stdint.h>

typedef struct {
    uint8_t a;
    uint8_t x;
    uint8_t y;
    uint8_t p;
    uint8_t sp;
    uint8_t pc;
} cpu_registers_t;

extern struct cpu_registers_t cpu_registers;

#endif