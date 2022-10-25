#ifndef PATTERNS_H
#define PATTERNS_H

#include "config.h"

// Techncially the star is 16 LEDs wide and 15 LEDs tall, and is only 144 LEDs total. 
// In order to make the math for rotation and flipping easier, we assume it's 16x16 square.
// Thus, the top row of the pattern will be ignored when updating the lights.
// 
// In other words, if XX is a LED, data outside of XX will be thrown away:
// 
//      1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16
//  1  __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
//  2  __ __ __ __ __ __ __ XX XX __ __ __ __ __ __ __
//  3  __ __ __ __ __ __ XX XX XX XX __ __ __ __ __ __
//  4  __ __ __ __ __ __ XX XX XX XX __ __ __ __ __ __
//  5  __ __ __ __ __ XX XX XX XX XX XX __ __ __ __ __
//  6  XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX
//  7  XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX
//  8  __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX __
//  9  __ __ XX XX XX XX XX XX XX XX XX XX XX XX __ __
// 10  __ __ __ XX XX XX XX XX XX XX XX XX XX __ __ __
// 11  __ __ XX XX XX XX XX XX XX XX XX XX XX XX __ __
// 12  __ __ XX XX XX XX XX XX XX XX XX XX XX XX __ __
// 13  __ XX XX XX XX XX XX XX XX XX XX XX XX XX XX __
// 14  __ XX XX XX XX XX XX __ __ XX XX XX XX XX XX __
// 15  XX XX XX XX XX __ __ __ __ __ __ XX XX XX XX XX
// 16  XX XX XX __ __ __ __ __ __ __ __ __ __ XX XX XX
//

struct pattern {
    uint8_t active[DISPLAY_LIGHTS_HEIGHT][DISPLAY_LIGHTS_WIDTH];
} typedef pattern_t;

static const uint8_t checkered[DISPLAY_LIGHTS_HEIGHT][DISPLAY_LIGHTS_WIDTH] = {
    { 255, 200,   0,   0, 255, 200,   0,   0, 255, 200,   0,   0, 255, 200,   0,   0 },
    { 200, 100,   0,   0, 200, 100,   0,   0, 200, 100,   0,   0, 200, 100,   0,   0 },
    {   0,   0, 255, 200,   0,   0, 255, 200,   0,   0, 255, 200,   0,   0, 255, 200 },
    {   0,   0, 200, 100,   0,   0, 200, 100,   0,   0, 200, 100,   0,   0, 200, 100 },
    { 255, 200,   0,   0, 255, 200,   0,   0, 255, 200,   0,   0, 255, 200,   0,   0 },
    { 200, 100,   0,   0, 200, 100,   0,   0, 200, 100,   0,   0, 200, 100,   0,   0 },
    {   0,   0, 255, 200,   0,   0, 255, 200,   0,   0, 255, 200,   0,   0, 255, 200 },
    {   0,   0, 200, 100,   0,   0, 200, 100,   0,   0, 200, 100,   0,   0, 200, 100 },
    { 255, 200,   0,   0, 255, 200,   0,   0, 255, 200,   0,   0, 255, 200,   0,   0 },
    { 200, 100,   0,   0, 200, 100,   0,   0, 200, 100,   0,   0, 200, 100,   0,   0 },
    {   0,   0, 255, 200,   0,   0, 255, 200,   0,   0, 255, 200,   0,   0, 255, 200 },
    {   0,   0, 200, 100,   0,   0, 200, 100,   0,   0, 200, 100,   0,   0, 200, 100 },
    { 255, 200,   0,   0, 255, 200,   0,   0, 255, 200,   0,   0, 255, 200,   0,   0 },
    { 200, 100,   0,   0, 200, 100,   0,   0, 200, 100,   0,   0, 200, 100,   0,   0 },
    {   0,   0, 255, 200,   0,   0, 255, 200,   0,   0, 255, 200,   0,   0, 255, 200 },
    {   0,   0, 200, 100,   0,   0, 200, 100,   0,   0, 200, 100,   0,   0, 200, 100 },
};

