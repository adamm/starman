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

typedef unsigned char byte;

void music_callback(bool (*callback)(uint32_t));
size_t music_get_score_length(const byte* score);
uint32_t music_playscore(const byte* score);
uint32_t music_playscore_at_time(const byte*, uint32_t);
uint8_t music_gettempo(void);
void music_settempo(uint8_t);
void music_amp_unmute(void);
void music_amp_mute(void);
void music_init(void);
void music_stop(void);


#endif
