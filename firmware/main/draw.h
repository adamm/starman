#pragma once

#include "display.h"

void draw_pixel(display_t* d, int x, int y, uint8_t c);
void draw_line(display_t* d, double x1, double x2, double y1, double y2, uint8_t c);
void draw_line_aa(display_t* d, double x1, double x2, double y1, double y2, uint8_t c);
void draw_circle(display_t* d, double x, double y, double r, uint8_t c);
