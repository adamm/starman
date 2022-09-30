#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/ledc.h>
#include <esp_log.h>

#include "music.h"

static const char *TAG = "starman-music";

static const char notes[][4] = {
   "C1", "C#1", "D1", "D#1", "E1", "F1", "F#1", "G1", "G#1", "A1", "A#1", "B1",
   "C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2", "A2", "A#2", "B2",
   "C3", "C#3", "D3", "D#3", "E3", "F3", "F#3", "G3", "G#3", "A3", "A#3", "B3",
   "C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4", "A4", "A#4", "B4",
   "C5", "C#5", "D5", "D#5", "E5", "F5", "F#5", "G5", "G#5", "A5", "A#5", "B5",
   "C6", "C#6", "D6", "D#6", "E6", "F6", "F#6", "G6", "G#6", "A6", "A#6", "B6",
   "C7", "C#7", "D7", "D#7", "E7", "F7", "F#7", "G7", "G#7", "A7", "A#7", "B7",
   "C8", "C#8", "D8", "D#8", "E8", "F8", "F#8", "G8", "G#8", "A8", "A#8", "B8",
};

// Table of midi note frequencies times 8
//   They are times 8 for greater accuracy.
//   Generated from Excel by =ROUND(8*440/32*(2^((x-9)/12)),0) for 0<x<128
static const uint32_t freqs[] = {
   // C1, C#1,  D#, D#1,  E1,  F1, F#1,  G1, G#1, A1,  A#1,  B1
      33,  35,  37,  39,  41,  44,  46,  49, 52,  55,  58,   62,
   // C2, C#2,  D#, D#2,  E2,  F2, F#2,  G2, G#2, A2,  A#2,  B2
      65,  69,  73,  78,  82,  87,  92,  98, 104, 110, 117, 123,
   // C3, C#3,  D#, D#3,  E3,  F3, F#3,  G3, G#3, A3,  A#3,  B3
     131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247,
   // C4, C#4,  D#, D#4,  E4,  F4, F#4,  G4, G#4, A4,  A#4,  B4
     262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 
   // C5, C#5,  D#, D#5,  E5,  F5, F#5,  G5, G#5, A5,  A#5,  B5
     523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988,
   // C6, C#6,  D#, D#6,  E6,  F6, F#6,  G6, G#6, A6,  A#6,  B6
    1047,1109,1175,1245,1319,1397,1480,1568,1661,1760,1865,1976,
   // C7, C#7,  D#, D#7,  E7,  F7, F#7,  G7, G#7, A7,  A#7,  B7
    2093,2217,2349,2489,2637,2794,2960,3136,3322,3520,3729,3951,
   // C8, C#8,  D#, D#8,  E8,  F8, F#8,  G8, G#8, A8,  A#8,  B8
    4186,4435,4699,4978,5274,5588,5920,6272,6645,7040,7459,7902,
//    8372, 8870, 9397, 9956, 10548, 11175, 11840, 12544, 13290, 14080,
//    14917, 15804, 16744, 17740, 18795, 19912, 21096, 22351, 23680,
//    25088, 26580, 28160, 29834, 31609, 33488, 35479, 37589, 39824,
//    42192, 44701, 47359, 50175, 53159, 56320, 59669, 63217, 66976,
//    70959, 75178, 79649, 84385, 89402, 94719, 100351
   };

#define MAX_CHANNELS 4

