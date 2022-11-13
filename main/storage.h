#ifndef STORAGE_H
#define STORAGE_H

#include <esp_err.h>

#define STORAGE_NAMESPACE "starman"
#define STORAGE_FIRMWARE_URL_KEY "firmware-url"
#define STORAGE_FIRMWARE_TRACK_KEY "firmware-track"

void storage_init();
void storage_stop();
esp_err_t storage_get_str(const char* key, char* value, size_t* len);
esp_err_t storage_set_str(const char* key, const char* value);

// char* storage_get_firmware_url();
// esp_err_t storage_set_firmware_url(char* value);
// char* storage_get_firmware_track();
// esp_err_t storage_set_firmware_track(char* value);

#endif
