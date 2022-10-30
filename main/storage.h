#ifndef STORAGE_H
#define STORAGE_H

#include <esp_err.h>

#define STORAGE_NAMESPACE "starman"
#define STORAGE_XSECRET_KEY "x-secret"
#define STORAGE_IMAGE_URL_KEY "image-url"
#define STORAGE_FIRMWARE_URL_KEY "firmware-url"

void storage_init();
void storage_stop();
esp_err_t storage_get_str(const char* key, char* value, size_t* len);
esp_err_t storage_set_str(const char* key, const char* value);

// char* storage_get_xsecret();
// esp_err_t storage_set_xsecret(char* value);
// char* storage_get_image_url();
// esp_err_t storage_set_image_url(char* value);

#endif
