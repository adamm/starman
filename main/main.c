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

#include <esp_err.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_netif.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "adc.h"
#include "buttons.h"
#include "config.h"
#include "display.h"
#include "game.h"
#include "httpd.h"
#include "music.h"
#include "ota.h"
#include "patterns.h"
#include "patterns_gol.h"
#include "random.h"
#include "sparkle.h"
#include "storage.h"
#include "wifi_manager.h"
#include "util.h"

static const char *TAG = "starman";

/*
  HOW TO PLAY:

  - Press the button to start the game.
  - Start the game with three lives at level 1 (overworld).  If you lose all lives, it's game over.
  - There is a 50% chance you will complete each level.  If you do, you move onto next level.  If not, you die and need to retry.
    - Death can occur randomly within the level.
    - Press the button again to start the next level, or retry the same level.
  - There is a 40% chance you'll get a star.
    - Stars can occur randomly within the level.
    - Stars do not negate death (ie, falling into a pit).
  - There is a 25% chance you'll get a 1-up, resulting in an extra life.
  - If you make it to level 4 (castle) and complete it, there is a 25% chance the princess will be there and the game is over.

  Game logic implemented in game.c.
*/


void monitoring_task(void *pvParameter)
{
    for(;;){
        ESP_LOGI(TAG, "free heap: %d",esp_get_free_heap_size());
        vTaskDelay( pdMS_TO_TICKS(10000) );
    }
}


void wifi_connected(void *pvParameter) {
    ip_event_got_ip_t* param = (ip_event_got_ip_t*)pvParameter;

    // Transform IP to human readable string, preceeded by 4 spaces
    // This makes it easier to see as it scrolls by during the sparkle routine
    char ip_str[21] = {0};
    memset(ip_str, ' ', 4);
    esp_ip4addr_ntoa(&param->ip_info.ip, ip_str+4, IP4ADDR_STRLEN_MAX);

    ESP_LOGI(TAG, "Our IP address is %s!", ip_str);

    ota_init();
    ota_upgrade(); // Combines checking if an upgrade exists, downloading, and installing it.

    // If OTA update is successful, starman will reboot with the new firmware version.
    // Otherwise, it will scroll the current IP, and begin the normal game process.

    // Leaving a little space infront of the IP address makes it easier to read when scrolling.
    sparkle_scroll_string(ip_str);
}


void app_main(void) {
    ESP_LOGI(TAG, "Main startup");
    config_init();
    storage_init();
    adc_init();
    buttons_init();
    display_init();
    music_init();
    random_init();

    // Execute the play_game() function when the play button is pressed or /play is accessed
    buttons_play_callback(game_start);
    httpd_play_callback(game_start);

    // Loop music with lights via step_sequence();
    music_callback(game_step_sequence);

    // Sparkle runs continiously in another thread while we wait for the user
    // to press Play
    sparkle_start();

    ESP_LOGI(TAG, "ambient light: %d", adc_get_ambient_light_level());

    // Wifi is last as it can take a few moments -- this way the sparkle and
    // game can begin even without wifi being ready.
    wifi_manager_start();
    wifi_manager_set_callback(WM_EVENT_STA_GOT_IP, &wifi_connected);
    http_app_set_handler_hook(HTTP_GET, &httpd_handler);

    xTaskCreatePinnedToCore(&monitoring_task, "monitoring_task", 2048, NULL, 1, NULL, 1);
}
