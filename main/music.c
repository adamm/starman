#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/ledc.h>
#include <esp_log.h>
#include <string.h>

#include "config.h"

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
   "C9", "C#9", "D9", "D#9", "E9", "F9", "F#9", "G9", "G#9", "A9", "A#9", "B9",
   "C10", "C#10", "D10", "D#10", "E10", "F10", "F#10", "G10", "G#10", "A10", "A#10", "B10",
   "C11", "C#11", "D11", "D#11", "E11", "F11", "F#11", "G11", "G#11", "A11", "A#11", "B11",
   "C12", "C#12", "D12", "D#12", "E12", "F12", "F#12", "G12",
};

// Table of midi note frequencies times 8
//   They are times 8 for greater accuracy.
//   Generated from Excel by =ROUND(8*440/32*(2^((x-9)/12)),0) for 0<x<128
static const uint32_t freqs[] = {
   // C1,    C#1,    D#,   D#1,    E1,    F1,   F#1,    G1,   G#1,   A1,    A#1,    B1,
      33,     35,    37,    39,    41,    44,    46,    49,   52,    55,     58,    62,
   // C2,    C#2,    D2,   D#2,    E2,    F2,   F#2,    G2,   G#2,    A2,   A#2,    B2,
      65,     69,    73,    78,    82,    87,    92,    98,   104,   110,   117,   123,   // 0..11
   // C3,    C#3,    D3,   D#3,    E3,    F3,   F#3,    G3,   G#3,    A3,   A#3,    B3, 
     131,    139,   147,   156,   165,   175,   185,   196,   208,   220,   233,   247,   // 12..23
   // C4,    C#4,    D4,   D#4,    E4,    F4,   F#4,    G4,   G#4,    A4,   A#4,    B4, 
     262,    277,   294,   311,   330,   349,   370,   392,   415,   440,   466,   494,   // 24..35
   // C5,    C#5,    D5,   D#5,    E5,    F5,   F#5,    G5,   G#5,    A5,   A#5,    B5, 
     523,    554,   587,   622,   659,   698,   740,   784,   831,   880,   932,   988,   // 36..47
   // C6,    C#6,    D6,   D#6,    E6,    F6,   F#6,    G6,   G#6,    A6,   A#6,    B6, 
    1047,   1109,  1175,  1245,  1319,  1397,  1480,  1568,  1661,  1760,  1865,  1976,   // 48..59
   // C7,    C#7,    D7,   D#7,    E7,    F7,   F#7,    G7,   G#7,    A7,   A#7,    B7, 
    2093,   2217,  2349,  2489,  2637,  2794,  2960,  3136,  3322,  3520,  3729,  3951,   // 60..71
   // C8,    C#8,    D8,   D#8,    E8,    F8,   F#8,    G8,   G#8,    A8,   A#8,    B8, 
    4186,   4435,  4699,  4978,  5274,  5588,  5920,  6272,  6645,  7040,  7459,  7902,   // 72..83
   // C9,    C#9,    D9,   D#9,    E9,    F9,   F#9,    G9,   G#9,    A9,   A#9,    B9, 
    8372,   8870,  9397,  9956, 10548, 11175, 11840, 12544, 13290, 14080, 14917, 15804,   // 84..95
   // C10,  C#10,   D10,  D#10,   E10,   F10,  F#10,   G10,  G#10,   A10,  A#10,   B10, 
    16744, 17740, 18795, 19912, 21096, 22351, 23680, 25088, 26580, 28160, 29834, 31609,  // 96..107
   // C11,  C#11,   D11,  D#11,   E11,   F11,  F#11,   G11,  G#11,   A11,  A#11,   B11, 
    33488, 35479, 37589, 39824, 42192, 44701, 47359, 50175, 53159, 56320, 59669, 63217, // 108..119
   // C12,  C#12,   D12,  D#12,   E12,   F12,  F#12,    G12,
    66976, 70959, 75178, 79649, 84385, 89402, 94719, 100351                             // 120..127
   };

#define MAX_CHANNELS 4

static ledc_timer_config_t* ledc_timer;
static ledc_channel_config_t* ledc_channel;

uint8_t _tune_speed = 100;
volatile const byte *score_start = 0;
volatile const byte *score_cursor = 0;
volatile bool music_playing = false;
static bool volume_present = false;
static bool (*callback_func)(uint32_t) = NULL;

