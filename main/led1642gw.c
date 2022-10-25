#include <driver/gpio.h>
#include <esp_log.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "led1642gw.h"

#define LED1642GW_NO_LATCH              0
#define LED1642GW_WRITE_LATCH           2
#define LED1642GW_DATA_LATCH            4
#define LED1642GW_GLOBAL_LATCH          6
#define LED1642GW_WRITE_CONFIG_LATCH    7
#define LED1642GW_READ_CONFIG_LATCH     8
#define LED1642GW_START_OPEN_ERR_LATCH  9
#define LED1642GW_START_SHORT_ERR_LATCH 10
#define LED1642GW_START_COMBINED_ERR_LATCH 11
#define LED1642GW_END_ERR_LATCH         12
#define LED1642GW_THERMAL_ERR_LATCH     13

#define NUM_LED1642GW_ICs      9
#define NUM_LED1642GW_CHANNELS 16
#define NUM_LED_CHANNELS (NUM_LED1642GW_CHANNELS*NUM_LED1642GW_ICs)

static const char *TAG = "starman-led1642gw";

#ifdef NATIVE_SPI
#include <driver/spi_master.h>
static spi_device_handle_t spi;
#endif
static uint16_t ledbuffer[NUM_LED_CHANNELS];
static config_register_t config_register[NUM_LED1642GW_ICs];


/*
 * Write 16 bits of \data, with LE set high
 * for the number of clock cycles specified in \le_clocks.
 * MSB comes first, LSB is last.
 */
static void write_data(uint16_t data, uint8_t le_clocks)
{
    uint16_t mask = 0x8000;
    int8_t bit;

    gpio_set_level(LIGHTS_LE_GPIO, 0);

    // TODO: Verify our speed of transmission bit-banging the clock, MOSI, and latch.
    // If its too slow, switch to the native SPI API.
#ifndef NATIVE_SPI
    gpio_set_level(LIGHTS_SCLK_GPIO, 0);

    for (bit=15; bit>=le_clocks; bit--) {
        if (data & mask) {
            gpio_set_level(LIGHTS_MOSI_GPIO, 1);
        }
        else {
            gpio_set_level(LIGHTS_MOSI_GPIO, 0);
        }
        gpio_set_level(LIGHTS_SCLK_GPIO, 1);
        mask >>= 1;
        gpio_set_level(LIGHTS_SCLK_GPIO, 0);
    }
    
    gpio_set_level(LIGHTS_LE_GPIO, 1);
    for ( ; bit >= 0; bit--) {
        if (data & mask) {
            gpio_set_level(LIGHTS_MOSI_GPIO, 1);
        }
        else { 
            gpio_set_level(LIGHTS_MOSI_GPIO, 0);
        }
        gpio_set_level(LIGHTS_SCLK_GPIO, 1);
        mask >>= 1;
        gpio_set_level(LIGHTS_SCLK_GPIO, 0);
    }
    // set all pins to low after transmission
    gpio_set_level(LIGHTS_SCLK_GPIO, 0);
    gpio_set_level(LIGHTS_MOSI_GPIO, 0);
#else
#error "NATIVE SPI NOT YET IMPLEMENTED"
#endif
    gpio_set_level(LIGHTS_LE_GPIO, 0);
}


/*
 * Write data to BRIGHTNESS DATA LATCH register.
 * that means setting LE high for 3 or 4 clock cycles
 */
static void write_data_latch(uint16_t data)
{
    ESP_LOGD(TAG, "Sending data with latch");
    write_data(data, LED1642GW_DATA_LATCH);
}


/* 
 * Write data to BRIGHTNESS GLOBAL LATCH register.
 * that means setting LE high for 5 or 6 clock cycles
 */
static void write_global_latch(uint16_t data)
{
    ESP_LOGD(TAG, "Sending data with global latch");
    write_data(data, LED1642GW_GLOBAL_LATCH);
}


/*
 * This function shifts data through the 16bit shift
 * register of the LED1642GW, without writing the data
 * to any internal register of the IC.
 * This way, we can daisy chain an bunch of LED1642GW ICs,
 * and still get data through to any of those.
 */
static void write_no_command(uint16_t data)
{
    ESP_LOGD(TAG, "Sending data, without latch");
    write_data(data, LED1642GW_NO_LATCH);
}


/* 
 * Turn all channels on, so the data in the DATA LATCH 
 * register affects the LEDs attached to the IC.
 */
void led1642gw_activate(void)
{
    ESP_LOGD(TAG, "Activate all lights");
    for (uint8_t ic = 0; ic < NUM_LED1642GW_ICs-1; ic++) {
        write_no_command(0xffff);
    }
    write_data(0xffff, LED1642GW_WRITE_LATCH);
}


/* 
 * Turn all channels off,
 */
void led1642gw_deactivate(void)
{
    ESP_LOGD(TAG, "Deactivate all lights");
    for (uint8_t ic = 0; ic < NUM_LED1642GW_ICs-1; ic++) {
        write_no_command(0x0000);
    }
    write_data(0x0000, LED1642GW_WRITE_LATCH);
}


