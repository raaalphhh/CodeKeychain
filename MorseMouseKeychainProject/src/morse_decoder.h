#pragma once

#include <stdint.h>

typedef struct {
    uint8_t key;
    uint8_t modifier;
    int found;
} morse_result_t;

morse_result_t morse_decode(const char *morse);