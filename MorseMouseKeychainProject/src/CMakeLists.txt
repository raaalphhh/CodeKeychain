idf_component_register(
    SRCS
        "main.c"
        "morse_decoder.c"
        "hid_output.c"
        "button_input.c"
        "morse_input.c"
        "joystick_input.c"
        "esp_hidd_prf_api.c"
        "hid_dev.c"
        "hid_device_le_prf.c"
    INCLUDE_DIRS "."
    REQUIRES bt esp_hid nvs_flash esp_event
)