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

#ifndef DISPLAY_H
#define DISPLAY_H

#include "config.h"
#include "patterns.h"

struct display {
    uint8_t background[DISPLAY_LIGHTS_HEIGHT][DISPLAY_LIGHTS_WIDTH];
    uint8_t overlay[DISPLAY_LIGHTS_HEIGHT][DISPLAY_LIGHTS_WIDTH];
    uint8_t** text;
    uint16_t text_height;
    uint16_t text_width;
    uint8_t grid[GOL_GRID_HEIGHT][GOL_GRID_WIDTH];
    pattern_t const * pattern;
    uint16_t pattern_x;
    uint16_t pattern_y;
} typedef display_t;

static const uint8_t LED_LUT[DISPLAY_LIGHTS_HEIGHT][DISPLAY_LIGHTS_WIDTH] = {
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0, 114, 127,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0, 115, 113, 128, 126,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0, 116, 120, 121, 125,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0, 117, 118, 119, 122, 123, 124,   0,   0,   0,   0,   0 },
    {   9,  10,   6,   7,   8, 139, 138, 137, 136, 135, 134, 105, 106, 107, 103, 104 },
    {  13,  12,  11,   5,   4, 140,   0, 144, 129,   0, 132, 109, 108, 102, 101, 100 },
    {   0,  14,  16,   1,   3, 141,   0, 143, 130,   0, 133, 110, 112,  97,  99,   0 },
    {   0,   0,  15,   2,  30,  31,   0, 142, 131,   0,  81,  82, 111,  98,   0,   0 },
    {   0,   0,   0,  18,  17,  32,  29,  28,  85,  84,  83,  96,  95,   0,   0,   0 },
    {   0,   0,  19,  20,  24,  25,  26,  27,  86,  87,  88,  89,  93,  94,   0,   0 },
    {   0,   0,  23,  22,  21,  51,  50,  49,  64,  63,  62,  92,  91,  90,   0,   0 },
    {   0,  44,  45,  47,  46,  53,  52,  56,  57,  61,  60,  67,  66,  79,  78,   0 },
    {   0,  34,  33,  48,  43,  54,  55,   0,   0,  58,  59,  70,  65,  80,  77,   0 },
    {  38,  39,  40,  41,  42,   0,   0,   0,   0,   0,   0,  71,  72,  73,  74,  75 },
    {  37,  36,  35,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  69,  68,  76 },
};

void display_update_leds(display_t*);
void display_update_leds_raw(uint8_t*);
void display_reset(display_t*);
uint8_t display_get_brightness();
void display_set_brightness(uint8_t);
void display_init(void);

#endif