/*
   Copyright 2022 Adam McDaniel

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef CONFIG_H
#define CONFIG_H

// PCB Display Lights Layout
// Techncially the star is 16 LEDs wide and 15 LEDs tall, and is only 144 LEDs total.
// But we still need to treat the drawable area as a 16x16 square when the patterns are applied.
// See patterns.h
#define DISPLAY_LIGHTS_WIDTH  16
#define DISPLAY_LIGHTS_HEIGHT 16
#define DISPLAY_LIGHTS_TOTAL 144
#define DISPLAY_LIGHTS_TOTAL_AREA (DISPLAY_LIGHTS_WIDTH * DISPLAY_LIGHTS_HEIGHT)

// Gain must be 50 or less -- turning on all LEDs will overdraw power/overheat components!
#define DISPLAY_LIGHTS_MAX_GAIN      100
#define DISPLAY_LIGHTS_DEFAULT_GAIN  25

// Game of Life has a playgrid larger than the display area.  This allows for more
// complex designs that aren't constrainted to the 16x16 display framebuffer
#define GOL_GRID_HEIGHT   32
#define GOL_GRID_WIDTH    24
#define GOL_GRID_OFFSET_Y 8
#define GOL_GRID_OFFSET_X 4
#define GOL_GRID_TOTAL_AREA (GOL_GRID_HEIGHT * GOL_GRID_WIDTH)

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
#define GAME_ENDING_PERCENT  25

#if (CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3)

// GPIO pin ports
#define PLAY_GAME_GPIO        1

#define ADC_LIGHT_CHANNEL    ADC1_CHANNEL_3 // GPIO 4

#define MUSIC_CHANNEL_1_GPIO 17
#define MUSIC_CHANNEL_2_GPIO 18
#define MUSIC_CHANNEL_3_GPIO 19
#define MUSIC_CHANNEL_4_GPIO 20
#define MUSIC_AMP_MUTE_GPIO  21

#define RGB_LED_GPIO         38

#define LIGHTS_LE_GPIO       33
#define LIGHTS_MISO_GPIO     34
#define LIGHTS_MOSI_GPIO     35
#define LIGHTS_SCLK_GPIO     36

#else

#error Only ESP32S2 and ESP32S3 configuration supported

#endif

/* Set EXTERN macro: */
#ifdef CONFIG_IMPORT
    #define CONFIG_EXTERN
#else
    #define CONFIG_EXTERN extern
#endif

#ifndef CONFIG_ESP_MAX_RETRY
#define CONFIG_ESP_MAX_RETRY 5
#endif
#ifndef CONFIG_ESP32_WIFI_STATIC_RX_BUFFER_NUM
#define CONFIG_ESP32_WIFI_STATIC_RX_BUFFER_NUM 32
#endif
#ifndef CONFIG_ESP32_WIFI_DYNAMIC_RX_BUFFER_NUM
#define CONFIG_ESP32_WIFI_DYNAMIC_RX_BUFFER_NUM 32
#endif
#ifndef CONFIG_ESP32_WIFI_TX_BUFFER_TYPE
#define CONFIG_ESP32_WIFI_TX_BUFFER_TYPE 1
#endif

#define CONFIG_FIRMWARE_URL_DEFAULT "https://starman.array.org"
#define CONFIG_FIRMWARE_URL_MAXLEN 50
CONFIG_EXTERN char config_firmware_service_url[CONFIG_FIRMWARE_URL_MAXLEN]
#ifdef CONFIG_IMPORT
= CONFIG_FIRMWARE_URL_DEFAULT
#endif
;

#define CONFIG_FIRMWARE_TRACK_STABLE   "stable"
#define CONFIG_FIRMWARE_TRACK_TESTING  "testing"
#define CONFIG_FIRMWARE_TRACK_UNSTABLE "unstable"

#define CONFIG_FIRMWARE_TRACK_DEFAULT CONFIG_FIRMWARE_TRACK_STABLE 
#define CONFIG_FIRMWARE_TRACK_MAXLEN 10
CONFIG_EXTERN char config_firmware_track[CONFIG_FIRMWARE_TRACK_MAXLEN]
#ifdef CONFIG_IMPORT
= CONFIG_FIRMWARE_TRACK_DEFAULT
#endif
;

CONFIG_EXTERN int8_t config_brightness
#ifdef CONFIG_IMPORT
= DISPLAY_LIGHTS_DEFAULT_GAIN
#endif
;

CONFIG_EXTERN uint8_t config_theme
#ifdef CONFIG_IMPORT
= 0
#endif
;

void config_set_firmware_track(char*);
void config_set_brightness(int8_t);
void config_set_theme(uint8_t);
void config_save(void);
void config_init(void);

#endif
