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

void led1642gw_activate(void);
void led1642gw_deactivate(void);
void led1642gw_set_gain(uint8_t);
void led1642gw_read_config(void);
void led1642gw_flush_config(void);
void led1642gw_flush_buffer(void);
void led1642gw_set_channel(uint8_t, uint16_t);
void led1642gw_set_buffer(uint16_t*, size_t);
void led1642gw_clear(void);
void led1642gw_init(void);

#endif