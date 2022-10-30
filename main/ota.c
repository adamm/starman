#include <esp_crt_bundle.h>
#include <esp_http_client.h>
#include <esp_https_ota.h>
#include <esp_log.h>
#include <esp_ota_ops.h>
#include <esp_system.h>
#include <esp_flash_partitions.h>
#include <esp_partition.h>
#include <string.h>

#include "config.h"
#include "status.h"
#include "ota.h"

#define BUFFSIZE 1024
#define HASH_LEN 32 /* SHA-256 digest length */

static const char *TAG = "starman-ota";


static esp_err_t validate_image_header(esp_app_desc_t *new_app_info, char* server_channel)
{
    int compVersion = 0;
    char* device_channel = server_channel;
    // FIXME: Retrieve the device OTA channel from NVS, if it's there.
    // FIXME: Remember to set it after a sucessful OTA upgrade!

    if (new_app_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_app_desc_t running_app_info;
    if (esp_ota_get_partition_description(running, &running_app_info) == ESP_OK)
        ESP_LOGI(TAG, "Device firmware version: %s (%s)", running_app_info.version, device_channel);
    else
        return ESP_ERR_INVALID_ARG;

    ESP_LOGI(TAG, "Server firmware version: %s (%s)", new_app_info->version, server_channel);

    compVersion = memcmp(new_app_info->version, running_app_info.version, sizeof(new_app_info->version));

    if (compVersion == 0) {
        ESP_LOGW(TAG, "Current device version is the same as the firmware server. Nothing to update.");
        return ESP_FAIL;
    }
    else if (compVersion < 0) {
        // Technically if the user is transitioning the device from the
        // unstable channel to testing or stable, the firmware on the server
        // will be older.  We should still allow the OTA download and "upgrade"
        // to the new channel's build version.
        if (strcmp(device_channel, server_channel) == 0) {
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
    esp_err_t err = ESP_OK;
    /* Uncomment to add custom headers to HTTP request */
    // err = esp_http_client_set_header(http_client, "Custom-Header", "Value");
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


uint8_t ota_init() {
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


uint8_t ota_upgrade(char* channel) {
    char firmware_url[50] = {0};

    bzero(firmware_url, sizeof(firmware_url));
    strlcpy(firmware_url, config_firmware_service_url, strlen(config_firmware_service_url) + 1);
    strlcat(firmware_url, "/", strlen(firmware_url) + 2);
    strlcat(firmware_url, channel, strlen(firmware_url) + strlen(channel) + 1);
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
    esp_err_t err = esp_https_ota_begin(&ota_config, &https_ota_handle);
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
    err = validate_image_header(&app_desc, channel);
    if (err != ESP_OK) {
        // ESP_LOGE(TAG, "image header verification failed");
        goto ota_end;
    }

    while (1) {
        status_downloading();
        err = esp_https_ota_perform(https_ota_handle);
        if (err != ESP_ERR_HTTPS_OTA_IN_PROGRESS) {
            break;
        }
        // esp_https_ota_perform returns after every read operation which gives user the ability to
        // monitor the status of OTA upgrade by calling esp_https_ota_get_image_len_read, which gives length of image
        // data read so far.
        ESP_LOGI(TAG, "Image bytes read: %d", esp_https_ota_get_image_len_read(https_ota_handle));
        status_waiting();
    }
    status_resetting();

    if (esp_https_ota_is_complete_data_received(https_ota_handle) != true) {
        // the OTA image was not completely received and user can customise the response to this situation.
        ESP_LOGE(TAG, "Complete data was not received.");
    } else {
        ota_finish_err = esp_https_ota_finish(https_ota_handle);
        if ((err == ESP_OK) && (ota_finish_err == ESP_OK)) {
            ESP_LOGI(TAG, "ESP_HTTPS_OTA upgrade successful. Rebooting ...");
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            return 1;
        } else {
            if (ota_finish_err == ESP_ERR_OTA_VALIDATE_FAILED) {
                ESP_LOGE(TAG, "Image validation failed, image is corrupted");
            }
            ESP_LOGE(TAG, "ESP_HTTPS_OTA upgrade failed 0x%x", ota_finish_err);
            return 0;
        }
    }
    return 1;

ota_end:
    esp_https_ota_abort(https_ota_handle);
    ESP_LOGI(TAG, "ESP_HTTPS_OTA stopped");
    return 0;
}