static void music_stepscore(void);
static void music_stopscore(void);

static void music_test_chords(void) {
    
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
}


void music_callback(bool (*callback)(uint32_t)) {
    callback_func = callback;
}


static void music_playnote(byte chan, byte note) {
    uint32_t freq = freqs[note];

    if (chan > MAX_CHANNELS)
        return;

    ESP_LOGI(TAG, "chan: %d\tnote: %s\tfreq: %d", chan, notes[note], freq);
    ledc_set_freq(ledc_channel[chan].speed_mode, ledc_timer[chan].timer_num, freq);
    ledc_set_duty(ledc_channel[chan].speed_mode, ledc_channel[chan].channel, freq);
    ledc_update_duty(ledc_channel[chan].speed_mode, ledc_channel[chan].channel);
}


static void music_stopnote(byte chan) {
    if (chan > MAX_CHANNELS)
        return;

    // ESP_LOGI(TAG, "chan: %d\tstop", chan);
    ledc_set_duty(ledc_channel[chan].speed_mode, ledc_channel[chan].channel, 0);
    ledc_update_duty(ledc_channel[chan].speed_mode, ledc_channel[chan].channel);
}


void music_playscore(const byte* score) {
    music_header_t file_header;

    score_start = score;

    // Read header
    // look for the optional file header
    memcpy(&file_header, score, sizeof(music_header_t)); // copy possible header from PROGMEM to RAM
    if (file_header.id1 == 'P' && file_header.id2 == 't') { // validate it
        volume_present = file_header.f1 & HDR_F1_VOLUME_PRESENT;
        ESP_LOGI(TAG, "header: volume_present=%d, tones=%d", volume_present, file_header.num_tgens);
        score_start += file_header.hdr_length; // skip the whole header
    }
    score_cursor = score_start;
    music_playing = true;
    music_stepscore();  /* execute initial commands */
}


static void music_stepscore(void) {
    byte cmd, opcode, chan, note;
    unsigned duration;

    /* if CMD < 0x80, then the other 7 bits and the next byte are a 15-bit big-endian number of msec to wait */
    while (1) {
        cmd = *score_cursor++;
        ESP_LOGD(TAG, "cmd: %x", cmd);
        if (cmd < 0x80) { /* wait count in msec. */
            duration = ((unsigned)cmd << 8) | (*score_cursor++);
            if (_tune_speed != 100)
                duration = (unsigned) (((unsigned long)duration * 100UL) / _tune_speed);

            ESP_LOGI(TAG, "wait: %d ms", duration);

            if (callback_func != NULL && music_playing) {
                bool stop = callback_func((uint32_t)(score_cursor - score_start));
                if (stop) {
                    music_stopscore();
                    break;
                }
            }

            vTaskDelay(duration / portTICK_PERIOD_MS);
        }
        else {
            opcode = cmd & 0xf0;
            chan = cmd & 0x0f;

            if (opcode == CMD_STOPNOTE) { /* stop note */
                music_stopnote(chan);
            }
            else if (opcode == CMD_PLAYNOTE) { /* play note */
                note = *score_cursor++;
                if (volume_present) {
                    // TODO: find a way to implement volume, until then...
                    score_cursor++; // ... just ignore volume byte if present
                }

                // Note represented in score is offset by two octives above. Compensate.
                note -= 12*2;

                music_playnote(chan, note);
            }
            else if (opcode == CMD_RESTART) { /* restart score */
                score_cursor = score_start;
            }
            else if (opcode == CMD_TEMPO) { /* set tempo */
                _tune_speed = *score_cursor++;
            }
            else if (opcode == CMD_STOP) { /* stop score */
                music_stopscore();
                break;
            }
        }
    }
}


void music_settempo(uint8_t new_tempo) {
    _tune_speed = new_tempo;
}


static void music_stopscore(void) {
    for (int i = 0; i < MAX_CHANNELS; i++) {
        ledc_set_duty(ledc_channel[i].speed_mode, ledc_channel[i].channel, 0);
        ledc_update_duty(ledc_channel[i].speed_mode, ledc_channel[i].channel);
    }

    ESP_LOGI(TAG, "done");

    music_playing = false;
}


