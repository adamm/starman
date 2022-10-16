#include <esp_err.h>
#include <esp_log.h>

#include "buttons.h"
#include "config.h"
#include "lights.h"
#include "music.h"
#include "patterns.h"
#include "random.h"
#include "status.h"
#include "smb.h"
#include "smb2.h"
#include "smb3.h"
#include "smw.h"

static const char *TAG = "starman";
  
/*
  HOW TO PLAY:

  - Press the button to start the game.
  - Start the game with three lives at level 1 (overworld).  If you lose all lives, it's game over.
  - There is a 50% chance you will complete each level.  If you do, you move onto next level.  If not, you die and need to retry.
    - Death can occur randomly within the level.
    - Press the button again to start the next level, or retry the same level.
  - There is a 40% chance you'll get a star.
    - Stars can occur randomly within the level.
    - Stars do not negate death (ie, falling into a pit).
  - There is a 25% chance you'll get a 1-up, resulting in an extra life.
  - If you make it to level 4 (castle) and complete it, there is a 25% chance the princess will be there and the game is over.
*/

static uint8_t level = 0;
static uint8_t lives = GAME_START_LIVES;
static bool playing  = false;
static uint32_t player_gets_star    = 0;
static uint32_t player_gets_1up     = 0;
static uint32_t player_gets_fanfare = 0;
static uint32_t player_dies         = 0;
static const byte* music;
 
bool step_sequence(uint32_t time) {
    patterns_step_sequence();

    // Stop the current track if the random time has passed to get a star, 1up, or die
    if (player_gets_star && player_gets_star < time) {
        return true;
    }
    else if (player_gets_1up && player_gets_1up < time) {
        return true;
    }
    else if (player_dies && player_dies < time) {
        return true;
    }

    // Allow the music to continue
    return false;
}


void play_game(void) {
    // Ignore subsequent play_game() calls if game is already playing
    if (playing == false)
        playing = true;
    else
        return;

    level++;
    player_gets_star = random_bool_under_percent(GAME_STAR_PERCENT);
    player_gets_1up = random_bool_under_percent(GAME_1UP_PERCENT);
    player_gets_fanfare = random_bool_under_percent(GAME_FANFARE_PERCENT);
    player_dies = random_bool_under_percent(GAME_DIE_PERCENT);

    ESP_LOGI(TAG, "Player level:  %d", level);
    ESP_LOGI(TAG, "Player lives:  %d", lives);
    ESP_LOGI(TAG, "Player gets star?  %s", player_gets_star ? "Yes" : "No");
    ESP_LOGI(TAG, "Player gets 1up?  %s", player_gets_1up ? "Yes" : "No");
    if (level == 4)
        ESP_LOGI(TAG, "Player gets fanfare?  %s", player_gets_1up ? "Yes" : "No");
    ESP_LOGI(TAG, "Player dies?  %s", player_dies ? "Yes" : "No");

    // If the user gets a 1up, star, interrupt the music at random point and contunue when sound effect is done
    // If the user dies, interrupt the music at a random point and stop when sound effect is done
    uint32_t length = 0;

    if (level == 1) {
        patterns_sprinkle();
        music = smb_overworld;
        length = sizeof(smb_overworld);
    }
    else if (level == 2) {
        patterns_lines();
        music = smb_underworld;
        length = sizeof(smb_underworld);
    }
    else if (level == 3) {
        patterns_waves();
        music = smb_underwater;
        length = sizeof(smb_underwater);
    }
    else if (level == 4) {
        patterns_siren();
        music = smb_castle;
        length = sizeof(smb_castle);
    }
    else {
        // We shouldn't get here.  Reset!
        level = 0;
        lives = GAME_START_LIVES;
        playing = false;
        play_game();
        return;
    }

    // Now that the music has been identified, pick a random location in
    // the song to stop and play the 1up, starman, or die music.
    if (player_gets_1up)
        player_gets_1up = random_value_within_int(length);
    if (player_gets_star)
        player_gets_star = random_value_within_int(length);
    if (player_dies)
        player_dies = random_value_within_int(length);

    music_playscore(music);

    if (player_gets_star) {
        player_gets_star = 0;
        patterns_flash();
        music_playscore(smb_block);
        music_playscore(smb_powerup);
        patterns_swoosh();
        music_playscore(smb_starman);
    }

    if (player_gets_1up) {
        lives++;
        player_gets_1up = 0;
        patterns_flash();
        music_playscore(smb_block);
        patterns_checkered();
        music_playscore(smb_1up);
    }

    // TODO: If the user gets a star or 1up, continue the original level music 
    // from where it was interrupted.

    if (level == 4 && !player_dies) {
        if (player_gets_fanfare) {
            player_gets_fanfare = 0;
            patterns_checkered();
            music_playscore(smb_fanfare);

            patterns_diamonds();
            music_playscore(smb_ending);
        }
        level = 0;
        lives = GAME_START_LIVES;
    }

    if (player_dies) {
        lives--;
        level--; // level gets incremented when starting play --
                 //decrementing means user needs to replay the same level again
        player_dies = 0;
        patterns_spiral();
        music_playscore(smb_death);
    }
    else {
        patterns_radar();
        music_playscore(smb_course_clear);
    }

    if (lives == 0) {
        patterns_sweep();
        music_playscore(smb_gameover);

        level = 0;
        lives = GAME_START_LIVES;
    }

    ESP_LOGI(TAG, "Done!");
    playing = false;
}


void app_main(void) {
    ESP_LOGI(TAG, "Main startup");
    buttons_init();
    lights_init();
    music_init();
    status_init();
    random_init();

    // Execute the play_game() function when the play button is pressed.
    buttons_play_callback(play_game);
    // Loop music with lights via step_sequence();
    music_callback(step_sequence);

    patterns_sparkle();
    // Sparkle runs continiously while we wait for the user to press Play

    // TODO: Implement another thread for step_sequence() to loop
    // while playing = false. The thread should pause while playing = true,
    // then automatically switch back on when playing is false again.
}
