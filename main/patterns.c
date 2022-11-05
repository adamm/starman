#include <esp_log.h>
#include <esp_random.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>

#include "config.h"
#include "display.h"
#include "gol.h"
#include "patterns.h"
#include "patterns_gol.h"
#include "text.h"

static const char *TAG = "starman-patterns";

static TaskHandle_t text_task;
static void (*callback_func)(void) = NULL;
static display_t display;

// The star border is a special pattern that allows targeting the pixels along the PCB perimeter
// Format is { y, x, intensity }, the latter of which is dynamically set/rotated when used.
static uint8_t star_border[][3] = {
    { 1, 7, 0 }, { 1, 8, 0 }, { 2, 9, 0 }, { 3, 9, 0 }, { 4, 10, 0 }, { 5, 11, 0 }, { 5, 12, 0 }, { 5, 13, 0 },
    { 5, 14, 0 }, { 5, 15, 0 }, { 6, 15, 0 }, { 7, 14, 0 }, { 8, 13, 0 }, { 9, 12, 0 }, { 10, 13, 0 }, { 11, 13, 0 },
    { 12, 14, 0 }, { 13, 14, 0 }, { 14, 15, 0 }, { 15, 15, 0 }, { 15, 14, 0 }, { 15, 13, 0 }, { 14, 12, 0 },
    { 14, 11, 0 }, { 13, 10, 0 }, { 13, 9, 0 }, { 12, 8, 0 }, { 12, 7, 0 }, { 13, 6, 0 }, { 13, 5, 0 }, { 14, 4, 0 },
    { 14, 3, 0 }, { 15, 2, 0 }, { 15, 1, 0 }, { 15, 0, 0 }, { 14, 0, 0 }, { 13, 1, 0 }, { 12, 1, 0 }, { 11, 2, 0 },
    { 10, 2, 0 }, { 9, 3, 0 }, { 8, 2, 0 }, { 7, 1, 0 }, { 6, 0, 0 }, { 5, 0, 0 }, { 5, 1, 0 }, { 5, 2, 0 },
    { 5, 3, 0 }, { 5, 4, 0 }, { 4, 5, 0 }, { 3, 6, 0 }, { 2, 6, 0 }
};

static void patterns_castle_step();
static void patterns_checkered_step();
static void patterns_curtains_step();
static void patterns_diamonds_step();
static void patterns_fireworks_step();
static void patterns_flash_step();
static void patterns_gameover_step();
static void patterns_gameover_text_step();
static void patterns_gol_step();
static void patterns_lines_step();
static void patterns_question_step();
static void patterns_radar_step();
static void patterns_random_step();
static void patterns_siren_step();
static void patterns_spiral_step();
static void patterns_sweep_step();
static void patterns_swipe_step();
static void patterns_swoosh_step();
static void patterns_swoop_step();
static void patterns_thump_step();
static void patterns_waves_step();


static void invert() {
    for (uint8_t y = 0; y < DISPLAY_LIGHTS_HEIGHT; y++) {
        for (uint8_t x = 0; x < DISPLAY_LIGHTS_WIDTH; x++) {
            display.active[y][x] = 255 - display.active[y][x];
        }
    }
}


