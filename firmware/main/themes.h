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

#ifndef THEMES_H
#define THEMES_H

#include "game.h"
#include "stage.h"

typedef struct theme {
    enum STAGE stage;
    const unsigned char* score;
    void (*pattern)(void);
} theme_t;

typedef struct themes {
    const char*    title;
    const theme_t* theme;
    const uint8_t  total_stages;
    void (*game_start)(void);
    bool (*game_step)(uint32_t);
} themes_t;

// Music only should be included by game.c, not by other files that are only
// interested in the theme struct and total themes available
//
// One solution would be to move the below block to a new themes.c, but
// it's bad practice to call "#include <theme.c>" from game.c.
#ifdef INCLUDE_THEME_MUSIC_AND_PATTERNS

#include "music/smb.h"
#include "music/smb2.h"
#include "music/smb3.h"
#include "music/smw.h"
#include "music/loz.h"
#include "music/tet.h"
#include "patterns.h"

const theme_t theme_smb[] = {
    { STAGE_level_1,  smb_overworld,    patterns_swipe },
    { STAGE_level_2,  smb_underworld,   patterns_lines },
    { STAGE_level_3,  smb_underwater,   patterns_waves },
    { STAGE_level_4,  smb_castle,       patterns_castle },
    { STAGE_block,    smb_block,        patterns_question },
    { STAGE_1up,      smb_1up,          patterns_checkered },
    { STAGE_powerup,  smb_powerup,      patterns_flash },
    { STAGE_starman,  smb_starman,      patterns_starman },
    { STAGE_warning,  smb_warning,      patterns_siren },
    { STAGE_success,  smb_flagpole,     patterns_sweep },
    { STAGE_clear,    smb_course_clear, patterns_radar },
    { STAGE_death,    smb_death,        patterns_spiral },
    { STAGE_gameover, smb_gameover,     patterns_gameover },
    { STAGE_fanfare,  smb_fanfare,      patterns_checkered },
    { STAGE_ending,   smb_ending,       patterns_diamonds },
};

const theme_t theme_smb2[] = {
    { STAGE_level_1,  smb2_intro,       patterns_sweep },
    { STAGE_level_2,  smb2_overworld,   patterns_swipe },
    { STAGE_level_3,  smb2_underworld,  patterns_lines },
    { STAGE_level_4,  smb2_boss,        patterns_castle },
    { STAGE_block,    smb_block,        patterns_question },  // Fallback to SMB
    { STAGE_1up,      smb_1up,          patterns_checkered }, // Fallback to SMB
    { STAGE_powerup,  smb_powerup,      patterns_flash },     // Fallback to SMB
    { STAGE_starman,  smb_starman,      patterns_starman },   // Fallback to SMB
    { STAGE_warning,  smb_warning,      patterns_siren },     // Fallback to SMB
    { STAGE_success,  smb_flagpole,     patterns_sweep },     // Fallback to SMB
    { STAGE_clear,    smb2_clear,       patterns_radar },
    { STAGE_death,    smb2_death,       patterns_spiral },
    { STAGE_gameover, smb2_gameover,    patterns_gameover },
    { STAGE_fanfare,  smb2_fanfare,     patterns_checkered },
    { STAGE_ending,   smb2_ending,      patterns_diamonds },
};

const theme_t theme_smb3[] = {
    { STAGE_level_1,  smb3_overworld,   patterns_swipe },
    { STAGE_level_2,  smb3_athletic,    patterns_swoosh },
    { STAGE_level_3,  smb3_underwater,  patterns_waves },
    { STAGE_level_4,  smb3_castle,      patterns_castle },
    { STAGE_block,    smb_block,        patterns_question },  // Fallback to SMB
    { STAGE_1up,      smb_1up,          patterns_checkered }, // Fallback to SMB
    { STAGE_powerup,  smb_powerup,      patterns_flash },     // Fallback to SMB       
    { STAGE_starman,  smb_starman,      patterns_starman },   // Fallback to SMB
    { STAGE_warning,  smb_warning,      patterns_siren },     // Fallback to SMB
    { STAGE_success,  smb_block,        patterns_sweep },     // No real "success" sound, so instead just bump the block
    { STAGE_clear,    smb3_course_clear,patterns_radar },
    { STAGE_death,    smb_death,        patterns_spiral },    // Fallback to SMB
    { STAGE_gameover, smb3_gameover,    patterns_gameover },
    { STAGE_fanfare,  smb3_fanfare,     patterns_checkered },
    { STAGE_ending,   smb3_ending,       patterns_diamonds },
};

