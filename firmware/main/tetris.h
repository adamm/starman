#pragma once

#include "display.h"

// What is our coordinate domain?
typedef struct {
    int8_t y;
    int8_t x;
} coords_t;

// What types of blocks are there?
enum TYPE {
    TYPE_O = 0,
    TYPE_I,
    TYPE_T,
    TYPE_7,
    TYPE_J,
    TYPE_S,
    TYPE_Z,
};

#define TYPE_TOTAL_COUNT 7

// How can a block be positioned?
enum ROT {
    ROT_up = 0,
    ROT_right,
    ROT_down,
    ROT_left,
};

// What is a block, anyway?
typedef struct {
    enum TYPE type;
    uint8_t color;
    coords_t pos[4][4];
} block_t;

// All possible block types, greyscale, and orientations
static const block_t blocks[] = {
    {
        TYPE_O,
        0x1F,
        {
            { { 0, 0 }, { 0, 1 },
              { 1, 0 }, { 1, 1 } },

            { { 0, 0 }, { 0, 1 },
              { 1, 0 }, { 1, 1 } },

            { { 0, 0 }, { 0, 1 },
              { 1, 0 }, { 1, 1 } },

            { { 0, 0 }, { 0, 1 },
              { 1, 0 }, { 1, 1 } },
        }
    }, {
        TYPE_I,
        0x3F,
        {
            { { 0,-1 }, { 0, 0 }, { 0, 1 }, { 0, 2 } },

            {           {-1, 0 },
                        { 0, 0 },
                        { 1, 0 },
                        { 2, 0 } },

            { { 1,-1 }, { 1, 0 }, { 1, 1 }, { 1, 2 } },

            {                     {-1, 1 },
                                  { 0, 1 },
                                  { 1, 1 },
                                  { 2, 1 } },
        }
    }, {
        TYPE_T,
        0xDF,
        {
            {           {-1, 0 },
              { 0,-1 }, { 0, 0 }, { 0, 1 } },

            {           {-1, 0 },
                        { 0, 0 }, { 0, 1 },
                        { 1, 0 } },

            { { 0,-1 }, { 0, 0 }, { 0, 1 },
                        { 1, 0 } },

            {           {-1, 0 },
              { 0,-1 }, { 0, 0 },
                        { 1, 0 } },
        }
    }, {
        TYPE_7,
        0x6F,
        {
            {                     {-1, 1 },
              { 0,-1 }, { 0, 0 }, { 0, 1 } },

            {           {-1, 0 },
                        { 0, 0 },
                        {-1, 0 }, {-1, 1 } },

            { { 0,-1 }, { 0, 0 }, { 0, 1 },
              {-1,-1 } },

            { {-1,-1 }, {-1, 0 },
                        { 0, 0 },
                        { 2, 0 } },
        }
    }, {
        TYPE_J,
        0xBF,
        {
            { {-1,-1 },
              { 0,-1 }, { 0, 0 }, { 0, 1 } },

            {           {-1, 0 }, {-1, 1 },
                        { 0, 0 },
                        { 1, 0 } },

            { { 0,-1 }, { 0, 0 }, { 0, 1 },
                                  { 1, 1 } },

            {           {-1, 0 },
                        { 0, 0 },
              { 1,-1 }, { 1, 0 } },
        }
    }, {
        TYPE_S,
        0x9F,
        {
            {           {-1, 0 }, {-1, 1 },
              { 0,-1 }, { 0, 0 } },

            {           {-1, 0 },
                        { 0, 0 }, { 0, 1 },
                                  { 1, 1 } },

            {           { 0, 0 }, { 0, 1 },
              { 1,-1 }, { 1, 0 } },

            { {-1,-1 },
              { 0,-1 }, { 0, 0 },
                        { 1, 0 } },
        }
    }, {
        TYPE_Z,
        0xFF,
        {
            { {-1,-1 }, {-1, 0 },
                        { 0, 0 }, { 0, 1 } },

            {                     {-1, 1 },
                        { 0, 0 }, { 0, 1 },
                        { 1, 0 } },

            { { 0,-1 }, { 0, 0 },
                        { 1, 0 }, { 1, 1 } },

            {           {-1, 0 },
              { 0,-1 }, { 0, 0 },
              { 1,-1 } },
        }
    },
};

// Store the position for all active blocks in the game
// including a linked-list of past blocks
typedef struct {
    block_t* block;
    enum ROT rot;
    coords_t pos;
    void* next;
    void* prev;
} game_block_t;


void tetris_user_move_left(void);
void tetris_user_move_right(void);
void tetris_user_rotate_left(void);
void tetris_user_rotate_right(void);
bool tetris_step_game(void);
void tetris_stop(void);
void tetris_start(void);
void tetris_init(display_t* display);
