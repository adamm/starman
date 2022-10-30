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


void storage_stop() {
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

    ESP_LOGI(TAG, "Setting %s = %s", key, value);

    nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &handle);
    err = nvs_set_str(handle, key, value);
    nvs_close(handle);

    if (err != ESP_OK)
        ESP_LOGW(TAG, "nvs_set_str returned err: %d", err);

    return err;
}

