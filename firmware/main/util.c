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
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "util.h"

static const char *TAG = "starman-util";
static char mac_str[20] = {0};


void util_delay(uint32_t ms) {
    vTaskDelay(ms / portTICK_RATE_MS);
}


char* util_get_mac_addr(void) {
    if (mac_str != NULL)
        return mac_str;

    uint8_t mac[6];

    esp_wifi_get_mac(WIFI_IF_STA, mac);
    sprintf(mac_str, "%02X%02X%02X%02X%02X%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    ESP_LOGI(TAG, "Device MAC address: %s", mac_str);

    return mac_str;
}
