#include <esp_err.h>
#include <esp_log.h>

#include "buttons.h"
#include "config.h"
#include "lights.h"
#include "music.h"
#include "patterns.h"
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

 
bool step_sequence(uint32_t time) {
    patterns_step_sequence();

    // TODO: Return true to stop the music early (ie, player got a star, 1up, died, etc.)
    return false;
}


void play_game(void) {
    static bool playing = 0;

    // Ignore subsequent play_game() calls if game is already playing
    if (playing == false)
        playing = true;
    else
        return;

    // TODO: Implement the "How to play" logic as described above, including the PLAY button interrupt

    patterns_sprinkle();
    music_callback(step_sequence);
    music_playscore(smb_overworld);

    patterns_lines();
    music_playscore(smb_underworld);

    patterns_waves();
    music_playscore(smb_underwater);

    patterns_siren();
    music_playscore(smb_castle);

    patterns_checkered();
    music_playscore(smb_fanfare);

    patterns_diamonds();
    music_playscore(smb_ending);

    ESP_LOGI(TAG, "Done!");
    playing = false;

    // TODO: Go back to sparkle pattern, no music.  Wait for the user to press Play button again
}


void app_main(void) {
    ESP_LOGI(TAG, "Main startup");
    lights_init();
    music_init();
    status_init();
    buttons_init();

    // Execute the play_game() function when the play button is pressed.
    buttons_play_callback(play_game);

    patterns_sparkle();
    // Sparkle runs continiously while we wait for the user to press Play
}