// Set the global brightness adjust.
void led1642gw_set_gain(uint8_t gain)
{
    uint16_t g = gain;

    if (g > 0x3f)
        g = 0x3f;

    ESP_LOGI(TAG, "Setting gain: %d", g);
    for (uint8_t ic = 0; ic < NUM_LED1642GW_ICs; ic++) {
        // config_register[ic] &= ~(0x003f);
        // config_register[ic] |=  g;
        config_register[ic].current_gain_adj = g;
    }
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


void led1642gw_read_config()
{

    // NOTE: LE is HIGH for 8 SCLK cycles
}


/*
 * Write data to CONFIG register.
 * that means setting LE high for 7 clock cycles
 */
void led1642gw_flush_config()
{
    uint8_t ic;
    uint16_t data;

    for (ic = 0; ic < NUM_LED1642GW_ICs-1; ic++) {
        memcpy(&data, &config_register[ic], sizeof(uint16_t));
        write_no_command(data);
    }
    memcpy(&data, &config_register[ic], sizeof(uint16_t));
    ESP_LOGI(TAG, "Sending config:");
    ESP_LOG_BUFFER_HEX(TAG, config_register, sizeof(config_register_t)*NUM_LED1642GW_ICs);
    write_data(data, LED1642GW_WRITE_CONFIG_LATCH);
}


/*
 * Transmit data from the ledbuffer to the BRIGHTNESS latches of
 * the LED driver ICs.
 * Let's assume, we have n LED1642GW ICs daisy chained. Then
 * we write n-1 times with write_no_command, to shift all
 * data through the 16bit shift registers of each of the ICs.
 * Then we once write with write_data_latch to store the data
 * in the BRIGHTNESS DATA registers of the respective ICs.
 * We do this for all but the last set of brightness data,
 * where we don't write to the DATA LATCH, but to the GLOBAL DATA LATCH.
 */
void led1642gw_flush_buffer(void)
{
    uint8_t ic;

    // ESP_LOGI(TAG, "Sending ledbuffer:");
    // ESP_LOG_BUFFER_HEX_LEVEL(TAG, ledbuffer, sizeof(ledbuffer), ESP_LOG_INFO);

    // for each of the first 15 channels, do the following:
    for (uint8_t channel = 0; channel < NUM_LED1642GW_CHANNELS; channel++) {
        // shift data throught the first n-1 ICs with write_no_command
        for (ic = 0; ic < NUM_LED1642GW_ICs-1; ic++) {
            write_no_command(ledbuffer[channel+(NUM_LED1642GW_CHANNELS*ic)]);
        }
        // then, when the brightness data has propagated through the
        // shift registers, write all data into the DATA LATCH of
        // all of the ICs.
        // for the 16th channel, we don't write to the DATA LATCH, but
        // to the CLOBAL data latch.
        // once more, we do the trick with write_no_command, to 
        // shift data through all the ICs
        if (channel < NUM_LED1642GW_CHANNELS-1) {
            write_data_latch(ledbuffer[channel+(NUM_LED1642GW_CHANNELS*ic)]);
        }
        else {
            write_global_latch(ledbuffer[channel+(NUM_LED1642GW_CHANNELS*ic)]);
        }
    }
}


void led1642gw_set_channel(uint8_t channel, uint16_t value)
{
    if (channel < NUM_LED_CHANNELS) {
        ledbuffer[channel] = value;
    }
}


void led1642gw_set_buffer(uint16_t* buffer, size_t length) {
    led1642gw_clear();

    memcpy(ledbuffer, buffer, ((length > NUM_LED_CHANNELS) ? NUM_LED_CHANNELS : length)* 2);
}


void led1642gw_clear(void)
{
    memset(ledbuffer, 0x00, sizeof(ledbuffer));
}



void led1642gw_init(void) {
    esp_err_t err;
    gpio_config_t io_conf = {};

    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << LIGHTS_LE_GPIO);
#ifndef NATIVE_SPI
    io_conf.pin_bit_mask |= (1ULL << LIGHTS_MOSI_GPIO);
    io_conf.pin_bit_mask |= (1ULL << LIGHTS_SCLK_GPIO);
#endif
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    err = gpio_config(&io_conf);
    ESP_ERROR_CHECK(err);

    gpio_set_level(LIGHTS_LE_GPIO, 0);
#ifndef NATIVE_SPI
    gpio_set_level(LIGHTS_MOSI_GPIO, 0);
    gpio_set_level(LIGHTS_SCLK_GPIO, 0);

    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << LIGHTS_MISO_GPIO);
    err = gpio_config(&io_conf);
    ESP_ERROR_CHECK(err);
#else
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
#endif
    led1642gw_clear();

    // TODO: Verify on real hardware whether  config values need to be set,
    // like sdo_delay, gradual_output_delay, etc.
    for (uint8_t ic = 0; ic < NUM_LED1642GW_ICs-1; ic++) {
        memset(&config_register[ic], 0, sizeof(config_register_t));
    }

    if (DISPLAY_LIGHTS_GAIN <= 50) {
        led1642gw_set_gain(DISPLAY_LIGHTS_GAIN);
    }
    led1642gw_flush_config();
    led1642gw_activate();

    ESP_LOGI(TAG, "Init sucessful");
}

