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
