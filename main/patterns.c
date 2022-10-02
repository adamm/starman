#include <esp_log.h>

#include "patterns.h"

static const char *TAG = "starman-patterns";


void patterns_step_sequence() {
    // Function is called each time the music note changes. 
    // Use this to imcrement the pattern sequence and update the lights.
}


void patterns_diamonds() {
    ESP_LOGI(TAG, "Begin DIAMONDS pattern");
}


void patterns_fireworks() {
    ESP_LOGI(TAG, "Begin FIREWORKS pattern");
}


void patterns_flash() {
    ESP_LOGI(TAG, "Begin FLASH pattern");
}


void patterns_lines() {
    ESP_LOGI(TAG, "Begin LINES pattern");
}


void patterns_sparkle() {
    ESP_LOGI(TAG, "Begin SPARKLE pattern");
}


void patterns_sprinkle(){
    ESP_LOGI(TAG, "Begin SPRINKLE pattern");
}


void patterns_sweep(){
    ESP_LOGI(TAG, "Begin SWEEP pattern");
}


void patterns_swoosh() {
    ESP_LOGI(TAG, "Begin SWOOP pattern");
}


void patterns_swoop() {
    ESP_LOGI(TAG, "Begin SWOOP pattern");
}


void patterns_waves() {
    ESP_LOGI(TAG, "Begin WAVES pattern");
}
