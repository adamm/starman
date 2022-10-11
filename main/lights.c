#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_log.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "lights.h"
#include "patterns.h"

static const char *TAG = "starman-lights";
static spi_device_handle_t spi;

//  0 indicates no LED in physical display
// >0 indicates where in the MOSI array the LED is found
static const uint8_t LED_LUT[LIGHTS_HEIGHT][LIGHTS_WIDTH] = {
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,   0,  18,  31,   0,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,  19,  17,  32,  30,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,   0,  20,  24,  25,  29,   0,   0,   0,   0,   0,   0 },
    {   0,   0,   0,   0,   0,  21,  22,  23,  26,  27,  28,   0,   0,   0,   0,   0 },
    {  41,  42,  38,  39,  40,  11,  10,   9,   8,   7,   6, 137, 138, 139, 135, 136 },
    {  45,  44,  43,  37,  36,  13,   0,  16,   1,   0,   5, 141, 140, 134, 133, 132 },
    {   0,  46,  48,  33,  35,  12,   0,  15,   2,   0,   4, 142, 144, 129, 131,   0 },
    {   0,   0,  47,  34,  63,  64,   0,  14,   3,   0, 114, 115, 143, 130,   0,   0 },
    {   0,   0,   0,  50,  49,  62,  61,  60, 117, 116, 113, 128, 127,   0,   0,   0 },
    {   0,   0,  51,  52,  56,  57,  58,  59, 118, 119, 120, 121, 125, 126,   0,   0 },
    {   0,   0,  55,  54,  53,  83,  82,  81,  96,  95,  94, 124, 123, 122,   0,   0 },
    {   0,  67,  66,  79,  78,  85,  84,  88,  89,  93,  92,  99,  98, 100, 101,   0 },
    {   0,  68,  65,  80,  75,  86,  87,   0,   0,  90,  91, 102,  97, 112, 111,   0 },
    {  70,  71,  72,  73,  74,   0,   0,   0,   0,   0,   0, 103, 104, 105, 106, 107 },
    {  69,  77,  76,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 110, 109, 108 },
};
uint8_t lights_out[LIGHTS_TOTAL];


// Funcion called on each pattern refresh cycle. It will map each active pattern pixel to the physical LED output array.
void lights_map_led(pattern_t pattern) {
    bzero(lights_out, LIGHTS_TOTAL);

    for (int y = 0; y < LIGHTS_HEIGHT; y++) {
        for (int x = 0; x < LIGHTS_WIDTH; x++) {
            if (LED_LUT[x][y] > 0) {
                lights_out[LED_LUT[x][y]-1] = pattern.active[x][y];
            }
        }
    }

    // TODO: Send lights_out array to LED1642GW via MOSI pin
}


void lights_init(void) {
    esp_err_t err;
    gpio_config_t io_conf = {};

    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << LIGHTS_LE_GPIO);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
    gpio_set_level(LIGHTS_LE_GPIO, 1);

    spi_bus_config_t buscfg = {
        .mosi_io_num = LIGHTS_MOSI_GPIO,
        .miso_io_num = LIGHTS_MISO_GPIO,
        .sclk_io_num = LIGHTS_SCLK_GPIO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4092,
    };
    spi_device_interface_config_t devcfg = {
        //.flags = SPI_DEVICE_HALFDUPLEX,
        .mode = 0,
        .clock_speed_hz = 1*1000*1000,   // 10 MHz
        .spics_io_num = -1,
        .queue_size = 1,
        // CS is controlled manually during spiWrite() and spiWriteRead()
    };

    err = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(err);

    err = spi_bus_add_device(SPI2_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(err);

    ESP_LOGE(TAG, "Init sucessful");
}
