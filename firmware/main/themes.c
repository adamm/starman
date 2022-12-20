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

#include "stage.h"

#include "themes.h"

static const char *TAG = "starman-themes";

const theme_t* themes_load_stage(const char* title, enum STAGE stage) {
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