static void scroll(int8_t rows, int8_t cols, bool copy, uint8_t fill) {
    if (rows > 0) {
        uint8_t temp[DISPLAY_LIGHTS_WIDTH] = {0};
        memcpy(temp, display.active[0], DISPLAY_LIGHTS_WIDTH);
        for (uint8_t y = 0; y < DISPLAY_LIGHTS_HEIGHT - 1; y++) {
            memcpy(display.active[y], display.active[y + 1], DISPLAY_LIGHTS_WIDTH);
        }
        if (!copy)
            memset(temp, fill, DISPLAY_LIGHTS_WIDTH);
        memcpy(display.active[DISPLAY_LIGHTS_HEIGHT - 1], temp, DISPLAY_LIGHTS_WIDTH);
    }
    else if (rows < 0) {
        uint8_t temp[DISPLAY_LIGHTS_WIDTH] = {0};
        memcpy(temp, display.active[DISPLAY_LIGHTS_HEIGHT-1], DISPLAY_LIGHTS_WIDTH);
        for (uint8_t y = DISPLAY_LIGHTS_HEIGHT - 1; y > 0; y--) {
            memcpy(display.active[y], display.active[y - 1], DISPLAY_LIGHTS_WIDTH);
        }
        if (!copy)
            memset(temp, fill, DISPLAY_LIGHTS_WIDTH);
        memcpy(display.active[0], temp, DISPLAY_LIGHTS_WIDTH);
    }
    if (cols > 0) {
        uint8_t temp = 0;
        for (uint8_t y = 0; y < DISPLAY_LIGHTS_HEIGHT; y++) {
            temp = display.active[y][0];
            for (uint8_t x = 0; x < DISPLAY_LIGHTS_WIDTH - 1; x++) {
                display.active[y][x] = display.active[y][x + 1];
            }
            if (!copy)
                temp = fill;
            display.active[y][DISPLAY_LIGHTS_WIDTH - 1] = temp;
        }
    }
    else if (cols < 0) {
        uint8_t temp = 0;
        for (uint8_t y = 0; y < DISPLAY_LIGHTS_HEIGHT; y++) {
            temp = display.active[y][DISPLAY_LIGHTS_WIDTH-1];
            for (uint8_t x = DISPLAY_LIGHTS_WIDTH - 1; x > 0; x--) {
                display.active[y][x] = display.active[y][x - 1];
            }
            if (!copy)
                temp = fill;
            display.active[y][0] = temp;
        }
    }
}


static void swap(uint8_t *x, uint8_t *y) {
    uint8_t temp = *x;
    *x = *y;
    *y = temp;
}


static void rotate(int8_t degrees) {
    // TODO: For rotations not 90˚, implement https://en.wikipedia.org/wiki/Rotation_matrix
    if (degrees == 90) {
        // Transpose x,y then reverse columns
        for (uint8_t y = 0; y < DISPLAY_LIGHTS_HEIGHT; y++) {
            for (uint8_t x = y+1; x < DISPLAY_LIGHTS_WIDTH; x++) {
                swap(&display.active[y][x], &display.active[x][y]);
            }
        }

        for (uint8_t y = 0; y < DISPLAY_LIGHTS_HEIGHT; y++) {
            for (uint8_t x = 0; x < DISPLAY_LIGHTS_WIDTH / 2; x++) {
                swap(&display.active[y][x], &display.active[y][DISPLAY_LIGHTS_WIDTH - x - 1]);
            }
        }
    }
    else if (degrees == -90) {
        // transpose x,y then reverse rows
        for (uint8_t y = 0; y < DISPLAY_LIGHTS_HEIGHT; y++) {
            for (uint8_t x = y+1; x < DISPLAY_LIGHTS_WIDTH; x++) {
                swap(&display.active[y][x], &display.active[x][y]);
            }
        }

        for (uint8_t y = 0; y < DISPLAY_LIGHTS_HEIGHT / 2; y++) {
            for (uint8_t x = 0; x < DISPLAY_LIGHTS_WIDTH; x++) {
                swap(&display.active[y][x], &display.active[DISPLAY_LIGHTS_HEIGHT - y - 1][x]);
            }
        }
    }
}


void patterns_step_sequence() {
    // Function is called each time the music note changes. 
    // Use this to imcrement the pattern sequence and update the lights.

    if (callback_func != NULL) {
        callback_func();
    }

    // ESP_LOG_BUFFER_HEXDUMP(TAG, display.active, DISPLAY_LIGHTS_TOTAL_AREA, ESP_LOG_INFO);

    display_update_leds(&display);
}


