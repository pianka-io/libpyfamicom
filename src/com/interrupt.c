#include "com/interrupt.h"

struct interrupt_t* interrupt_create() {
    return (struct interrupt_t*)calloc(1, sizeof(struct interrupt_t));
}

void interrupt_destroy(struct interrupt_t* interrupt) {
    free(interrupt);
}