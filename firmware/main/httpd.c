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

#include "config.h"
#include "display.h"
#include "game.h"
#include "storage.h"
#include "sparkle.h"
#include "themes.h"
#include "util.h"

#include "httpd.h"

static const char *TAG = "starman-httpd";
static void (*play_callback_func)(void) = NULL;
static TaskHandle_t http_task;

extern const char about_html_start[] asm("_binary_about_html_start");
extern const char about_html_end[] asm("_binary_about_html_end");
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


static void reset_task() {
    // Delay the reset to allow the HTTP response to complete
    vTaskDelay(500 / portTICK_RATE_MS);
    esp_restart();
}


static esp_err_t post_handler(httpd_req_t *req, char* content, size_t len) {
    size_t recv_size = MIN(req->content_len, len);

    ESP_LOGI(TAG, "recv_size: %d", recv_size);

    int ret = httpd_req_recv(req, content, recv_size);
    if (ret <= 0) {  /* 0 return value indicates connection closed */
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "bytes read: %d", ret);
    return ESP_OK;
}


esp_err_t httpd_get_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "GET %s", req->uri);
    if(strcmp(req->uri, "/") == 0 ||
       strcmp(req->uri, "/index.html") == 0) {

        const uint32_t index_html_len = index_html_end - index_html_start;

        httpd_resp_set_status(req, "200 OK");
        httpd_resp_set_type(req, "text/html");
        httpd_resp_send(req, index_html_start, index_html_len);
    }
    else if(strcmp(req->uri, "/about.html") == 0) {
        const uint32_t about_html_len = about_html_end - about_html_start;

        httpd_resp_set_status(req, "200 OK");
        httpd_resp_set_type(req, "text/html");
        httpd_resp_send(req, about_html_start, about_html_len);
    }
    else if(strcmp(req->uri, "/starman.js") == 0) {
        const uint32_t starman_js_len = starman_js_end - starman_js_start;

        httpd_resp_set_status(req, "200 OK");
        httpd_resp_set_type(req, "text/javascript");
        httpd_resp_send(req, starman_js_start, starman_js_len);
    }
    else if(strcmp(req->uri, "/style.css") == 0) {
        const uint32_t style_css_len = style_css_end - style_css_start;

        httpd_resp_set_status(req, "200 OK");
        httpd_resp_set_type(req, "text/css");
        httpd_resp_send(req, style_css_start, style_css_len);
    }
    else if(strcmp(req->uri, "/status.json") == 0) {
        char* json = calloc(140, sizeof(char));

        const esp_partition_t *running = esp_ota_get_running_partition();
        esp_app_desc_t running_app_info;
        if (esp_ota_get_partition_description(running, &running_app_info) != ESP_OK)
            strcpy(running_app_info.version, "unknown");

        sprintf(json, "{\"level\":%d,\"lives\":%d,\"state\":\"%s\",\"brightness\":%d,\"fw_version\":\"%s\",\"fw_track\":\"%s\"}",
            game_get_level(), game_get_lives(), game_get_playing_state() ? "playing" : "idle",
            display_get_brightness(), running_app_info.version, config_firmware_track);

        httpd_resp_set_status(req, "200 OK");
        httpd_resp_set_type(req, "application/json");
        httpd_resp_send(req, json, strlen(json));

        free(json);
    }
    else if (strcmp(req->uri, "/brightness") == 0) {
        char* json = calloc(50, sizeof(char));

        sprintf(json, "{\"config\":%d,\"actual\":%d}",
            config_brightness, display_get_brightness());

        httpd_resp_set_status(req, "200 OK");
        httpd_resp_set_type(req, "application/json");
        httpd_resp_send(req, json, strlen(json));

        free(json);
    }
    else if (strcmp(req->uri, "/play") == 0) {
        xTaskCreate(play_task, "play", 8192, NULL, 5, &http_task);

        const char* response = "OK";

        httpd_resp_set_status(req, "200 OK");
        httpd_resp_set_type(req, "text/plain");
        httpd_resp_send(req, response, strlen(response));
    }
    else if (strcmp(req->uri, "/reset") == 0) {
        const char* response = "OK";

        httpd_resp_set_status(req, "200 OK");
        httpd_resp_set_type(req, "text/plain");
        httpd_resp_send(req, response, strlen(response));

        // Delay the reset to allow the HTTP response to complete
        xTaskCreate(reset_task, "reset", 2048, NULL, 5, &http_task);
    }
    else if (strcmp(req->uri, "/theme") == 0) {
        char* json = calloc(140, sizeof(char));
        char* themes_str = calloc(20, sizeof(char));

        for (uint8_t i = 0; i < TOTAL_THEMES_AVAILABLE; i++) {
            strcat(themes_str, "\"");
            strcat(themes_str, themes[i].title);
            strcat(themes_str, "\",");
        }
        // Chop off the trailing comma
        themes_str[strlen(themes_str)-1] = 0;

        sprintf(json, "{\"current\":%d,\"available\":[%s]}",
            config_theme, themes_str);

        httpd_resp_set_status(req, "200 OK");
        httpd_resp_set_type(req, "application/json");
        httpd_resp_send(req, json, strlen(json));

        free(json);
        free(themes_str);
    }
    else if (strcmp(req->uri, "/track") == 0) {
        const char* response = config_firmware_track;

        httpd_resp_set_status(req, "200 OK");
        httpd_resp_set_type(req, "text/plain");
        httpd_resp_send(req, response, strlen(response));
    }
    else {
        httpd_resp_send_404(req);
    }

    return ESP_OK;
}


