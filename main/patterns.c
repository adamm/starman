#include <esp_log.h>
#include <esp_random.h>
#include <string.h>

#include "config.h"
#include "gol.h"
#include "lights.h"
#include "patterns.h"

static const char *TAG = "starman-patterns";

static void (*callback_func)(void) = NULL;
static pattern_t framebuffer;

static void patterns_game_of_life_step();
static void patterns_checkered_step();
static void patterns_curtains_step();
static void patterns_diamonds_step();
static void patterns_fireworks_step();
static void patterns_flash_step();
static void patterns_lines_step();
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


static void scroll(int8_t rows, int8_t cols, bool copy, int8_t fill) {
    if (rows) {
        uint8_t temp[DISPLAY_LIGHTS_WIDTH] = {0};
        memcpy(temp, framebuffer.active[0], DISPLAY_LIGHTS_WIDTH);
        for (uint8_t y = 0; y < DISPLAY_LIGHTS_HEIGHT - 1; y++) {
            memcpy(framebuffer.active[y], framebuffer.active[y + 1], DISPLAY_LIGHTS_WIDTH);
        }
        memcpy(framebuffer.active[DISPLAY_LIGHTS_HEIGHT - 1], temp, DISPLAY_LIGHTS_WIDTH);
    }
    if (cols) {
        uint8_t temp = 0;
        for (uint8_t y = 0; y < DISPLAY_LIGHTS_HEIGHT; y++) {
            temp = framebuffer.active[y][0];
            for (uint8_t x = 0; x < DISPLAY_LIGHTS_WIDTH - 1; x++) {
                framebuffer.active[y][x] = framebuffer.active[y][x + 1];
            }
            framebuffer.active[y][DISPLAY_LIGHTS_WIDTH - 1] = temp;
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



static void patterns_game_of_life_step() {
    // Use Game of Life rules to animate the active pattern
    gol_next_generation(&framebuffer);
}




void patterns_checkered() {
    ESP_LOGI(TAG, "Begin CHECKERED pattern");

    memcpy(framebuffer.active, checkered, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_checkered_step;
}


static void patterns_checkered_step() {
    // Scroll the checkered display up and to the left every frame
    scroll(-1, 0, true, 0);
}


void patterns_curtains() {
    ESP_LOGI(TAG, "Begin CURTAINS pattern");

    memcpy(framebuffer.active, curtains, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_curtains_step;
}


static void patterns_curtains_step() {
    // Scroll the curtains display down, add a row of 255 on top.
    scroll(-1, 0, false, 255);
}


void patterns_diamonds() {
    ESP_LOGI(TAG, "Begin DIAMONDS pattern");

    // TBD
    callback_func = patterns_diamonds_step;
}


static void patterns_diamonds_step() {
    // TBD
}


void patterns_fireworks() {
    ESP_LOGI(TAG, "Begin FIREWORKS pattern");

    // TBD
    callback_func = patterns_fireworks_step;
}


static void patterns_fireworks_step() {
    // TBD
}


void patterns_flash() {
    ESP_LOGI(TAG, "Begin FLASH pattern");

    memset(framebuffer.active, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_flash_step;
}


static void patterns_flash_step() {
    invert();
}


void patterns_lines() {
    ESP_LOGI(TAG, "Begin LINES pattern");

    memcpy(framebuffer.active, lines, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_lines_step;
}


static void patterns_lines_step() {
    scroll(0, 1, true, 0);
}


void patterns_radar() {
    ESP_LOGI(TAG, "Begin RADAR pattern");

    memcpy(framebuffer.active, radar, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_radar_step;
}


static void patterns_radar_step() {
    // TODO: Actually only rotate 15˚ every frame. Alternatively, draw the
    // lines dynamically and fill.
    rotate(-90);
}


void patterns_random() {
    ESP_LOGI(TAG, "Begin RANDOM pattern");

    memset(framebuffer.active, 0, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_lines_step;
    patterns_random_step();
}


static void patterns_random_step() {
    esp_fill_random(framebuffer.active, DISPLAY_LIGHTS_TOTAL_AREA);
}


void patterns_siren() {
    ESP_LOGI(TAG, "Begin SIREN pattern");

    memcpy(framebuffer.active, radar, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_siren_step;
}
    

static void patterns_siren_step() {
    // Flip vertically, then horizontally, then vertically, then horizontally (effectively rotate 90˚ each frame)
    rotate(90);
}


void patterns_sprinkles(){
    ESP_LOGI(TAG, "Begin SPRINKLES pattern");

    // Sprinkles (like sparkles, but 1x3 and 3x1 Game Of Life beacons)
    memcpy(framebuffer.active, gol_sprinkles, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_game_of_life_step;
}


void patterns_spiral() {
    ESP_LOGI(TAG, "Begin SPIRAL pattern");

    memcpy(framebuffer.active, spiral, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_spiral_step;
}


static void patterns_spiral_step() {
    // XXX: Debug random crash in rotate()
    rotate(90);
}

void patterns_sweep() {
    ESP_LOGI(TAG, "Begin SWEEP pattern");

    memcpy(framebuffer.active, sweep, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_sweep_step;
}


static void patterns_sweep_step() {
    // Sweep from right to left
    scroll(0, 1, true, 0);
}


void patterns_swipe() {
    ESP_LOGI(TAG, "Begin SWIPE pattern");

    memcpy(framebuffer.active, swipe, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_swipe_step;
}


static void patterns_swipe_step() {
    // Sweep from right to left
    scroll(0, 1, true, 0);
}


void patterns_swoosh() {
    ESP_LOGI(TAG, "Begin SWOOP pattern");

    memcpy(framebuffer.active, swoosh, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_swoosh_step;
}
    

static void patterns_swoosh_step() {
    scroll(0, 1, true, 0);
}


void patterns_swoop() {
    ESP_LOGI(TAG, "Begin SWOOP pattern");

    // TBD
    callback_func = patterns_swoop_step;
}


static void patterns_swoop_step() {
    scroll(0, 1, true, 0);
}


void patterns_thump() {
    ESP_LOGI(TAG, "Begin WAVES pattern");

    memcpy(framebuffer.active, thump[0], DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_thump_step;
}


static void patterns_thump_step() {
    // TBD:  Jump right to largest "thump" image at instrument play, decay to smallest "thump" image while not playing
    // You know, like a drum.
}


void patterns_waves() {
    ESP_LOGI(TAG, "Begin WAVES pattern");

    memcpy(framebuffer.active, waves, DISPLAY_LIGHTS_TOTAL_AREA);
    callback_func = patterns_waves_step;
}


static void patterns_waves_step() {
    static int8_t x = 0;
    static bool down = true;
    // The waves pattern scrolls from left to right, and simultaneously slides up and down

    if (x < -3 || x > 3)
        down = !down;

    if (down) {
        scroll(1, 1, false, 0);
        x++;
    }
    else {
        scroll(-1, 1, false, 255);
        x--;
    }
}
