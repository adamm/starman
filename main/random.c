#include <esp_log.h>
#include <esp_random.h>

#include "random.h"

static const char *TAG = "starman-random";


bool random_percent(uint8_t percent) {
    uint64_t max_pass = percent * UINT32_MAX / 100;
    uint32_t rnd = esp_random();

    return (rnd < max_pass);
}


void random_init(void) {
    ESP_LOGI(TAG, "Init success");

    // TODO: Enable hardware RNG?  Read the esp32 docs :)
}