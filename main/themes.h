#pragma once

#include "music/smb.h"
#include "music/smb3.h"
#include "music/smw.h"
#include "patterns.h"

enum THEME_STAGE {
    THEME_STAGE_level_1,
    THEME_STAGE_level_2,
    THEME_STAGE_level_3,
    THEME_STAGE_level_4,
    THEME_STAGE_block,
    THEME_STAGE_1up,
    THEME_STAGE_powerup,
    THEME_STAGE_starman,
    THEME_STAGE_warning,
    THEME_STAGE_success,
    THEME_STAGE_clear,
    THEME_STAGE_death,
    THEME_STAGE_gameover,
    THEME_STAGE_fanfare,
    THEME_STAGE_ending,
};
#define NUM_THEME_STAGES 13

typedef struct theme {
    enum THEME_STAGE stage;
    const byte* score;
    void (*pattern)(void);
} theme_t;

typedef struct themes {
    const char*    title;
    const theme_t* theme;
} themes_t;

static const theme_t theme_smb[] = {
    { THEME_STAGE_level_1,  smb_overworld,    patterns_swipe },
    { THEME_STAGE_level_2,  smb_underworld,   patterns_lines },
    { THEME_STAGE_level_3,  smb_underwater,   patterns_waves },
    { THEME_STAGE_level_4,  smb_castle,       patterns_castle },
    { THEME_STAGE_block,    smb_block,        patterns_question },
    { THEME_STAGE_1up,      smb_1up,          patterns_checkered },
    { THEME_STAGE_powerup,  smb_powerup,      patterns_flash },
    { THEME_STAGE_starman,  smb_starman,      patterns_starman },
    { THEME_STAGE_warning,  smb_warning,      patterns_siren },
    { THEME_STAGE_success,  smb_flagpole,     patterns_sweep },
    { THEME_STAGE_clear,    smb_course_clear, patterns_radar },
    { THEME_STAGE_death,    smb_death,        patterns_spiral },
    { THEME_STAGE_gameover, smb_gameover,     patterns_gameover },
    { THEME_STAGE_fanfare,  smb_fanfare,      patterns_checkered },
    { THEME_STAGE_ending,   smb_ending,       patterns_diamonds },
};

static const theme_t theme_smb3[] = {
    { THEME_STAGE_level_1,  smb3_overworld,    patterns_swipe },
    { THEME_STAGE_level_2,  NULL,              patterns_lines },     // Null pointer bad!
    { THEME_STAGE_level_3,  smb3_underwater,   patterns_waves },
    { THEME_STAGE_level_4,  smb3_castle,       patterns_castle },
    { THEME_STAGE_block,    smb_block,         patterns_question },  // Fallback to SMB
    { THEME_STAGE_1up,      smb_1up,           patterns_checkered }, // Fallback to SMB
    { THEME_STAGE_powerup,  smb_powerup,       patterns_flash },     // Fallback to SMB       
    { THEME_STAGE_starman,  smb_starman,       patterns_starman },   // Fallback to SMB
    { THEME_STAGE_warning,  smb_warning,       patterns_siren },     // Fallback to SMB
    { THEME_STAGE_success,  NULL,              patterns_sweep },     // NULL pointer bad!
    { THEME_STAGE_clear,    smb3_course_clear, patterns_radar },
    { THEME_STAGE_death,    NULL,              patterns_spiral },    // NULL pointer bad!
    { THEME_STAGE_gameover, smb3_gameover,     patterns_gameover },
};

static const theme_t theme_smw[] = {
    { THEME_STAGE_level_1,  smw_overworld,    patterns_swipe },
    { THEME_STAGE_level_2,  smw_donut,        NULL },               // Null pointer bad!
    { THEME_STAGE_level_3,  smw_underwater,   patterns_waves },
    { THEME_STAGE_level_4,  smw_castle,       patterns_castle },
    { THEME_STAGE_block,    smb_block,        patterns_question },  // Fallback to SMB
    { THEME_STAGE_1up,      smb_1up,          patterns_checkered }, // Fallback to SMB
    { THEME_STAGE_powerup,  smb_powerup,      patterns_flash },     // Fallback to SMB
    { THEME_STAGE_starman,  smb_starman,      patterns_starman },   // Fallback to SMB
    { THEME_STAGE_warning,  smb_warning,      patterns_siren },     // Fallback to SMB
    { THEME_STAGE_success,  NULL,             patterns_sweep },     // Null pointer bad!
    { THEME_STAGE_clear,    smw_course_clear, patterns_radar },
    { THEME_STAGE_death,    smw_death,        patterns_spiral },
    { THEME_STAGE_gameover, smw_gameover,     patterns_gameover },
};

// XXX: The null pointers above will cause a crash.  For now leave the theme
// commented out and disabled below until the music/pattern can be populated

static const themes_t themes[] = {
    { "SMB",  theme_smb },
//  { "SMB3", theme_smb3 },
//  { "SMW",  theme_smw },
};
