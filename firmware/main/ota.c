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

#include <esp_crt_bundle.h>
#include <esp_http_client.h>
#include <esp_https_ota.h>
#include <esp_log.h>
#include <esp_ota_ops.h>
#include <esp_system.h>
#include <esp_flash_partitions.h>
#include <esp_partition.h>
#include <esp_wifi.h>
#include <math.h>
#include <string.h>

#include "config.h"
#include "display.h"
#include "sparkle.h"
#include "storage.h"
#include "text.h"
#include "util.h"

#include "ota.h"

#define BUFFSIZE 1024
#define HASH_LEN 32 /* SHA-256 digest length */

static const char *TAG = "starman-ota";


static esp_err_t validate_image_header(esp_app_desc_t *new_app_info, char* server_track, char* device_track)
{
    int compVersion = 0;

    if (new_app_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_app_desc_t running_app_info;
    if (esp_ota_get_partition_description(running, &running_app_info) == ESP_OK)
        ESP_LOGI(TAG, "Device firmware version: %s (%s)", running_app_info.version, device_track);
    else
        return ESP_ERR_INVALID_ARG;

    ESP_LOGI(TAG, "Server firmware version: %s (%s)", new_app_info->version, server_track);

    compVersion = memcmp(new_app_info->version, running_app_info.version, sizeof(new_app_info->version));

    if (compVersion == 0) {
        ESP_LOGW(TAG, "Current device version is the same as the firmware server. Nothing to update.");
        return ESP_FAIL;
    }
    else if (compVersion < 0) {
        // Technically if the user is transitioning the device from the
        // unstable track to testing or stable, the firmware on the server
        // will be older.  We should still allow the OTA download and "upgrade"
        // to the new track's build version.
        if (strcmp(device_track, server_track) == 0) {
            ESP_LOGW(TAG, "Current device version is newer than the firmware server. Nothing to update.");
            return ESP_FAIL;
        }
        else {
            ESP_LOGW(TAG, "Current device version is newer than the firmware server, but the track changed. Proceed with the update!");
            return ESP_OK;
        }
    }
    else if (compVersion > 0) {
        ESP_LOGW(TAG, "Current device version is the older than the firmware server. Proceed with the update!");
        return ESP_OK;
    }

    return ESP_OK;
}


static esp_err_t _http_client_init_cb(esp_http_client_handle_t http_client)
{
    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_app_desc_t running_app_info;
    esp_err_t err = ESP_OK;

    if (esp_ota_get_partition_description(running, &running_app_info) == ESP_OK)
        err = esp_http_client_set_header(http_client, "x-firmware", running_app_info.version);

    // Send the MAC address and current firmware version as HTTP headers
    err = esp_http_client_set_header(http_client, "x-mac-address", util_get_mac_addr());
    return err;
}


static void print_sha256 (const uint8_t *image_hash, const char *label)
{
    char hash_print[HASH_LEN * 2 + 1];
    hash_print[HASH_LEN * 2] = 0;
    for (int i = 0; i < HASH_LEN; ++i) {
        sprintf(&hash_print[i * 2], "%02x", image_hash[i]);
    }
    ESP_LOGI(TAG, "%s: %s", label, hash_print);
}


uint8_t ota_init(void) {
    uint8_t sha_256[HASH_LEN] = { 0 };
    esp_partition_t partition;

    // get sha256 digest for the partition table
    partition.address   = ESP_PARTITION_TABLE_OFFSET;
    partition.size      = ESP_PARTITION_TABLE_MAX_LEN;
    partition.type      = ESP_PARTITION_TYPE_DATA;
    esp_partition_get_sha256(&partition, sha_256);
    print_sha256(sha_256, "SHA-256 for the partition table: ");

    // get sha256 digest for bootloader
    partition.address   = ESP_BOOTLOADER_OFFSET;
    partition.size      = ESP_PARTITION_TABLE_OFFSET;
    partition.type      = ESP_PARTITION_TYPE_APP;
    esp_partition_get_sha256(&partition, sha_256);
    print_sha256(sha_256, "SHA-256 for bootloader: ");

    // get sha256 digest for running partition
    esp_partition_get_sha256(esp_ota_get_running_partition(), sha_256);
    print_sha256(sha_256, "SHA-256 for current firmware: ");

    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_ota_img_states_t ota_state;
    esp_app_desc_t running_app_info;
    if (esp_ota_get_partition_description(running, &running_app_info) == ESP_OK)
        ESP_LOGI(TAG, "Device firmware version: %s", running_app_info.version);

    if (esp_ota_get_state_partition(running, &ota_state) == ESP_OK) {
        if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
            if (esp_ota_mark_app_valid_cancel_rollback() == ESP_OK) {
                ESP_LOGI(TAG, "App is valid, rollback cancelled successfully");
            } else {
                ESP_LOGE(TAG, "Failed to cancel rollback");
            }
        }
    }
    return 1;
}


