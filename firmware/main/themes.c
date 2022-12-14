#include <esp_log.h>
#include <string.h>

#include "themes.h"

static const char *TAG = "starman-themes";

const theme_t* themes_load_stage(enum THEME_STAGE stage, const char* title) {
    const theme_t* active_theme = NULL;
    uint8_t total_stages = 0;

    for (uint8_t i = 0; i < TOTAL_THEMES_AVAILABLE; i++) {
        if (strcmp(themes[i].title, title) == 0) {
            active_theme = themes[i].theme;
            total_stages = themes[i].total_stages;
        }
    }

    if (active_theme == NULL) {
        ESP_LOGE(TAG, "%s theme not found", title);
        return NULL;
    }

    ESP_LOGI(TAG, "Found %s theme at %p", title, active_theme);

    for (uint8_t i = 0; i < total_stages; i++) {
        if (active_theme[i].stage == stage) {
            ESP_LOGI(TAG, "Found %s stage %d at pos: %d", title, stage, i);
            return &active_theme[i];
        }
    }
    ESP_LOGW(TAG, "%s stage %d not found", title, stage);

    return NULL;
}