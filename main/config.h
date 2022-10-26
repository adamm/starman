#ifndef CONFIG_H
#define CONFIG_H

// PCB Display Lights Layout
// Techncially the star is 16 LEDs wide and 15 LEDs tall, and is only 144 LEDs total.
// But we still need to treat the drawable area as a 16x16 square when the patterns are applied.
// See patterns.h
#define DISPLAY_LIGHTS_GAIN   25 // WARNING: Do not exceed 50
#define DISPLAY_LIGHTS_WIDTH  16
#define DISPLAY_LIGHTS_HEIGHT 16
#define DISPLAY_LIGHTS_TOTAL 144
#define DISPLAY_LIGHTS_TOTAL_AREA (DISPLAY_LIGHTS_WIDTH * DISPLAY_LIGHTS_HEIGHT)

#if DISPLAY_LIGHTS_GAIN > 50
#error DISPLAY_LIGHTS_GAIN must be 50 or less -- turning on all LEDs will overdraw power/overheat components!
#endif

// Game of Life has a playgrid larger than the display area.  This allows for more
// complex designs that aren't constrainted to the 16x16 display framebuffer
#define GOL_GRID_HEIGHT   32
#define GOL_GRID_WIDTH    24
#define GOL_GRID_OFFSET_Y 8
#define GOL_GRID_OFFSET_X 4

// When the game isn't playing, the star sparkles
#define SPARKLE_MAX_LEDS     10
#define SPARKLE_MAX_STATE   100
#define SPARKLE_STEP          4
#define SPARKLE_STEPS        64
#define SPARKLE_RATE_MS      50

// Game logic defaults
#define GAME_START_LIVES      3
#define GAME_STAR_PERCENT    40
#define GAME_1UP_PERCENT     25
#define GAME_WARNING_PERCENT 25
#define GAME_DIE_PERCENT     50
#define GAME_FANFARE_PERCENT 25


// GPIO pin ports
#define PLAY_GAME_GPIO        1

#ifdef CONFIG_IDF_TARGET_ESP32C3

#define MUSIC_CHANNEL_1_GPIO  2
#define MUSIC_CHANNEL_2_GPIO  3
#define MUSIC_CHANNEL_3_GPIO  4
#define MUSIC_CHANNEL_4_GPIO  5
#define MUSIC_AMP_MUTE_GPIO   6

#define STATUS_LED_GPIO       8

#define LIGHTS_LE_GPIO        9
#define LIGHTS_MISO_GPIO     10
#define LIGHTS_MOSI_GPIO     11
#define LIGHTS_SCLK_GPIO     12

#else

#define MUSIC_CHANNEL_1_GPIO 17
#define MUSIC_CHANNEL_2_GPIO 18
#define MUSIC_CHANNEL_3_GPIO 19
#define MUSIC_CHANNEL_4_GPIO 20
#define MUSIC_AMP_MUTE_GPIO  21

#define STATUS_LED_GPIO      38

#define LIGHTS_LE_GPIO       33
#define LIGHTS_MISO_GPIO     34
#define LIGHTS_MOSI_GPIO     35
#define LIGHTS_SCLK_GPIO     36

#endif

#endif