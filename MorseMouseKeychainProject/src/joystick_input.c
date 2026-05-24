#include "joystick_input.h"

#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define JOY_X_ADC_CHANNEL ADC_CHANNEL_0 // GPIO0
#define JOY_Y_ADC_CHANNEL ADC_CHANNEL_1 // GPIO1
#define JOY_ORIENTATION 0

#define JOY_SW_GPIO GPIO_NUM_2
#define CLICK_GAP_MS 350

#define JOY_DEADZONE 350

static int sw_was_pressed = 0;
static int click_count = 0;
static int64_t last_click_time = 0;

static int joy_center_x = 2048;
static int joy_center_y = 2048;

static const char *TAG = "JOYSTICK";

static adc_oneshot_unit_handle_t adc1_handle;

typedef enum
{
    MOUSE_SPEED_LOW = 0,
    MOUSE_SPEED_MID,
    MOUSE_SPEED_FAST
} mouse_speed_t;

static mouse_speed_t mouse_speed = MOUSE_SPEED_MID;

static int get_speed_divisor(void)
{
    switch (mouse_speed)
    {
    case MOUSE_SPEED_LOW:
        return 500;

    case MOUSE_SPEED_FAST:
        return 220;

    case MOUSE_SPEED_MID:
    default:
        return 350;
    }
}

static int get_max_delta(void)
{
    switch (mouse_speed)
    {
    case MOUSE_SPEED_LOW:
        return 6;

    case MOUSE_SPEED_FAST:
        return 18;

    case MOUSE_SPEED_MID:
    default:
        return 10;
    }
}

static int convert_axis_to_delta(int raw, int center)
{
    int offset = raw - center;

    if (abs(offset) < JOY_DEADZONE)
    {
        return 0;
    }

    int speed_divisor = get_speed_divisor();
    int max_delta = get_max_delta();

    int delta = offset / speed_divisor;

    if (delta > max_delta)
        delta = max_delta;
    if (delta < -max_delta)
        delta = -max_delta;

    return delta;
}

int joystick_input_is_button_held(void)
{
    return gpio_get_level(JOY_SW_GPIO) == 0;
}

void joystick_input_cycle_speed(void)
{
    if (mouse_speed == MOUSE_SPEED_LOW)
    {
        mouse_speed = MOUSE_SPEED_MID;
        ESP_LOGI(TAG, "Mouse speed: MID");
    }
    else if (mouse_speed == MOUSE_SPEED_MID)
    {
        mouse_speed = MOUSE_SPEED_FAST;
        ESP_LOGI(TAG, "Mouse speed: FAST");
    }
    else
    {
        mouse_speed = MOUSE_SPEED_LOW;
        ESP_LOGI(TAG, "Mouse speed: LOW");
    }
}

void joystick_input_update_button(int64_t now_ms, joystick_event_t *event)
{
    event->type = JOYSTICK_EVENT_NONE;

    int sw_pressed = gpio_get_level(JOY_SW_GPIO) == 0;

    if (sw_pressed && sw_was_pressed == 0)
    {
        sw_was_pressed = 1;
    }

    if (!sw_pressed && sw_was_pressed == 1)
    {
        sw_was_pressed = 0;
        click_count++;
        last_click_time = now_ms;
    }

    if (click_count > 0 && (now_ms - last_click_time) >= CLICK_GAP_MS)
    {
        if (click_count == 1)
        {
            event->type = JOYSTICK_EVENT_LEFT_CLICK;
        }
        else if (click_count == 2)
        {
            event->type = JOYSTICK_EVENT_RIGHT_CLICK;
        }
        else
        {
            event->type = JOYSTICK_EVENT_SPEED_CYCLE;
        }

        click_count = 0;
    }
}

void joystick_input_init(void)
{
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc1_handle));

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };

    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, JOY_X_ADC_CHANNEL, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, JOY_Y_ADC_CHANNEL, &config));

    gpio_config_t sw_conf = {
        .pin_bit_mask = (1ULL << JOY_SW_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE};

    gpio_config(&sw_conf);

    int sum_x = 0;
    int sum_y = 0;

    for (int i = 0; i < 50; i++)
    {
        int raw_x = 0;
        int raw_y = 0;

        adc_oneshot_read(adc1_handle, JOY_X_ADC_CHANNEL, &raw_x);
        adc_oneshot_read(adc1_handle, JOY_Y_ADC_CHANNEL, &raw_y);

        sum_x += raw_x;
        sum_y += raw_y;

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    joy_center_x = sum_x / 50;
    joy_center_y = sum_y / 50;

    ESP_LOGI(TAG, "Joystick center calibrated: X=%d Y=%d", joy_center_x, joy_center_y);

    ESP_LOGI(TAG, "Joystick ADC initialized");
}

void joystick_input_read_scroll_delta(int8_t *wheel)
{
    int raw_y = 0;
    adc_oneshot_read(adc1_handle, JOY_Y_ADC_CHANNEL, &raw_y);

    int scroll = convert_axis_to_delta(raw_y, joy_center_y);

    // Adjust direction if scroll feels reversed.
    scroll = -scroll;

    *wheel = (int8_t)scroll;
}

void joystick_input_read_mouse_delta(int8_t *dx, int8_t *dy)
{
    int raw_x = 0;
    int raw_y = 0;

    adc_oneshot_read(adc1_handle, JOY_X_ADC_CHANNEL, &raw_x);
    adc_oneshot_read(adc1_handle, JOY_Y_ADC_CHANNEL, &raw_y);

    int move_x = convert_axis_to_delta(raw_x, joy_center_x);
    int move_y = convert_axis_to_delta(raw_y, joy_center_y);

    switch (JOY_ORIENTATION)
    {
    case 0: // normal
        *dx = (int8_t)move_x;
        *dy = (int8_t)move_y;
        break;

    case 1: // rotate 90 degrees
        *dx = (int8_t)move_y;
        *dy = (int8_t)(-move_x);
        break;

    case 2: // rotate 180 degrees
        *dx = (int8_t)(-move_x);
        *dy = (int8_t)(-move_y);
        break;

    case 3: // rotate 270 degrees
        *dx = (int8_t)(-move_y);
        *dy = (int8_t)move_x;
        break;
    default:
        *dx = (int8_t)move_x;
        *dy = (int8_t)move_y;
        break;
    }
}