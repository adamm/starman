#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_log.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "display.h"
#include "led1642gw.h"

static const char *TAG = "starman-display";

//  0 indicates no LED in physical display
// >0 indicates where in the ledbuffer array the LED is located
uint16_t display_out[DISPLAY_LIGHTS_TOTAL];


// Funcion called on each pattern refresh cycle. It will map each active pattern pixel to the physical LED output array.
void display_update_leds(display_t display) {
    memset(display_out, 0, DISPLAY_LIGHTS_TOTAL * 2);

    for (int y = 0; y < DISPLAY_LIGHTS_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_LIGHTS_WIDTH; x++) {
            // Positions in the LED_LUT that are 0 have no physical LED; just ignore out the pattern data.
            if (LED_LUT[y][x] > 0) {
                // The pattern range is 8-bits, but the LED PWM driver supports 16-bits of brightnesses.
                // Until the pattern is expanded to 16-bits, just multiply the pattern value by 256.
                display_out[LED_LUT[y][x]-1] = display.active[y][x] * 256;
                if (display.overlay[y][x] > 0) {
                    display_out[LED_LUT[y][x]-1] = display.overlay[y][x] * 256;
                }
            }
        }
    }

    led1642gw_set_buffer(display_out, DISPLAY_LIGHTS_TOTAL);
    led1642gw_flush_buffer();
}


// Raw access to LEDs output array, ignoring the LUT.  (Useful to debug the LUT!)
void display_update_leds_raw(uint8_t* active) {
    memset(display_out, 0, DISPLAY_LIGHTS_TOTAL * 2);

    for (int i = 0; i < DISPLAY_LIGHTS_TOTAL; i++) {
        display_out[i] = active[i] * 256;
    }

    led1642gw_set_buffer(display_out, DISPLAY_LIGHTS_TOTAL);
    led1642gw_flush_buffer();
}


void display_init(void) {
    led1642gw_init();

    ESP_LOGI(TAG, "Init sucessful");
}
