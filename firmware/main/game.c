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

#include <esp_log.h>

#include "config.h"
#include "music.h"
#include "random.h"
#include "sparkle.h"
#define INCLUDE_THEME_MUSIC_AND_PATTERNS
#include "themes.h"

#include "game.h"

static const char *TAG = "starman-game";


static uint8_t level = 1;
static uint8_t lives = 3;
static bool playing  = false;
static uint32_t player_gets_star    = 0;
static uint32_t player_gets_1up     = 0;
static uint32_t player_gets_warning = 0;
static uint32_t player_gets_ending  = 0;
static uint32_t player_dies         = 0;
static uint32_t player_finishes     = 0;


uint8_t game_get_level() {
    return level;
}


uint8_t game_get_lives() {
    return lives;
}


uint8_t game_get_playing_state() {
    return playing;
}


bool game_smb_step_sequence(uint32_t time) {
    patterns_step_sequence();

    // Stop the current track if the random time has passed to get a star, 1up, warning, or die
    if (player_gets_star && player_gets_star < time) {
        ESP_LOGW(TAG, "Pause level: Player got the star");
        return true;
    }
    else if (player_gets_1up && player_gets_1up < time) {
        ESP_LOGW(TAG, "Pause level: Player got the 1up");
        return true;
    }
    else if (player_gets_warning && player_gets_warning < time) {
        ESP_LOGW(TAG, "Pause level: Player got the warning");
        return true;
    }
    else if (player_dies && player_dies < time) {
        ESP_LOGW(TAG, "Pause level: Player died");
        return true;
    }
    else if (player_finishes && player_finishes < time) {
        ESP_LOGW(TAG, "Pause level: Player finished the level");
        return true;
    }

    // Allow the music to continue
    return false;
}



