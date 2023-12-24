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

#include <esp_log.h>
#include <esp_random.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <math.h>
#include <string.h>

#include "config.h"
#include "draw.h"
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
static void patterns_sand_step();
static void patterns_siren_step();
static void patterns_spiral_step();
static void patterns_sweep_step();
static void patterns_swipe_step();
static void patterns_swoosh_step();
static void patterns_starman_step();
static void patterns_swoop_step();
static void patterns_thump_step();
static void patterns_waves_step();


static void invert_background() {
    for (uint8_t y = 0; y < DISPLAY_LIGHTS_HEIGHT; y++) {
        for (uint8_t x = 0; x < DISPLAY_LIGHTS_WIDTH; x++) {
            // In order to avoid overly flickering the screen during invert,
            // subtract 51 (which is 1/4 of 255 max value) from each pixel. The
            // end result is inverting the screen but not as quickly
            display.background[y][x] = 51 - display.background[y][x];
        }
    }
}


static void scroll_background(int8_t rows, int8_t cols, bool copy, uint8_t fill) {
    if (rows > 0) {
        uint8_t temp[DISPLAY_LIGHTS_WIDTH] = {0};
        memcpy(temp, display.background[0], DISPLAY_LIGHTS_WIDTH);
        for (uint8_t y = 0; y < DISPLAY_LIGHTS_HEIGHT - 1; y++) {
            memcpy(display.background[y], display.background[y + 1], DISPLAY_LIGHTS_WIDTH);
        }
        if (!copy)
            memset(temp, fill, DISPLAY_LIGHTS_WIDTH);
        memcpy(display.background[DISPLAY_LIGHTS_HEIGHT - 1], temp, DISPLAY_LIGHTS_WIDTH);
    }
    else if (rows < 0) {
        uint8_t temp[DISPLAY_LIGHTS_WIDTH] = {0};
        memcpy(temp, display.background[DISPLAY_LIGHTS_HEIGHT-1], DISPLAY_LIGHTS_WIDTH);
        for (uint8_t y = DISPLAY_LIGHTS_HEIGHT - 1; y > 0; y--) {
            memcpy(display.background[y], display.background[y - 1], DISPLAY_LIGHTS_WIDTH);
        }
        if (!copy)
            memset(temp, fill, DISPLAY_LIGHTS_WIDTH);
        memcpy(display.background[0], temp, DISPLAY_LIGHTS_WIDTH);
    }
    if (cols > 0) {
        uint8_t temp = 0;
        for (uint8_t y = 0; y < DISPLAY_LIGHTS_HEIGHT; y++) {
            temp = display.background[y][0];
            for (uint8_t x = 0; x < DISPLAY_LIGHTS_WIDTH - 1; x++) {
                display.background[y][x] = display.background[y][x + 1];
            }
            if (!copy)
                temp = fill;
            display.background[y][DISPLAY_LIGHTS_WIDTH - 1] = temp;
        }
    }
    else if (cols < 0) {
        uint8_t temp = 0;
        for (uint8_t y = 0; y < DISPLAY_LIGHTS_HEIGHT; y++) {
            temp = display.background[y][DISPLAY_LIGHTS_WIDTH-1];
            for (uint8_t x = DISPLAY_LIGHTS_WIDTH - 1; x > 0; x--) {
                display.background[y][x] = display.background[y][x - 1];
            }
            if (!copy)
                temp = fill;
            display.background[y][0] = temp;
        }
    }
}


static void swap(uint8_t *x, uint8_t *y) {
    uint8_t temp = *x;
    *x = *y;
    *y = temp;
}


