#include "morse_input.h"

#include "esp_log.h"
#include <string.h>

static const char *TAG = "MORSE_INPUT";

static char morse_buffer[MORSE_BUFFER_SIZE];
static int morse_index = 0;
static int button_was_pressed = 0;
static int64_t press_start_time = 0;
static int64_t last_release_time = 0;
static int word_gap_sent = 0;

void morse_input_init(void)
{
    morse_index = 0;
    button_was_pressed = 0;
    press_start_time = 0;
    last_release_time = 0;
    morse_buffer[0] = '\0';
    word_gap_sent = 0;
}

void morse_input_update(int button_pressed, int64_t now_ms, morse_event_t *event)
{
    event->type = MORSE_EVENT_NONE;
    event->sequence[0] = '\0';

    // Button pressed
    if (button_pressed && button_was_pressed == 0)
    {
        button_was_pressed = 1;
        press_start_time = now_ms;

        ESP_LOGI(TAG, "Button press started");
    }

    // Button released
    if (!button_pressed && button_was_pressed == 1)
    {
        button_was_pressed = 0;

        int64_t press_duration_ms = now_ms - press_start_time;

        if (morse_index < MORSE_BUFFER_SIZE - 1)
        {
            if (press_duration_ms < DOT_DASH_THRESHOLD_MS)
            {
                morse_buffer[morse_index++] = '.';
                ESP_LOGI(TAG, "DOT added");
            }
            else
            {
                morse_buffer[morse_index++] = '-';
                ESP_LOGI(TAG, "DASH added");
            }

            morse_buffer[morse_index] = '\0';
            last_release_time = now_ms;
            word_gap_sent = 0;

            ESP_LOGI(TAG, "Current Morse: %s", morse_buffer);
        }
    }

    // End of letter gap
    if (!button_pressed && button_was_pressed == 0 && morse_index > 0)
    {
        int64_t gap_ms = now_ms - last_release_time;

        if (gap_ms >= LETTER_GAP_MS)
        {
            event->type = MORSE_EVENT_LETTER_READY;
            strncpy(event->sequence, morse_buffer, MORSE_BUFFER_SIZE);
            event->sequence[MORSE_BUFFER_SIZE - 1] = '\0';

            morse_index = 0;
            morse_buffer[0] = '\0';
        }
    }

    // Word gap
    if (!button_pressed &&
        button_was_pressed == 0 &&
        morse_index == 0 &&
        !word_gap_sent)
    {
        int64_t gap_ms = now_ms - last_release_time;

        if (gap_ms >= WORD_GAP_MS)
        {
            word_gap_sent = 1;

            event->type = MORSE_EVENT_WORD_GAP;

            ESP_LOGI(TAG, "WORD GAP");
        }
    }
}

void morse_input_reset(void)
{
    morse_index = 0;
    button_was_pressed = 0;
    press_start_time = 0;
    last_release_time = 0;
    morse_buffer[0] = '\0';
}