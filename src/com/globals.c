#include "globals.h"

bool is_flag_set(byte value, byte flag) {
    return (value & flag) == flag;
}

byte set_flag(byte value, byte flag) {
    return value | flag;
}

byte clear_flag(byte value, byte flag) {
    return value & ~flag;
}