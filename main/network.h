#ifndef NETWORK_H
#define NETWORK_H

#ifndef CONFIG_ESP_TLS_USING_MBEDTLS
#define CONFIG_ESP_TLS_USING_MBEDTLS 1
#endif

#define MAX_MD5_LEN 16

char* network_get_https_uri(const char* url, size_t* bufLen, esp_err_t* err);
char* network_post_https_uri(const char* url, char* postData, size_t postLen, size_t* bufLen, esp_err_t* err);
void network_download_finished(void);
esp_err_t network_stream_https_uri(const char*, void (*callback)(char*, size_t), unsigned char*);


#endif
