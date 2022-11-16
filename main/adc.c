#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <esp_log.h>

#include "adc.h"
#include "config.h"

#if CONFIG_IDF_TARGET_ESP32
#define ADC_CALIBRATION_SCHEME     ESP_ADC_CAL_VAL_EFUSE_VREF
#elif CONFIG_IDF_TARGET_ESP32S2
#define ADC_CALIBRATION_SCHEME     ESP_ADC_CAL_VAL_EFUSE_TP
#elif CONFIG_IDF_TARGET_ESP32C3
#define ADC_CALIBRATION_SCHEME     ESP_ADC_CAL_VAL_EFUSE_TP
#elif CONFIG_IDF_TARGET_ESP32S3
#define ADC_CALIBRATION_SCHEME     ESP_ADC_CAL_VAL_EFUSE_TP_FIT
#endif

static const char *TAG = "starman-adc";
static bool calibration_enabled = false;
static esp_adc_cal_characteristics_t adc1_chars = {0};


uint16_t adc_get_ambient_light_level() {
    uint32_t voltage = 0;
    int adc_raw = adc1_get_raw(ADC_LIGHT_CHANNEL);
    ESP_LOGD(TAG, "adc_raw  = %d", adc_raw);

    if (calibration_enabled) {
        voltage = esp_adc_cal_raw_to_voltage(adc_raw, &adc1_chars);
        ESP_LOGD(TAG, "adc_volt = %d mV", voltage);
    }

    // The lower the ADC raw value is, the brighter the reading.
    // Thus, subtract the MAX from it to inverse the results:
    // -> higher should equal brighter
    return (uint16_t) ADC_MAX_AMBIENT_LIGHT_LEVEL - adc_raw;
}


void adc_init() {
    esp_err_t err;

    err = esp_adc_cal_check_efuse(ADC_CALIBRATION_SCHEME);
    if (err == ESP_ERR_NOT_SUPPORTED) {
        ESP_LOGW(TAG, "Calibration scheme not supported, skip software calibration");
    } else if (err == ESP_ERR_INVALID_VERSION) {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    } else if (err == ESP_OK) {
        calibration_enabled = true;
        esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);
    } else {
        ESP_LOGE(TAG, "Invalid arg");
    }

    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC_LIGHT_CHANNEL, ADC_ATTEN_DB_11));

    ESP_LOGI(TAG, "Init sucessful");
}