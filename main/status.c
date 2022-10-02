#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <led_strip.h>
#include <esp_log.h>

#include "config.h"
#include "util.h"

#include "status.h"

static led_strip_t *status_light;
static TaskHandle_t flashing_task;
static const char *TAG = "starman-status";

#define MAX_BRIGHTNESS 16
#define RED   MAX_BRIGHTNESS
#define GREEN MAX_BRIGHTNESS
#define BLUE  MAX_BRIGHTNESS

void status_init() {
    // One addressable LED is found on the STATUS_LED_PIN port
    status_light = led_strip_init(0, STATUS_LED_PIN, 1);
    status_clear();
}


void status_clear() {
    ESP_LOGI(TAG, "Clear");
    status_light->clear(status_light, 50);
}


void status_connecting() {
    ESP_LOGI(TAG, "Connecting");
    status_light->set_pixel(status_light, 0, RED, 0, BLUE);
    status_light->refresh(status_light, 100);
}



void flash_provisioning() {
    while (1) {
        status_light->set_pixel(status_light, 0, RED, GREEN, BLUE);
        status_light->refresh(status_light, 100);
        vTaskDelay(500 / portTICK_RATE_MS);
        status_light->clear(status_light, 50);
        vTaskDelay(500 / portTICK_RATE_MS);
    }
}


void status_provisioning() {
    ESP_LOGI(TAG, "Provisioning");
    xTaskCreate(flash_provisioning, "flash", 2048, NULL, 5, &flashing_task);
}


void status_provisioned() {
    ESP_LOGI(TAG, "Provisioned");
    status_light->clear(status_light, 50);
    vTaskDelete(flashing_task);
}


void status_resetting() {
    ESP_LOGI(TAG, "Resetting");
    status_light->set_pixel(status_light, 0, RED, GREEN, BLUE);
    status_light->refresh(status_light, 100);
}


void status_ready() {
    ESP_LOGI(TAG, "Ready");
    status_light->set_pixel(status_light, 0, 0, GREEN, 0);
    status_light->refresh(status_light, 100);
}


void status_waiting() {
    ESP_LOGD(TAG, "Waiting");
    status_light->set_pixel(status_light, 0, RED, GREEN, 0);
    status_light->refresh(status_light, 100);
}


void status_downloading() {
    ESP_LOGD(TAG, "Downloading");
    status_light->set_pixel(status_light, 0, 0, GREEN, BLUE);
    status_light->refresh(status_light, 100);
}


void status_refreshing() { 
    ESP_LOGI(TAG, "refreshing");
    status_light->set_pixel(status_light, 0, 0, 0, BLUE);
    status_light->refresh(status_light, 100);
}


void status_error() {
    uint8_t blinks = 5;
    ESP_LOGE(TAG, "Error");
    for (uint8_t i = 0; i < blinks; i++) {
        status_light->set_pixel(status_light, 0, RED, 0, 0);
        status_light->refresh(status_light, 100);
        delay(500);
        status_light->set_pixel(status_light, 0, 0, 0, 0);
        status_light->refresh(status_light, 100);
        delay(500);
    }
}
