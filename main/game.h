#ifndef GAME_H
#define GAME_H

uint8_t game_get_level();
uint8_t game_get_lives();
uint8_t game_get_playing_state();
bool game_step_sequence(uint32_t time);
void game_start(void);

#endif