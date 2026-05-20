#pragma once

#include <stdint.h>

void hid_output_send_key(uint16_t conn_id, uint8_t key, uint8_t modifier);
void hid_output_send_mouse_move(uint16_t conn_id, int8_t dx, int8_t dy);
void hid_output_mouse_click(uint16_t conn_id, uint8_t button);
void hid_output_mouse_scroll(uint16_t conn_id, int8_t wheel);
void hid_output_mouse_drag_move(uint16_t conn_id, int8_t dx, int8_t dy);
void hid_output_mouse_release(uint16_t conn_id);