esp_err_t httpd_post_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "POST %s", req->uri);
    if (strcmp(req->uri, "/brightness") == 0) {
        uint16_t const value_maxlen = 10;
        char* value_str = calloc(value_maxlen, sizeof(char));
        esp_err_t err;

        err = post_handler(req, value_str, value_maxlen);
        ESP_LOGI(TAG, "Got value for /brightness: %s", value_str);
        if (err == ESP_OK) {
            int base = 10;
            intmax_t value = 0;

            value = strtoimax(value_str, NULL, base);

            if (value == -1) {
                config_set_brightness((int8_t)value);
                // Begin automatic brightness setting procedure
            }
            else if (0 <= value && value <= DISPLAY_LIGHTS_MAX_GAIN) {
                config_set_brightness((int8_t)value);
                display_set_brightness((int8_t)value);
            }
            else {
                // TODO: Return an HTTP error response that brightness must be between -1 and configured max gain
                ESP_LOGE(TAG, "Brightness must be between -1 and %d", DISPLAY_LIGHTS_MAX_GAIN);
            }
            httpd_resp_set_status(req, "200 OK");
            httpd_resp_send(req, NULL, 0);
        }
        else {
            httpd_resp_set_status(req, "400 Bad Request");
            httpd_resp_send(req, NULL, 0);
        }
        free(value_str);
    }
    else if (strcmp(req->uri, "/genie") == 0) {
        uint16_t const value_maxlen = 20;
        char* value_str = calloc(value_maxlen, sizeof(char));
        esp_err_t err;

        err = post_handler(req, value_str, value_maxlen);
        ESP_LOGI(TAG, "Got value for /genie: %s", value_str);
        if (err == ESP_OK) {
            if (strcmp(value_str, util_get_mac_addr()) == 0)
                player_invincible = true;
            else
                player_invincible = false;

            httpd_resp_set_status(req, "200 OK");
            httpd_resp_send(req, NULL, 0);
        }
        free(value_str);
    }
    else if (strcmp(req->uri, "/text") == 0) {
        uint16_t const text_maxlen = 255;
        char* text_str = calloc(text_maxlen, sizeof(char));
        esp_err_t err;

        // XXX: Only allow pushed text to display during the sparkle routine.

        err = post_handler(req, text_str, text_maxlen);
        ESP_LOGI(TAG, "Got value for /text: %s", text_str);
        if (err == ESP_OK) {
            sparkle_scroll_string(text_str);
            httpd_resp_set_status(req, "200 OK");
            httpd_resp_send(req, NULL, 0);
        }
        else {
            httpd_resp_set_status(req, "400 Bad Request");
            httpd_resp_send(req, NULL, 0);
        }
        free(text_str);
    }
    else if (strcmp(req->uri, "/theme") == 0) {
        uint16_t const value_maxlen = 10;
        char* value_str = calloc(value_maxlen, sizeof(char));
        esp_err_t err;

        err = post_handler(req, value_str, value_maxlen);
        ESP_LOGI(TAG, "Got value for /theme: %s", value_str);
        if (err == ESP_OK) {
            int base = 10;
            intmax_t value = 0;

            value = strtoimax(value_str, NULL, base);

            if (value < TOTAL_THEMES_AVAILABLE) {
                config_set_theme((uint8_t)value);
            }
            else {
                // TODO: Return an HTTP error response that theme must be between 0 and TOTAL_THEMES_AVAILABLE-1
                ESP_LOGE(TAG, "Theme must be between 0 and %d", TOTAL_THEMES_AVAILABLE-1);
            }
            httpd_resp_set_status(req, "200 OK");
            httpd_resp_send(req, NULL, 0);
        }
        else {
            httpd_resp_set_status(req, "400 Bad Request");
            httpd_resp_send(req, NULL, 0);
        }
        free(value_str);
    }
    else if (strcmp(req->uri, "/track") == 0) {
        uint16_t const text_maxlen = CONFIG_FIRMWARE_TRACK_MAXLEN;
        char* text_str = calloc(text_maxlen, sizeof(char));
        esp_err_t err;

        err = post_handler(req, text_str, text_maxlen);
        ESP_LOGI(TAG, "Got value for /track: %s", text_str);
        if (err == ESP_OK &&
           (strcmp(text_str, CONFIG_FIRMWARE_TRACK_STABLE) == 0 ||
            strcmp(text_str, CONFIG_FIRMWARE_TRACK_TESTING) == 0 ||
            strcmp(text_str, CONFIG_FIRMWARE_TRACK_UNSTABLE) == 0)) {
            config_set_firmware_track(text_str);
            httpd_resp_set_status(req, "200 OK");
            httpd_resp_send(req, NULL, 0);
        }
        else {
            httpd_resp_set_status(req, "400 Bad Request");
            httpd_resp_send(req, NULL, 0);
        }
        free(text_str);
    }
    else {
        ESP_LOGW(TAG, "Cannot find %s (%d)", req->uri, req->method);
        httpd_resp_send_404(req);
    }
    return ESP_OK;
}
