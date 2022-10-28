#ifndef LIGHTS_H
#define LIGHTS_H

#include "patterns.h"

void lights_update_leds(display_t);
void lights_update_leds_raw(uint8_t*);
void lights_init(void);

#endif