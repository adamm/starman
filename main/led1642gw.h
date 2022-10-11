#ifndef LED1642GW_H
#define LED1642GW_H

struct config_register {
    uint16_t current_gain_adj    : 6;
    uint16_t current_range       : 1;
    uint16_t error_detect_mode   : 1;
    uint16_t short_led_threshold : 2;
    uint16_t auto_off_shutdown   : 1;
    uint16_t output_onoff_time   : 2;
    uint16_t sdo_delay           : 1;
    uint16_t gradual_output_delay: 1;
    uint16_t pwm_brightness      : 1;
} typedef config_register_t;

void led1642gw_init(void);

#endif