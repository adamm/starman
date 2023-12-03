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

#ifndef MUSIC_H
#define MUSIC_H

#define CMD_PLAYNOTE   0x90  /* play a note: low nibble is generator #, note is next byte */
#define CMD_STOPNOTE   0x80  /* stop a note: low nibble is generator # */
#define CMD_INSTRUMENT 0xC0  /* change instrument; low nibble is generator #, instrument is next byte */
#define CMD_RESTART    0XE0  /* restart the score from the beginning */
#define CMD_STOP       0XF0  /* stop playing */
#define CMD_TEMPO      0XFE  /* set tempo */

#define HDR_F1_VOLUME_PRESENT      0x80
#define HDR_F1_INSTRUMENTS_PRESENT 0x40
#define HDR_F1_PERCUSSION_PRESENT  0x20

typedef struct {  // the optional bytestream file header
    char id1;     // 'P'
    char id2;     // 't'
    unsigned char hdr_length; // length of whole file header
    unsigned char f1;         // flag byte 1
    unsigned char f2;         // flag byte 2
    unsigned char num_tgens;  // how many tone generators are used by this score
} music_header_t;

typedef enum {
   C1, Csharp1, D1, Dsharp1, E1, F1, Fsharp1, G1, Gsharp1, A1, Asharp1, B1,
   C2, Csharp2, D2, Dsharp2, E2, F2, Fsharp2, G2, Gsharp2, A2, Asharp2, B2,
   C3, Csharp3, D3, Dsharp3, E3, F3, Fsharp3, G3, Gsharp3, A3, Asharp3, B3,
   C4, Csharp4, D4, Dsharp4, E4, F4, Fsharp4, G4, Gsharp4, A4, Asharp4, B4,
   C5, Csharp5, D5, Dsharp5, E5, F5, Fsharp5, G5, Gsharp5, A5, Asharp5, B5,
   C6, Csharp6, D6, Dsharp6, E6, F6, Fsharp6, G6, Gsharp6, A6, Asharp6, B6,
   C7, Csharp7, D7, Dsharp7, E7, F7, Fsharp7, G7, Gsharp7, A7, Asharp7, B7,
   C8, Csharp8, D8, Dsharp8, E8, F8, Fsharp8, G8, Gsharp8, A8, Asharp8, B8,
   C9, Csharp9, D9, Dsharp9, E9, F9, Fsharp9, G9, Gsharp9, A9, Asharp9, B9,
   C10, Csharp10, D10, Dsharp10, E10, F10, Fsharp10, G10, Gsharp10, A10, Asharp10, B10,
   C11, Csharp11, D11, Dsharp11, E11, F11, Fsharp11, G11, Gsharp11, A11, Asharp11, B11,
   C12, Csharp12, D12, Dsharp12, E12, F12, Fsharp12, G12,
} music_notes_t;

typedef unsigned char byte;

void music_callback(bool (*callback)(uint32_t));
size_t music_get_score_length(const byte* score);
uint32_t music_playscore(const byte* score);
uint32_t music_playscore_at_time(const byte*, uint32_t);
uint8_t music_gettempo(void);
void music_settempo(uint8_t);
void music_amp_unmute(void);
void music_amp_mute(void);
void music_play_note(music_notes_t note, float time);
void music_init(void);
void music_stop(void);


#endif
