#ifndef TEXT_H
#define TEXT_H

#include <string.h>

#include "display.h"

void text_scroll(display_t* display);
void text_draw_char(display_t*, uint8_t, uint8_t, char);
void text_write_string(display_t* display, char* string);
void text_clear_string(display_t* display);

#endif