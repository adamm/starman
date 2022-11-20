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
#include <driver/rmt.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <led_strip.h>
#include <esp_log.h>

#include "config.h"
#include "util.h"

#include "rgb.h"

static led_strip_t *rgb_light;
static TaskHandle_t flashing_task;
static const char *TAG = "starman-rgb";
static bool rgb_gpio_ready = false;

#define MAX_BRIGHTNESS 16
#define RED   MAX_BRIGHTNESS
#define GREEN MAX_BRIGHTNESS
#define BLUE  MAX_BRIGHTNESS


static void IRAM_ATTR ws2812b_bit_0() {
  gpio_set_level(RGB_LED_GPIO, 1);

  //delay 0.4us
  asm("nop; nop;");
  asm("nop; nop;");
  asm("nop; nop;");
  asm("nop; nop;");

  gpio_set_level(RGB_LED_GPIO, 0);

  // delay 0.85us (additional dleay happens within color_display();
  asm("nop; nop; nop; nop; nop; nop; nop; nop;"
    "nop; nop; nop; nop; nop; nop; nop; nop;"
    "nop; nop; nop; nop;");
}

static void IRAM_ATTR ws2812b_bit_1() {
  gpio_set_level(RGB_LED_GPIO, 1);

  //delay 0.8us
  asm("nop; nop; nop; nop; nop; nop; nop; nop;"
    "nop; nop; nop; nop; nop; nop; nop; nop;"
    "nop; nop; nop; nop; nop; nop; nop; nop;"
    "nop; nop; nop; nop; nop; nop; nop; nop;"
    "nop; nop; nop; nop; nop; nop; nop; nop;"
    "nop; nop; nop; nop; nop; nop; nop; nop;"
    "nop; nop; nop; nop; nop; nop; nop; nop;");

  gpio_set_level(RGB_LED_GPIO, 0);

  // delay 0.45us happens naturally within color_display() function
  asm("nop; nop;");
  asm("nop; nop;");
  asm("nop; nop;");
  asm("nop; nop;");
}


static void reverse(uint8_t* num) {
    uint8_t tmp = 0;

    for (uint8_t i = 0; i < (sizeof(uint8_t) * 8 - 1); i++) {
       tmp |= *num & 1;
       *num >>= 1;
       tmp <<= 1;
    }

    *num = tmp;
}



static void color_display(uint8_t red, uint8_t green, uint8_t blue) {
    uint8_t i = 0;
    uint16_t color;

    if (!rgb_gpio_ready)
        return;

    portDISABLE_INTERRUPTS();

    reverse(&green);
    reverse(&red);
    reverse(&blue);

    // Force setting GPIO to low, which speeds up the subsequent calls to bitbang the protocol
    gpio_set_level(RGB_LED_GPIO, 0);

    for (i = 0; i < 8; i++) {
        if (green & 1)
            ws2812b_bit_1();
        else
            ws2812b_bit_0();
        green >>= 1;
    }

    for (i = 0; i < 8; i++) {
        if (red & 1)
            ws2812b_bit_1();
        else
            ws2812b_bit_0();
        red >>= 1;
    }

    for (i = 0; i < 8; i++) {
        if (blue & 1)
            ws2812b_bit_1();
        else
            ws2812b_bit_0();
        blue >>= 1;
    }

    portENABLE_INTERRUPTS();
}




void rgb_init() {
    // One addressable LED is found on the RGB_LED_GPIO port
    // Unfortunately RMT wont work as there aren't any timers left.
    // Instead the WS2812B is bitbanged.

    gpio_config_t io_conf = {};

    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << RGB_LED_GPIO);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    gpio_set_level(RGB_LED_GPIO, 0);
    rgb_gpio_ready = true;
    rgb_clear();

    ESP_LOGI(TAG, "Init success");
}


void rgb_clear() {
    ESP_LOGI(TAG, "Clear");
    color_display(0, 0, 0);
}


void rgb_color_red() {
    ESP_LOGI(TAG, "Red");
    color_display(255, 0, 0);
}


void rgb_color_green() {
    ESP_LOGI(TAG, "Green");
    color_display(0, 255, 0);
}


void rgb_color_blue() {
    ESP_LOGI(TAG, "Blue");
    color_display(0, 0, 255);
}


void rgb_color_white() {
    ESP_LOGI(TAG, "White");
    color_display(255, 255, 255);
}


void flash_connecting() {
    while (1) {
        // rgb_light->set_pixel(rgb_light, 0, RED, GREEN, BLUE);
        // rgb_light->refresh(rgb_light, 100);
        rgb_color_white();
        vTaskDelay(500 / portTICK_RATE_MS);

        // rgb_light->clear(rgb_light, 50);
        rgb_clear();
        vTaskDelay(500 / portTICK_RATE_MS);
    }
}


void rgb_connecting() {
    ESP_LOGI(TAG, "Connecting");
    xTaskCreate(flash_connecting, "flash", 2048, NULL, 0, &flashing_task);
}


void rgb_connected() {
    ESP_LOGI(TAG, "Connected");
    vTaskDelete(flashing_task);
    color_display(0, 0, 0);
}


void rgb_error() {
    uint8_t blinks = 5;
    ESP_LOGE(TAG, "Error");
    for (uint8_t i = 0; i < blinks; i++) {
        rgb_color_red();
        vTaskDelay(500 / portTICK_RATE_MS);
        rgb_clear();
        vTaskDelay(500 / portTICK_RATE_MS);
    }
}
