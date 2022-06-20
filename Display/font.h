#pragma once

#include <stdint.h>
#include "display.h"

// chars are 8x8 rects

uint8_t spacer[CHAR_SPACER];

uint8_t heart[CHAR_LENGTH] = {
    0b00011110,
    0b00111111,
    0b01111111,
    0b11111110,
    0b01111111,
    0b00111111,
    0b00011110,
};
