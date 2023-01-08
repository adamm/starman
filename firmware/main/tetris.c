#include <esp_log.h>
#include <string.h>
#include "config.h"
#include "display.h"

#include "tetris.h"

#define GAME_AREA_WIDTH 10
#define GAME_AREA_WIDTH_OFFSET 3 // (DISPLAY_LIGHTS_WIDTH - GAME_AREA_WIDTH) / 2

#define GAME_AREA_HEIGHT DISPLAY_LIGHTS_HEIGHT
#define GAME_AREA_HEIGHT_OFFSET 0

// Game play area
static uint8_t game_area[GAME_AREA_HEIGHT][GAME_AREA_WIDTH] = {0};
static display_t display;

// active block is the block controlled by the user
static game_block_t* active_block = NULL;


static void display_game_area(void) {
    memset(&(display.overlay), 0, sizeof(display.overlay));

    // Draw game area boundary
    for (uint8_t y = 0; y < GAME_AREA_HEIGHT; y++) {
        display.overlay[y][GAME_AREA_WIDTH_OFFSET-1] = 0x50;
        display.overlay[y][GAME_AREA_WIDTH+GAME_AREA_WIDTH_OFFSET+1] = 0x50;
    }

    // Draw active block
    for (uint8_t i = 0; i < 4; i++) {
        coords_t xy = active_block->block->pos[active_block->rot][i];
        display.overlay[xy.y+GAME_AREA_HEIGHT_OFFSET][xy.x+GAME_AREA_WIDTH_OFFSET]
                = active_block->block->color;
    }

    // Draw placed blocks
    for (uint8_t y = 0; y < GAME_AREA_HEIGHT; y++) {
        for (uint8_t x = 0; x < GAME_AREA_WIDTH; y++) {
            display.overlay[y+GAME_AREA_HEIGHT_OFFSET][x+GAME_AREA_WIDTH_OFFSET] = game_area[y][x];
        }
    }

    display_update_leds(&display);
}


static void place_block(void) {
    for (uint8_t i = 0; i < 4; i++) {
        coords_t xy = active_block->block->pos[active_block->rot][i];
        game_area[xy.y][xy.x] = active_block->block->color;
    }
}


static game_block_t* new_block(void) {
    game_block_t* block = calloc(1, sizeof(game_block_t));

    if (active_block != NULL) {
        active_block->next = (game_block_t*)block;
        block->prev = (game_block_t*)active_block;
    }
    active_block = block;

    block->pos.x = GAME_AREA_WIDTH / 2;
    block->pos.y = 0;
    block->block = (block_t*)&(blocks[0]);
    block->rot = ROT_up;

    return block;
}


static bool active_block_can_move(coords_t direction) {
    coords_t xy;
    bool can_move = true;

    for (uint8_t i = 0; i < 4; i++) {
        xy = active_block->block->pos[active_block->rot][i];
        if (active_block->pos.x + xy.x + direction.x >= GAME_AREA_HEIGHT ||
            active_block->pos.y + xy.y + direction.y >= GAME_AREA_WIDTH ||
            active_block->pos.y + xy.y + direction.y < 0)
            can_move = false;
        else if (game_area[active_block->pos.x + xy.x + direction.x][active_block->pos.y + xy.y + direction.y] != 0)
            can_move = false;
    }

    return can_move;
}

#define DOWN  (coords_t){1, 0}
#define LEFT  (coords_t){0,-1}
#define RIGHT (coords_t){0, 1}

void tetris_user_move_left(void) {
    if (active_block_can_move(LEFT)) {
        active_block->pos.y--;
        // Refresh the display with the game_area and active_block
        display_game_area();
    }
}


void tetris_user_move_right(void) {
    if (active_block_can_move(RIGHT)) {
        active_block->pos.y--;
        // Refresh the display with the game_area and active_block
        display_game_area();
    }
}


void tetris_game_step(void) {
    if (active_block_can_move(DOWN)) {
        active_block->pos.x++;
    }
    else {
        // Save the block to game_area
        place_block();

        // Queue a new block
        new_block();
    }

    display_game_area();
}


void tetris_init(void) {
    memset(&game_area, 0, sizeof(game_area));
    display_reset(&display);
    new_block();
}

