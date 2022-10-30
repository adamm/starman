#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_log.h>
#include <stdlib.h>
#include <string.h>

#include "buttons.h"
#include "config.h"
#include "display.h"
#include "random.h"
#include "sparkle.h"

static const char *TAG = "starman-sparkle";
static uint8_t sparkle_leds[SPARKLE_MAX_LEDS];
static int16_t sparkle_state[SPARKLE_MAX_LEDS];
static bool sparkle_direction[SPARKLE_MAX_LEDS];
static int16_t sparkle_delay[SPARKLE_MAX_LEDS];
static TaskHandle_t sparkle_task;
static display_t display;

// static const uint8_t sparkle_1[3][3] = {
//     {  25,  92,  25 },
//     {  92, 217,  92 },
//     {  25,  92,  25 },
// };

// static const uint8_t sparkle_2[5][5] = {
//     {   0,  28,  48,  28,   0 },
//     {  28,  98, 152,  98,  28 },
//     {  48, 152, 234, 152,  48 },
//     {  28,  98, 152,  98,  28 },
//     {   0,  28,  48,  28,   0 },
// };

// static const uint8_t sparkle_3[7][7] = {
//     {   0,   4,  16,  32,  16,   4,   0 },
//     {   4,  48,  84, 102,  84,  48,   4 },
//     {  16,  84, 141, 184, 141,  84,  16 },
//     {  32, 102, 184, 241, 184, 102,  32 },
//     {  16,  84, 141, 184, 141,  84,  16 },
//     {   4,  48,  84, 102,  84,  48,   4 },
//     {   0,   4,  16,  32,  16,   4,   0 },
// };

static const uint8_t sparkle_ball[9][9] = {
    {   0,   0,   0,  22,  22,  22,   0,   0,   0 },
    {   0,   9,  49,  62,  76,  62,  49,   9,   0 },
    {   0,  49,  91, 120, 135, 120,  91,  49,   0 },
    {  22,  62, 120, 168, 202, 168, 120,  62,  22 },
    {  22,  76, 135, 202, 244, 202, 135,  76,  22 },
    {  22,  62, 120, 168, 202, 168, 120,  62,  22 },
    {   0,  49,  91, 120, 135, 120,  91,  49,   0 },
    {   0,   9,  49,  62,  76,  62,  49,   9,   0 },
    {   0,   0,   0,  22,  22,  22,   0,   0,   0 },
};

void sparkle_step() {
    while(1) {
        memset(display.active, 1, DISPLAY_LIGHTS_TOTAL_AREA);
        for (uint8_t i = 0; i < SPARKLE_MAX_LEDS; i++) {
            // Go through each of the active sparkle LEDs, and use 
            // sparkle_state to identify how "bright" the sparkle is.
            // If sparkle_direction is true, increase the sparkle_state
            // "brightness" up to SPARKLE_MAX_STATE, then reverse the
            // direction.  When sparkle_state is zero, pick a new LED at
            // random and restart the process.

            sparkle_delay[i] -= SPARKLE_RATE_MS;
            if (sparkle_delay[i] > 0)
                continue;

            if (sparkle_direction[i] == true) {
                sparkle_state[i] += SPARKLE_STEP;
                // ESP_LOGI(TAG, "sparkle %d (led: %d) getting bigger: %d", i, sparkle_leds[i], sparkle_state[i]);
            }
            else {
                sparkle_state[i] -= SPARKLE_STEP;
                // ESP_LOGI(TAG, "sparkle %d (led: %d) getting smaller: %d", i, sparkle_leds[i], sparkle_state[i]);
            }

            if (sparkle_state[i] >= SPARKLE_MAX_STATE) {
                sparkle_state[i] = SPARKLE_MAX_STATE;
                sparkle_direction[i] = false;
            }

            if (sparkle_state[i] <= 0) {
                sparkle_state[i] = 0;
                sparkle_leds[i] = random_value_within_int(DISPLAY_LIGHTS_TOTAL_AREA);
                // sparkle_delay[i] = random_value_within_int(2000);
                sparkle_delay[i] = 0;
                sparkle_direction[i] = true;
            }

            int8_t x = sparkle_leds[i] / DISPLAY_LIGHTS_WIDTH;
            int8_t y = sparkle_leds[i] % DISPLAY_LIGHTS_WIDTH;

            // ESP_LOGI(TAG, "(%d, %d) = %x", x, y, sparkle_state[i]);
            // display.active[y][x] = sparkle_state[i];

            // Use sparkle_state to identify adjacent LEDs. Set their
            // brightness relative to the distance from the centre of the
            // sparkle.

            // FIXME: This is pretty inefficient -- figure out the algorithm properly :)

            for (int8_t sy = 0; sy < 9; sy++) {
                for (int8_t sx = 0; sx < 9; sx++) {
                    int8_t dx = sx - 5;
                    int8_t dy = sy - 5;
                    if ((y+dy < 0) || (x+dx < 0) || (y+dy > DISPLAY_LIGHTS_WIDTH) || (x+dx > DISPLAY_LIGHTS_HEIGHT)) {
                        continue;
                    }

                    // Reduce the sparkle brightness proprotational to the current state
                    uint16_t px = sparkle_state[i] * sparkle_ball[sy][sx] / SPARKLE_MAX_STATE;
                    
                    // Only draw the sparkle ball pixel if the current pixel is darker
                    if (px > display.active[y+dy][x+dx]) {
                        // ESP_LOGI(TAG, "ball(%d) xy(%d,%d) sxy(%d,%d) dxy(%d,%d) xdxy(%d,%d) state(%d) ball(%d) px(%d) -> fb(%d)", i, x, y, sx, sy, dx, dy, x+dx, dx+dy, sparkle_state[i], sparkle_1[sy][sx], px, display.active[y+sy][x+sx]);
                        display.active[y+dy][x+dx] = px;
                    }
                }
            }
        }

        // ESP_LOG_BUFFER_HEX(TAG, display.active, DISPLAY_LIGHTS_TOTAL_AREA);
        display_update_leds(display);

        vTaskDelay(SPARKLE_RATE_MS / portTICK_RATE_MS);
    }
}


void debug_step() {
    static uint8_t leds[DISPLAY_LIGHTS_TOTAL] = {0};
    static int i = 0;

    leds[i] = 0x4f;
    ESP_LOGI(TAG, "Light up #%d", i+1);
    display_update_leds_raw(leds);

    i++;
}


void sparkle_start(void) {
    ESP_LOGI(TAG, "Start sparkle thread");

    // Seed the sparkle pattern by selecting random LEDs, each of which have
    // a random state, and moving in a random direction.
    for (uint8_t i = 0; i < SPARKLE_MAX_LEDS; i++) {
        sparkle_leds[i] = random_value_within_int(DISPLAY_LIGHTS_TOTAL_AREA);
        sparkle_state[i] = 0;
        sparkle_direction[i] = true;
        sparkle_delay[i] = i * 150;
    }

    // DEBUG: swap commented lines to debug light orientation.
    xTaskCreate(sparkle_step, "sparkle", 8192*3, NULL, 0, &sparkle_task);
    // buttons_play_callback(debug_step);
}


void sparkle_stop(void) {
    ESP_LOGI(TAG, "Stop sparkle thread");

    vTaskDelete(sparkle_task);
}
