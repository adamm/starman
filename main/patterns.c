#include <esp_log.h>
#include <esp_random.h>
#include <string.h>

#include "config.h"
#include "gol.h"
#include "lights.h"
#include "patterns.h"
#include "patterns_gol.h"

static const char *TAG = "starman-patterns";

static void (*callback_func)(void) = NULL;
static display_t framebuffer;

static void patterns_castle_step();
static void patterns_checkered_step();
static void patterns_curtains_step();
static void patterns_diamonds_step();
static void patterns_fireworks_step();
static void patterns_flash_step();
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
            framebuffer.active[y][x] = 255 - framebuffer.active[y][x];
        }
    }
}


static void scroll(int8_t rows, int8_t cols, bool copy, uint8_t fill) {
    if (rows > 0) {
        uint8_t temp[DISPLAY_LIGHTS_WIDTH] = {0};
        memcpy(temp, framebuffer.active[0], DISPLAY_LIGHTS_WIDTH);
        for (uint8_t y = 0; y < DISPLAY_LIGHTS_HEIGHT - 1; y++) {
            memcpy(framebuffer.active[y], framebuffer.active[y + 1], DISPLAY_LIGHTS_WIDTH);
        }
        if (!copy)
            memset(temp, fill, DISPLAY_LIGHTS_WIDTH);
        memcpy(framebuffer.active[DISPLAY_LIGHTS_HEIGHT - 1], temp, DISPLAY_LIGHTS_WIDTH);
    }
    else if (rows < 0) {
        uint8_t temp[DISPLAY_LIGHTS_WIDTH] = {0};
        memcpy(temp, framebuffer.active[DISPLAY_LIGHTS_HEIGHT-1], DISPLAY_LIGHTS_WIDTH);
        for (uint8_t y = DISPLAY_LIGHTS_HEIGHT - 1; y > 0; y--) {
            memcpy(framebuffer.active[y], framebuffer.active[y - 1], DISPLAY_LIGHTS_WIDTH);
        }
        if (!copy)
            memset(temp, fill, DISPLAY_LIGHTS_WIDTH);
        memcpy(framebuffer.active[0], temp, DISPLAY_LIGHTS_WIDTH);
    }
    if (cols > 0) {
        uint8_t temp = 0;
        for (uint8_t y = 0; y < DISPLAY_LIGHTS_HEIGHT; y++) {
            temp = framebuffer.active[y][0];
            for (uint8_t x = 0; x < DISPLAY_LIGHTS_WIDTH - 1; x++) {
                framebuffer.active[y][x] = framebuffer.active[y][x + 1];
            }
            if (!copy)
                temp = fill;
            framebuffer.active[y][DISPLAY_LIGHTS_WIDTH - 1] = temp;
        }
    }
    else if (cols < 0) {
        uint8_t temp = 0;
        for (uint8_t y = 0; y < DISPLAY_LIGHTS_HEIGHT; y++) {
            temp = framebuffer.active[y][DISPLAY_LIGHTS_WIDTH-1];
            for (uint8_t x = DISPLAY_LIGHTS_WIDTH - 1; x > 0; x--) {
                framebuffer.active[y][x] = framebuffer.active[y][x - 1];
            }
            if (!copy)
                temp = fill;
            framebuffer.active[y][0] = temp;
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
                swap(&framebuffer.active[y][x], &framebuffer.active[x][y]);
            }
        }

        for (uint8_t y = 0; y < DISPLAY_LIGHTS_HEIGHT; y++) {
            for (uint8_t x = 0; x < DISPLAY_LIGHTS_WIDTH / 2; x++) {
                swap(&framebuffer.active[y][x], &framebuffer.active[y][DISPLAY_LIGHTS_WIDTH - x - 1]);
            }
        }
    }
    else if (degrees == -90) {
        // transpose x,y then reverse rows
        for (uint8_t y = 0; y < DISPLAY_LIGHTS_HEIGHT; y++) {
            for (uint8_t x = y+1; x < DISPLAY_LIGHTS_WIDTH; x++) {
                swap(&framebuffer.active[y][x], &framebuffer.active[x][y]);
            }
        }

        for (uint8_t y = 0; y < DISPLAY_LIGHTS_HEIGHT / 2; y++) {
            for (uint8_t x = 0; x < DISPLAY_LIGHTS_WIDTH; x++) {
                swap(&framebuffer.active[y][x], &framebuffer.active[DISPLAY_LIGHTS_HEIGHT - y - 1][x]);
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

    // ESP_LOG_BUFFER_HEXDUMP(TAG, framebuffer.active, DISPLAY_LIGHTS_TOTAL_AREA, ESP_LOG_INFO);

    lights_update_leds(framebuffer);
}


void patterns_castle() {
    ESP_LOGI(TAG, "Begin CASTLE pattern");

    framebuffer.pattern = &castle;
    memcpy(framebuffer.active, castle.data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(framebuffer.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_castle_step;
}


static void patterns_castle_step() {
    // Scroll the checkered display up and to the left every frame
    scroll(0, 1, true, 0);
}


void patterns_checkered() {
    ESP_LOGI(TAG, "Begin CHECKERED pattern");

    framebuffer.pattern = &checkered;
    memcpy(framebuffer.active, checkered.data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(framebuffer.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_checkered_step;
}


static void patterns_checkered_step() {
    // Scroll the checkered display up and to the left every frame
    scroll(-1, 0, true, 0);
}


void patterns_curtains() {
    ESP_LOGI(TAG, "Begin CURTAINS pattern");

    framebuffer.pattern = &curtains;
    memcpy(framebuffer.active, curtains.data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(framebuffer.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_curtains_step;
}


static void patterns_curtains_step() {
    // Scroll the curtains display down, add a row of 255 on top.
    scroll(-1, 0, false, 255);
}


void patterns_diamonds() {
    ESP_LOGI(TAG, "Begin DIAMONDS pattern");

    framebuffer.pattern = &diamonds;
    memcpy(framebuffer.active, diamonds.data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(framebuffer.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_diamonds_step;
}


static void patterns_diamonds_step() {
    invert();
}


void patterns_fireworks() {
    ESP_LOGI(TAG, "Begin FIREWORKS pattern");

    // TBD
    // framebuffer.pattern = &fireworks;
    // memcpy(framebuffer.active, fireworks.data, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_fireworks_step;
}


static void patterns_fireworks_step() {
    // TBD
}


void patterns_flash() {
    ESP_LOGI(TAG, "Begin FLASH pattern");

    framebuffer.pattern = NULL;
    memset(framebuffer.active, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(framebuffer.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_flash_step;
}


static void patterns_flash_step() {
    invert();
}


static void patterns_gol_step() {
    // Use Game of Life rules to animate the active pattern
    gol_next_generation(&framebuffer);
}


void patterns_gol_cross_2() {
    ESP_LOGI(TAG, "Begin GOL_CROSS_2 pattern");

    framebuffer.pattern = NULL;
    memcpy(framebuffer.grid, gol_cross_2, GOL_GRID_TOTAL_AREA);
    memset(framebuffer.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_gol_step;
}


void patterns_gol_four_blinkers_four_blocks() {
    ESP_LOGI(TAG, "Begin GOL_FOUR_BLINKERS_FOUR_BLOCKS pattern");

    framebuffer.pattern = NULL;
    memcpy(framebuffer.grid, gol_four_blinkers_four_blocks, GOL_GRID_TOTAL_AREA);
    memset(framebuffer.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_gol_step;
}


void patterns_gol_galaxy() {
    ESP_LOGI(TAG, "Begin GOL_GALAXY pattern");

    framebuffer.pattern = NULL;
    memcpy(framebuffer.grid, gol_galaxy, GOL_GRID_TOTAL_AREA);
    memset(framebuffer.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_gol_step;
}


void patterns_gol_pentadecathlon() {
    ESP_LOGI(TAG, "Begin GOL_PENTADECATHLON pattern");

    framebuffer.pattern = NULL;
    memcpy(framebuffer.grid, gol_pentadecathlon, GOL_GRID_TOTAL_AREA);
    memset(framebuffer.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_gol_step;
}


void patterns_gol_sprinkles() {
    ESP_LOGI(TAG, "Begin GOL_SPRINKLES pattern");

    // Sprinkles (like sparkles, but 1x3 and 3x1 Game Of Life beacons)
    framebuffer.pattern = NULL;
    memcpy(framebuffer.grid, gol_sprinkles, GOL_GRID_TOTAL_AREA);
    memset(framebuffer.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_gol_step;
}


void patterns_lines() {
    ESP_LOGI(TAG, "Begin LINES pattern");

    framebuffer.pattern = &lines;
    memcpy(framebuffer.active, lines.data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(framebuffer.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_lines_step;
}


static void patterns_lines_step() {
    scroll(0, 1, true, 0);
}


void patterns_question() {
    ESP_LOGI(TAG, "Begin QUESTION pattern");

    framebuffer.pattern = &question;
    memcpy(framebuffer.active, question.data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(framebuffer.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_question_step;
}


static void patterns_question_step() {
    invert();
}


void patterns_radar() {
    ESP_LOGI(TAG, "Begin RADAR pattern");

    framebuffer.pattern = &radar;
    memcpy(framebuffer.active, radar.data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(framebuffer.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_radar_step;
}


static void patterns_radar_step() {
    // TODO: Actually only rotate 15˚ every frame. Alternatively, draw the
    // lines dynamically and fill.
    rotate(-90);
}


void patterns_random() {
    ESP_LOGI(TAG, "Begin RANDOM pattern");

    framebuffer.pattern = NULL;
    memset(framebuffer.active, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(framebuffer.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_lines_step;
    patterns_random_step();
}


static void patterns_random_step() {
    esp_fill_random(framebuffer.active, DISPLAY_LIGHTS_TOTAL_AREA);
}


void patterns_siren() {
    ESP_LOGI(TAG, "Begin SIREN pattern");

    framebuffer.pattern = &siren;
    memcpy(framebuffer.active, siren.data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(framebuffer.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_siren_step;
}
    

static void patterns_siren_step() {
    // Flip vertically, then horizontally, then vertically, then horizontally (effectively rotate 90˚ each frame)
    rotate(90);
}


void patterns_spiral() {
    ESP_LOGI(TAG, "Begin SPIRAL pattern");

    framebuffer.pattern = &spiral;
    memcpy(framebuffer.active, spiral.data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(framebuffer.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_spiral_step;
}


static void patterns_spiral_step() {
    // XXX: Debug random crash in rotate()
    rotate(90);
}


void patterns_sweep() {
    ESP_LOGI(TAG, "Begin SWEEP pattern");

    framebuffer.pattern = &sweep;
    memcpy(framebuffer.active, sweep.data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(framebuffer.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_sweep_step;
}


static void patterns_sweep_step() {
    // Sweep from right to left
    scroll(1, 0, true, 0);
}


void patterns_swipe() {
    ESP_LOGI(TAG, "Begin SWIPE pattern");

    framebuffer.pattern = &swipe;
    memcpy(framebuffer.active, swipe.data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(framebuffer.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_swipe_step;
}


static void patterns_swipe_step() {
    // Sweep from right to left
    scroll(0, 1, true, 0);
}


void patterns_swoosh() {
    ESP_LOGI(TAG, "Begin SWOOP pattern");

    framebuffer.pattern = &swoosh;
    memcpy(framebuffer.active, swoosh.data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(framebuffer.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
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
    framebuffer.overlay[y][x] = star_border[border_length-1][2];

    for (i = 0; i < border_length - 1; i++ ) {
        y = star_border[i][0];
        x = star_border[i][1];
        star_border[i][2] = star_border[i+1][2];
        framebuffer.overlay[y][x] = star_border[i][2];
    }

    scroll(0, 1, true, 0);
}


void patterns_swoop() {
    ESP_LOGI(TAG, "Begin SWOOP pattern");

    // TBD
    // framebuffer.pattern = &swoop;
    // memcpy(framebuffer.active, swoop.data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(framebuffer.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_swoop_step;
}


static void patterns_swoop_step() {
    scroll(0, 1, true, 0);
}


void patterns_thump() {
    ESP_LOGI(TAG, "Begin WAVES pattern");

    // TBD
    // framebuffer.pattern = thump[0];
    // memcpy(framebuffer.active, thump[0].data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(framebuffer.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
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

    framebuffer.pattern = &waves;
    memcpy(framebuffer.active, waves.data, DISPLAY_LIGHTS_TOTAL_AREA);
    memset(framebuffer.overlay, 0, DISPLAY_LIGHTS_TOTAL_AREA);
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