void music_init(void) {
    ledc_timer_config_t ledc_timer[] = {
        {
            .duty_resolution = LEDC_TIMER_13_BIT,
            .freq_hz = 2000,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .timer_num = LEDC_TIMER_0,
            .clk_cfg = LEDC_AUTO_CLK,
        }, {
            .duty_resolution = LEDC_TIMER_13_BIT,
            .freq_hz = 2000,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .timer_num = LEDC_TIMER_1,
            .clk_cfg = LEDC_AUTO_CLK,
        }, {
            .duty_resolution = LEDC_TIMER_13_BIT,
            .freq_hz = 2000,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .timer_num = LEDC_TIMER_2,
            .clk_cfg = LEDC_AUTO_CLK,
        }, {
            .duty_resolution = LEDC_TIMER_13_BIT,
            .freq_hz = 2000,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .timer_num = LEDC_TIMER_3,
            .clk_cfg = LEDC_AUTO_CLK,
        },
    };

    for (int i = 0; i < MAX_CHANNELS; i++) {
        esp_err_t err = ledc_timer_config(&ledc_timer[i]);
        ESP_LOGE(TAG, "ledc_timer_config[%d]: %d", i, err);
    }

    ledc_channel_config_t ledc_channel[] = {
        {
            .channel = LEDC_CHANNEL_0,
            .duty = 0,
            .gpio_num = 17,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .hpoint = 0,
            .timer_sel = LEDC_TIMER_0,
            .flags.output_invert = 0,
        }, {
            .channel = LEDC_CHANNEL_1,
            .duty = 0,
            .gpio_num = 18,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .hpoint = 0,
            .timer_sel = LEDC_TIMER_1,
            .flags.output_invert = 0,
        }, {
            .channel = LEDC_CHANNEL_2,
            .duty = 0,
            .gpio_num = 19,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .hpoint = 0,
            .timer_sel = LEDC_TIMER_2,
            .flags.output_invert = 0,
        }, {
            .channel = LEDC_CHANNEL_3,
            .duty = 0,
            .gpio_num = 20,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .hpoint = 0,
            .timer_sel = LEDC_TIMER_3,
            .flags.output_invert = 0,
        }
    };

    for (int i = 0; i < MAX_CHANNELS; i++) {
        esp_err_t err = ledc_channel_config(&ledc_channel[i]);
        ESP_LOGE(TAG, "ledc_channel_config[%d]: %d", i, err);
    }
    
    int total = sizeof(freqs) / sizeof(uint32_t);

    for (int i = 36; i < total - 10; i++) {

        ESP_LOGI(TAG, "Music note %d: %s %dhz", i, notes[i], freqs[i]);
        ledc_set_freq(ledc_channel[0].speed_mode, ledc_timer[0].timer_num, freqs[i]);
        ledc_set_duty(ledc_channel[0].speed_mode, ledc_channel[0].channel, freqs[i]);
        ledc_update_duty(ledc_channel[0].speed_mode, ledc_channel[0].channel);
        vTaskDelay(1000 / portTICK_PERIOD_MS * 0.5);

        ESP_LOGI(TAG, "Music note %d: %s %dhz, %s %dhz", i, notes[i], freqs[i], notes[i+4], freqs[i+4]);
        ledc_set_freq(ledc_channel[1].speed_mode, ledc_timer[1].timer_num, freqs[i+4]);
        ledc_set_duty(ledc_channel[1].speed_mode, ledc_channel[1].channel, freqs[i+4]);
        ledc_update_duty(ledc_channel[1].speed_mode, ledc_channel[1].channel);
        vTaskDelay(1000 / portTICK_PERIOD_MS * 0.5);

        ESP_LOGI(TAG, "Music note %d: %s %dhz, %s %dhz, %s %dhz", i, notes[i], freqs[i], notes[i+4], freqs[i+4], notes[i+7], freqs[i+7]);
        ledc_set_freq(ledc_channel[2].speed_mode, ledc_timer[2].timer_num, freqs[i+7]);
        ledc_set_duty(ledc_channel[2].speed_mode, ledc_channel[2].channel, freqs[i+7]);
        ledc_update_duty(ledc_channel[2].speed_mode, ledc_channel[2].channel);
        vTaskDelay(1000 / portTICK_PERIOD_MS * 0.5);

        ESP_LOGI(TAG, "Music note %d: %s %dhz, %s %dhz, %s %dhz, %s %dhz", i, notes[i], freqs[i], notes[i+4], freqs[i+4], notes[i+7], freqs[i+7], notes[i+12], freqs[i+12]);
        ledc_set_freq(ledc_channel[3].speed_mode, ledc_timer[3].timer_num, freqs[i+12]);
        ledc_set_duty(ledc_channel[3].speed_mode, ledc_channel[3].channel, freqs[i+12]);
        ledc_update_duty(ledc_channel[3].speed_mode, ledc_channel[3].channel);
        vTaskDelay(1000 / portTICK_PERIOD_MS * 0.5);

        ESP_LOGI(TAG, "Music note %d: %s %dhz, %s %dhz, %s %dhz", i, notes[i+4], freqs[i+4], notes[i+7], freqs[i+7], notes[i+12], freqs[i+12]);
        ledc_set_duty(ledc_channel[0].speed_mode, ledc_channel[0].channel, 0);
        ledc_update_duty(ledc_channel[0].speed_mode, ledc_channel[0].channel);
        vTaskDelay(1000 / portTICK_PERIOD_MS * 0.5);

        ESP_LOGI(TAG, "Music note %d: %s %dhz, %s %dhz", i, notes[i+7], freqs[i+7], notes[i+12], freqs[i+12]);
        ledc_set_duty(ledc_channel[1].speed_mode, ledc_channel[1].channel, 0);
        ledc_update_duty(ledc_channel[1].speed_mode, ledc_channel[1].channel);
        vTaskDelay(1000 / portTICK_PERIOD_MS * 0.5);

        ESP_LOGI(TAG, "Music note %d: %s %dhz", i, notes[i+12], freqs[i+12]);
        ledc_set_duty(ledc_channel[2].speed_mode, ledc_channel[2].channel, 0);
        ledc_update_duty(ledc_channel[2].speed_mode, ledc_channel[2].channel);
        vTaskDelay(1000 / portTICK_PERIOD_MS * 0.5);

        ESP_LOGI(TAG, "Music note %d: ", i );
        ledc_set_duty(ledc_channel[3].speed_mode, ledc_channel[3].channel, 0);
        ledc_update_duty(ledc_channel[3].speed_mode, ledc_channel[3].channel);
        vTaskDelay(1000 / portTICK_PERIOD_MS * 0.5);
    }

    ESP_LOGI(TAG, "Music off");
    for (int i = 0; i < MAX_CHANNELS; i++) {
        ledc_set_duty(ledc_channel[i].speed_mode, ledc_channel[i].channel, 0);
        ledc_update_duty(ledc_channel[i].speed_mode, ledc_channel[i].channel);
    }

    ESP_LOGI(TAG, "Music initialized");
}