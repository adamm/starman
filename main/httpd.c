#include <esp_log.h>
#include <http_app.h>
#include <string.h>

#include "httpd.h"

static const char *TAG = "starman-httpd";
static void (*play_callback_func)(void) = NULL;


void httpd_play_callback(void (*callback)(void)) {
    play_callback_func = callback;
}



esp_err_t httpd_handler(httpd_req_t *req) {
	/* our custom page sits at /helloworld in this example */
	if(strcmp(req->uri, "/") == 0) {
		ESP_LOGI(TAG, "Serving page /");

		const char* response = "<html><body><h1><a href='/play'>Play</a></h1></body></html>";

		httpd_resp_set_status(req, "200 OK");
		httpd_resp_set_type(req, "text/html");
		httpd_resp_send(req, response, strlen(response));
	}
    else if (strcmp(req->uri, "/play") == 0) {
        // TODO: Move this to another thread so it doens't block the HTTP Response
        if (play_callback_func != NULL)
            play_callback_func();

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
