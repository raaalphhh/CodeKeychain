#pragma once

#include <stdint.h>

typedef enum {
    JOYSTICK_EVENT_NONE = 0,
    JOYSTICK_EVENT_LEFT_CLICK,
    JOYSTICK_EVENT_RIGHT_CLICK,
    JOYSTICK_EVENT_SPEED_CYCLE
} joystick_event_type_t;

typedef struct {
    joystick_event_type_t type;
} joystick_event_t;

void joystick_input_init(void);
void joystick_input_read_mouse_delta(int8_t *dx, int8_t *dy);
void joystick_input_update_button(int64_t now_ms, joystick_event_t *event);
void joystick_input_cycle_speed(void);
int joystick_input_is_button_held(void);
void joystick_input_read_scroll_delta(int8_t *wheel);