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

#ifndef STORAGE_H
#define STORAGE_H

#include <esp_err.h>

#define STORAGE_NAMESPACE "starman"
#define STORAGE_FIRMWARE_URL_KEY "firmware-url"
#define STORAGE_FIRMWARE_TRACK_KEY "firmware-track"
#define STORAGE_DEVICE_TRACK_KEY "device-track"
#define STORAGE_BRIGHTNESS_KEY "brightness"
#define STORAGE_THEME_KEY "theme"

void storage_init();
esp_err_t storage_get_str(const char* key, char* value, size_t* len);
esp_err_t storage_set_str(const char* key, const char* value);
esp_err_t storage_get_int8(const char* key, int8_t* value);
esp_err_t storage_set_int8(const char* key, int8_t value);
esp_err_t storage_get_uint8(const char* key, uint8_t* value);
esp_err_t storage_set_uint8(const char* key, uint8_t value);

#endif
