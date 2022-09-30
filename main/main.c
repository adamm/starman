#include <driver/ledc.h>
#include <esp_err.h>
#include <stdio.h>

#include "lights.h"
#include "music.h"
#include "config.h"
#include "smb.h"

static const char *TAG = "starman";


void app_main(void)
{
    lights_init();
    music_init();

    music_playscore(smb_overworld);
}
