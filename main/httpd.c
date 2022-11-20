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
#include <esp_ota_ops.h>
#include <esp_partition.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <http_app.h>
#include <string.h>

#include "display.h"
#include "game.h"

#include "httpd.h"

static const char *TAG = "starman-httpd";
static void (*play_callback_func)(void) = NULL;
static TaskHandle_t http_task;

extern const char index_html_start[] asm("_binary_index_html_start");
extern const char index_html_end[] asm("_binary_index_html_end");
extern const char starman_js_start[] asm("_binary_starman_js_start");
extern const char starman_js_end[] asm("_binary_starman_js_end");
extern const char style_css_start[] asm("_binary_style_css_start");
extern const char style_css_end[] asm("_binary_style_css_end");


void httpd_play_callback(void (*callback)(void)) {
    play_callback_func = callback;
}


static void play_task() {
    if (play_callback_func != NULL)
        play_callback_func();
    http_task = NULL;
    vTaskDelete(NULL);
}


esp_err_t httpd_handler(httpd_req_t *req) {
    /* our custom page sits at /helloworld in this example */
    if(strcmp(req->uri, "/") == 0 ||
       strcmp(req->uri, "/index.html") == 0) {
        ESP_LOGI(TAG, "Serving page /");

        const uint32_t index_html_len = index_html_end - index_html_start;

        httpd_resp_set_status(req, "200 OK");
        httpd_resp_set_type(req, "text/html");
        httpd_resp_send(req, index_html_start, index_html_len);
    }
    if(strcmp(req->uri, "/starman.js") == 0) {
        ESP_LOGI(TAG, "Serving page /starman.js");

        const uint32_t starman_js_len = starman_js_end - starman_js_start;

        httpd_resp_set_status(req, "200 OK");
        httpd_resp_set_type(req, "text/javascript");
        httpd_resp_send(req, starman_js_start, starman_js_len);
    }
    if(strcmp(req->uri, "/style.css") == 0) {
        ESP_LOGI(TAG, "Serving page /style.css");

        const uint32_t style_css_len = style_css_end - style_css_start;

        httpd_resp_set_status(req, "200 OK");
        httpd_resp_set_type(req, "text/css");
        httpd_resp_send(req, style_css_start, style_css_len);
    }
    if(strcmp(req->uri, "/status.json") == 0) {
        ESP_LOGI(TAG, "Serving page /status.json");

        char* json = malloc(130);
        memset(json, 0, 130);

        const esp_partition_t *running = esp_ota_get_running_partition();
        esp_app_desc_t running_app_info;
        if (esp_ota_get_partition_description(running, &running_app_info) != ESP_OK)
            strcpy(running_app_info.version, "unknown");

        sprintf(json, "{\"level\":%d,\"lives\":%d,\"state\":\"%s\",\"brightness\":%d,\"firmware\":\"%s\"}",
            game_get_level(), game_get_lives(), game_get_playing_state() ? "playing" : "idle",
            display_get_brightness(), running_app_info.version);

        httpd_resp_set_status(req, "200 OK");
        httpd_resp_set_type(req, "application/json");
        httpd_resp_send(req, json, strlen(json));

        free(json);
    }
    else if (strcmp(req->uri, "/brightness") == 0) {
        uint8_t gain;

        // TODO: Get the value from the HTTP POST
        gain = 25;

        display_set_brightness(gain);
    }
    else if (strcmp(req->uri, "/play") == 0) {
        xTaskCreate(play_task, "play", 8192, NULL, 5, &http_task);

        const char* response = "OK";

        httpd_resp_set_status(req, "200 OK");
        httpd_resp_set_type(req, "text/plain");
        httpd_resp_send(req, response, strlen(response));
    }
    else {
        /* send a 404 otherwise */
        httpd_resp_send_404(req);
    }

    return ESP_OK;
}
