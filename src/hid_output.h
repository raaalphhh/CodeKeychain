#pragma once

#include <stdint.h>

void hid_output_send_key(uint16_t conn_id, uint8_t key, uint8_t modifier);