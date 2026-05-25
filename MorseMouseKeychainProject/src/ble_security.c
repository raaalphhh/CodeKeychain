#include "ble_security.h"

#include "esp_gap_ble_api.h"
#include "esp_bt_defs.h"
#include "esp_log.h"
#include <stdlib.h>
#include "esp_timer.h"
#include <string.h>

#define PAIRING_WINDOW_MS 60000

#define RECONNECT_BLOCK_MS 30000

static esp_bd_addr_t blocked_bda;
static bool blocked_bda_valid = false;
static int64_t blocked_bda_start_ms = 0;

static bool pairing_window_active = false;
static int64_t pairing_window_start_ms = 0;

extern esp_bd_addr_t connected_bda;
extern bool connected_bda_valid;
extern bool sec_conn;

static const char *TAG = "BLE_SECURITY";

void ble_security_disconnect(void)
{
    if (sec_conn && connected_bda_valid)
    {
        ESP_LOGI(TAG, "Disconnecting BLE host");

        esp_ble_gap_disconnect(connected_bda);
    }
}

void ble_security_clear_bonds(void)
{
    int dev_num = esp_ble_get_bond_device_num();

    if (dev_num == 0)
    {
        ESP_LOGI(TAG, "No bonded BLE devices to remove");
        return;
    }

    esp_ble_bond_dev_t *bonded_devices = malloc(sizeof(esp_ble_bond_dev_t) * dev_num);

    if (bonded_devices == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate bonded device list");
        return;
    }

    esp_ble_get_bond_device_list(&dev_num, bonded_devices);

    for (int i = 0; i < dev_num; i++)
    {
        esp_ble_remove_bond_device(bonded_devices[i].bd_addr);
        ESP_LOGI(TAG, "Removed bonded device %d", i);
    }

    free(bonded_devices);

    connected_bda_valid = false;
    sec_conn = false;
}

void ble_security_enter_pairing_mode(void)
{
    ble_security_disconnect();
    ble_security_clear_bonds();

    pairing_window_active = true;
    pairing_window_start_ms = esp_timer_get_time() / 1000;

    ESP_LOGI(TAG, "Pairing window opened for 60 seconds");
}

int ble_security_is_pairing_window_active(void)
{
    return pairing_window_active;
}

void ble_security_update(int64_t now_ms)
{
    if (pairing_window_active &&
        (now_ms - pairing_window_start_ms) >= PAIRING_WINDOW_MS)
    {
        pairing_window_active = false;
        ESP_LOGW(TAG, "Pairing window expired");
    }

    if (blocked_bda_valid &&
        (now_ms - blocked_bda_start_ms) >= RECONNECT_BLOCK_MS)
    {
        blocked_bda_valid = false;
        ESP_LOGI(TAG, "Previous device block expired");
    }
}

void ble_security_close_pairing_window(void)
{
    pairing_window_active = false;
    ESP_LOGI(TAG, "Pairing window closed");
}

void ble_security_block_device_temporarily(const esp_bd_addr_t bd_addr)
{
    memcpy(blocked_bda, bd_addr, sizeof(esp_bd_addr_t));
    blocked_bda_valid = true;
    blocked_bda_start_ms = esp_timer_get_time() / 1000;

    ESP_LOGW(TAG, "Previous device blocked for 30 seconds");
}

int ble_security_is_blocked_device(const esp_bd_addr_t bd_addr)
{
    if (!blocked_bda_valid)
    {
        return 0;
    }

    return memcmp(blocked_bda, bd_addr, sizeof(esp_bd_addr_t)) == 0;
}