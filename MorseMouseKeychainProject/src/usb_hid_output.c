#include "usb_hid_output.h"

#include "tinyusb.h"
#include "tusb.h"
#include "class/hid/hid_device.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "USB_HID";

#define EPNUM_HID   0x81
#define CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN)

static const uint8_t hid_report_descriptor[] = {
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(1)),
    TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(2))
};

static const uint8_t configuration_descriptor[] = {
    TUD_CONFIG_DESCRIPTOR(
        1,
        1,
        0,
        CONFIG_TOTAL_LEN,
        0,
        100),

    TUD_HID_DESCRIPTOR(
        0,
        0,
        HID_ITF_PROTOCOL_NONE,
        sizeof(hid_report_descriptor),
        EPNUM_HID,
        16,
        10)
};

const uint8_t *tud_hid_descriptor_report_cb(uint8_t instance)
{
    (void) instance;
    return hid_report_descriptor;
}

uint16_t tud_hid_get_report_cb(
    uint8_t instance,
    uint8_t report_id,
    hid_report_type_t report_type,
    uint8_t *buffer,
    uint16_t reqlen)
{
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;
    return 0;
}

void tud_hid_set_report_cb(
    uint8_t instance,
    uint8_t report_id,
    hid_report_type_t report_type,
    uint8_t const *buffer,
    uint16_t bufsize)
{
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) bufsize;
}

void usb_hid_output_init(void)
{
    tinyusb_config_t tusb_cfg = {
        .device_descriptor = NULL,
        .string_descriptor = NULL,
        .string_descriptor_count = 0,
        .external_phy = false,
        .configuration_descriptor = configuration_descriptor,
    };

    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    ESP_LOGI(TAG, "USB HID initialized");
}

void usb_hid_output_send_key(uint8_t key, uint8_t modifier)
{
    if (!tud_mounted())
    {
        return;
    }

    uint8_t keycode[6] = {0};
    keycode[0] = key;

    tud_hid_keyboard_report(1, modifier, keycode);
    vTaskDelay(pdMS_TO_TICKS(100));
    tud_hid_keyboard_report(1, 0, NULL);
}

void usb_hid_output_send_mouse_move(int8_t dx, int8_t dy)
{
    if (!tud_mounted())
    {
        return;
    }

    tud_hid_mouse_report(2, 0, dx, dy, 0, 0);
}

int usb_hid_output_is_ready(void)
{
    return tud_mounted();
}