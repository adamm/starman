#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_log.h>
#include <stdlib.h>
#include <string.h>

#include "display.h"
#include "music.h"
#include "stage.h"
#include "themes.h"

#include "test.h"

static const char *TAG = "starman-test";

static TaskHandle_t test_task;
// static display_t display;
struct {
    const char* theme;
    enum STAGE stage;
} typedef test_param_t;
    

static void start_test_stage(void* arg) {
    test_param_t* test_param = (test_param_t*)arg;
    const theme_t* stage = themes_load_stage(test_param->theme, test_param->stage);
    
    if (stage != NULL) {
        ESP_LOGI(TAG, "Starting test stage: %s : %d", test_param->theme, test_param->stage);

        music_playscore(stage->score);
    }
    else {
        ESP_LOGE(TAG, "Couldn't test stage: %s : %d", test_param->theme, test_param->stage);
    }

    // The task never exits until a new test is run or device is reset
    for(;;) {
    }
}


void test_play_stage(const char* theme, enum STAGE stage) {
    test_param_t* test_param = calloc(1, sizeof(test_param_t));

    test_param->theme = theme;
    test_param->stage = stage;

    music_callback(NULL);

    if (test_task != NULL)
        test_stop_stage();

    xTaskCreate(start_test_stage, "test_stage", 8192, (void*)test_param, 0, &test_task);
}


void test_stop_stage(void) {
    if (test_task != NULL)
        vTaskDelete(test_task);
}

