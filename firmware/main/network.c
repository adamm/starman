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

#include <string.h>
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_event.h>
#include <esp_netif.h>
#include <lwip/err.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/netdb.h>
#include <lwip/dns.h>
#include <mbedtls/md5.h>

#include <esp_tls.h>
#include <esp_crt_bundle.h>
#include <esp_http_client.h>

#include "config.h"
#include "storage.h"
#include "network.h"

/* Constants that aren't configurable in menuconfig */
#define MAX_HTTP_RECV_BUFFER 4090
#define MAX_HTTP_OUTPUT_BUFFER 2048


static const char *TAG = "starman-network";
static char* output_buffer = NULL;
static size_t output_len = 0;

esp_err_t _http_event_handler(esp_http_client_event_t *evt) {

    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                if (output_buffer == NULL) {
                    output_buffer = (char*) malloc(esp_http_client_get_content_length(evt->client));
                    output_len = 0;
                    if (output_buffer == NULL) {
                        ESP_LOGE(TAG, "Failed to allocate memory for the output buffer");
                        return ESP_FAIL;
                    }
                }
                memcpy(output_buffer + output_len, evt->data, evt->data_len);
            }
            output_len += evt->data_len;
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
            int mbedtls_err = 0;
            esp_err_t err = esp_tls_get_and_clear_last_error(evt->data, &mbedtls_err, NULL);
            if (err != ESP_OK) {
                network_download_finished();
                ESP_LOGI(TAG, "Last ESP error code: 0x%x", err);
                ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            }
            break;
    }

    return ESP_OK;
}


char* network_get_https_uri(const char* url, size_t* bufLen, esp_err_t* err) {
    esp_http_client_config_t config = {
        .url = url,
        .event_handler = _http_event_handler,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_method(client, HTTP_METHOD_GET);

    // Clear the buffer from the previous download, if any
    network_download_finished();

    ESP_LOGI(TAG, "GET %s", url);
    *err = esp_http_client_perform(client);
    if (*err == ESP_OK) {
        ESP_LOGI(TAG, "HTTPS Status = %d, content_length = %d",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
        *bufLen = output_len;
    } else {
        ESP_LOGE(TAG, "Error perform http request %s", esp_err_to_name(*err));
        network_download_finished();
    }
    esp_http_client_cleanup(client);

    return output_buffer;
}


char* network_post_https_uri(const char* url, char* postData, size_t postLen, size_t* bufLen, esp_err_t* err) {
    esp_http_client_config_t config = {
        .url = url,
        .event_handler = _http_event_handler,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, postData, postLen);


    // Clear the buffer from the previous download, if any
    network_download_finished();

    ESP_LOGI(TAG, "POST %s", url);
    ESP_LOGI(TAG, "%s", postData);
    *err = esp_http_client_perform(client);
    if (*err == ESP_OK) {
        ESP_LOGI(TAG, "HTTPS Status = %d, content_length = %d",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
        *bufLen = output_len;
    } else {
        ESP_LOGE(TAG, "Error perform http request %s", esp_err_to_name(*err));
        network_download_finished();
    }
    esp_http_client_cleanup(client);

    return output_buffer;
}



void network_download_finished(void) {
    if (output_buffer != NULL) {
        free(output_buffer);
        output_buffer = NULL;
    }
    output_len = 0;
}


esp_err_t network_stream_https_uri(const char* url, void (*callback)(char*, size_t), unsigned char* digest) {
    char *buffer = malloc(MAX_HTTP_RECV_BUFFER + 1);
    if (buffer == NULL) {
        ESP_LOGE(TAG, "Cannot malloc http receive buffer");
        return ESP_ERR_NO_MEM;
    }
    esp_http_client_config_t config = {
        .url = url,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err;

    ESP_LOGI(TAG, "GET %s", url);
    if ((err = esp_http_client_open(client, 0)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        free(buffer);
        return err;
    }

    int content_length = esp_http_client_fetch_headers(client);
    int total_read_len = 0, read_len = 0, max_read_len = 0;
    uint64_t start = esp_timer_get_time(), now = start, then = now;
    uint16_t speed;
    mbedtls_md5_context md5;

    mbedtls_md5_init(&md5);
    mbedtls_md5_starts_ret(&md5);

    while (total_read_len < content_length) {
        max_read_len = (content_length - total_read_len >= MAX_HTTP_RECV_BUFFER) ? MAX_HTTP_RECV_BUFFER : content_length - total_read_len;
        read_len = esp_http_client_read(client, buffer, max_read_len);
        total_read_len += read_len;
        if (read_len <= 0) {
            ESP_LOGE(TAG, "Error read data");
        }
        buffer[read_len] = 0;
        if (LOG_LOCAL_LEVEL == 0 || 1) {
            then = now;
            now = esp_timer_get_time();
            speed = read_len * 1000 / (now - then);
            ESP_LOGD(TAG, "read_len = %d, total_read_len = %d, content_length = %d, diff = %lld ms", read_len, total_read_len, content_length, (now - then));
            ESP_LOGI(TAG, "%d/%d @ %d kbps", total_read_len, content_length, speed);
        }
        else {
            putchar('.');
            fflush(stdout);
        }
        mbedtls_md5_update_ret(&md5, (const unsigned char*)buffer, read_len);
        callback(buffer, read_len);
    }
    putchar('\n');
    now = esp_timer_get_time();
    speed = total_read_len * 1000 / (now - start);
    ESP_LOGI(TAG, "%d bytes downloaded in %f seconds at %d kbps", total_read_len, (float)(now - start)/1000000L, speed);
    ESP_LOGI(TAG, "HTTP Stream reader Status = %d, content_length = %d",
                    esp_http_client_get_status_code(client),
                    esp_http_client_get_content_length(client));
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
    free(buffer);

    mbedtls_md5_finish_ret(&md5, digest);
    char digest_str[MAX_MD5_LEN*2] = {0};

    for (int i = 0; i < MAX_MD5_LEN; i++) {
        sprintf(&digest_str[i*2], "%02x", digest[i]);
    }
    ESP_LOGI(TAG, "MD5 Hash of download is: %s", digest_str);

    return ESP_OK;
}
