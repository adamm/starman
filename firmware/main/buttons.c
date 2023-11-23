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
#include <esp_log.h>

#include "buttons.h"
#include "config.h"

static const char *TAG = "starman-buttons";
static void (*callback_func)(void) = NULL;
static xQueueHandle gpio_evt_queue = NULL;


static void IRAM_ATTR gpio_isr_handler(void* arg) {
    uint32_t gpio_num = (uint32_t) arg;

    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}


static void gpio_play_task() {
    uint32_t io_num;

    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            int state = gpio_get_level(io_num);
            printf("GPIO[%ld] intr, val: %d\n", io_num, state);
            // The play button GPIO is active-low
            if (callback_func != NULL && state == 0)
                callback_func();
        }
    }
}


void buttons_play_callback(void (*callback)(void)) {
    callback_func = callback;
}


void buttons_init(void) {
    gpio_config_t io_conf = {};

    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.pin_bit_mask = (1ULL << PLAY_GAME_GPIO);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 1;  // This shouldn't be necessary as R2 is doing this -- verify

    gpio_config(&io_conf);

    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(gpio_play_task, "gpio_play_task", 8196, NULL, 10, NULL);

    gpio_install_isr_service(ESP_INTR_FLAG_LOWMED);
    gpio_isr_handler_add(PLAY_GAME_GPIO, gpio_isr_handler, (void*) PLAY_GAME_GPIO);

    ESP_LOGI(TAG, "Init success");
}
