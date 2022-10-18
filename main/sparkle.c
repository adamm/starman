#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_log.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "lights.h"
#include "patterns.h"
#include "random.h"
#include "sparkle.h"

static const char *TAG = "starman-sparkle";
static uint8_t sparkle_leds[SPARKLE_MAX_LEDS];
static int16_t sparkle_state[SPARKLE_MAX_LEDS];
static bool sparkle_direction[SPARKLE_MAX_LEDS];
static TaskHandle_t sparkle_task;
static pattern_t framebuffer;


void sparkle_step() {
    while(1) {
        memset(framebuffer.active, 0, DISPLAY_LIGHTS_TOTAL_AREA);
        for (uint8_t i = 0; i < SPARKLE_MAX_LEDS; i++) {
            // Go through each of the active sparkle LEDs, and use 
            // sparkle_state to identify how "bright" the sparkle is.
            // If sparkle_direction is true, increase the sparkle_state
            // "brightness" up to SPARKLE_MAX_STATE, then reverse the
            // direction.  When sparkle_state is zero, pick a new LED at
            // random and restart the process.

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
                sparkle_direction[i] = true;
            }

            uint8_t x = sparkle_leds[i] / DISPLAY_LIGHTS_WIDTH;
            uint8_t y = sparkle_leds[i] % DISPLAY_LIGHTS_WIDTH;

            // ESP_LOGI(TAG, "(%d, %d) = %x", x, y, sparkle_state[i]);
            framebuffer.active[y][x] = sparkle_state[i];
        }

        // Use sparkle_state to identify adjacent LEDs. Set their
        // brightness relative to the distance from the centre of the
        // sparkle.

        // FIXME: This is stupid inefficient -- figure out the algorithm properly :)

        /*
        if (sparkleystate[i] < 64) {
            // No adjacent LEDs are lit up
        }
        else if (sparkleystate[i] < 128) {
            // Directlx adjacent LEDs are lit up
            framebuffer.active[y-1][x] = sparkle_state[i] - 64;
            framebuffer.active[y+1][x] = sparkle_state[i] - 64;
            framebuffer.active[y][x-1] = sparkle_state[i] - 64;
            framebuffer.active[y][x+1] = sparkle_state[i] - 64;
            framebuffer.active[y+1][x+1] = sparkle_state[i] - 128;
            framebuffer.active[y+1][x-1] = sparkle_state[i] - 128;
            framebuffer.active[y-1][x+1] = sparkle_state[i] - 128;
            framebuffer.active[y-1][x-1] = sparkle_state[i] - 128;
        }
        else if (sparkle_state[i] < 192) {
            // Two directlx adjacent LEDs are lit up
            framebuffer.active[y-1][x] = sparkle_state[i] - 64;
            framebuffer.active[y+1][x] = sparkle_state[i] - 64;
            framebuffer.active[y][x-1] = sparkle_state[i] - 64;
            framebuffer.active[y][x+1] = sparkle_state[i] - 64;
            framebuffer.active[y+1][x+1] = sparkle_state[i] - 128;
            framebuffer.active[y+1][x-1] = sparkle_state[i] - 128;
            framebuffer.active[y-1][x+1] = sparkle_state[i] - 128;
            framebuffer.active[y-1][x-1] = sparkle_state[i] - 128;
            framebuffer.active[y+2][x+2] = sparkle_state[i] - 192;
            framebuffer.active[y+2][x-2] = sparkle_state[i] - 192;
            framebuffer.active[y-2][x+2] = sparkle_state[i] - 192;
            framebuffer.active[y-2][x-2] = sparkle_state[i] - 192;
        }
        else {
            // Three directlx adjacent LEDs are lit up
            framebuffer.active[y-1][x] = sparkle_state[i] - 64;
            framebuffer.active[y+1][x] = sparkle_state[i] - 64;
            framebuffer.active[y][x-1] = sparkle_state[i] - 64;
            framebuffer.active[y][x+1] = sparkle_state[i] - 64;
            framebuffer.active[y+1][x+1] = sparkle_state[i] - 128;
            framebuffer.active[y+1][x-1] = sparkle_state[i] - 128;
            framebuffer.active[y-1][x+1] = sparkle_state[i] - 128;
            framebuffer.active[y-1][x-1] = sparkle_state[i] - 128;
            framebuffer.active[y+2][x+2] = sparkle_state[i] - 192;
            framebuffer.active[y+2][x-2] = sparkle_state[i] - 192;
            framebuffer.active[y-2][x+2] = sparkle_state[i] - 192;
            framebuffer.active[y-2][x-2] = sparkle_state[i] - 192;
            framebuffer.active[y+3][x+3] = sparkle_state[i] - 256;
            framebuffer.active[y+3][x-3] = sparkle_state[i] - 256;
            framebuffer.active[y-3][x+3] = sparkle_state[i] - 256;
            framebuffer.active[y-3][x-3] = sparkle_state[i] - 256;
        }
        */

        // ESP_LOG_BUFFER_HEX(TAG, framebuffer.active, DISPLAY_LIGHTS_TOTAL_AREA);
        lights_update_leds(framebuffer);

        vTaskDelay(250 / portTICK_RATE_MS);
    }
}


void sparkle_start(void) {
    ESP_LOGI(TAG, "Start sparkle thread");

    // Seed the sparkle pattern by selecting random LEDs, each of which have
    // a random state, and moving in a random direction.
    for (uint8_t i = 0; i < SPARKLE_MAX_LEDS; i++) {
        sparkle_leds[i] = random_value_within_int(DISPLAY_LIGHTS_TOTAL_AREA);
        sparkle_state[i] = random_value_within_int(SPARKLE_STEPS) * SPARKLE_STEP;
        sparkle_direction[i] = random_value_within_int(1);
    }

    xTaskCreate(sparkle_step, "sparkle", 8192, NULL, 5, &sparkle_task);
}


void sparkle_stop(void) {
    ESP_LOGI(TAG, "Stop sparkle thread");

    vTaskDelete(sparkle_task);
}