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

#pragma once

#include "display.h"

void draw_pixel(display_t* d, int x, int y, uint8_t c);
void draw_line(display_t* d, double x1, double x2, double y1, double y2, uint8_t c);
void draw_line_aa(display_t* d, double x1, double x2, double y1, double y2, uint8_t c);
void draw_circle(display_t* d, double x, double y, double r, uint8_t c);
