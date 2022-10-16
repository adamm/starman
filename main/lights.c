#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_log.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "lights.h"
#include "led1642gw.h"
#include "patterns.h"

static const char *TAG = "starman-lights";

//  0 indicates no LED in physical display
// >0 indicates where in the ledbuffer array the LED is located
static const uint8_t LED_LUT[DISPLAY_LIGHTS_HEIGHT][DISPLAY_LIGHTS_WIDTH] = {
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,  18,  31,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,  19,  17,  32,  30,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,  20,  24,  25,  29,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,  21,  22,  23,  26,  27,  28,   0,   0,   0,   0,   0 },
    {  41,  42,  38,  39,  40,  11,  10,   9,   8,   7,   6, 137, 138, 139, 135, 136 },
    {  45,  44,  43,  37,  36,  13,   0,  16,   1,   0,   5, 141, 140, 134, 133, 132 },
    {   0,  46,  48,  33,  35,  12,   0,  15,   2,   0,   4, 142, 144, 129, 131,   0 },
    {   0,   0,  47,  34,  63,  64,   0,  14,   3,   0, 114, 115, 143, 130,   0,   0 },
    {   0,   0,   0,  50,  49,  62,  61,  60, 117, 116, 113, 128, 127,   0,   0,   0 },
    {   0,   0,  51,  52,  56,  57,  58,  59, 118, 119, 120, 121, 125, 126,   0,   0 },
    {   0,   0,  55,  54,  53,  83,  82,  81,  96,  95,  94, 124, 123, 122,   0,   0 },
    {   0,  67,  66,  79,  78,  85,  84,  88,  89,  93,  92,  99,  98, 100, 101,   0 },
    {   0,  68,  65,  80,  75,  86,  87,   0,   0,  90,  91, 102,  97, 112, 111,   0 },
    {  70,  71,  72,  73,  74,   0,   0,   0,   0,   0,   0, 103, 104, 105, 106, 107 },
    {  69,  77,  76,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 110, 109, 108 },
};
uint16_t lights_out[DISPLAY_LIGHTS_TOTAL];


// Funcion called on each pattern refresh cycle. It will map each active pattern pixel to the physical LED output array.
void lights_update_leds(pattern_t pattern) {
    memset(lights_out, 0, DISPLAY_LIGHTS_TOTAL * 2);

    for (int y = 0; y < DISPLAY_LIGHTS_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_LIGHTS_WIDTH; x++) {
            // Positions in the LED_LUT that are 0 have no physical LED; just ignore out the pattern data.
            if (LED_LUT[x][y] > 0) {
                // The pattern range is 8-bits, but the LED PWM driver supports 16-bits of brightnesses.
                // Until the pattern is expanded to 16-bits, just multiply the pattern value by 256.
                lights_out[LED_LUT[x][y]-1] = pattern.active[x][y] * 256;
            }
        }
    }

    led1642gw_set_buffer(lights_out, DISPLAY_LIGHTS_TOTAL);
    led1642gw_flush_buffer();
}


void lights_init(void) {
    led1642gw_init();

    ESP_LOGI(TAG, "Init sucessful");
}
