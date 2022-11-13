#pragma once

#include <esp_system.h>
#include <http_app.h>


void httpd_play_callback(void (*callback)(void));
esp_err_t httpd_handler(httpd_req_t *req);