#ifndef pytendo_controller_h
#define pytendo_controller_h

#include "com/globals.h"

//typedef struct controller_state_t {
//    byte right : 1;
//    byte left : 1;
//    byte down : 1;
//    byte up : 1;
//    byte start : 1;
//    byte select : 1;
//    byte b : 1;
//    byte a : 1;
//} controller_state_t;

typedef struct controller_t {
    byte state;
    byte shift_register;
} controller_t;

byte controller_press(byte, byte);
byte controller_release(byte, byte);

#endif