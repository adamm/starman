#include <driver/gpio.h>

#include "config.h"

#include "ext.h"


static uint8_t lit_led = 0;

void ext_led_flip() {
    lit_led = !lit_led;

    gpio_set_level(EXT_LED1_GPIO, lit_led);
    gpio_set_level(EXT_LED2_GPIO, !lit_led);
}


void ext_led_off() {
    lit_led = 0;
    gpio_set_level(EXT_LED1_GPIO, 0);
    gpio_set_level(EXT_LED2_GPIO, 0);
}


void ext_init() {
    gpio_config_t io_conf = {};

    io_conf.pin_bit_mask = ((1ULL << EXT_LED1_GPIO) | (1ULL << EXT_LED2_GPIO));
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_down_en = 1;
    io_conf.pull_up_en = 0;

    gpio_config(&io_conf);

    ext_led_off();
}