void game_smb_start(void) {
    // Ignore subsequent play_game() calls if game is already playing
    if (playing == false)
        playing = true;
    else
        return;

    const theme_t* active_theme = themes[config_theme].theme;

    music_amp_unmute();

    player_gets_star = random_bool_under_percent(GAME_STAR_PERCENT);
    player_gets_1up = random_bool_under_percent(GAME_1UP_PERCENT);
    player_gets_warning = random_bool_under_percent(GAME_WARNING_PERCENT);
    player_gets_ending = random_bool_under_percent(GAME_FANFARE_PERCENT);
    player_dies = random_bool_under_percent(GAME_DIE_PERCENT);

    // Stop sparkling becuase we're about to play some music/lights!
    sparkle_stop();

    // If the user gets a 1up, star, interrupt the music at random point and contunue when sound effect is done
    // If the user dies, interrupt the music at a random point and stop when sound effect is done
    uint32_t length = 0;
    const byte* level_music;
    void (*level_pattern)(void);

    if (level == 1) {
        level_pattern = active_theme[THEME_STAGE_level_1].pattern;
        level_music = active_theme[THEME_STAGE_level_1].score;
        length = music_get_score_length(active_theme[THEME_STAGE_level_1].score);
    }
    else if (level == 2) {
        level_pattern = active_theme[THEME_STAGE_level_2].pattern;
        level_music = active_theme[THEME_STAGE_level_2].score;
        length = music_get_score_length(active_theme[THEME_STAGE_level_2].score);
    }
    else if (level == 3) {
        level_pattern = active_theme[THEME_STAGE_level_3].pattern;
        level_music = active_theme[THEME_STAGE_level_3].score;
        length = music_get_score_length(active_theme[THEME_STAGE_level_3].score);
    }
    else if (level == 4) {
        level_pattern = active_theme[THEME_STAGE_level_4].pattern;
        level_music = active_theme[THEME_STAGE_level_4].score;
        length = music_get_score_length(active_theme[THEME_STAGE_level_4].score);
    }
    else {
        // We shouldn't get here.  Reset!
        level = 1;
        lives = GAME_START_LIVES;
        playing = false;
        game_smb_start();
        return;
    }

    // Ignore the song header when calculating the song length
    length -= 6;

    // Now that the music has been identified, pick a random location in
    // the song to stop and play the 1up, starman, time warning, or die music.
    if (player_gets_1up)
        player_gets_1up = random_value_within_int(length);
    if (player_gets_star)
        player_gets_star = random_value_within_int(length);
    if (player_gets_warning)
        // The warning music should be at the last third of the level
        player_gets_warning = (length/3*2);
    if (player_dies)
        player_dies = random_value_within_int(length);
    else
        player_finishes = random_value_within_int(length/2) + length/2;


    ESP_LOGI(TAG, "Player level:  %d", level);
    ESP_LOGI(TAG, "Player lives:  %d", lives);
    ESP_LOGI(TAG, "Level length:  %d", length);
    ESP_LOGI(TAG, "Player gets star?  %s", player_gets_star ? "Yes" : "No");
    if (player_gets_star)
        ESP_LOGI(TAG, " ... at %d", player_gets_star);
    ESP_LOGI(TAG, "Player gets 1up?  %s", player_gets_1up ? "Yes" : "No");
    if (player_gets_1up)
        ESP_LOGI(TAG, " ... at %d", player_gets_1up);
    ESP_LOGI(TAG, "Player gets time warning?  %s", player_gets_warning ? "Yes" : "No");
    if (player_gets_warning)
        ESP_LOGI(TAG, " ... at %d", player_gets_warning);
    if (level == 4)
        ESP_LOGI(TAG, "Player gets ending?  %s", player_gets_ending ? "Yes" : "No");
    ESP_LOGI(TAG, "Player dies?  %s", player_dies ? "Yes" : "No");
    if (player_dies)
        ESP_LOGI(TAG, " ... at %d", player_dies);
    ESP_LOGI(TAG, "Player finishes?  %s", player_finishes ? "Yes" : "No");
    if (player_finishes)
        ESP_LOGI(TAG, " ... at %d", player_finishes);

    uint32_t stopped_music_time = 0;
    music_settempo(100);

    // If the user is destined to get a star, 1up, or time warning, play the
    // sound effect queue, then resume the normal level music.
    while (stopped_music_time < length) {
        level_pattern();
        stopped_music_time = music_playscore_at_time(level_music, stopped_music_time);

        if (player_gets_star && player_gets_star <= stopped_music_time) {
            uint8_t prev_tempo = music_gettempo();
            player_gets_star = 0;

            active_theme[THEME_STAGE_block].pattern();
            music_playscore(active_theme[THEME_STAGE_block].score);

            active_theme[THEME_STAGE_powerup].pattern();
            music_playscore(active_theme[THEME_STAGE_powerup].score);

            active_theme[THEME_STAGE_starman].pattern();
            music_playscore(active_theme[THEME_STAGE_starman].score);

            music_settempo(prev_tempo);
        }

        if (player_gets_1up && player_gets_1up <= stopped_music_time) {
            lives++;
            uint8_t prev_tempo = music_gettempo();
            player_gets_1up = 0;

            active_theme[THEME_STAGE_block].pattern();
            music_playscore(active_theme[THEME_STAGE_block].score);

            active_theme[THEME_STAGE_1up].pattern();
            music_playscore(active_theme[THEME_STAGE_1up].score);

            music_settempo(prev_tempo);
        }

        if (player_gets_warning && player_gets_warning <= stopped_music_time) {
            player_gets_warning = 0;

            active_theme[THEME_STAGE_warning].pattern();
            music_playscore(active_theme[THEME_STAGE_warning].score);
            music_settempo(150);
        }

        // If the user is destined to die, the level music will not resume
        if (player_dies && player_dies <= stopped_music_time)
            break;

        // The player could finish the level before the music is done, and death is averted
        if (player_finishes && player_finishes <= stopped_music_time) {
            player_dies = 0;
            break;
        }
    }

    music_settempo(100);

    if (player_dies) {
        lives--;
        player_dies = 0;

        active_theme[THEME_STAGE_death].pattern();
        music_playscore(active_theme[THEME_STAGE_death].score);
    }
    else if (level == 4) {
        active_theme[THEME_STAGE_fanfare].pattern();
        music_playscore(active_theme[THEME_STAGE_fanfare].score);

        if (player_gets_ending) {
            player_gets_ending = 0;
            player_finishes = 0;
            player_dies = 0;

            active_theme[THEME_STAGE_ending].pattern();
            music_playscore(active_theme[THEME_STAGE_ending].score);
        }
        level = 1;
        lives = GAME_START_LIVES;
    }
    else {
        active_theme[THEME_STAGE_success].pattern();
        music_playscore(active_theme[THEME_STAGE_success].score);

        active_theme[THEME_STAGE_clear].pattern();
        music_playscore(active_theme[THEME_STAGE_clear].score);

        level++;
    }

    if (lives == 0) {
        active_theme[THEME_STAGE_gameover].pattern();
        music_playscore(active_theme[THEME_STAGE_gameover].score);
        patterns_gameover_stop();

        level = 1;
        lives = GAME_START_LIVES;
    }

    music_amp_mute();

    ESP_LOGI(TAG, "Done!");
    playing = false;

    sparkle_start();
}


bool game_loz_step_sequence(uint32_t time) {
    patterns_step_sequence();

    // Stop the current track if the random time has passed to get a star, 1up, warning, or die
    if (player_dies && player_dies < time) {
        ESP_LOGW(TAG, "Pause level: Player died");
        return true;
    }
    else if (player_finishes && player_finishes < time) {
        ESP_LOGW(TAG, "Pause level: Player finished the level");
        return true;
    }

    // Allow the music to continue
    return false;
}



