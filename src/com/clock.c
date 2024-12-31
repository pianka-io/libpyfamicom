#include <stdlib.h>
#include <printf.h>
#include "com/clock.h"

struct nes_clock_t* clock_create(){
    return (struct nes_clock_t*)calloc(1, sizeof(struct nes_clock_t));
}

void clock_destroy(struct nes_clock_t* clock) {
    free(clock);
}