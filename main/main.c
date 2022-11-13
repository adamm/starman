#include <esp_err.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_netif.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "buttons.h"
#include "config.h"
#include "display.h"
#include "music.h"
#include "ota.h"
#include "patterns.h"
#include "patterns_gol.h"
#include "random.h"
#include "rgb.h"
#include "sparkle.h"
#include "storage.h"
#include "smb.h"
#include "smb2.h"
#include "smb3.h"
#include "smw.h"
#include "text.h"
#include "wifi_manager.h"
#include "util.h"

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
static uint8_t lives = 3;
static bool playing  = false;
static uint32_t player_gets_star    = 0;
static uint32_t player_gets_1up     = 0;
static uint32_t player_gets_warning = 0;
static uint32_t player_gets_ending  = 0;
static uint32_t player_dies         = 0;

void monitoring_task(void *pvParameter)
{
    for(;;){
        ESP_LOGI(TAG, "free heap: %d",esp_get_free_heap_size());
        vTaskDelay( pdMS_TO_TICKS(10000) );
    }
}


bool step_sequence(uint32_t time) {
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

    // Allow the music to continue
    return false;
}


void play_game(void) {
    // Ignore subsequent play_game() calls if game is already playing
    if (playing == false)
        playing = true;
    else
        return;

    music_amp_unmute();

    level++;
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
        level_pattern = patterns_swipe;
        level_music = smb_overworld;
        length = sizeof(smb_overworld);
    }
    else if (level == 2) {
        level_pattern = patterns_lines;
        level_music = smb_underworld;
        length = sizeof(smb_underworld);
    }
    else if (level == 3) {
        level_pattern = patterns_waves;
        level_music = smb_underwater;
        length = sizeof(smb_underwater);
    }
    else if (level == 4) {
        level_pattern = patterns_castle;
        level_music = smb_castle;
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

    // Ignore the song header when calculating the song length
    length -= 6;

    // Now that the music has been identified, pick a random location in
    // the song to stop and play the 1up, starman, time warning, or die music.
    if (player_gets_1up)
        player_gets_1up = random_value_within_int(length);
    if (player_gets_star)
        player_gets_star = random_value_within_int(length);
    if (player_gets_warning)
        // The warning music should be within 900 notes from the end of the level
        player_gets_warning = random_value_within_int(900) + length - 900;
    if (player_dies)
        player_dies = random_value_within_int(length);


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

    uint32_t stopped_music_time = 0;
    music_settempo(100);

    // If the user is destined to get a star, 1up, or time warning, play the
    // sound effect queue, then resume the normal level music.
    while (stopped_music_time < length) {
        level_pattern();
        stopped_music_time = music_playscore_at_time(level_music, stopped_music_time);

        if (player_gets_star && player_gets_star <= stopped_music_time) {
            player_gets_star = 0;
            patterns_question();
            music_playscore(smb_block);
            patterns_flash();
            music_playscore(smb_powerup);
            patterns_swoosh();
            music_playscore(smb_starman);
        }

        if (player_gets_1up && player_gets_1up <= stopped_music_time) {
            lives++;
            player_gets_1up = 0;
            patterns_question();
            music_playscore(smb_block);
            patterns_checkered();
            music_playscore(smb_1up);
        }

        if (player_gets_warning && player_gets_warning <= stopped_music_time) {
            player_gets_warning = 0;
            patterns_siren();
            music_playscore(smb_warning);
            music_settempo(150);
        }

        // If the user is destined to die, the level music will not resume
        if (player_dies && player_dies <= stopped_music_time)
            break;
    }

    music_settempo(100);

    if (player_dies) {
        lives--;
        level--; // level gets incremented when starting play --
                 //d ecrementing means user needs to replay the same level again
        player_dies = 0;
        patterns_spiral();
        music_playscore(smb_death);
    }
    else if (level == 4) {
        patterns_checkered();
        music_playscore(smb_fanfare);

        if (player_gets_ending) {
            player_gets_ending = 0;
            patterns_diamonds();
            music_playscore(smb_ending);
        }
        level = 0;
        lives = GAME_START_LIVES;
    }
    else {
        patterns_sweep();
        music_playscore(smb_flagpole);
        patterns_radar();
        music_playscore(smb_course_clear);
    }

    if (lives == 0) {
        patterns_gameover();
        music_playscore(smb_gameover);
        patterns_gameover_stop();

        level = 0;
        lives = GAME_START_LIVES;
    }

    music_amp_mute();

    ESP_LOGI(TAG, "Done!");
    playing = false;

    sparkle_start();
}
    

void wifi_connected(void *pvParameter) {
    ip_event_got_ip_t* param = (ip_event_got_ip_t*)pvParameter;

    /* transform IP to human readable string */
    char ip_str[20] = {0};
    ip_str[0] = ' ';
    ip_str[1] = ' ';
    ip_str[2] = ' ';
    esp_ip4addr_ntoa(&param->ip_info.ip, ip_str+3, IP4ADDR_STRLEN_MAX);

    ESP_LOGI(TAG, "I have a connection and my IP is %s!", ip_str);

    ota_init();
    ota_upgrade(); // Combines checking if an upgrade exists, downloading, and installing it.

    // If OTA update is successful, the device reboots.  Otherwise, continue
    // to show the current IP and start the normal game process.

    // Leaving a little space infront of the IP address makes it easier to read when scrolling.
    sparkle_scroll_string(ip_str);
}


void app_main(void) {
    ESP_LOGI(TAG, "Main startup");
    config_init();
    storage_init();
    buttons_init();
    display_init();
    music_init();
    random_init();
    // rgb_init();

    // Execute the play_game() function when the play button is pressed.
    buttons_play_callback(play_game);
    // Loop music with lights via step_sequence();
    music_callback(step_sequence);

    // Sparkle runs continiously in another thread while we wait for the user
    // to press Play
    sparkle_start();

    // Wifi is last as it can take a few moments -- this way the sparkle and
    // game can begin even without wifi being ready.
    wifi_manager_start();
    wifi_manager_set_callback(WM_EVENT_STA_GOT_IP, &wifi_connected);

    xTaskCreatePinnedToCore(&monitoring_task, "monitoring_task", 2048, NULL, 1, NULL, 1);
}
