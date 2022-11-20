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

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <string.h>
#include <stdio.h>

#include "display.h"
#include "font.h"

#include "text.h"

#define FONT simple_font

static const char *TAG = "starman-text";


void text_scroll(display_t* display) {
    uint8_t temp = 0;
    for (uint8_t y = 0; y < FONT.char_height; y++) {
        temp = display->text[y][0];
        for (uint8_t x = 0; x < display->text_width - 1; x++) {
            display->text[y][x] = display->text[y][x + 1];
        }
        display->text[y][display->text_width - 1] = temp;
    }
}


void text_draw_char(display_t* display, uint8_t x, uint8_t y, char chr) {
    // Draw the character into the display at (x,y)
    const unsigned char* ptr = FONT.pixel_data;
    uint8_t ord = (uint8_t) chr;
    uint8_t row = 0, col = 0;

    // Ensure all chars are upper-case
    if (ord > 96)
        ord -= 32;

    // The font only contains ASCII characters between 32..96
    if (ord < 32 || ord > 96)
        return;
    ord -= 32;

    // ESP_LOGI(TAG, "Draw %c(%d) to (%d,%d)", chr, ord+32, x, y);
    // ESP_LOGI(TAG, "font_data: %p", ptr);

    // Moves the pointer to the first row of the character
    // ESP_LOGI(TAG, "ord = %d", ord);
    uint16_t adj = (ord / (FONT.data_width / FONT.char_width)) * FONT.char_height * FONT.data_width;
    // ESP_LOGI(TAG, "adj = %d", adj);
    uint16_t off = (ord % (FONT.data_width / FONT.char_width)) * FONT.char_width;
    // ESP_LOGI(TAG, "off = %d", off);
    ptr += adj + off;
    // ESP_LOGI(TAG, "char_data: %p", ptr);

    // For each pixel row, add FONT.data_width
    for (row = 0; row < FONT.char_height; row++) {
        // Copy the FONT.char_width pixels to the display buffer
        for (col = 0; col < FONT.char_width; col++) {
            // ESP_LOGI(TAG, "(%d, %d) %p = %d", row, col+x, ptr + col, *(ptr + col));
            display->text[row][col + x] = *(ptr + col);
        }
        ptr += FONT.data_width;
    }
}


void text_write_string(display_t* display, char* string) {
    uint16_t pos = 0;
    uint16_t x = 3;

    ESP_LOGI(TAG, "%s", string);

    text_clear_string(display);

    // Only one line of text is supported.
    display->text_height = FONT.char_height;
    display->text_width = x + (strlen(string) * FONT.char_width);
    display->text = malloc(display->text_height * sizeof(uint8_t*));
    for (uint8_t i = 0; i < display->text_height; i++) {
        display->text[i] = malloc(display->text_width * sizeof(uint8_t));
        memset(display->text[i], 0, display->text_width);
    }

    for (pos = 0; pos < strlen(string); pos++) {
        text_draw_char(display, x, 0, string[pos]);
        // Although this is a mono-spaced font, identify the period as 2 pixels wide, not 4.
        // This makes reading the IP address easier as it scrolls by.
        if (string[pos] == '.')
            x += 2;
        else
            x += FONT.char_width;
    }

/*
    for (uint8_t c = 0; c < display->text_width; c++) {
        printf("\t%d", c);
    }
    printf("\n");
    for (uint8_t r = 0; r < display->text_height; r++) {
        printf("%d", r);
        for (uint8_t c = 0; c < display->text_width; c++) {
            printf("\t%d", display->text[r][c]);
        }
        printf("\n");
    }
*/
}


void text_clear_string(display_t* display) {
    if (display->text != NULL) {
        for (uint8_t i = 0; i < display->text_height; i++) {
            free(display->text[i]);
        }
        free(display->text);
        display->text = NULL;
    }
    display->text_height = 0;
    display->text_width = 0;
}