const theme_t theme_smw[] = {
    { STAGE_level_1,  smw_overworld,    patterns_swipe },
    { STAGE_level_2,  smw_athletic,     patterns_swoosh },    // Get better pattern
    { STAGE_level_3,  smw_underwater,   patterns_waves },
    { STAGE_level_4,  smw_castle,       patterns_castle },
    { STAGE_block,    smb_block,        patterns_question },  // Fallback to SMB
    { STAGE_1up,      smb_1up,          patterns_checkered },
    { STAGE_powerup,  smb_powerup,      patterns_flash },     // Fallback to SMB
    { STAGE_starman,  smw_bonus,        patterns_starman },
    { STAGE_warning,  smb_warning,      patterns_siren },     // Fallback to SMB
    { STAGE_success,  smb_block,        patterns_sweep },     // Fallback to SMB
    { STAGE_clear,    smw_course_clear, patterns_radar },
    { STAGE_death,    smw_death,        patterns_spiral },
    { STAGE_gameover, smw_gameover,     patterns_gameover },
    { STAGE_fanfare,  smw_world_clear,  patterns_checkered },
    { STAGE_ending,   smw_ending,       patterns_diamonds },
};

const theme_t theme_loz[] = {
    { STAGE_level_1,  loz_overworld,    patterns_swipe },
    { STAGE_level_2,  loz_kariki,       patterns_sweep },
    { STAGE_level_3,  loz_lost_woods,   patterns_swoosh },
    { STAGE_level_4,  loz_castle,       patterns_castle },
    { STAGE_block,    smb_block,        patterns_sweep },
    { STAGE_1up,      loz_secret,       patterns_checkered },
    { STAGE_success,  loz_treasure_chest, patterns_sweep },
    { STAGE_clear,    loz_item,         patterns_radar },
    { STAGE_death,    loz_death,        patterns_spiral },
    { STAGE_gameover, loz_gameover,     patterns_gameover },
    { STAGE_fanfare,  loz_get_master_sword, patterns_checkered },
    { STAGE_ending,   loz_ending,       patterns_diamonds },
};

const theme_t theme_tet[] = {
    { STAGE_level_1,  tet_theme_a,      patterns_swipe },
    { STAGE_level_2,  tet_theme_b,      patterns_sweep },
    { STAGE_level_3,  tet_theme_c,      patterns_swoosh },
    { STAGE_gameover, tet_game_over,    patterns_spiral },
};

#else

extern const theme_t theme_smb[];
extern const theme_t theme_smb2[];
extern const theme_t theme_smb3[];
extern const theme_t theme_smw[];
extern const theme_t theme_loz[];
extern const theme_t theme_tet[];

#endif

static const themes_t themes[] = {
    { "SMB",  theme_smb,  15, game_smb_start, game_smb_step_sequence },
    { "SMB2", theme_smb2, 15, game_smb_start, game_smb_step_sequence },
    { "SMB3", theme_smb3, 15, game_smb_start, game_smb_step_sequence },
    { "SMW",  theme_smw,  15, game_smb_start, game_smb_step_sequence },
    { "LOZ",  theme_loz,  12, game_loz_start, game_loz_step_sequence },
    { "TET",  theme_tet,   4, game_tet_start, game_tet_step_sequence },
};

#define TOTAL_THEMES_AVAILABLE 6

const theme_t* themes_load_stage(const char* title, enum STAGE stage);

#endif
