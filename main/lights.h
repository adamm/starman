#ifndef LIGHTS_H
#define LIGHTS_H

#include "patterns.h"

#define LIGHTS_WIDTH  16
#define LIGHTS_HEIGHT 16
#define LIGHTS_TOTAL 144

void lights_map_led(pattern_t);
void lights_init(void);

#endif