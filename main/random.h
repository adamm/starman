#ifndef RANDOM_H
#define RANDOM_H

#include <stdbool.h>

bool random_bool_under_percent(uint8_t);
uint32_t random_value_within_int(uint32_t);
void random_init(void);

#endif