static const uint8_t curtains[DISPLAY_LIGHTS_HEIGHT][DISPLAY_LIGHTS_WIDTH] = {
    { 242, 255, 255, 242, 255, 255, 242, 255, 255, 242, 255, 255, 242, 255, 255, 242 },
    { 126, 255, 255, 126, 255, 255, 126, 255, 255, 126, 255, 255, 126, 255, 255, 126 },
    {  42, 143, 143,  42, 143, 143,  42, 143, 143,  42, 143, 143,  42, 143, 143,  42 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
};

static const uint8_t radar[DISPLAY_LIGHTS_HEIGHT][DISPLAY_LIGHTS_WIDTH] = {
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,  72, 162, 180, 189, 192, 198, 198, 198, 207 },
    {   0,   0,   0,   0,   0,   0,   3,  81, 147, 183, 201, 213, 222, 225, 231, 234 },
    {   0,   0,   0,   0,   0,   0,  24,  72, 120, 153, 177, 189, 201, 210, 213, 219 },
    {   0,   0,   0,   0,   0,   3,  36,  69, 102, 129, 153, 171, 183, 192, 201, 204 },
    {   0,   0,   0,   0,   0,  18,  42,  69,  96, 117, 138, 153, 168, 180, 186, 195 },
    {   0,   0,   0,   0,   3,  24,  45,  69,  90, 108, 129, 141, 156, 165, 174, 183 },
    {   0,   0,   0,   0,  12,  30,  48,  69,  87, 105, 117, 132, 144, 156, 165, 174 },
    {   0,   0,   0,   6,  21,  36,  51,  66,  81,  99, 111, 126, 138, 147, 159, 165 },
    {   0,   0,   3,  10,  24,  38,  50,  65,  85,  95, 111, 126, 138, 147, 159, 165 },
};

