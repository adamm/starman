#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_log.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "led1642gw.h"

static const char *TAG = "starman-led1642gw";
static spi_device_handle_t spi;
static config_register_t config_register;


void _write_switch(void) {
    // Activate the output.

    // NOTE: LE is HIGH for 1-2 SCLK cycles

}

void _data_latch(void) {
    // For each of the 16 channels, 16-bits of data is sent.  Set the data latch or the first 15 cycles.

    // NOTE: LE is HIGH for 3-4 SCLK cycles
}


void _global_latch(void) {
    // For the last channel, 16-bits of data is still sent.  Send the global latch along with it, completing 256 bits of data

    // NOTE: LE is HIGH for 5-6 SCLK cycles
}


void _write_config_register(void) {
    // Write the data from the config register variable to SPI

    // NOTE: LE is HIGH for 7 SCLK cycles

    // TODO: write the config register data
    // TODO: How does this work for daisy chained devices?  Which devices' config are we writing, or are they all synced?
}


void _read_config_register(void) {
    // Read the data into the config register variable via SPI

    // NOTE: LE is HIGH for 8 SCLK cycles

    // TODO: How does this work for daisy chained devices?  Which devices' config are we receiving, or are they all synced?
}


void _start_open_error_detection(void) {
    // NOTE: LE is HIGH for 9 SCLK cycles
}


void _start_short_error_detection(void) {
    // NOTE: LE is HIGH for 10 SCLK cycles
}


void _start_combined_error_detection(void) {
    // NOTE: LE is HIGH for 11 SCLK cycles
}


void _end_error_detection(void) {
    // NOTE: LE is HIGH for 12 SCLK cycles
}


void _thermal_error_reading(void) {
    // NOTE: LE is HIGH for 13 SCLK cycles
}


void led1642gw_init(void) {
    esp_err_t err;
    gpio_config_t io_conf = {};

    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << LIGHTS_LE_GPIO);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
    gpio_set_level(LIGHTS_LE_GPIO, 0);

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
        .clock_speed_hz = 1*1000*1000,   // 1 MHz
        .spics_io_num = -1,
        .queue_size = 1,
        // CS is controlled manually during spiWrite() and spiWriteRead()
    };

    err = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(err);

    err = spi_bus_add_device(SPI2_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(err);

    ESP_LOGI(TAG, "Init sucessful");
}