void patterns_castle() {
    ESP_LOGI(TAG, "Begin CASTLE pattern");

    display.pattern = &castle;
    memcpy(display.active, castle.data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(display.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_castle_step;
}


static void patterns_castle_step() {
    // Scroll the checkered display up and to the left every frame
    scroll(0, 1, true, 0);
}


void patterns_checkered() {
    ESP_LOGI(TAG, "Begin CHECKERED pattern");

    display.pattern = &checkered;
    memcpy(display.active, checkered.data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(display.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_checkered_step;
}


static void patterns_checkered_step() {
    // Scroll the checkered display up and to the left every frame
    scroll(-1, 0, true, 0);
}


void patterns_curtains() {
    ESP_LOGI(TAG, "Begin CURTAINS pattern");

    display.pattern = &curtains;
    memcpy(display.active, curtains.data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(display.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_curtains_step;
}


static void patterns_curtains_step() {
    // Scroll the curtains display down, add a row of 255 on top.
    scroll(-1, 0, false, 255);
}


void patterns_diamonds() {
    ESP_LOGI(TAG, "Begin DIAMONDS pattern");

    display.pattern = &diamonds;
    memcpy(display.active, diamonds.data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(display.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_diamonds_step;
}


static void patterns_diamonds_step() {
    invert();
}


void patterns_fireworks() {
    ESP_LOGI(TAG, "Begin FIREWORKS pattern");

    // TBD
    // display.pattern = &fireworks;
    // memcpy(display.active, fireworks.data, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_fireworks_step;
}


static void patterns_fireworks_step() {
    // TBD
}


void patterns_flash() {
    ESP_LOGI(TAG, "Begin FLASH pattern");

    display.pattern = NULL;
    memset(display.active, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(display.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_flash_step;
}


static void patterns_flash_step() {
    invert();
}


void patterns_gameover() {
    display.pattern = &sweep;

    // memcpy(display.active, sweep.data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(display.active, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(display.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    text_write_string(&display, "   GAME OVER");
    // callback_func = patterns_gameover_step;
    xTaskCreate(patterns_gameover_text_step, "gameover", 8192, NULL, 5, &text_task);
}


static void patterns_gameover_step() {
    scroll(-1, 0, true, 0);
}


static void patterns_gameover_text_step() {
    while(1) {
        text_scroll(&display);
        vTaskDelay(75 / portTICK_RATE_MS);
    }
}


void patterns_gameover_stop() {
    vTaskDelete(text_task);
}


static void patterns_gol_step() {
    // Use Game of Life rules to animate the active pattern
    gol_next_generation(&display);
}


void patterns_gol_cross_2() {
    ESP_LOGI(TAG, "Begin GOL_CROSS_2 pattern");

    display.pattern = NULL;
    memcpy(display.grid, gol_cross_2, GOL_GRID_TOTAL_AREA);
    memset(display.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_gol_step;
}


void patterns_gol_four_blinkers_four_blocks() {
    ESP_LOGI(TAG, "Begin GOL_FOUR_BLINKERS_FOUR_BLOCKS pattern");

    display.pattern = NULL;
    memcpy(display.grid, gol_four_blinkers_four_blocks, GOL_GRID_TOTAL_AREA);
    memset(display.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_gol_step;
}


void patterns_gol_galaxy() {
    ESP_LOGI(TAG, "Begin GOL_GALAXY pattern");

    display.pattern = NULL;
    memcpy(display.grid, gol_galaxy, GOL_GRID_TOTAL_AREA);
    memset(display.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_gol_step;
}


void patterns_gol_pentadecathlon() {
    ESP_LOGI(TAG, "Begin GOL_PENTADECATHLON pattern");

    display.pattern = NULL;
    memcpy(display.grid, gol_pentadecathlon, GOL_GRID_TOTAL_AREA);
    memset(display.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_gol_step;
}


void patterns_gol_sprinkles() {
    ESP_LOGI(TAG, "Begin GOL_SPRINKLES pattern");

    // Sprinkles (like sparkles, but 1x3 and 3x1 Game Of Life beacons)
    display.pattern = NULL;
    memcpy(display.grid, gol_sprinkles, GOL_GRID_TOTAL_AREA);
    memset(display.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_gol_step;
}


void patterns_lines() {
    ESP_LOGI(TAG, "Begin LINES pattern");

    display.pattern = &lines;
    memcpy(display.active, lines.data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(display.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_lines_step;
}


static void patterns_lines_step() {
    scroll(0, 1, true, 0);
}


void patterns_question() {
    ESP_LOGI(TAG, "Begin QUESTION pattern");

    display.pattern = &question;
    memcpy(display.active, question.data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(display.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_question_step;
}


static void patterns_question_step() {
    invert();
}


void patterns_radar() {
    ESP_LOGI(TAG, "Begin RADAR pattern");

    display.pattern = &radar;
    memcpy(display.active, radar.data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(display.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_radar_step;
}


static void patterns_radar_step() {
    // TODO: Actually only rotate 15˚ every frame. Alternatively, draw the
    // lines dynamically and fill.
    rotate(-90);
}


void patterns_random() {
    ESP_LOGI(TAG, "Begin RANDOM pattern");

    display.pattern = NULL;
    memset(display.active, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(display.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_lines_step;
    patterns_random_step();
}


static void patterns_random_step() {
    esp_fill_random(display.active, DISPLAY_LIGHTS_TOTAL_AREA);
}


void patterns_siren() {
    ESP_LOGI(TAG, "Begin SIREN pattern");

    display.pattern = &siren;
    memcpy(display.active, siren.data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(display.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_siren_step;
}
    

static void patterns_siren_step() {
    // Flip vertically, then horizontally, then vertically, then horizontally (effectively rotate 90˚ each frame)
    rotate(90);
}


void patterns_spiral() {
    ESP_LOGI(TAG, "Begin SPIRAL pattern");

    display.pattern = &spiral;
    memcpy(display.active, spiral.data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(display.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_spiral_step;
}


static void patterns_spiral_step() {
    // XXX: Debug random crash in rotate()
    rotate(90);
}


void patterns_sweep() {
    ESP_LOGI(TAG, "Begin SWEEP pattern");

    display.pattern = &sweep;
    memcpy(display.active, sweep.data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(display.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_sweep_step;
}


static void patterns_sweep_step() {
    // Sweep from right to left
    scroll(1, 0, true, 0);
}


void patterns_swipe() {
    ESP_LOGI(TAG, "Begin SWIPE pattern");

    display.pattern = &swipe;
    memcpy(display.active, swipe.data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(display.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_swipe_step;
}


static void patterns_swipe_step() {
    // Sweep from right to left
    scroll(0, 1, true, 0);
}


void patterns_swoosh() {
    ESP_LOGI(TAG, "Begin SWOOP pattern");

    display.pattern = &swoosh;
    memcpy(display.active, swoosh.data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(display.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_swoosh_step;

    size_t border_length = sizeof(star_border)/3;
    uint8_t intensity = 0xF0;

    for (uint8_t i = 0; i < border_length; i++ ) {
        star_border[i][2] = intensity;
        intensity -= 0x30;
    }
}
    

static void patterns_swoosh_step() {
    size_t border_length = sizeof(star_border)/3;
    uint8_t i, x, y;

    y = star_border[border_length-1][0];
    x = star_border[border_length-1][1];
    star_border[border_length-1][2] = star_border[0][2];
    display.overlay[y][x] = star_border[border_length-1][2];

    for (i = 0; i < border_length - 1; i++ ) {
        y = star_border[i][0];
        x = star_border[i][1];
        star_border[i][2] = star_border[i+1][2];
        display.overlay[y][x] = star_border[i][2];
    }

    scroll(0, 1, true, 0);
}


void patterns_swoop() {
    ESP_LOGI(TAG, "Begin SWOOP pattern");

    // TBD
    // display.pattern = &swoop;
    // memcpy(display.active, swoop.data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(display.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_swoop_step;
}


static void patterns_swoop_step() {
    scroll(0, 1, true, 0);
}


void patterns_thump() {
    ESP_LOGI(TAG, "Begin WAVES pattern");

    // TBD
    // display.pattern = thump[0];
    // memcpy(display.active, thump[0].data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(display.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_thump_step;
}


static void patterns_thump_step() {
    // TBD:  Jump right to largest "thump" image at instrument play, decay to smallest "thump" image while not playing
    // You know, like a drum.
}


// Incase the waves pattern is interrupted and needs to restart, store the state statically and reset it at start;
static int8_t waves_height = 0;
static bool waves_down = true;

void patterns_waves() {
    ESP_LOGI(TAG, "Begin WAVES pattern");

    waves_height = 0;
    waves_down = true;

    display.pattern = &waves;
    memcpy(display.active, waves.data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(display.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_waves_step;
}


static void patterns_waves_step() {
    // The waves pattern scrolls from left to right, and simultaneously slides up and down
    if (waves_height < -2 || waves_height > 2)
        waves_down = !waves_down;

    if (waves_down) {
        scroll(-1, 0, false, 4);
        scroll(0, -1, true, 4);
        waves_height++;
    }
    else {
        scroll(1, 0, false, 64);
        scroll(0, -1, true, 64);
        waves_height--;
    }
}
