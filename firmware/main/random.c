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
#include <esp_random.h>

#include "random.h"

static const char *TAG = "starman-random";


bool random_bool_under_percent(uint8_t percent) {
    uint32_t rnd = esp_random();
    uint64_t max_pass = percent * (uint64_t)UINT32_MAX / 100;

    ESP_LOGD(TAG, "random_value_under_percent() percent: %d, rnd: %lu, max_pass: %llu", percent, rnd, max_pass);

    return (rnd < max_pass);
}

uint32_t random_value_within_int(uint32_t max_value) {
    uint32_t rnd = esp_random();
    uint64_t value = (uint64_t)rnd * (uint64_t)max_value / (uint64_t)UINT32_MAX;
    ESP_LOGD(TAG, "random_value_within_int() max_value: %lu, rnd: %lu, value: %llu", max_value, rnd, value);

    return (uint32_t)value;
}


void random_init(void) {
    ESP_LOGI(TAG, "Init success");

    // TODO: Enable hardware RNG?  Read the esp32 docs :)
}
