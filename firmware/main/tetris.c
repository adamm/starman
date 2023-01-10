#include <esp_log.h>
#include <string.h>
#include "config.h"
#include "display.h"
#include "random.h"

#include "tetris.h"

#define GAME_AREA_WIDTH 10
#define GAME_AREA_WIDTH_OFFSET 3 // (DISPLAY_LIGHTS_WIDTH - GAME_AREA_WIDTH) / 2

#define GAME_AREA_HEIGHT 12
#define GAME_AREA_HEIGHT_OFFSET 1

static const char *TAG = "starman-tetris";

// Game play area
static uint8_t game_area[GAME_AREA_HEIGHT][GAME_AREA_WIDTH] = {0};
static display_t* display;

// active block is the block controlled by the user
static game_block_t* active_block = NULL;


static void display_game_area(void) {
    //memset(&(display->overlay), 0, sizeof(display->overlay));

    // // Draw game area boundary
    // for (uint8_t y = 0; y < GAME_AREA_HEIGHT+GAME_AREA_HEIGHT_OFFSET; y++) {
    //     display->overlay[y][GAME_AREA_WIDTH_OFFSET-1] = 0x50;
    //     display->overlay[y][GAME_AREA_WIDTH+GAME_AREA_WIDTH_OFFSET] = 0x50;
    // }

    // Draw placed blocks
    for (uint8_t y = 0; y < GAME_AREA_HEIGHT; y++) {
        for (uint8_t x = 0; x < GAME_AREA_WIDTH; x++) {
            display->overlay[y+GAME_AREA_HEIGHT_OFFSET][x+GAME_AREA_WIDTH_OFFSET] = (game_area[y][x] == 0 ? 1 : game_area[y][x]);
        }
    }

    // Draw active block
    for (uint8_t i = 0; i < 4; i++) {
        coords_t xy = active_block->block->pos[active_block->rot][i];
        xy.y += active_block->pos.y + GAME_AREA_HEIGHT_OFFSET;
        xy.x += active_block->pos.x + GAME_AREA_WIDTH_OFFSET;
        if (xy.y >= 0 && xy.y < DISPLAY_LIGHTS_HEIGHT) {
            // ESP_LOGI(TAG, "active block part (%d): (%d,%d) = %d", i, xy.y, xy.x, active_block->block->color);
            display->overlay[xy.y][xy.x] = active_block->block->color;
        }
    }

    display_update_leds(display);
}


static void place_block(void) {
    for (uint8_t i = 0; i < 4; i++) {
        coords_t xy = active_block->block->pos[active_block->rot][i];
        xy.x += active_block->pos.x;
        xy.y += active_block->pos.y;
        if (0 <= xy.y && xy.y < GAME_AREA_HEIGHT &&
            0 <= xy.x && xy.x < GAME_AREA_WIDTH) {
            ESP_LOGW(TAG, "Setting pos (%d,%d) as %d", xy.y, xy.x, active_block->block->color);
            game_area[xy.y][xy.x] = active_block->block->color;
        }
    }
}


static game_block_t* new_block(void) {
    game_block_t* block = calloc(1, sizeof(game_block_t));
    uint8_t pick_a_block = (uint8_t)random_value_within_int(TYPE_TOTAL_COUNT);

    if (active_block != NULL) {
        active_block->next = (game_block_t*)block;
        block->prev = (game_block_t*)active_block;
    }
    active_block = block;

    block->pos.x = GAME_AREA_WIDTH / 2;
    block->pos.y = 0;
    block->block = (block_t*)&(blocks[pick_a_block]);
    block->rot = ROT_up;

    // Square(#0) and line (#1) blocks should start one position to the left, so
    // that arrive centred on the play area.
    if (pick_a_block < 3)
        block->pos.x--;

    // Drop in a random position.  Temporary measure until the user can control the blocks.
    block->pos.x += (int8_t)(random_value_within_int(7))-3;

    return block;
}


static bool active_block_can_move(coords_t direction) {
    coords_t xy;
    bool can_move = true;

    for (uint8_t i = 0; i < 4; i++) {
        xy = active_block->block->pos[active_block->rot][i];
        if (active_block->pos.x + xy.x + direction.x >= GAME_AREA_WIDTH ||
            active_block->pos.x + xy.x + direction.x < 0 ||
            active_block->pos.y + xy.y + direction.y >= GAME_AREA_HEIGHT)
            can_move = false;
        else if (game_area[active_block->pos.y + xy.y + direction.y][active_block->pos.x + xy.x + direction.x] != 0)
            can_move = false;
    }

    return can_move;
}

#define DOWN  (coords_t){1, 0}
#define LEFT  (coords_t){0,-1}
#define RIGHT (coords_t){0, 1}

void tetris_user_move_left(void) {
    if (active_block_can_move(LEFT)) {
        active_block->pos.x--;
        // Refresh the display with the game_area and active_block
        display_game_area();
    }
}


void tetris_user_move_right(void) {
    if (active_block_can_move(RIGHT)) {
        active_block->pos.x--;
        // Refresh the display with the game_area and active_block
        display_game_area();
    }
}


void tetris_user_rotate_left(void) {
    switch(active_block->rot) {
        case ROT_left:
            active_block->rot = ROT_down;
            break;

        case ROT_down:
            active_block->rot = ROT_right;
            break;

        case ROT_right:
            active_block->rot = ROT_up;
            break;

        case ROT_up:
            active_block->rot = ROT_left;
            break;

        default:
            break;
    }

    if (!active_block_can_move(RIGHT))
        active_block->pos.x--;
    if (!active_block_can_move(LEFT))
        active_block->pos.x++;

    display_game_area();
}


void tetris_user_rotate_right(void) {
    switch(active_block->rot) {
        case ROT_left:
            active_block->rot = ROT_up;
            break;

        case ROT_down:
            active_block->rot = ROT_left;
            break;

        case ROT_right:
            active_block->rot = ROT_down;
            break;

        case ROT_up:
            active_block->rot = ROT_right;
            break;

        default:
            break;
    }

    if (!active_block_can_move(RIGHT))
        active_block->pos.x--;
    if (!active_block_can_move(LEFT))
        active_block->pos.x++;

    display_game_area();
}



bool tetris_step_game(void) {
    bool game_over = false;

    if (active_block_can_move(DOWN)) {
        active_block->pos.y++;
    }
    else {
        // Save the block to game_area
        place_block();

        // Spawn a new block
        new_block();

        // TODO: Detect if we spawn a new block in the empty space available.
        // set game_over = true;
    }

    display_game_area();

    if (game_over) {
        free(active_block); // XXX: and all other blocks created!
    }

    return game_over;
}


void tetris_init(display_t* patterns_display) {
    ESP_LOGI(TAG, "tetris_init() begin");

    // Get a pointer to the patterns display struct.  This way the game can
    // update display->overlay, and patterns can update display->background
    display = patterns_display;
    memset(&game_area, 0, sizeof(game_area));

    new_block();
}