uint8_t ota_upgrade(void) {
    char firmware_url[50] = {0};
    char* firmware_track = config_firmware_track;
    char* device_track = calloc(10, sizeof(char));
    size_t device_track_len;
    esp_err_t err;

    err = storage_get_str(STORAGE_DEVICE_TRACK_KEY, device_track, &device_track_len);
    if (err != 0 || device_track_len == 0 ||
        !(strcmp(device_track, CONFIG_FIRMWARE_TRACK_STABLE) == 0 ||
          strcmp(device_track, CONFIG_FIRMWARE_TRACK_TESTING) == 0 ||
          strcmp(device_track, CONFIG_FIRMWARE_TRACK_UNSTABLE) == 0)
       ) {
        strcpy(device_track, config_firmware_track);
        storage_set_str(STORAGE_DEVICE_TRACK_KEY, config_firmware_track);
    }
    ESP_LOGI(TAG, "Device current firmware track is %s", device_track);
    ESP_LOGI(TAG, "User Configured firmware track is %s", firmware_track);

    bzero(firmware_url, sizeof(firmware_url));
    strlcpy(firmware_url, config_firmware_service_url, strlen(config_firmware_service_url) + 1);
    strlcat(firmware_url, "/firmware/", strlen(firmware_url) + 11);
    strlcat(firmware_url, firmware_track, strlen(firmware_url) + strlen(firmware_track) + 1);
    strlcat(firmware_url, ".bin", strlen(firmware_url) + 5);
    ESP_LOGI(TAG, "Starting OTA: %s", firmware_url);

    esp_err_t ota_finish_err = ESP_OK;
    esp_http_client_config_t config = {
        .url = firmware_url,
        .timeout_ms = 3000,
        .keep_alive_enable = true,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    esp_https_ota_config_t ota_config = {
        .http_config = &config,
        .http_client_init_cb = _http_client_init_cb, // Register a callback to be invoked after esp_http_client is initialized
    };

    esp_https_ota_handle_t https_ota_handle = NULL;
    err = esp_https_ota_begin(&ota_config, &https_ota_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ESP_HTTPS_OTA failed");
        return 0;
    }

    esp_app_desc_t app_desc;
    err = esp_https_ota_get_img_desc(https_ota_handle, &app_desc);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_https_ota_read_img_desc failed");
        goto ota_end;
    }
    err = validate_image_header(&app_desc, firmware_track, device_track);
    if (err != ESP_OK) {
        // ESP_LOGE(TAG, "image header verification failed");
        goto ota_end;
    }

    uint32_t image_total_size = esp_https_ota_get_image_size(https_ota_handle);
    uint32_t image_download_size = 0;
    float image_download_percent = 0;
    uint8_t image_download_percent_rounded = 0;
    char progress_text[10] = {0};
    display_t* display = NULL;

    sparkle_stop();
    display = malloc(sizeof(display_t));
    memset(display, 0, sizeof(display_t));

    while (1) {
        err = esp_https_ota_perform(https_ota_handle);
        if (err != ESP_ERR_HTTPS_OTA_IN_PROGRESS) {
            break;
        }
        image_download_size = esp_https_ota_get_image_len_read(https_ota_handle);
        image_download_percent = (float)image_download_size / (float)image_total_size * 100.0;
        image_download_percent_rounded = floor(image_download_percent);

        // ESP_LOGI(TAG, "OTA download: %d of %d bytes which is %f%% or %d%%", image_download_size, image_total_size, image_download_percent, image_download_percent_rounded);

        sprintf(progress_text, "%d%%", image_download_percent_rounded);
        text_write_string(display, progress_text);
        display_update_leds(display);
    }
    free(display);

    if (esp_https_ota_is_complete_data_received(https_ota_handle) != true) {
        // the OTA image was not completely received and user can customise the response to this situation.
        ESP_LOGE(TAG, "Complete data was not received.");
        sparkle_start();
    } else {
        ota_finish_err = esp_https_ota_finish(https_ota_handle);
        if ((err == ESP_OK) && (ota_finish_err == ESP_OK)) {
            storage_set_str(STORAGE_DEVICE_TRACK_KEY, firmware_track);
            ESP_LOGI(TAG, "ESP_HTTPS_OTA upgrade successful. Rebooting ...");
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            esp_restart();
            return 1;
        } else {
            if (ota_finish_err == ESP_ERR_OTA_VALIDATE_FAILED) {
                ESP_LOGE(TAG, "Image validation failed, image is corrupted");
            }
            ESP_LOGE(TAG, "ESP_HTTPS_OTA upgrade failed 0x%x", ota_finish_err);
            sparkle_start();
            return 0;
        }
    }
    return 1;

ota_end:
    esp_https_ota_abort(https_ota_handle);
    ESP_LOGI(TAG, "ESP_HTTPS_OTA stopped");
    return 0;
}
