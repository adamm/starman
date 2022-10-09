#include <esp_log.h>
#include <esp_random.h>
#include <string.h>

#include "patterns.h"

static const char *TAG = "starman-patterns";

static void (*callback_func)(void) = NULL;
static uint8_t framebuffer[PATTERN_HEIGHT][PATTERN_WIDTH] = {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};


static void patterns_checkered_step();
static void patterns_curtains_step();
static void patterns_diamonds_step();
static void patterns_fireworks_step();
static void patterns_flash_step();
static void patterns_lines_step();
static void patterns_radar_step();
static void patterns_random_step();
static void patterns_siren_step();
static void patterns_sparkle_step();
static void patterns_sprinkle_step();
static void patterns_spiral_step();
static void patterns_sweep_step();
static void patterns_swoosh_step();
static void patterns_swoop_step();
static void patterns_thump_step();
static void patterns_waves_step();


static void invert() {
    for (uint8_t y = 0; y < PATTERN_HEIGHT; y++) {
        for (uint8_t x = 0; x < PATTERN_WIDTH; x++) {
            framebuffer[y][x] += 255;
        }
    }
}


static void scroll(int8_t rows, int8_t cols, bool copy, int8_t fill) {
    for (uint8_t y = 0; y < PATTERN_HEIGHT; y++) {
        for (uint8_t x = 0; x < PATTERN_WIDTH; x++) {
        }
    }
}


static void rotate(int8_t degrees) {

    // TODO: For rotations not 90˚, implement https://en.wikipedia.org/wiki/Rotation_matrix

    if (degrees == 90) {
        uint8_t temp = 0;

        // Transpose x,y then reverse columns
        for (uint8_t y = 0; y < PATTERN_HEIGHT; y++) {
            for (uint8_t x = 0; x < PATTERN_WIDTH; x++) {
                temp = framebuffer[x][y];
                framebuffer[x][y] = framebuffer[y][x];
                framebuffer[y][x] = temp;
            }
        }

        for (uint8_t y = 0; y < PATTERN_HEIGHT; y++) {
            temp = framebuffer[y][0];
            for (uint8_t x = 0; x < PATTERN_WIDTH - 1; x++) {
                framebuffer[y][x] = framebuffer[y][x + 1];
            }
            framebuffer[y][PATTERN_WIDTH] = temp;
        }
    }
    else if (degrees == -90) {
        uint8_t temp[PATTERN_WIDTH];

        // transpose x,y then reverse rows
        for (uint8_t y = 0; y < PATTERN_HEIGHT; y++) {
            for (uint8_t x = 0; x < PATTERN_WIDTH; x++) {
                temp[0] = framebuffer[x][y];
                framebuffer[x][y] = framebuffer[y][x];
                framebuffer[y][x] = temp[0];
            }
        }

        for (uint8_t y = 0; y < PATTERN_HEIGHT; y++) {
            for (uint8_t x = 0; x < PATTERN_WIDTH; x++) {
                if (x == 0) {
                    temp[x] = framebuffer[y][x];
                }
                else if (x < PATTERN_WIDTH - 1) {
                    framebuffer[y-1][x] = framebuffer[y][x];
                }
                else {
                    framebuffer[y][x] = temp[x];
                }
            }
        }
    }
}


void patterns_step_sequence() {
    // Function is called each time the music note changes. 
    // Use this to imcrement the pattern sequence and update the lights.

    // TODO: Call pattern-specific increment function to update the framebuffer,
    // then send the framebuffer to the lights display
    if (callback_func != NULL) {
        callback_func();
    }
}


void patterns_checkered() {
    ESP_LOGI(TAG, "Begin CHECKERED pattern");

    memcpy(framebuffer, checkered, PATTERN_LENGTH);
    callback_func = patterns_checkered_step;
}


static void patterns_checkered_step() {
    // Scroll the checkered display up and to the left every frame
    scroll(-1, -1, true, 0);
}


void patterns_curtains() {
    ESP_LOGI(TAG, "Begin CURTAINS pattern");

    memcpy(framebuffer, curtains, PATTERN_LENGTH);
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

    memset(framebuffer, 0, PATTERN_LENGTH);
    callback_func = patterns_flash_step;
}


static void patterns_flash_step() {
    invert();
}


void patterns_lines() {
    ESP_LOGI(TAG, "Begin LINES pattern");

    // TBD
    callback_func = patterns_lines_step;
}


static void patterns_lines_step() {
    // TBD
}


void patterns_radar() {
    ESP_LOGI(TAG, "Begin RADAR pattern");

    memcpy(framebuffer, radar, PATTERN_LENGTH);
    callback_func = patterns_radar_step;
}


static void patterns_radar_step() {
    // Rotate 15˚ every frame
    rotate(15);
}


void patterns_random() {
    ESP_LOGI(TAG, "Begin RANDOM pattern");

    memset(framebuffer, 0, PATTERN_LENGTH);
    callback_func = patterns_lines_step;
    patterns_random_step();
}


static void patterns_random_step() {
    esp_fill_random(framebuffer, PATTERN_LENGTH);
}


void patterns_siren() {
    ESP_LOGI(TAG, "Begin SIREN pattern");

    memcpy(framebuffer, radar, PATTERN_LENGTH);
    callback_func = patterns_siren_step;
}
    

static void patterns_siren_step() {
    // Flip vertically, then horizontally, then vertically, then horizontally (effectively rotate 90˚ each frame)
    rotate(90);
}


void patterns_sparkle() {
    ESP_LOGI(TAG, "Begin SPARKLE pattern");

    // TBD
    callback_func = patterns_sparkle_step;
}


static void patterns_sparkle_step() {
    // Sparkle
}


void patterns_sprinkle(){
    ESP_LOGI(TAG, "Begin SPRINKLE pattern");

    // TBD
    callback_func = patterns_sprinkle_step;
}


static void patterns_sprinkle_step() {
    // Sprinkle (like sparkles, but 1x2 and 1x3 lines)
}


void patterns_spiral() {
    ESP_LOGI(TAG, "Begin SPIRAL pattern");

    memcpy(framebuffer, spiral, PATTERN_LENGTH);
    callback_func = patterns_spiral_step;
}


static void patterns_spiral_step() {
    rotate(90);
}

void patterns_sweep() {
    ESP_LOGI(TAG, "Begin SWEEP pattern");

    memcpy(framebuffer, sweep, PATTERN_LENGTH);
    callback_func = patterns_sweep_step;
}


static void patterns_sweep_step() {
    // Sweep from right to left
    scroll(0, 1, true, 0);
}


void patterns_swoosh() {
    ESP_LOGI(TAG, "Begin SWOOP pattern");

    memcpy(framebuffer, swoosh, PATTERN_LENGTH);
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

    memcpy(framebuffer, thump[0], PATTERN_LENGTH);
    callback_func = patterns_thump_step;
}


static void patterns_thump_step() {
    // TBD:  Jump right to largest "thump" image at instrument play, decay to smallest "thump" image while not playing
    // You know, like a drum.
}


void patterns_waves() {
    ESP_LOGI(TAG, "Begin WAVES pattern");

    memcpy(framebuffer, waves, PATTERN_LENGTH);
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
