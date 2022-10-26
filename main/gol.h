#ifndef GOL_H
#define GOL_H

#include "patterns.h"

static const uint8_t gol_cross_2[DISPLAY_LIGHTS_HEIGHT][DISPLAY_LIGHTS_WIDTH] = {
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0, 255, 255, 255, 255,   0, 255, 255, 255, 255,   0,   0,   0,   0 },
    {   0,   0,   0, 255,   0,   0, 255,   0, 255,   0,   0, 255,   0,   0,   0,   0 },
    {   0, 255, 255, 255,   0,   0, 255, 255, 255,   0,   0, 255, 255, 255,   0,   0 },
    {   0, 255,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 255,   0,   0 },
    {   0, 255,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 255,   0,   0 },
    {   0, 255, 255, 255,   0,   0,   0,   0,   0,   0,   0, 255, 255, 255,   0,   0 },
    {   0,   0,   0, 255,   0,   0,   0,   0,   0,   0,   0, 255,   0,   0,   0,   0 },
    {   0, 255, 255, 255,   0,   0,   0,   0,   0,   0,   0, 255, 255, 255,   0,   0 },
    {   0, 255,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 255,   0,   0 },
    {   0, 255,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 255,   0,   0 },
    {   0, 255, 255, 255,   0,   0, 255, 255, 255,   0,   0, 255, 255, 255,   0,   0 },
    {   0,   0,   0, 255,   0,   0, 255,   0, 255,   0,   0, 255,   0,   0,   0,   0 },
    {   0,   0,   0, 255, 255, 255, 255,   0, 255, 255, 255, 255,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
};

static const uint8_t gol_galaxy[DISPLAY_LIGHTS_HEIGHT][DISPLAY_LIGHTS_WIDTH] = {
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0, 255, 255, 255, 255, 255, 255,   0, 255, 255,   0,   0,   0,   0 },
    {   0,   0,   0, 255, 255, 255, 255, 255, 255,   0, 255, 255,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 255, 255,   0,   0,   0,   0 },
    {   0,   0,   0, 255, 255,   0,   0,   0,   0,   0, 255, 255,   0,   0,   0,   0 },
    {   0,   0,   0, 255, 255,   0,   0,   0,   0,   0, 255, 255,   0,   0,   0,   0 },
    {   0,   0,   0, 255, 255,   0,   0,   0,   0,   0, 255, 255,   0,   0,   0,   0 },
    {   0,   0,   0, 255, 255,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0, 255, 255,   0, 255, 255, 255, 255, 255, 255,   0,   0,   0,   0 },
    {   0,   0,   0, 255, 255,   0, 255, 255, 255, 255, 255, 255,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
};
    
// FIXME: Replace the top 1x3 with the bottom of a By Flops.  This will allow the top of the star to flash
// in a 1x2, 1x2 alternating pattern.  (This will require a grid greater than the PCB size
static const uint8_t gol_sprinkles[DISPLAY_LIGHTS_HEIGHT][DISPLAY_LIGHTS_WIDTH] = {
    {   0,   0,   0,   0,   0,   0,   0, 255,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0, 255,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0, 255,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0, 255,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0, 255,   0,   0,   0, 255, 255, 255,   0,   0, 255,   0 },
    { 255, 255, 255,   0,   0, 255,   0,   0,   0,   0,   0,   0,   0,   0, 255,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 255,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 255,   0,   0,   0,   0 },
    {   0,   0,   0, 255, 255, 255,   0,   0,   0,   0,   0, 255,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 255,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0, 255,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0, 255,   0,   0,   0, 255, 255, 255,   0,   0, 255,   0 },
    { 255, 255, 255,   0,   0, 255,   0,   0,   0,   0,   0,   0,   0,   0, 255,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 255,   0 },
};

static const uint8_t gol_four_blinkers_four_blocks[DISPLAY_LIGHTS_HEIGHT][DISPLAY_LIGHTS_WIDTH] = {
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0, 255, 255, 255,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0, 255, 255,   0,   0, 255, 255,   0, 255,   0,   0,   0 },
    {   0,   0,   0,   0,   0, 255,   0,   0,   0,   0, 255,   0, 255,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 255,   0,   0,   0 },
    {   0,   0,   0, 255,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0, 255,   0, 255,   0,   0,   0,   0, 255,   0,   0,   0,   0,   0 },
    {   0,   0,   0, 255,   0, 255, 255,   0,   0, 255, 255,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0, 255, 255, 255,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
};

static const uint8_t gol_pentadecathlon[DISPLAY_LIGHTS_HEIGHT][DISPLAY_LIGHTS_WIDTH] = {
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0, 255,   0,   0,   0,   0, 255,   0,   0,   0,   0,   0 },
    {   0,   0,   0, 255, 255,   0, 255, 255, 255, 255,   0, 255, 255,   0,   0,   0 },
    {   0,   0,   0,   0,   0, 255,   0,   0,   0,   0, 255,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
};



void gol_next_generation(pattern_t* pattern);

#endif