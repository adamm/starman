/*
   Copyright 2022 Adam McDaniel

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <string.h>
#include <stdlib.h>

#include "adc.h"
#include "config.h"
#include "display.h"
#include "led1642gw.h"

static const char *TAG = "starman-display";

static SemaphoreHandle_t led1642gw_lock = NULL;
static TaskHandle_t brightness_task = NULL;
static uint8_t display_gain = DISPLAY_LIGHTS_DEFAULT_GAIN;


// Funcion called on each pattern refresh cycle. It will map each active pattern pixel to the physical LED output array.
void display_update_leds(display_t* display) {
    uint16_t display_out[DISPLAY_LIGHTS_TOTAL] = {0};

    for (int y = 0; y < DISPLAY_LIGHTS_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_LIGHTS_WIDTH; x++) {
            // Positions in the LED_LUT that are 0 have no physical LED; just ignore out the pattern data.
            if (LED_LUT[y][x] > 0) {
                // The background and overlay range is 8 bits, yet the LED PWM
                // driver supports 16 bits of brightnesses. Until the background
                // and overlay arrays are expanded to 16 bits, just multiply the
                // received values by 256.
                if (display->background[y][x] > 0) {
                    display_out[LED_LUT[y][x]-1] = display->background[y][x] * 256;
                }
                // A display overlay value of 0 is transparent, and allows the
                // background to show however if the overlay is 1, treat it as 0
                // and override the background
                if (display->overlay[y][x] == 1) {
                    display_out[LED_LUT[y][x]-1] = 0;
                }
                else if (display->overlay[y][x] > 1) {
                    display_out[LED_LUT[y][x]-1] = display->overlay[y][x] * 256;
                }
            }
        }
    }

    uint8_t yOffset = 9;

    if (display->text) {
        for (int y = 0; y < display->text_height; y++) {
            for (int x = 0; x < display->text_width; x++) {
                // ESP_LOGI(TAG, "(%d,%d) = %d", y+yOffset, x, display->text[y][x]);
                if (9+y < DISPLAY_LIGHTS_HEIGHT && x < DISPLAY_LIGHTS_WIDTH && LED_LUT[y+yOffset][x] > 0) {
                    // ESP_LOGI(TAG, "Write %d to (%d,%d) which is LUT pos %d", display->text[y][x], 9+y, x, LED_LUT[y+yOffset][x]-1);
                    display_out[LED_LUT[y+yOffset][x]-1] = display->text[y][x] * 100;
                }
            }
        }
    }

    if (xSemaphoreTake(led1642gw_lock, (TickType_t) 1000) == pdTRUE) {
        led1642gw_set_buffer(display_out, DISPLAY_LIGHTS_TOTAL);
        led1642gw_flush_buffer();

        xSemaphoreGive(led1642gw_lock);
    }
    else {
        // We should never be blocked for more than 1000 ticks unless something went very wrong.
        ESP_LOGW(TAG, "Failed to get semaphore lock -- skipping frame");
    }
}


// Raw access to LEDs output array, ignoring the LUT.  (Useful to debug the LUT!)
void display_update_leds_raw(uint8_t* active) {
    uint16_t display_out[DISPLAY_LIGHTS_TOTAL] = {0};

    for (int i = 0; i < DISPLAY_LIGHTS_TOTAL; i++) {
        display_out[i] = active[i] * 256;
    }

    if (xSemaphoreTake(led1642gw_lock, (TickType_t) 1000) == pdTRUE) {
        led1642gw_set_buffer(display_out, DISPLAY_LIGHTS_TOTAL);
        led1642gw_flush_buffer();
        xSemaphoreGive(led1642gw_lock);
    }
}


void display_reset(display_t* display) {
    if (display->text != NULL) {
        for (uint8_t i = 0; i < display->text_height; i++) {
            free(display->text[i]);
        }
        free(display->text);
        display->text = NULL;
    }
    memset(display, 0, sizeof(display_t));
}


static void set_brightness() {
    uint8_t current_gain = DISPLAY_LIGHTS_DEFAULT_GAIN;

    // Every 5 seconds measure the ambient brightness and update the display gain
    // when config_brightness is set to auto (-1).  Otherwise, set display gain to
    // config_brightness directly.
    for(;;) {
        if (config_brightness == -1) {
            uint16_t brightness = adc_get_ambient_light_level();
            display_gain = (uint8_t)((double)brightness * (double)DISPLAY_LIGHTS_MAX_GAIN / (double)ADC_MAX_AMBIENT_LIGHT_LEVEL);
        }
        else {
            display_gain = config_brightness;
        }

        if (display_gain != current_gain) {
            current_gain = display_gain;
            display_set_brightness(display_gain);
        }
        vTaskDelay(5000 / portTICK_RATE_MS);
    }
}


uint8_t display_get_brightness() {
    return display_gain;
}



void display_set_brightness(uint8_t gain) {
    display_gain = gain;

    ESP_LOGI(TAG, "Setting brightness to %d (max is %d)", display_gain, DISPLAY_LIGHTS_MAX_GAIN);

    if (xSemaphoreTake(led1642gw_lock, (TickType_t) 1000) == pdTRUE) {
        led1642gw_set_gain((uint8_t) display_gain);
        led1642gw_flush_config();

        xSemaphoreGive(led1642gw_lock);
    }
}




void display_init(void) {
    led1642gw_init();
    led1642gw_lock = xSemaphoreCreateMutex();

    xTaskCreate(set_brightness, "brightness", 8192, NULL, 0, &brightness_task);

    ESP_LOGI(TAG, "Init sucessful");
}
