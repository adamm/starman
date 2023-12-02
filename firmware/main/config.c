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
#include <string.h>

#define CONFIG_IMPORT

#include "storage.h"
#include "util.h"

#include "config.h"

static const char *TAG = "starman-config";


void config_set_firmware_track(char* value) {
    strncpy(config_firmware_track, value, MIN(strlen(value), CONFIG_FIRMWARE_TRACK_MAXLEN));
    storage_set_str(STORAGE_FIRMWARE_TRACK_KEY, value);
}


void config_set_brightness(int8_t value) {
    config_brightness = value;
    storage_set_int8(STORAGE_BRIGHTNESS_KEY, value);
}


void config_set_theme(uint8_t value) {
    config_theme = value;
    storage_set_uint8(STORAGE_THEME_KEY, value);
}


void config_save() {
    storage_set_str(STORAGE_FIRMWARE_URL_KEY, config_firmware_service_url);
    storage_set_str(STORAGE_FIRMWARE_TRACK_KEY, config_firmware_track);
    storage_set_int8(STORAGE_BRIGHTNESS_KEY, config_brightness);
    storage_set_int8(STORAGE_THEME_KEY, config_theme);
}


void config_init() {
    esp_err_t err;
    size_t len;

    // Note: if any config values are undefined in NVS storage, the defaults in config.h are used.

    err = storage_get_str(STORAGE_FIRMWARE_URL_KEY, config_firmware_service_url, &len);
    ESP_LOGW(TAG, "%s: %s (%d bytes) (%d err)", STORAGE_FIRMWARE_URL_KEY, config_firmware_service_url, len, err);

    err = storage_get_str(STORAGE_FIRMWARE_TRACK_KEY, config_firmware_track, &len);
    ESP_LOGW(TAG, "%s: %s (%d bytes) (%d err)", STORAGE_FIRMWARE_TRACK_KEY, config_firmware_track, len, err);

    err = storage_get_int8(STORAGE_BRIGHTNESS_KEY, &config_brightness);
    ESP_LOGW(TAG, "%s: %d (%d err)", STORAGE_BRIGHTNESS_KEY, config_brightness, err);

    err = storage_get_uint8(STORAGE_THEME_KEY, &config_theme);
    ESP_LOGW(TAG, "%s: %d (%d err)", STORAGE_THEME_KEY, config_theme, err);
}