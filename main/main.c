#include <driver/ledc.h>
#include <esp_err.h>
#include <stdio.h>

#include "lights.h"
#include "music.h"
#include "config.h"
#include "smb.h"
#include "smb2.h"
#include "smb3.h"
#include "smw.h"

static const char *TAG = "starman";


void app_main(void)
{
    lights_init();
    music_init();

    music_playscore(smb_overworld);
    music_playscore(smb_underworld);
    music_playscore(smb_underwater);
    music_playscore(smb_castle);
    music_playscore(smb_fanfare);
    music_playscore(smb_ending);
}
