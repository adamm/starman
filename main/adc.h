#ifndef ADC_H
#define ADC_H

#define ADC_MAX_AMBIENT_LIGHT_LEVEL 4096

uint16_t adc_get_ambient_light_level();
void adc_init();


#endif