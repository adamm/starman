#include <esp_log.h>
#include "storage.h"

#define CONFIG_IMPORT

#include "config.h"

static const char *TAG = "starman-config";

void config_init() {
    esp_err_t err;
    size_t len;

    // Note: if image-service or firmware-service URLs are undefined in
    // NVS storage, the defaults in config.h are used.

    err = storage_get_str(STORAGE_FIRMWARE_URL_KEY, config_firmware_service_url, &len);
    ESP_LOGW(TAG, "%s: %s (%d bytes) (%d err)", STORAGE_FIRMWARE_URL_KEY, config_firmware_service_url, len, err);

    err = storage_get_str(STORAGE_FIRMWARE_TRACK_KEY, config_firmware_track, &len);
    ESP_LOGW(TAG, "%s: %s (%d bytes) (%d err)", STORAGE_FIRMWARE_TRACK_KEY, config_firmware_service_url, len, err);
}


void config_save() {
    storage_set_str(STORAGE_FIRMWARE_URL_KEY, config_firmware_service_url);
    storage_set_str(STORAGE_FIRMWARE_TRACK_KEY, config_firmware_track);
}