void game_loz_start(void) {
    // Ignore subsequent play_game() calls if game is already playing
    if (playing == false)
        playing = true;
    else
        return;

    const theme_t* active_theme = themes[config_theme].theme;

    music_amp_unmute();

    player_gets_ending = random_bool_under_percent(GAME_FANFARE_PERCENT);
    player_dies = random_bool_under_percent(GAME_DIE_PERCENT);

    // Stop sparkling becuase we're about to play some music/lights!
    sparkle_stop();

    // If the user gets a 1up, star, interrupt the music at random point and contunue when sound effect is done
    // If the user dies, interrupt the music at a random point and stop when sound effect is done
    uint32_t length = 0;
    const byte* level_music;
    void (*level_pattern)(void);

    if (level == 1) {
        level_pattern = active_theme[THEME_STAGE_level_1].pattern;
        level_music = active_theme[THEME_STAGE_level_1].score;
        length = music_get_score_length(active_theme[THEME_STAGE_level_1].score);
    }
    else if (level == 2) {
        level_pattern = active_theme[THEME_STAGE_level_2].pattern;
        level_music = active_theme[THEME_STAGE_level_2].score;
        length = music_get_score_length(active_theme[THEME_STAGE_level_2].score);
    }
    else if (level == 3) {
        level_pattern = active_theme[THEME_STAGE_level_3].pattern;
        level_music = active_theme[THEME_STAGE_level_3].score;
        length = music_get_score_length(active_theme[THEME_STAGE_level_3].score);
    }
    else if (level == 4) {
        level_pattern = active_theme[THEME_STAGE_level_4].pattern;
        level_music = active_theme[THEME_STAGE_level_4].score;
        length = music_get_score_length(active_theme[THEME_STAGE_level_4].score);
    }
    else {
        // We shouldn't get here.  Reset!
        level = 1;
        lives = GAME_START_LIVES;
        playing = false;
        game_loz_start();
        return;
    }

    // Ignore the song header when calculating the song length
    length -= 6;

    // Now that the music has been identified, pick a random location in
    // the song to stop and play the die music.
    if (player_dies)
        player_dies = random_value_within_int(length);
    else
        player_finishes = random_value_within_int(length/2) + length/2;


    ESP_LOGI(TAG, "Player level:  %d", level);
    ESP_LOGI(TAG, "Player lives:  %d", lives);
    ESP_LOGI(TAG, "Level length:  %d", length);
    ESP_LOGI(TAG, "Player dies?  %s", player_dies ? "Yes" : "No");
    if (player_dies)
        ESP_LOGI(TAG, " ... at %d", player_dies);
    ESP_LOGI(TAG, "Player finishes?  %s", player_finishes ? "Yes" : "No");
    if (player_finishes)
        ESP_LOGI(TAG, " ... at %d", player_finishes);

    uint32_t stopped_music_time = 0;
    music_settempo(100);

    // If the user is destined to get a star, 1up, or time warning, play the
    // sound effect queue, then resume the normal level music.
    while (stopped_music_time < length) {
        level_pattern();
        stopped_music_time = music_playscore_at_time(level_music, stopped_music_time);

        // If the user is destined to die, the level music will not resume
        if (player_dies && player_dies <= stopped_music_time)
            break;

        // The player could finish the level before the music is done, and death is averted
        if (player_finishes && player_finishes <= stopped_music_time) {
            player_dies = 0;
            break;
        }
    }

    if (player_dies) {
        lives--;
        player_dies = 0;

        active_theme[THEME_STAGE_death].pattern();
        music_playscore(active_theme[THEME_STAGE_death].score);
    }
    else if (level == 4) {
        active_theme[THEME_STAGE_fanfare].pattern();
        music_playscore(active_theme[THEME_STAGE_fanfare].score);

        if (player_gets_ending) {
            player_gets_ending = 0;
            player_finishes = 0;
            player_dies = 0;

            active_theme[THEME_STAGE_ending].pattern();
            music_playscore(active_theme[THEME_STAGE_ending].score);
        }
        level = 1;
        lives = GAME_START_LIVES;
    }
    else {
        active_theme[THEME_STAGE_success].pattern();
        music_playscore(active_theme[THEME_STAGE_success].score);

        active_theme[THEME_STAGE_clear].pattern();
        music_playscore(active_theme[THEME_STAGE_clear].score);

        level++;
    }

    if (lives == 0) {
        active_theme[THEME_STAGE_gameover].pattern();
        music_playscore(active_theme[THEME_STAGE_gameover].score);
        patterns_gameover_stop();

        level = 1;
        lives = GAME_START_LIVES;
    }

    music_amp_mute();

    ESP_LOGI(TAG, "Done!");
    playing = false;

    sparkle_start();
}


void game_start(void) {
    // Specify the theme-specific game_step() routine as defined in themes.h to
    // be called on each musical note change
    music_callback(themes[config_theme].game_step);

    // Start the theme-specific game_start() routine as defined in themes.h
    themes[config_theme].game_start();
}