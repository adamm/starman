#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "util.h"


void delay(uint32_t ms) {
    vTaskDelay(ms / portTICK_RATE_MS);
}


void littleToBigEndian(uint32_t* buf, uint32_t size)
{
    for (int i = 0; i < size / sizeof(unsigned int); i++) {
        buf[i] = SWAP_32(buf[i]);
    }
}


int bigToLittleEndian(uint8_t* buf)
{
    return (((int)buf[0] << 24) | ((int)buf[1] << 16) | ((int)buf[2] << 8) |
            ((int)buf[3]) << 0);
}


void wordSwap(uint16_t* pWord, size_t wordCount)
{
    for (int i = 0; i < wordCount; i++) {
        pWord[i] = WORD_SWAP(pWord[i]);
    }
}


void PrintHex8(const char* TAG, uint8_t* data,
               size_t length) // prints 8-bit data in hex with leading zeroes
{
    ESP_LOG_BUFFER_HEX(TAG, data, length);
}


void PrintHex16(const char* TAG, uint16_t* data,
                size_t length) // prints 16-bit data in hex with leading zeroes
{
    ESP_LOG_BUFFER_HEX(TAG, data, length * 2);
}