static const uint8_t siren[DISPLAY_LIGHTS_HEIGHT][DISPLAY_LIGHTS_WIDTH] = {
    {   0,   0,   0,   0,   0,   0,   0,   0, 240, 255, 255, 255, 255, 255, 255, 255 },
    {   0,   0,   0,   0,   0,   0,   0,   0, 240, 255, 255, 255, 255, 255, 255, 255 },
    {   0,   0,   0,   0,   0,   0,   0,   0, 240, 255, 255, 255, 255, 255, 255, 255 },
    {   0,   0,   0,   0,   0,   0,   0,   0, 240, 255, 255, 255, 255, 255, 255, 255 },
    {   0,   0,   0,   0,   0,   0,   0,   0, 240, 255, 255, 255, 255, 255, 255, 255 },
    {   0,   0,   0,   0,   0,   0,   0,   0, 240, 255, 255, 255, 255, 255, 255, 255 },
    {   0,   0,   0,   0,   0,   0, 173, 240, 240, 255, 255, 255, 255, 255, 255, 255 },
    {   0,   0,   0,   0,   0,   0, 240, 255, 255, 240, 255, 255, 255, 255, 255, 255 },
    {   0,   0,   0,   0,   0,   0, 240, 255, 255, 240, 240, 240, 240, 240, 240, 240 },
    {   0,   0,   0,   0,   0,   0, 173, 240, 240, 173,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
};

static const uint8_t spiral[DISPLAY_LIGHTS_HEIGHT][DISPLAY_LIGHTS_WIDTH] = {
    { 180,  47,  77, 116, 150, 179, 200, 216, 221, 220, 204, 185, 160, 127,  91,  50 },
    {  50,  77, 125, 168, 205, 183, 109,  56,  48,  52,  88, 158, 214, 181, 141,  96 },
    {  64, 117, 169, 210, 115,  42,  68,  86,  96,  88,  75,  49,  69, 197, 187, 141 },
    {  96, 153, 207, 112,  51,  94, 127, 150, 154, 153, 135, 106,  68,  47, 200, 180 },
    { 123, 182, 176,  42,  95, 142, 184, 213, 209, 217, 194, 158, 115,  67,  77, 212 },
    { 141, 203,  92,  72, 130, 187, 196,  69,  37,  56, 150, 208, 158, 103,  46, 173 },
    { 153, 219,  52,  89, 152, 213,  56,  75, 102,  86,  46, 155, 191, 131,  71, 103 },
    { 154, 221,  48, 100, 156, 207,  36, 110, 173, 138,  82,  61, 215, 151,  86,  55 },
    { 152, 216,  59,  85, 149, 211,  91,  48, 172, 168, 102,  38, 212, 154,  90,  48 },
    { 136, 198, 114,  63, 120, 172, 209, 198, 203, 150,  89,  52, 218, 153,  87,  51 },
    { 115, 175, 202,  45,  79, 122, 151, 164, 146, 108,  58, 122, 198, 138,  77,  85 },
    {  87, 142, 194, 175,  44,  67,  90, 102,  84,  56,  77, 210, 169, 113,  54, 143 },
    {  52, 103, 153, 196, 196, 102,  52,  48,  58, 127, 210, 181, 132,  80,  51, 211 },
    {  79,  62, 105, 143, 177, 201, 218, 221, 215, 196, 167, 131,  89,  41, 168, 191 },
    { 213,  76,  54,  89, 118, 140, 153, 154, 151, 135, 109,  78,  42, 137, 204, 154 },
    { 176, 213, 126,  45,  59,  77,  86,  88,  86,  73,  52,  55, 175, 203, 159, 113 },
};

static const uint8_t sweep[DISPLAY_LIGHTS_HEIGHT][DISPLAY_LIGHTS_WIDTH] = {
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4 },
    {  62,  62,  62,  62,  62,  62,  62,  62,  62,  62,  62,  62,  62,  62,  62,  62 },
    { 143, 143, 143, 143, 143, 143, 143, 143, 143, 143, 143, 143, 143, 143, 143, 143 },
    { 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207 },
    { 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244 },
    { 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224, 224 },
    { 171, 171, 171, 171, 171, 171, 171, 171, 171, 171, 171, 171, 171, 171, 171, 171 },
    {  96,  96,  96,  96,  96,  96,  96,  96,  96,  96,  96,  96,  96,  96,  96,  96 },
    {  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30 },
    {   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
};

static const uint8_t swipe[DISPLAY_LIGHTS_HEIGHT][DISPLAY_LIGHTS_WIDTH] = {
    {   0,   0,   0,  28,  93, 167, 220, 247, 233, 191, 124,  55,   9,   0,   0,   0 },
    {   0,   0,   0,  28,  93, 167, 220, 247, 233, 191, 124,  55,   9,   0,   0,   0 },
    {   0,   0,   0,  28,  93, 167, 220, 247, 233, 191, 124,  55,   9,   0,   0,   0 },
    {   0,   0,   0,  28,  93, 167, 220, 247, 233, 191, 124,  55,   9,   0,   0,   0 },
    {   0,   0,   0,  28,  93, 167, 220, 247, 233, 191, 124,  55,   9,   0,   0,   0 },
    {   0,   0,   0,  28,  93, 167, 220, 247, 233, 191, 124,  55,   9,   0,   0,   0 },
    {   0,   0,   0,  28,  93, 167, 220, 247, 233, 191, 124,  55,   9,   0,   0,   0 },
    {   0,   0,   0,  28,  93, 167, 220, 247, 233, 191, 124,  55,   9,   0,   0,   0 },
    {   0,   0,   0,  28,  93, 167, 220, 247, 233, 191, 124,  55,   9,   0,   0,   0 },
    {   0,   0,   0,  28,  93, 167, 220, 247, 233, 191, 124,  55,   9,   0,   0,   0 },
    {   0,   0,   0,  28,  93, 167, 220, 247, 233, 191, 124,  55,   9,   0,   0,   0 },
    {   0,   0,   0,  28,  93, 167, 220, 247, 233, 191, 124,  55,   9,   0,   0,   0 },
    {   0,   0,   0,  28,  93, 167, 220, 247, 233, 191, 124,  55,   9,   0,   0,   0 },
    {   0,   0,   0,  28,  93, 167, 220, 247, 233, 191, 124,  55,   9,   0,   0,   0 },
    {   0,   0,   0,  28,  93, 167, 220, 247, 233, 191, 124,  55,   9,   0,   0,   0 },
    {   0,   0,   0,  28,  93, 167, 220, 247, 233, 191, 124,  55,   9,   0,   0,   0 },
};

static const uint8_t swoosh[DISPLAY_LIGHTS_HEIGHT][DISPLAY_LIGHTS_WIDTH] = {
    { 222, 192, 160, 128,  96,  64,  32,   6,  32,  64,  96, 127, 160, 192, 222, 245 },
    { 192, 160, 128,  96,  64,  32,   6,  32,  64,  96, 128, 160, 192, 222, 245, 222 },
    { 160, 128,  96,  64,  32,   6,  32,  64,  96, 128, 160, 192, 222, 245, 222, 192 },
    { 128,  96,  64,  32,   6,  32,  64,  96, 128, 160, 192, 222, 245, 222, 192, 160 },
    {  96,  64,  32,   6,  32,  64,  96, 128, 160, 192, 222, 245, 222, 192, 160, 128 },
    {  64,  32,   6,  32,  64,  96, 128, 160, 192, 222, 245, 222, 192, 160, 128,  96 },
    {  32,   6,  32,  64,  96, 128, 160, 192, 222, 245, 222, 192, 160, 128,  96,  64 },
    {   6,  32,  64,  96, 128, 160, 192, 222, 245, 222, 192, 160, 128,  96,  64,  32 },
    {  32,  64,  96, 128, 160, 192, 222, 245, 222, 192, 160, 128,  96,  64,  32,   6 },
    {  64,  96, 128, 160, 192, 222, 245, 222, 192, 160, 128,  96,  64,  32,   6,  32 },
    {  96, 128, 160, 192, 222, 245, 222, 192, 160, 128,  96,  64,  32,   6,  32,  64 },
    { 128, 160, 192, 222, 245, 222, 192, 160, 128,  96,  64,  32,   6,  32,  64,  96 },
    { 160, 192, 222, 245, 222, 192, 160, 128,  96,  64,  32,   6,  32,  64,  96, 128 },
    { 192, 222, 245, 222, 192, 160, 127,  96,  64,  32,   6,  32,  64,  96, 128, 160 },
    { 222, 245, 222, 192, 160, 128,  96,  64,  32,   6,  32,  64,  96, 128, 160, 192 },
    { 245, 222, 192, 160, 128,  96,  64,  32,   6,  32,  64,  96, 128, 160, 192, 222 },
};

static const uint8_t thump[][DISPLAY_LIGHTS_HEIGHT][DISPLAY_LIGHTS_WIDTH] =  {
    {
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,  99,  99,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0, 165, 165,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0, 137, 137,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    }, {
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,  71,  71,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0, 152, 152,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,  99, 207, 207,  99,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0, 120, 177, 177, 120,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,  71,  71,  71,  71,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    }, {
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,  71,  71,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0, 152, 152,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,  71, 120, 207, 207, 120,  71,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,  71, 177, 240, 240, 177,  71,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0, 188, 255, 255, 188,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0, 152, 198, 198, 152,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,  71,  71,  71,  71,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    }, {
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,  71,  71,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0, 152, 152,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0, 188, 188,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0, 137, 188, 188, 225, 225, 188, 188, 137,   0,   0,   0,   0 },
        {   0,   0,   0,   0, 137, 207, 240, 255, 255, 240, 207, 137,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,  99, 207, 255, 255, 207,  99,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0, 188, 225, 225, 188,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,  71, 152, 137, 137, 152,  71,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,  71,  71,   0,   0,  71,  71,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    }, {
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0, 137, 137,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0, 188, 188,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0, 188, 188,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0, 137, 188, 188, 225, 225, 188, 188, 137,   0,   0,   0,   0 },
        {   0,   0,   0,   0, 137, 225, 255, 255, 255, 255, 225, 137,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0, 137, 225, 255, 255, 225, 137,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0, 120, 216, 225, 225, 216, 120,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0, 177, 216, 137, 137, 216, 177,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0, 137, 137,   0,   0, 137, 137,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    }, {
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0, 137, 137,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0, 188, 188,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0, 188, 188,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0, 137, 225, 225, 137,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0, 137, 188, 188, 225, 255, 255, 225, 188, 188, 137,   0,   0,   0 },
        {   0,   0,   0, 137, 198, 233, 255, 255, 255, 255, 233, 198, 137,   0,   0,   0 },
        {   0,   0,   0,   0,  71, 188, 248, 255, 255, 248, 188,  71,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0, 177, 248, 248, 248, 248, 177,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0, 188, 225, 177, 177, 225, 188,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0, 120, 177, 137,   0,   0, 137, 177, 120,   0,   0,   0,   0 },
        {   0,   0,   0,   0, 120, 120,   0,   0,   0,   0, 120, 120,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    }, {
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0, 120, 120,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0, 177, 177,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0, 188, 188,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0, 120, 216, 216, 120,   0,   0,   0,   0,   0,   0 },
        {   0,  71,  99,  99,  99,  99, 188, 248, 248, 188,  99,  99,  99,  99,  71,   0 },
        {   0,  71,  99, 165, 207, 207, 233, 255, 255, 233, 207, 207, 165,  99,  71,   0 },
        {   0,   0,   0, 137, 225, 255, 255, 255, 255, 255, 255, 225, 137,   0,   0,   0 },
        {   0,   0,   0,   0, 137, 225, 255, 255, 255, 255, 225, 137,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0, 188, 255, 255, 255, 255, 188,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,  99, 207, 248, 216, 216, 248, 207,  99,   0,   0,   0,   0 },
        {   0,   0,   0,   0, 165, 207, 177, 120, 120, 177, 207, 165,   0,   0,   0,   0 },
        {   0,   0,   0,   0, 137, 137,   0,   0,   0,   0, 137, 137,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    }, {
        {   0,   0,   0,   0,   0,   0,   0, 120, 120,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0, 177, 177,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0, 188, 188,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0, 120, 216, 216, 120,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0, 177, 248, 248, 177,   0,   0,   0,   0,   0,   0 },
        {   0, 137, 188, 188, 188, 188, 225, 255, 255, 225, 188, 188, 188, 188, 137,   0 },
        {   0, 137, 216, 248, 255, 255, 255, 255, 255, 255, 255, 255, 248, 216, 137,   0 },
        {   0,   0, 120, 188, 233, 255, 255, 255, 255, 255, 255, 233, 188, 120,   0,   0 },
        {   0,   0,   0,  71, 152, 225, 255, 255, 255, 255, 225, 152,  71,   0,   0,   0 },
        {   0,   0,   0,   0,  99, 207, 255, 255, 255, 255, 207,  99,   0,   0,   0,   0 },
        {   0,   0,   0,   0, 165, 240, 255, 225, 225, 255, 240, 165,   0,   0,   0,   0 },
        {   0,   0,   0,   0, 188, 255, 225, 137, 137, 225, 255, 188,   0,   0,   0,   0 },
        {   0,   0,   0,  99, 177, 198, 137,   0,   0, 137, 198, 177,  99,   0,   0,   0 },
        {   0,   0,   0,  99, 120,  71,   0,   0,   0,   0,  71, 120,  99,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    }, {
        {   0,   0,   0,   0,   0,   0,   0, 177, 177,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0, 188, 188,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0, 120, 216, 216, 120,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0, 177, 248, 248, 177,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0, 188, 255, 255, 188,   0,   0,   0,   0,   0,   0 },
        { 120, 177, 188, 188, 188, 188, 225, 255, 255, 225, 188, 188, 188, 188, 177, 120 },
        { 120, 177, 225, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 225, 177, 120 },
        {   0,   0, 137, 225, 255, 255, 255, 255, 255, 255, 255, 255, 225, 137,   0,   0 },
        {   0,   0,   0, 137, 207, 240, 255, 255, 255, 255, 240, 207, 137,   0,   0,   0 },
        {   0,   0,   0,   0, 165, 240, 255, 255, 255, 255, 240, 165,   0,   0,   0,   0 },
        {   0,   0,   0,   0, 188, 255, 255, 248, 248, 255, 255, 188,   0,   0,   0,   0 },
        {   0,   0,   0,  99, 207, 255, 233, 188, 188, 233, 255, 207,  99,   0,   0,   0 },
        {   0,   0,   0, 165, 240, 225, 152,  71,  71, 152, 225, 240, 165,   0,   0,   0 },
        {   0,   0,   0, 177, 216, 137,   0,   0,   0,   0, 137, 216, 177,   0,   0,   0 },
        {   0,   0,   0, 120, 120,   0,   0,   0,   0,   0,   0, 120, 120,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    }, {
        {   0,   0,   0,   0,   0,   0,   0, 188, 188,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0,  99, 207, 207,  99,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0, 165, 240, 240, 165,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0, 188, 255, 255, 188,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,  71, 198, 255, 255, 198,  71,   0,   0,   0,   0,   0 },
        { 137, 188, 188, 188, 188, 198, 233, 255, 255, 233, 198, 188, 188, 188, 188, 137 },
        { 137, 216, 248, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 248, 216, 137 },
        {   0, 120, 177, 225, 255, 255, 255, 255, 255, 255, 255, 255, 225, 177, 120,   0 },
        {   0,   0,   0, 137, 225, 255, 255, 255, 255, 255, 255, 225, 137,   0,   0,   0 },
        {   0,   0,   0,   0, 188, 255, 255, 255, 255, 255, 255, 188,   0,   0,   0,   0 },
        {   0,   0,   0,  71, 198, 255, 255, 255, 255, 255, 255, 198,  71,   0,   0,   0 },
        {   0,   0,   0, 152, 233, 255, 255, 225, 225, 255, 255, 233, 152,   0,   0,   0 },
        {   0,   0,   0, 188, 255, 233, 198, 137, 137, 198, 233, 255, 188,   0,   0,   0 },
        {   0,   0,   0, 188, 225, 152,  71,   0,   0,  71, 152, 225, 188,   0,   0,   0 },
        {   0,   0, 120, 177, 137,   0,   0,   0,   0,   0,   0, 137, 177, 120,   0,   0 },
        {   0,   0, 120, 120,   0,   0,   0,   0,   0,   0,   0,   0, 120, 120,   0,   0 },
    }, {
        {   0,   0,   0,   0,   0,   0,   0, 188, 188,   0,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0, 137, 225, 225, 137,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0, 188, 255, 255, 188,   0,   0,   0,   0,   0,   0 },
        {   0,   0,   0,   0,   0,   0, 188, 255, 255, 188,   0,   0,   0,   0,   0,   0 },
        {  99,  99,  99,  99,  99, 137, 216, 255, 255, 216, 137,  99,  99,  99,  99,  99 },
        { 165, 207, 207, 207, 207, 225, 248, 255, 255, 248, 225, 207, 207, 207, 207, 165 },
        { 137, 225, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 225, 137 },
        {   0, 137, 207, 240, 255, 255, 255, 255, 255, 255, 255, 255, 240, 207, 137,   0 },
        {   0,   0,  99, 165, 225, 255, 255, 255, 255, 255, 255, 225, 165,  99,   0,   0 },
        {   0,   0,   0,   0, 188, 255, 255, 255, 255, 255, 255, 188,   0,   0,   0,   0 },
        {   0,   0,   0, 120, 216, 255, 255, 255, 255, 255, 255, 216, 120,   0,   0,   0 },
        {   0,   0,   0, 177, 248, 255, 255, 225, 225, 255, 255, 248, 177,   0,   0,   0 },
        {   0,   0,   0, 188, 255, 248, 216, 137, 137, 216, 248, 255, 188,   0,   0,   0 },
        {   0,   0,  99, 207, 225, 177, 120,   0,   0, 120, 177, 225, 207,  99,   0,   0 },
        {   0, 120, 165, 207, 137,   0,   0,   0,   0,   0,   0, 137, 207, 165, 120,   0 },
        {   0, 120, 137, 137,   0,   0,   0,   0,   0,   0,   0,   0, 137, 137, 120,   0 },
    },
};

static const uint8_t waves[DISPLAY_LIGHTS_HEIGHT][DISPLAY_LIGHTS_WIDTH] = {
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,   0,   3,   0,  22,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,  40,  97, 117, 121, 129, 128, 104,  20,   0,   0,   0 },
    {   0,   0,   0,   0, 149, 181, 166, 182, 194, 198, 193, 170,  97,  26,   5,   2 },
    {   0,   0,  25, 229, 207, 197, 216, 235, 245, 246, 224, 188, 122,  52,  21,  18 },
    {  18,  31, 228, 202, 228, 243, 244, 254, 220, 169, 197, 186, 140,  71,  23,  11 },
    {  36, 224, 195, 242, 254, 254, 254, 168,   0,   0,   0,   0,   0,  24,  78,  46 },
    { 239, 214, 244, 255, 255, 255, 190, 120,   0,  70, 113, 117, 137, 140, 146, 133 },
    { 253, 253, 255, 255, 255, 255, 250, 241, 222, 176, 156, 178, 201, 215, 224, 239 },
    { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 245, 245, 255, 255, 255, 255 },
    { 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 },
    { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 },
    { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 },
};

void patterns_step_sequence();
void patterns_checkered();
void patterns_curtians();
void patterns_diamonds();
void patterns_fireworks();
void patterns_flash();
void patterns_lines();
void patterns_radar();
void patterns_random();
void patterns_siren();
void patterns_sprinkle();
void patterns_spiral();
void patterns_sweep();
void patterns_swirl();
void patterns_swipe();
void patterns_swoosh();
void patterns_swoop();
void patterns_thump();
void patterns_waves();

#endif