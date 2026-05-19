#include "hid_output.h"

#include "esp_hidd_prf_api.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void hid_output_send_key(uint16_t conn_id, uint8_t key, uint8_t modifier)
{
    esp_hidd_send_keyboard_value(conn_id, modifier, &key, 1);

    vTaskDelay(100 / portTICK_PERIOD_MS);

    esp_hidd_send_keyboard_value(conn_id, 0, NULL, 0);
}