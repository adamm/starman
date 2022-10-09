#ifndef CONFIG_H
#define CONFIG_H

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