#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <http_app.h>
#include <string.h>

#include "httpd.h"

static const char *TAG = "starman-httpd";
static void (*play_callback_func)(void) = NULL;
static TaskHandle_t http_task;

extern const char starman_html_start[] asm("_binary_starman_html_start");
extern const char starman_html_end[] asm("_binary_starman_html_end");
extern const char starman_js_start[] asm("_binary_starman_js_start");
extern const char starman_js_end[] asm("_binary_starman_js_end");
extern const char starman_css_start[] asm("_binary_starman_css_start");
extern const char starman_css_end[] asm("_binary_starman_css_end");


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
	if(strcmp(req->uri, "/") == 0) {
		ESP_LOGI(TAG, "Serving page /");

		const uint32_t starman_html_len = starman_html_end - starman_html_start;

		httpd_resp_set_status(req, "200 OK");
		httpd_resp_set_type(req, "text/html");
		httpd_resp_send(req, starman_html_start, starman_html_len);
	}
	if(strcmp(req->uri, "/starman.js") == 0) {
		ESP_LOGI(TAG, "Serving page /starman.js");

		const uint32_t starman_js_len = starman_js_end - starman_js_start;

		httpd_resp_set_status(req, "200 OK");
		httpd_resp_set_type(req, "text/javascript");
		httpd_resp_send(req, starman_js_start, starman_js_len);
	}
	if(strcmp(req->uri, "/starman.css") == 0) {
		ESP_LOGI(TAG, "Serving page /starman.css");

		const uint32_t starman_css_len = starman_css_end - starman_css_start;

		httpd_resp_set_status(req, "200 OK");
		httpd_resp_set_type(req, "text/css");
		httpd_resp_send(req, starman_css_start, starman_css_len);
	}
    else if (strcmp(req->uri, "/play") == 0) {
		xTaskCreate(play_task, "play", 8192, NULL, 0, &http_task);

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
