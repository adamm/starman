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
    {   0,   0,   0,   0,   0,   0,   0, 114, 127,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0, 115, 113, 128, 126,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0, 116, 120, 121, 125,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0, 117, 118, 119, 122, 123, 124,   0,   0,   0,   0,   0 },
    {   9,  10,   6,   7,   8, 139, 138, 137, 136, 135, 134, 105, 106, 107, 103, 104 },
    {  13,  12,  11,   5,   4, 140,   0, 144, 129,   0, 132, 109, 108, 102, 101, 100 },
    {   0,  14,  16,   1,   3, 141,   0, 143, 130,   0, 133, 110, 112,  97,  99,   0 },
    {   0,   0,  15,   2,  30,  31,   0, 142, 131,   0,  81,  82, 111,  98,   0,   0 },
    {   0,   0,   0,  18,  17,  32,  29,  28,  85,  84,  83,  96,  95,   0,   0,   0 },
    {   0,   0,  19,  20,  24,  25,  26,  27,  86,  87,  88,  89,  93,  94,   0,   0 },
    {   0,   0,  23,  22,  21,  51,  50,  49,  64,  63,  62,  92,  91,  90,   0,   0 },
    {   0,  44,  45,  47,  46,  53,  52,  56,  57,  61,  60,  67,  66,  79,  78,   0 },
    {   0,  34,  33,  48,  43,  54,  55,   0,   0,  58,  59,  70,  65,  80,  77,   0 },
    {  38,  39,  40,  41,  42,   0,   0,   0,   0,   0,   0,  71,  72,  73,  74,  75 },
    {  37,  36,  35,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  69,  68,  76 },
};
uint16_t lights_out[DISPLAY_LIGHTS_TOTAL];


// Funcion called on each pattern refresh cycle. It will map each active pattern pixel to the physical LED output array.
void lights_update_leds(display_t display) {
    memset(lights_out, 0, DISPLAY_LIGHTS_TOTAL * 2);

    for (int y = 0; y < DISPLAY_LIGHTS_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_LIGHTS_WIDTH; x++) {
            // Positions in the LED_LUT that are 0 have no physical LED; just ignore out the pattern data.
            if (LED_LUT[x][y] > 0) {
                // The pattern range is 8-bits, but the LED PWM driver supports 16-bits of brightnesses.
                // Until the pattern is expanded to 16-bits, just multiply the pattern value by 256.
                lights_out[LED_LUT[x][y]-1] = display.active[x][y] * 256;
            }
        }
    }

    led1642gw_set_buffer(lights_out, DISPLAY_LIGHTS_TOTAL);
    led1642gw_flush_buffer();
}


// Raw access to LEDs output array, ignoring the LUT.  (Useful to debug the LUT!)
void lights_update_leds_raw(uint8_t* active) {
    memset(lights_out, 0, DISPLAY_LIGHTS_TOTAL * 2);

    for (int i = 0; i < DISPLAY_LIGHTS_TOTAL; i++) {
        lights_out[i] = active[i] * 256;
    }

    led1642gw_set_buffer(lights_out, DISPLAY_LIGHTS_TOTAL);
    led1642gw_flush_buffer();
}


void lights_init(void) {
    led1642gw_init();

    ESP_LOGI(TAG, "Init sucessful");
}
