#include "emu/controller.h"

byte controller_press(byte state, uint8_t button) {
    state |= button;
    printf("press %02x\n", state);
    return state;
}

byte controller_release(byte state, uint8_t button) {
    state &= ~button;
    printf("release %02x\n", state);
    return state;
}
