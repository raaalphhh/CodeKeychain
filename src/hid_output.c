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

void hid_output_send_mouse_move(uint16_t conn_id, int8_t dx, int8_t dy)
{
    esp_hidd_send_mouse_value(conn_id, 0, dx, dy, 0);
}

#define MOUSE_BUTTON_LEFT  0x01
#define MOUSE_BUTTON_RIGHT 0x02

void hid_output_mouse_click(uint16_t conn_id, uint8_t button)
{
    esp_hidd_send_mouse_value(conn_id, button, 0, 0, 0);
    vTaskDelay(80 / portTICK_PERIOD_MS);
    esp_hidd_send_mouse_value(conn_id, 0, 0, 0, 0);
}

void hid_output_mouse_scroll(uint16_t conn_id, int8_t wheel)
{
    esp_hidd_send_mouse_value(conn_id, 0, 0, 0, wheel);
}
