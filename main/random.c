#include <esp_log.h>
#include <esp_random.h>

#include "random.h"

static const char *TAG = "starman-random";


bool random_bool_under_percent(uint8_t percent) {
    uint32_t rnd = esp_random();
    uint64_t max_pass = percent * (uint64_t)UINT32_MAX / 100;

    ESP_LOGI(TAG, "random_value_under_percent() percent: %d, rnd: %u, max_pass: %llu", percent, rnd, max_pass);

    return (rnd < max_pass);
}

uint32_t random_value_within_int(uint32_t max_value) {
    uint32_t rnd = esp_random();
    uint64_t value = (uint64_t)rnd * (uint64_t)max_value / (uint64_t)UINT32_MAX;
    ESP_LOGI(TAG, "random_value_within_int() max_value: %d, rnd: %u, value: %llu", max_value, rnd, value);

    return (uint32_t)value;
}


void random_init(void) {
    ESP_LOGI(TAG, "Init success");

    // TODO: Enable hardware RNG?  Read the esp32 docs :)
}