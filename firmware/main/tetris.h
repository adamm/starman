#pragma once

// What is our coordinate domain?
typedef struct {
    int8_t x;
    int8_t y;
} coords_t;

// What types of blocks are there?
enum TYPE {
    TYPE_square = 0,
    TYPE_line,
    TYPE_tee,
    TYPE_seven,
    TYPE_jay,
};

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

// All possible block types, colours, and sizes
const block_t blocks[] = {
    {
        TYPE_square,
        0x40,
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
        TYPE_line,
        0x80,
        {
            {           {-1, 0 },
                        { 0, 0 },
                        { 1, 0 },
                        { 2, 0 } },

            { { 0,-1 }, { 0, 0 }, { 0, 1 }, { 0, 2 } },

            {           {-1, 0 },
                        { 0, 0 },
                        { 1, 0 },
                        { 2, 0 } },

            { { 0,-1 }, { 0, 0 }, { 0, 1 }, { 0, 2 } },
        }
    }, {
        TYPE_tee,
        0xA0,
        {
            { { 0,-1 }, { 0, 0 }, { 0, 1 },
                        { 1, 0 } },

            {           {-1, 0 },
              { 0,-1 }, { 0, 0 },
                        { 1, 0 } },

            {           {-1, 0 },
              { 0,-1 }, { 0, 0 }, { 0, 1 } },

            {           {-1, 0 },
                        { 0, 0 }, { 0, 1 },
                        { 1, 0 } },
        }
    }, {
        TYPE_seven,
        0xC0,
        {
            { { 0,-1 }, { 0, 0 },
                        { 1, 0 },
                        { 2, 0 } },

            {           {-1, 0 },
    { 0,-2 }, { 0,-1 }, { 0, 0 } },

            {           {-2, 0 },
                        {-1, 0 },
                        { 0, 0 }, { 0, 1 } },

            {           { 0, 0 }, { 0, 1 }, { 0, 2 },
                        {-1, 0 } },
        }
    }, {
        TYPE_jay,
        0xF0,
        {
            {           {-2, 0 },
                        {-1, 0 },
              { 0,-1 }, { 0, 0 } },

            {           {-1, 0 },
                        { 0, 0 }, { 0, 1 }, { 0, 2 } },

            {           { 0, 0 }, { 0, 1 },
                        { 1, 0 },
                        { 2, 0 } },

  { { 0,-2 }, { 0,-1 }, { 0, 0 },
                        { 1, 0 } },
        }
    }
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
void tetris_game_step(void);
void tetris_init(void);
