#pragma once

#include <stdint.h>
#include "esp_bt_defs.h"
#include "esp_gap_ble_api.h"

void ble_security_disconnect(void);
void ble_security_clear_bonds(void);
void ble_security_enter_pairing_mode(void);
int ble_security_is_pairing_window_active(void);
void ble_security_update(int64_t now_ms);
void ble_security_close_pairing_window(void);
void ble_security_block_device_temporarily(const esp_bd_addr_t bd_addr);
int ble_security_is_blocked_device(const esp_bd_addr_t bd_addr);
void ble_security_set_advertising_params(esp_ble_adv_params_t *adv_params);