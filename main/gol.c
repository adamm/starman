#include <esp_log.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "gol.h"
#include "patterns.h"
#include "patterns_gol.h"

// An implementation of Conway's Game of Life to apply various oscillators as starman patterns


static uint8_t count_live_neighbours(int8_t grid[GOL_GRID_HEIGHT][GOL_GRID_WIDTH], int8_t y, int8_t x) {
    int8_t i, j, count = 0;
    for(i = y-1; i <= y+1; i++){
        for(j = x-1; j <= x+1; j++){
            if((i== y && j == x) || (i < 0 || j < 0) || ( i >= GOL_GRID_HEIGHT || j >= GOL_GRID_WIDTH)) {
                continue;
            }
            if(grid[i][j] == 1){
                count++;
            }
        }
    }
    return count;
}



void gol_next_generation(pattern_t *pattern) {
    int8_t y, x;
    int8_t this_gen[GOL_GRID_HEIGHT][GOL_GRID_WIDTH] = { 0 };
    int8_t next_gen[GOL_GRID_HEIGHT][GOL_GRID_WIDTH] = { 0 };

    for (y = 0; y < GOL_GRID_HEIGHT; y++) {
        for (x = 0; x < GOL_GRID_WIDTH; x++) {
            if (pattern->grid[y][x] > 0)
                this_gen[y][x] = 1;
        }
    }

    for (y = 0; y < GOL_GRID_HEIGHT; y++) {
        for (x = 0; x < GOL_GRID_WIDTH; x++) {
            int8_t count = count_live_neighbours(this_gen, y, x);
            if (this_gen[y][x] == 1 && (count == 2 || count == 3)) {
                next_gen[y][x] = 1;
            }
            else if (this_gen[y][x] == 0 && count == 3) {
                next_gen[y][x] = 1;
            }
            else {
                next_gen[y][x] = 0;
            }
        }
    }
 
    memset(pattern->active, 1, DISPLAY_LIGHTS_TOTAL_AREA);
    for (y = 0; y < DISPLAY_LIGHTS_HEIGHT; y++) {
        for (x = 0; x < DISPLAY_LIGHTS_WIDTH; x++) {
            if (next_gen[y+GOL_GRID_OFFSET_Y][x+GOL_GRID_OFFSET_X] > 0)
                pattern->active[y][x] = 255;
        }
    }

    memcpy(pattern->grid, next_gen, GOL_GRID_TOTAL_AREA);
}