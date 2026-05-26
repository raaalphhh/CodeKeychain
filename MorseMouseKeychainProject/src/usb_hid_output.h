#pragma once

#include <stdint.h>

void usb_hid_output_init(void);
void usb_hid_output_send_key(uint8_t key, uint8_t modifier);
void usb_hid_output_send_mouse_move(int8_t dx, int8_t dy);
int usb_hid_output_is_ready(void);