static void rotate_background(int8_t degrees) {
    // TODO: For rotations not 90˚, implement https://en.wikipedia.org/wiki/Rotation_matrix
    if (degrees == 90) {
        // Transpose x,y then reverse columns
        for (uint8_t y = 0; y < DISPLAY_LIGHTS_HEIGHT; y++) {
            for (uint8_t x = y+1; x < DISPLAY_LIGHTS_WIDTH; x++) {
                swap(&display.background[y][x], &display.background[x][y]);
            }
        }

        for (uint8_t y = 0; y < DISPLAY_LIGHTS_HEIGHT; y++) {
            for (uint8_t x = 0; x < DISPLAY_LIGHTS_WIDTH / 2; x++) {
                swap(&display.background[y][x], &display.background[y][DISPLAY_LIGHTS_WIDTH - x - 1]);
            }
        }
    }
    else if (degrees == -90) {
        // transpose x,y then reverse rows
        for (uint8_t y = 0; y < DISPLAY_LIGHTS_HEIGHT; y++) {
            for (uint8_t x = y+1; x < DISPLAY_LIGHTS_WIDTH; x++) {
                swap(&display.background[y][x], &display.background[x][y]);
            }
        }

        for (uint8_t y = 0; y < DISPLAY_LIGHTS_HEIGHT / 2; y++) {
            for (uint8_t x = 0; x < DISPLAY_LIGHTS_WIDTH; x++) {
                swap(&display.background[y][x], &display.background[DISPLAY_LIGHTS_HEIGHT - y - 1][x]);
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

    // ESP_LOG_BUFFER_HEXDUMP(TAG, display.background, DISPLAY_LIGHTS_TOTAL_AREA, ESP_LOG_INFO);

    display_update_leds(&display);
}


void patterns_castle() {
    ESP_LOGI(TAG, "Begin CASTLE pattern");

    display_reset(&display);
    display.pattern = &castle;
    memcpy(display.background, castle.data, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_castle_step;
}


static void patterns_castle_step() {
    // Scroll the checkered display up and to the left every frame
    scroll_background(0, 1, true, 0);
}


void patterns_checkered() {
    ESP_LOGI(TAG, "Begin CHECKERED pattern");

    display_reset(&display);
    display.pattern = &checkered;
    memcpy(display.background, checkered.data, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_checkered_step;
}


static void patterns_checkered_step() {
    // Scroll the checkered display up and to the left every frame
    scroll_background(-1, 0, true, 0);
}


void patterns_curtains() {
    ESP_LOGI(TAG, "Begin CURTAINS pattern");

    display_reset(&display);
    display.pattern = &curtains;
    memcpy(display.background, curtains.data, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_curtains_step;
}


static void patterns_curtains_step() {
    // Scroll the curtains display down, add a row of 255 on top.
    scroll_background(-1, 0, false, 255);
}


void patterns_diamonds() {
    ESP_LOGI(TAG, "Begin DIAMONDS pattern");

    display_reset(&display);
    display.pattern = &diamonds;
    memcpy(display.background, diamonds.data, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_diamonds_step;
}


static void patterns_diamonds_step() {
    invert_background();
}


void patterns_fireworks() {
    ESP_LOGI(TAG, "Begin FIREWORKS pattern");

    display_reset(&display);
    // TBD
    // display.pattern = &fireworks;
    // memcpy(display.background, fireworks.data, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_fireworks_step;
}


static void patterns_fireworks_step() {
    // TBD
}


void patterns_flash() {
    ESP_LOGI(TAG, "Begin FLASH pattern");

    display_reset(&display);
    display.pattern = NULL;
    callback_func = patterns_flash_step;
}


static void patterns_flash_step() {
    invert_background();
}


void patterns_gameover() {
    display.pattern = &sweep;

    display_reset(&display);
    memcpy(display.background, sweep.data, DISPLAY_LIGHTS_TOTAL_AREA);
    text_write_string(&display, "  GAME OVER");
    callback_func = patterns_gameover_step;
    xTaskCreate(patterns_gameover_text_step, "gameover", 8192, NULL, 5, &text_task);
}


static void patterns_gameover_step() {
    scroll_background(-1, 0, true, 0);
}


static void patterns_gameover_text_step() {
    while(1) {
        text_scroll(&display);
        display_update_leds(&display);
        vTaskDelay(100 / portTICK_RATE_MS);
    }
}


void patterns_gameover_stop() {
    vTaskDelete(text_task);
}


static void patterns_gol_step() {
    // Use Game of Life rules to animate the.background pattern
    gol_next_generation(&display);
}


void patterns_gol_cross_2() {
    ESP_LOGI(TAG, "Begin GOL_CROSS_2 pattern");

    display_reset(&display);
    memcpy(display.grid, gol_cross_2, GOL_GRID_TOTAL_AREA);
    callback_func = patterns_gol_step;
}


void patterns_gol_four_blinkers_four_blocks() {
    ESP_LOGI(TAG, "Begin GOL_FOUR_BLINKERS_FOUR_BLOCKS pattern");

    display_reset(&display);
    memcpy(display.grid, gol_four_blinkers_four_blocks, GOL_GRID_TOTAL_AREA);
    callback_func = patterns_gol_step;
}


void patterns_gol_galaxy() {
    ESP_LOGI(TAG, "Begin GOL_GALAXY pattern");

    display_reset(&display);
    memcpy(display.grid, gol_galaxy, GOL_GRID_TOTAL_AREA);
    callback_func = patterns_gol_step;
}


void patterns_gol_pentadecathlon() {
    ESP_LOGI(TAG, "Begin GOL_PENTADECATHLON pattern");

    display_reset(&display);
    memcpy(display.grid, gol_pentadecathlon, GOL_GRID_TOTAL_AREA);
    callback_func = patterns_gol_step;
}


void patterns_gol_sprinkles() {
    ESP_LOGI(TAG, "Begin GOL_SPRINKLES pattern");

    // Sprinkles (like sparkles, but 1x3 and 3x1 Game Of Life beacons)
    display_reset(&display);
    memcpy(display.grid, gol_sprinkles, GOL_GRID_TOTAL_AREA);
    callback_func = patterns_gol_step;
}


static uint8_t lines_direction = 0;
static uint16_t lines_time = 0;


void patterns_lines() {
    ESP_LOGI(TAG, "Begin LINES pattern");

    lines_direction = 0;
    lines_time = 0;
    display_reset(&display);
    display.pattern = &lines;
    memcpy(display.background, lines.data, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_lines_step;
}


static void patterns_lines_step() {
    // After every 22 steps, rotate the pattern and change scroll direction
    if (lines_time++ >= 22) {
        rotate_background(90);
        lines_direction++;
        lines_time = 0;
    }

    if (lines_direction == 0) {
        scroll_background(0, 1, true, 0);
    }
    else if (lines_direction == 1) {
        scroll_background(1, 0, true, 0);
    }
    else if (lines_direction == 2) {
        scroll_background(0, -1, true, 0);
    }
    else if (lines_direction == 3) {
        scroll_background(-1, 0, true, 0);
    }
    else {
        lines_direction = 0;
        scroll_background(0, 1, true, 0);
    }
}


void patterns_question() {
    ESP_LOGI(TAG, "Begin QUESTION pattern");

    display_reset(&display);
    display.pattern = &question;
    memcpy(display.background, question.data, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_question_step;
}


static void patterns_question_step() {
    invert_background();
}


void patterns_radar() {
    ESP_LOGI(TAG, "Begin RADAR pattern");

    display_reset(&display);
    display.pattern = &radar;
    memcpy(display.background, radar.data, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_radar_step;
}


static void patterns_radar_step() {
    // TODO: Actually only rotate 15˚ every frame. Alternatively, draw the
    // lines dynamically and fill.
    rotate_background(-90);
}


void patterns_random() {
    ESP_LOGI(TAG, "Begin RANDOM pattern");

    display_reset(&display);
    callback_func = patterns_random_step;
    patterns_random_step();
}


static void patterns_random_step() {
    uint8_t i, j;
    esp_fill_random(display.background, DISPLAY_LIGHTS_TOTAL_AREA);

    for (i = 0; i < 16; i++) {
        for (j = 0; j < 16; j++) {
            display.background[i][j] >>= 7;
            display.background[i][j] <<= 7;
        }
    }
}


static double sand_time = 0;
static const uint8_t sand_color[] = { 0, 64, 128, 192, 255, 192, 128, 64 };
static const uint8_t sand_z[] = { 0, 1, 2, 3, 4, 3, 2, 1 };


void patterns_sand() {
    ESP_LOGI(TAG, "Begin SIREN pattern");

    display_reset(&display);
    display.pattern = NULL;
    memset(display.background, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    sand_time = 0;

    callback_func = patterns_sand_step;
}


static void patterns_sand_step() {
    float c, v;

    for(int8_t i = -8; i < 8; i++) {
        for(int8_t j = -8; j < 8; j++) {
            c = i / (1 + abs(j)) + sin(j/8);
            v = j * sand_z[(int16_t)(c-sand_time)%8]/16;
            draw_line(&display, 8+i, 8+j-v, 8+i, 8+j,
                    sand_color[(int16_t)(c-sand_time)%8]);
        }
    }
    sand_time = sand_time - 1/10;
}


void patterns_siren() {
    ESP_LOGI(TAG, "Begin SIREN pattern");

    display_reset(&display);
    display.pattern = &siren;
    memcpy(display.background, siren.data, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_siren_step;
}


static void patterns_siren_step() {
    rotate_background(90);
}


void patterns_spiral() {
    ESP_LOGI(TAG, "Begin SPIRAL pattern");

    display_reset(&display);
    display.pattern = &spiral;
    memcpy(display.background, spiral.data, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_spiral_step;
}


static void patterns_spiral_step() {
    rotate_background(90);
}


void patterns_sweep() {
    ESP_LOGI(TAG, "Begin SWEEP pattern");

    display_reset(&display);
    display.pattern = &sweep;
    memcpy(display.background, sweep.data, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_sweep_step;
}


static void patterns_sweep_step() {
    // Sweep from right to left
    scroll_background(1, 0, true, 0);
}


void patterns_swipe() {
    ESP_LOGI(TAG, "Begin SWIPE pattern");

    display_reset(&display);
    display.pattern = &swipe;
    // pattern_set_background(&display, 0, 0);
    memcpy(display.background, swipe.data, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_swipe_step;
}


static void patterns_swipe_step() {
    // Sweep from right to left
    scroll_background(0, 1, true, 0);
}


void patterns_swoosh() {
    ESP_LOGI(TAG, "Begin SWOOSH pattern");

    display_reset(&display);
    display.pattern = &swoosh;
    memcpy(display.background, swoosh.data, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_swoosh_step;
}
    

static void patterns_swoosh_step() {
    scroll_background(0, 1, true, 0);
}


void patterns_starman() {
    ESP_LOGI(TAG, "Begin STARMAN pattern");

    display_reset(&display);
    display.pattern = &swoosh;
    memcpy(display.background, swoosh.data, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_starman_step;

    size_t border_length = sizeof(star_border)/3;
    uint8_t intensity = 0xF0;

    for (uint8_t i = 0; i < border_length; i++ ) {
        star_border[i][2] = intensity;
        intensity -= 0x30;
    }
}
    

static void patterns_starman_step() {
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

    scroll_background(0, 1, true, 0);
}


void patterns_swoop() {
    ESP_LOGI(TAG, "Begin SWOOP pattern");

    display_reset(&display);
    // TBD
    // display.pattern = &swoop;
    // memcpy(display.background, swoop.data, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_swoop_step;
}


static void patterns_swoop_step() {
    scroll_background(0, 1, true, 0);
}


void patterns_thump() {
    ESP_LOGI(TAG, "Begin WAVES pattern");

    display_reset(&display);
    // TBD
    // display.pattern = thump[0];
    // memcpy(display.background, thump[0].data, DISPLAY_LIGHTS_TOTAL_AREA);
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

    display_reset(&display);
    display.pattern = &waves;
    memcpy(display.background, waves.data, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_waves_step;
}


static void patterns_waves_step() {
    // The waves pattern scrolls from left to right, and simultaneously slides up and down
    if (waves_height < -2 || waves_height > 2)
        waves_down = !waves_down;

    if (waves_down) {
        scroll_background(-1, 0, false, 4);
        scroll_background(0, -1, true, 4);
        waves_height++;
    }
    else {
        scroll_background(1, 0, false, 64);
        scroll_background(0, -1, true, 64);
        waves_height--;
    }
}
