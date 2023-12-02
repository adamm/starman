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

#ifndef TEXT_H
#define TEXT_H

#include <string.h>

#include "display.h"

void text_scroll(display_t* display);
void text_draw_char(display_t*, uint8_t, uint8_t, char);
void text_write_string(display_t* display, char* string);
void text_clear_string(display_t* display);

#endif