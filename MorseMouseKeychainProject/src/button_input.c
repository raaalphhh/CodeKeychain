#include "button_input.h"

#include "driver/gpio.h"

#define BUTTON_GPIO GPIO_NUM_9

void button_input_init(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUTTON_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    gpio_config(&io_conf);
}

int button_input_is_pressed(void)
{
    return gpio_get_level(BUTTON_GPIO) == 0;
}