void music_init(void) {
    ledc_timer   = malloc(sizeof(ledc_timer_config_t) * MAX_CHANNELS);

    ledc_timer[0].duty_resolution = LEDC_TIMER_13_BIT;
    ledc_timer[0].freq_hz = 2000;
    ledc_timer[0].speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_timer[0].timer_num = LEDC_TIMER_0;
    ledc_timer[0].clk_cfg = LEDC_AUTO_CLK;

    ledc_timer[1].duty_resolution = LEDC_TIMER_13_BIT;
    ledc_timer[1].freq_hz = 2000;
    ledc_timer[1].speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_timer[1].timer_num = LEDC_TIMER_1;
    ledc_timer[1].clk_cfg = LEDC_AUTO_CLK;

    ledc_timer[2].duty_resolution = LEDC_TIMER_13_BIT;
    ledc_timer[2].freq_hz = 2000;
    ledc_timer[2].speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_timer[2].timer_num = LEDC_TIMER_2;
    ledc_timer[2].clk_cfg = LEDC_AUTO_CLK;

    ledc_timer[3].duty_resolution = LEDC_TIMER_13_BIT;
    ledc_timer[3].freq_hz = 2000;
    ledc_timer[3].speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_timer[3].timer_num = LEDC_TIMER_3;
    ledc_timer[3].clk_cfg = LEDC_AUTO_CLK;

    for (int i = 0; i < MAX_CHANNELS; i++) {
        esp_err_t err = ledc_timer_config(&ledc_timer[i]);
        ESP_LOGE(TAG, "ledc_timer_config[%d]: %d", i, err);
    }

    ledc_channel = malloc(sizeof(ledc_channel_config_t) * MAX_CHANNELS);

    ledc_channel[0].channel = LEDC_CHANNEL_0;
    ledc_channel[0].duty = 0;
    ledc_channel[0].gpio_num = MUSIC_CHANNEL_1_PIN;
    ledc_channel[0].speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_channel[0].hpoint = 0;
    ledc_channel[0].intr_type = LEDC_INTR_DISABLE;
    ledc_channel[0].timer_sel = LEDC_TIMER_0;
    ledc_channel[0].flags.output_invert = 0;

    ledc_channel[1].channel = LEDC_CHANNEL_1;
    ledc_channel[1].duty = 0;
    ledc_channel[1].gpio_num = MUSIC_CHANNEL_2_PIN;
    ledc_channel[1].speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_channel[1].hpoint = 0;
    ledc_channel[1].intr_type = LEDC_INTR_DISABLE;
    ledc_channel[1].timer_sel = LEDC_TIMER_1;
    ledc_channel[1].flags.output_invert = 0;

    ledc_channel[2].channel = LEDC_CHANNEL_2;
    ledc_channel[2].duty = 0;
    ledc_channel[2].gpio_num = MUSIC_CHANNEL_3_PIN;
    ledc_channel[2].speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_channel[2].hpoint = 0;
    ledc_channel[2].intr_type = LEDC_INTR_DISABLE;
    ledc_channel[2].timer_sel = LEDC_TIMER_2;
    ledc_channel[2].flags.output_invert = 0;

    ledc_channel[3].channel = LEDC_CHANNEL_3;
    ledc_channel[3].duty = 0;
    ledc_channel[3].gpio_num = MUSIC_CHANNEL_4_PIN;
    ledc_channel[3].speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_channel[3].hpoint = 0;
    ledc_channel[3].intr_type = LEDC_INTR_DISABLE;
    ledc_channel[3].timer_sel = LEDC_TIMER_3;
    ledc_channel[3].flags.output_invert = 0;

    for (int i = 0; i < MAX_CHANNELS; i++) {
        esp_err_t err = ledc_channel_config(&ledc_channel[i]);
        ESP_LOGE(TAG, "ledc_channel_config[%d]: %d", i, err);
    }

    ESP_LOGI(TAG, "Music initialized");
}


void music_stop(void) {
    for (int i = 0; i < MAX_CHANNELS; i++) {
        ledc_set_duty(ledc_channel[i].speed_mode, ledc_channel[i].channel, 0);
        ledc_update_duty(ledc_channel[i].speed_mode, ledc_channel[i].channel);
    }

    free(ledc_channel);
    free(ledc_timer);
}