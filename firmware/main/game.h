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

#ifndef GAME_H
#define GAME_H

#include "stage.h"

#ifdef INCLUDE_GAME_SETTINGS
bool player_invincible = false;
#else
extern bool player_invincible;
#endif

uint8_t game_get_level();
uint8_t game_get_lives();
uint8_t game_get_playing_state();
bool game_smb_step_sequence(uint32_t time);
void game_smb_start(void);
bool game_loz_step_sequence(uint32_t time);
void game_loz_start(void);
void game_start(void);

#endif