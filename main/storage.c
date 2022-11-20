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

#include <esp_log.h>
#include <driver/gpio.h>
#include <nvs_flash.h>
#include <nvs.h>
#include <string.h>

#include "config.h"

#include "storage.h"

static const char *TAG = "starman-storage";


void storage_init() {
    esp_err_t err;
    gpio_config_t io_conf = {};
    int factory_reset = 0;

    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << PLAY_GAME_GPIO);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    // Factory reset pin is active low
    factory_reset = !gpio_get_level(PLAY_GAME_GPIO);

    err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND || factory_reset) {
        ESP_LOGI(TAG, "Performing Factory Reset");
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}


esp_err_t storage_get_str(const char* key, char* value, size_t* len) {
    nvs_handle_t handle;
    esp_err_t err;

    *len = 0;
    nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &handle);
    // We need to first ask for the length, then the value+length.  
    // The length will always be 32 bytes plus null terminator.
    err = nvs_get_str(handle, key, NULL, len);
    if (err == ESP_OK) {
        err = nvs_get_str(handle, key, value, len);
        // ESP_LOGI(TAG, "Loading %s = %s (len: %d)", key, value, *len);
    }
    else {
        // incase value had a default value and storage returned an error,
        // report an accurate length from the old value.
        *len = strlen(value);
        ESP_LOGW(TAG, "nvs_get_str returned err: %d", err);
    }
    nvs_close(handle);

    return err;
}


esp_err_t storage_set_str(const char* key, const char* value) {
    nvs_handle_t handle;
    esp_err_t err;

    ESP_LOGI(TAG, "Setting %s = %s (str)", key, value);

    nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &handle);
    err = nvs_set_str(handle, key, value);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "nvs_set_str returned err: %d", err);
    }
    else {
        err = nvs_commit(handle);
        if (err != ESP_OK)
            ESP_LOGW(TAG, "nvs_commit returned err: %d", err);
    }
    nvs_close(handle);

    return err;
}


esp_err_t storage_get_int8(const char* key, int8_t* value) {
    nvs_handle_t handle;
    esp_err_t err;

    err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "nvs_open returned err: %d", err);
        return err;
    }
    err = nvs_get_i8(handle, key, value);
    // ESP_LOGI(TAG, "Loading %s = %d", key, *value);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "nvs_get_i8 returned err: %d", err);
    }
    nvs_close(handle);

    return err;
}


esp_err_t storage_set_int8(const char* key, int8_t value) {
    nvs_handle_t handle;
    esp_err_t err;

    ESP_LOGI(TAG, "Setting %s = %d (int8)", key, value);

    nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &handle);
    err = nvs_set_i8(handle, key, value);

    if (err != ESP_OK) {
        ESP_LOGW(TAG, "nvs_set_i8 returned err: %d", err);
    }
    else {
        err = nvs_commit(handle);
        if (err != ESP_OK)
            ESP_LOGW(TAG, "nvs_commit returned err: %d", err);
    }
    nvs_close(handle);

    if (err != ESP_OK)
        ESP_LOGW(TAG, "nvs_set_i8 returned err: %d", err);

    return err;
}

