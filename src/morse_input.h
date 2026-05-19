#pragma once

#include <stdint.h>

#define DOT_DASH_THRESHOLD_MS 300
#define LETTER_GAP_MS 900
#define MORSE_BUFFER_SIZE 12
#define WORD_GAP_MS 2100

typedef enum {
    MORSE_EVENT_NONE = 0,
    MORSE_EVENT_LETTER_READY,
    MORSE_EVENT_WORD_GAP
} morse_event_type_t;

typedef struct {
    morse_event_type_t type;
    char sequence[MORSE_BUFFER_SIZE];
} morse_event_t;

void morse_input_init(void);
void morse_input_update(int button_pressed, int64_t now_ms, morse_event_t *event);