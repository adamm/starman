#ifndef CONFIG_H
#define CONFIG_H

#define PLAY_GAME_PIN 1

#ifdef CONFIG_IDF_TARGET_ESP32C3

#define MUSIC_CHANNEL_1_PIN 1
#define MUSIC_CHANNEL_2_PIN 2
#define MUSIC_CHANNEL_3_PIN 3
#define MUSIC_CHANNEL_4_PIN 4
#define MUSIC_AMP_MUTE_PIN  5

#define STATUS_LED_PIN      8

#define LIGHTS_LE_PIN       9
#define LIGHTS_MISO_PIN     10
#define LIGHTS_MOSI_PIN     11
#define LIGHTS_SCLK_PIN     12

#else

#define MUSIC_CHANNEL_1_PIN 17
#define MUSIC_CHANNEL_2_PIN 18
#define MUSIC_CHANNEL_3_PIN 19
#define MUSIC_CHANNEL_4_PIN 20
#define MUSIC_AMP_MUTE_PIN  21

#define STATUS_LED_PIN      38

#define LIGHTS_LE_PIN       33
#define LIGHTS_MISO_PIN     34
#define LIGHTS_MOSI_PIN     35
#define LIGHTS_SCLK_PIN     36

#endif

#endif