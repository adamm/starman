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

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <driver/gpio.h>
#include <driver/timer.h>
#include <esp_log.h>
#include <string.h>

#include "buttons.h"
#include "config.h"
#include "display.h"
#include "music.h"
#include "sparkle.h"
#include "text.h"
#include "themes.h"

#define TIMER_SECONDS  2
#define TIMER_DIVIDER  16
#define TIMER_SCALE    (TIMER_BASE_CLK / TIMER_DIVIDER)
#define BUTTON_TIMEOUT (TIMER_SECONDS * TIMER_SCALE)

static const char *TAG = "starman-buttons";

static display_t* display = NULL;
static TaskHandle_t text_task;
static void (*callback_func)(void) = NULL;
static xQueueHandle gpio_evt_queue = NULL;
static xQueueHandle timer_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg) {
    uint32_t gpio_num = (uint32_t) arg;

    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}


static void IRAM_ATTR timer_isr_handler(void* arg) {
    uint64_t counter = timer_group_get_counter_value_in_isr(TIMER_GROUP_0, TIMER_0);

    xQueueSendFromISR(timer_evt_queue, &counter, NULL);
}


static void scroll_theme_text() {
    while(1) {
        text_scroll(display);
        display_update_leds(display);
        vTaskDelay(100 / portTICK_RATE_MS);
    }
}


static void gpio_button_task() {
    uint32_t io_num;
    static bool prev_state = 0;
    static bool theme_select_mode = 0;
    static uint64_t button_pressed_time = 0;
    static uint64_t button_released_time = 0;

    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            // The button GPIO is active-low
            bool now_state = !gpio_get_level(io_num);
            bool button_released = !now_state;

            if (now_state != prev_state) {
                if (now_state == 1) {
                    button_released = 0;
                    button_pressed_time = esp_timer_get_time();
                    button_released_time = 0;
                    timer_start(TIMER_GROUP_0, TIMER_0);
                }
                else {
                    button_released = 1;
                    button_released_time = esp_timer_get_time();
                    timer_pause(TIMER_GROUP_0, TIMER_0);
                    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
                }
            }
            prev_state = now_state;
            printf("GPIO[%d] intr, val: %d\n", io_num, now_state);

            // In normal mode, and user makes a short button press, play the game.
            // In normal mode, and user makes a long button press, go into theme select mode.
            // In theme-select mode, and user makes a short button press, rotate through the theme options.
            // In theme-select mode, and user makes a long button press, save the theme setting and return to normal mode.

            if (button_released && button_pressed_time > 0) {
                ESP_LOGI(TAG, "button release - pressed times: %lld - %lld = %lld",
                    button_released_time, button_pressed_time, button_released_time - button_pressed_time);
                if (button_released_time - button_pressed_time > 1000000) {
                    theme_select_mode = !theme_select_mode;
                    // If entering theme-select mode, do two beeps: low-high notes
                    // If leaving theme-select mode, do two beeps: high-low notes
                    if (theme_select_mode) {
                        char theme_text[15] = {0};

                        ESP_LOGI(TAG, "Entering theme-select mode");
                        music_amp_unmute();
                        music_play_note(C5, 0.2);
                        music_play_note(C6, 0.2);
                        music_amp_mute();

                        // Clear the display and show the currently selected theme
                        sparkle_stop();
                        display = calloc(1, sizeof(display_t));
                        memset(display, 0, sizeof(display_t));

                        sprintf(theme_text, " %s ", themes[config_theme].title);
                        text_write_string(display, theme_text);
                        display_update_leds(display);

                        // Set a timer that calles text_scroll(display) to rotate through the name
                        xTaskCreate(scroll_theme_text, "theme", 8192, NULL, 5, &text_task);
                    }
                    else {
                        ESP_LOGI(TAG, "Leaving theme-select mode");
                        music_amp_unmute();
                        music_play_note(C6, 0.2);
                        music_play_note(C5, 0.2);
                        music_amp_mute();

                        // Stop scrolling and reset the display to the sparkle routine
                        vTaskDelete(text_task);
                        // Give the scroller a chance to complete its final update
                        vTaskDelay(150 / portTICK_RATE_MS);
                        free(display);
                        display = NULL;
                        sparkle_start();
                    }
                    // TODO: start a timer to auto-exit theme-select mode after a five seconds of inactivty.
                }
                else {
                    if (theme_select_mode) {
                        char theme_text[10] = {0};

                        ESP_LOGI(TAG, "Selecting next theme");

                        // make a single beep: medium note
                        music_amp_unmute();
                        music_play_note(G5, 0.1);
                        music_amp_mute();

                        // select the next theme, save it, and scroll its name on the display
                        config_theme++;
                        if (config_theme >= TOTAL_THEMES_AVAILABLE)
                            config_theme = 0;
                        config_set_theme(config_theme);

                        sprintf(theme_text, " %s ", themes[config_theme].title);
                        text_write_string(display, theme_text);
                        display_update_leds(display);
                        // Text should continue to scroll from the established scroll_theme_text() task
                    }
                    else {
                        // Execute normal button press functinoality. 
                        // ie, play the game!
                        ESP_LOGI(TAG, "Calling main program");
                        if (callback_func != NULL)
                            callback_func();
                    }
                }
                button_pressed_time = 0;
            }
        }
    }
}


void buttons_play_callback(void (*callback)(void)) {
    callback_func = callback;
}


void buttons_init(void) {
    timer_config_t timer_conf = {};

    timer_conf.divider = TIMER_DIVIDER;
    timer_conf.counter_dir = TIMER_COUNT_UP;
    timer_conf.counter_en = TIMER_PAUSE;
    timer_conf.alarm_en = TIMER_ALARM_EN;
    timer_conf.auto_reload = false;

    timer_init(TIMER_GROUP_0, TIMER_0, &timer_conf);
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, BUTTON_TIMEOUT);
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);

    timer_isr_callback_add(TIMER_GROUP_0, TIMER_0, timer_evt_queue, NULL, 0);
    // Timer is actually started when the button is pressed; stopped when its released or alarm triggered

    gpio_config_t io_conf = {};

    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.pin_bit_mask = (1ULL << PLAY_GAME_GPIO);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 1;  // This shouldn't be necessary as R2 is doing this -- verify

    gpio_config(&io_conf);

    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(gpio_button_task, "gpio_button_task", 8196, NULL, 10, NULL);

    gpio_install_isr_service(ESP_INTR_FLAG_LOWMED);

    ESP_LOGI(TAG, "Init success");
}


void buttons_start() {
    gpio_isr_handler_add(PLAY_GAME_GPIO, gpio_isr_handler, (void*) PLAY_GAME_GPIO);
    ESP_LOGI(TAG, "Starting button handler");
}


void buttons_stop() {
    gpio_isr_handler_remove(PLAY_GAME_GPIO);
    ESP_LOGI(TAG, "Stopped button handler");
}