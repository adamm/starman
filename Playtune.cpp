/*********************************************************************************************

     Playtune: An Arduino polyphonic music generator
     ESP32 version

                         About Playtune, generally

   Playtune is a family of music players for Arduino-like microcontrollers. They
   each intepret a bytestream of commands that represent a polyphonic musical
   score, and play it using different techniques.

   (1) The original Playtune, first released in 2011, uses a separate hardware timer
   to generate a square wave for each note played simultaneously. The timers run at twice
   the frequency of the note being played, and the interrupt routine flips the output bit.
   It can play only as many simultaneous notes as there are timers available. The sound
   quality? Buzzy square waves.
   https://github.com/LenShustek/arduino-playtune

   (2) The second ("polling") version uses only one hardware timer that interrupts often,
   by default at 20 Khz, or once every 50 microseconds. The interrupt routine determines
   which, if any, of the currently playing notes need to be toggled. It also implements
   primitive volume modulation by changing the duty cycle of the square wave.
   The advantage over the first version is that the number of simultaneous notes is not
   limited by the number of timers, only by the number of output pins. The sound quality
   is still "buzzy square waves".
   https://github.com/LenShustek/playtune_poll

   (3) The third version also uses only one hardware timer interrupting frequently, but
   uses the hardware digital-to-analog converter on high-performance microntrollers like
   the Teensy to generate an analog wave that is the sum of stored samples of sounds. The
   samples are scaled to the right frequency and volume, and any number of instrument
   samples can be used and mapped to MIDI patches. The sound quality is much better,
   although not in league with real synthesizers. It currently only supports Teensy.
   https://github.com/LenShustek/playtune_samp

   (4) The fourth version is an audio object for the PJRC Audio Library.
   https://www.pjrc.com/teensy/td_libs_Audio.html
   It allows up to 16 simultaneous sound generators that are internally mixed, at
   the appropriate volume, to produce one monophonic audio stream.
   Sounds are created from sampled one-cycle waveforms for any number of instruments,
   each with its own attack-hold-decay-sustain-release envelope. Percussion sounds
   (from MIDI channel 10) are generated from longer sampled waveforms of a complete
   instrument strike. Each generator's volume is independently adjusted according to
   the MIDI velocity of the note being played before all channels are mixed.
   www.github.com/LenShustek/Playtune_synth

   (5) This fifth version is for the Teensy 3.1/3.2, and uses the four Periodic Interval
   Timers in the Cortex M4 processor to support up to 4 simultaneous notes.
   It uses less CPU time than the polling version, but is limited to 4 notes at a time.
   (This was written to experiment with multi-channel multi-Tesla Coil music playing,
   where I use Flexible Timer Module FTM0 for generating precise one-shot pulses.
   But I ultimately switched to the polling version to play more simultaneous notes.)
   www.github.com/LenShustek/Playtune_Teensy

   For all versions, once a score starts playing, the processing happens in
   the interrupt routine.  Any other "real" program can be running at the same time
   as long as it doesn't use the timers or the output pins that Playtune is using.

   Each timer (tone generator) can be associated with any digital output pin,
   not just the pins that are internally connected to the timer. The exception is
   when the Teensy version is compiled for playing music on Tesla coils, in which
   case the outputs must be on digital pins 20, 21, 22, and 23.

   The easiest way to hear the music is to connect each of the output pins to a resistor
   (500 ohms, say).  Connect other ends of the resistors together and then to one
   terminal of an 8-ohm speaker.  The other terminal of the speaker is connected to
   ground.  No other hardware is needed!  But using an amplifier is nicer.

  ****  The public Playtune interface  ****

  There are fivb public functions and one public variable.

  void tune_initchan(byte pin)

    Call this to initialize each of the tone generators you want to use.  The argument
    says which pin to use as output.  The pin numbers are the digital "D" pins
    silkscreened on the Arduino board.  Calling this more times than your processor
    has timers will do no harm, but will not help either.

  void tune_playscore(byte *score)

    Call this pointing to a "score bytestream" to start playing a tune.  It will
    only play as many simultaneous notes as you have initialized tone generators;
    any more will be ignored.  See below for the format of the score bytestream.

  boolean tune_playing

    This global variable will be "true" if a score is playing, and "false" if not.
    You can use this to see when a score has finished.

  void tune_stopscore()

    This will stop a currently playing score without waiting for it to end by itself.

  void tune_delay(unsigned int msec)

    Delay for "msec" milliseconds.  This is provided for compatibility with other versions
    of Playtune that break the "delay' function if you use all of your processor's
    timers for generating tones. (This one doesn't, and delay() works fine.)

  void tune_stopchans()

    This disconnects all the timers from their pins and stops the interrupts.
    Do this when you don't want to play any more tunes.

  void tune_speed(unsigned int percent)

    New for the Teensy version, this changes playback speed to the specified percent
    of normal. The minimum is percent=20 (1/5 slow speed) and the maximum is
    percent=500 (5x fast speed).

   *****  The score bytestream  *****

   The bytestream is a series of commands that can turn notes on and off, and can
   start a waiting period until the next note change.  Here are the details, with
   numbers shown in hexadecimal.

   If the high-order bit of the byte is 1, then it is one of the following commands:

     9t nn  Start playing note nn on tone generator t.  Generators are numbered
            starting with 0.  The notes numbers are the MIDI numbers for the chromatic
            scale, with decimal 60 being Middle C, and decimal 69 being Middle A
            at 440 Hz.  The highest note is decimal 127 at about 12,544 Hz. except
            that percussion notes (instruments, really) range from 128 to 255 when
            relocated from track 9 by Miditones with the -pt option. This version of
            Playtune ignores those percussion notes.

      [vv]  If ASSUME_VOLUME is set to 1, or the file header tells us to,
            then we expect a third byte with the volume ("velocity") value from 1 to
            127. You can generate this from Miditones with the -v option.
            (Everything breaks for headerless files if the assumption is wrong!)
            This version of Playtune ignores volume information.

     8t     Stop playing the note on tone generator t.

     Ct ii  Change tone generator t to play instrument ii from now on.  Miditones will
            generate this with the -i option. This version of Playtune gives that to
            the Tesla coil routine, if so compiled, so it can decide which coil(s)
            to play the note on.

     F0     End of score: stop playing.

     E0     End of score: start playing again from the beginning.

   If the high-order bit of the byte is 0, it is a command to wait.  The other 7 bits
   and the 8 bits of the following byte are interpreted as a 15-bit big-endian integer
   that is the number of milliseconds to wait before processing the next command.
   For example,

     07 D0

   would cause a wait of 0x07d0 = 2000 decimal millisconds or 2 seconds.  Any tones
   that were playing before the wait command will continue to play.

   Playtune bytestream files generated by later versions of the Miditones progam using
   the -d option begin with a small header that describe what optional data is present
   in the file. This makes the file more self-describing, and this version of Playtune
   uses that if it is present.

    'Pt'   2 ascii characters that signal the presence of the header
     nn    The length (in one byte) of the entire header, 6..255
     ff1   A byte of flag bits, three of which are currently defined:
               80 velocity information is present
               40 instrument change information is present
               20 translated percussion notes are present
     ff2    Another byte of flags, currently undefined
     tt     The number (in one byte) of tone generators actually used in this music.
            We use that the scale the volume when combining simulatneous notes.

   Any subsequent header bytes covered by the count, if present, are currently undefined
   and are ignored.The score is stored in Flash memory ("PROGMEM") along with the program, because
   there's a lot more of that than data memory.


   ****  Where does the score data come from?  ****

   Well, you can write the score by hand from the instructions above, but that's
   pretty hard.  An easier way is to translate MIDI files into these score commands,
   and I've written a program called "Miditones" to do that.  See the separate
   documentation for that program, which is also open source at
   https://github.com/lenshustek/miditones

   ****  Nostalgia from me  ****

   Writing Playtune was a lot of fun, because it essentially duplicates what I did
   as a graduate student at Stanford University in about 1973.  That project used the
   then-new Intel 8008 microprocessor, plus three hardware square-wave generators that
   I built out of 7400-series TTL.  The music compiler was written in Pascal and read
   scores that were hand-written in a notation I made up, which looked something like
   this:  C  Eb  4G  8G+  2R  +  F  D#
   This was before MIDI had been invented, and anyway I wasn't a pianist so I would
   not have been able to record my own playing.  I could barely read music well enough
   to transcribe scores, but I slowly did quite a few of them. MIDI is better!

   Len Shustek, originally 4 Feb 2011, then updated occasionally over the years.

  ------------------------------------------------------------------------------------
   The MIT License (MIT)
  Copyright (c) 2011, 2016, 2021 Len Shustek

  Permission is hereby granted, free of charge, to any person obtaining a copy of
  this software and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the rights to use,
  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
  Software, and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.
**************************************************************************************/
/*
  Change log
   19 January 2011, L.Shustek, V1.0
      - Initial release, inspired by Brett Hagman's Tone Generator Library,
        https://github.com/bhagman/Tone
   23 February 2011, L. Shustek, V1.1
      - prevent hang if delay rounds to count of 0
    4 December 2011, L. Shustek, V1.2
      - add special TESLA_COIL mods
   10 June 2013, L. Shustek, V1.3
      - change for compatibility with Arduino IDE version 1.0.5
    6 April 2015, L. Shustek, V1.4
      - change for compatibility with Arduino IDE version 1.6.x
   28 May 2016, T. Wasiluk
      - added support for ATmega32U4
   10 July 2016, Nick Shvelidze
      - Fixed include file names for Arduino 1.6 on Linux.
   15 August 2016, L. Shustek,
      - Fixed a timing error: T Wasiluk's change to using a 16-bit timer instead
        of an 8-bit timer for score waits exposed a old bug that was in the original
        Brett Hagman code: when writing the timer OCR value, we need to clear the
        timer counter, or else (the manual says) "the counter [might] miss the compare
        match...and will have to count to its maximum value (0xFFFF) and wrap around
        starting at 0 before the compare match can occur". This caused an error that
        was small and not noticeable for the 8-bit timer, but could be hundreds of
        milliseconds for the 16-bit counter. Thanks go to Joey Babcock for pushing me
        to figure out why his music sounded weird, and for discovering that it worked
        ok with the 2013 version that used the 8-bit timer for score waits.
      - Support the optional bytestream header to recognize when volume data is present.
      - Parse and ignore instrument change data.
      - Various reformatting to make it easier to read.
      - Allow use of the fourth timer on the ATmega32U4 (Micro, Leonardo)
      - Change to the more permissive MIT license.
   20 March 2021, L. Shustek,
      - Rewrite for Teensy 3.2 using the four K20 interval timers, in support of
        playing music on multiple Tesla coils reepresenting different instruments.
      - Make Playtune not a C++ object, which is causing constructor/destructor
        delay and interlocks. (Can you tell I hate C++?)
      - Add tune_speed() to adjust playing speed without changing tones.
      - Reformat in a more compact linguistic style.

  -----------------------------------------------------------------------------------------*/

#include <Arduino.h>
#include "Playtune.h"

#ifndef DBUG
#define DBUG 1          // debugging?
#endif
#define ASSUME_VOLUME 0    // assume volume information is present in bytestream files without headers?
#define TESLA_COIL 0       // special Tesla Coil version?
#define AVAILABLE_TIMERS 4 // what the Cortex M4 on the Teensy 3.2 MK20DX256VLH7 processor has

struct file_hdr_t {  // the optional bytestream file header
  char id1;     // 'P'
  char id2;     // 't'
  unsigned char hdr_length; // length of whole file header
  unsigned char f1;         // flag byte 1
  unsigned char f2;         // flag byte 2
  unsigned char num_tgens;  // how many tone generators are used by this score
} file_header;
#define HDR_F1_VOLUME_PRESENT 0x80
#define HDR_F1_INSTRUMENTS_PRESENT 0x40
#define HDR_F1_PERCUSSION_PRESENT 0x20

// timer ports and masks

hw_timer_t* timer0 = NULL;
hw_timer_t* timer1 = NULL;
hw_timer_t* timer2 = NULL;
hw_timer_t* timer3 = NULL;
hw_timer_t* timers[4] = {timer0, timer1, timer2, timer3 };
void ISRtimer0(void);
void ISRtimer1(void);
void ISRtimer2(void);
void ISRtimer3(void);
void (*ISRs[4])(void) = {&ISRtimer0, &ISRtimer1, &ISRtimer2, &ISRtimer3 };
// the first interval timer is also used to time score waits, whether or not that timer is playing a note

//  local varables

byte _tune_pins[AVAILABLE_TIMERS];
byte _tune_volume[AVAILABLE_TIMERS] = {0 }; // last volume for each timer
byte _tune_num_chans = 0;
unsigned _tune_speed = 100;

volatile unsigned wait_timer_frequency8;       /* its current frequency x 8 */
volatile boolean wait_timer_playing = false;   /* is it currently playing a note? */
volatile unsigned long wait_toggle_count;      /* countdown score waits */

volatile const byte *score_start = 0;
volatile const byte *score_cursor = 0;
volatile boolean tune_playing = false;
boolean volume_present = ASSUME_VOLUME;

void (*callback_func)(void) = NULL;
// Table of midi note frequencies times 8
//   They are times 8 for greater accuracy.
//   Generated from Excel by =ROUND(8*440/32*(2^((x-9)/12)),0) for 0<x<128

const uint32_t PROGMEM tune_frequencies8_PGM[128] = {
   65, 69, 73, 78, 82, 87, 92, 98, 104, 110, 117, 123, 131, 139,
   147, 156, 165, 175, 185, 196, 208, 220, 233, 247, 262, 277, 294,
   311, 330, 349, 370, 392, 415, 440, 466, 494, 523, 554, 587, 622,
   659, 698, 740, 784, 831, 880, 932, 988, 1047, 1109, 1175, 1245,
   1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976, 2093, 2217, 2349,
   2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951, 4186, 4435,
   4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459, 7902, 8372,
   8870, 9397, 9956, 10548, 11175, 11840, 12544, 13290, 14080,
   14917, 15804, 16744, 17740, 18795, 19912, 21096, 22351, 23680,
   25088, 26580, 28160, 29834, 31609, 33488, 35479, 37589, 39824,
   42192, 44701, 47359, 50175, 53159, 56320, 59669, 63217, 66976,
   70959, 75178, 79649, 84385, 89402, 94719, 100351 };

void tune_playnote (byte chan, byte note);
void tune_stopnote (byte chan);
void tune_stepscore (void);

#if TESLA_COIL
void teslacoil_rising_edge(byte channel);
byte teslacoil_checknote(byte channel, byte note);
void teslacoil_change_instrument(byte channel, byte instrument);
void teslacoil_change_volume(byte channel, byte volume);
#endif

void tune_callback(void (*callback)(void)) {
  callback_func = callback;
}

//------------------------------------------------------
// Initialize a music channel on a specific output pin
//------------------------------------------------------
// this is defined in pins_teensy.c:
extern const uint8_t PROGMEM digital_pin_table_PGM[];
void tune_initchan(byte pin) {
  if (_tune_num_chans < AVAILABLE_TIMERS) {
    byte chan = _tune_num_chans;
    _tune_pins[chan] = pin;
    pinMode(pin, OUTPUT);
#if DBUG
    Serial.print("init pin "); Serial.print(pin);
    Serial.print(" on channel "); Serial.println(chan);
#endif
    timers[chan] = timerBegin(chan, 80, true);
    timerAttachInterrupt(timers[chan], ISRs[chan], true);
    timerStop(timers[chan]);
    _tune_num_chans++;
  }
}

//-----------------------------------------------
// Start playing a note on a particular channel
//-----------------------------------------------
unsigned long stackpointer(void) {
  register unsigned long int stackptr asm("sp");
  return (unsigned long) stackptr;
}

void tune_playnote (byte chan, byte note) {
#if DBUG
  Serial.print ("Play at ");
  Serial.print(score_cursor - score_start, HEX);
  Serial.print(", ch");
  Serial.print(chan); Serial.print(' ');
  Serial.print(note, HEX);
#endif
  if (chan < _tune_num_chans) {
    #if TESLA_COIL
    note = teslacoil_checknote(chan, note);  // let teslacoil modify the note
    #endif
    if (note > 127) note = 127;
    unsigned int frequency8 = pgm_read_dword (tune_frequencies8_PGM + note);
    if (chan == 0) {
      wait_timer_frequency8 = frequency8;
      wait_timer_playing = true; }
    // for Tesla coils, interrupt at the true note frequency, not twice that
    unsigned int interval = (TESLA_COIL ? 8000000UL : 4000000UL) / frequency8;
    timerAlarmWrite(timers[chan], interval, true); // usec period
    timerAlarmEnable(timers[chan]);
    timerRestart(timers[chan]);
#if DBUG
    Serial.print(" usec "); Serial.print(interval);
#endif
  }
#if DBUG
  Serial.println();
#endif
}

//-----------------------------------------------
// Stop playing a note on a particular channel
//-----------------------------------------------

void tune_stopnote (byte chan) {
#if DBUG
  Serial.print ("Stop note ");
  Serial.println(chan, DEC);
#endif
  if (chan < _tune_num_chans) {
#if !TESLA_COIL
    byte pin = _tune_pins[chan];
    digitalWrite(pin, 0);
#endif
    if (chan == 0)
      wait_timer_playing = false;
    else { // if not the channel we use to time waits,
      timerStop(timers[chan]);
      timerAlarmDisable(timers[chan]);
    }
  }
}

//-----------------------------------------------
// Stop playing a score
//-----------------------------------------------

void tune_stopscore (void) {
  timerStop(timers[0]);
  timerAlarmDisable(timers[0]);
  for (int i = 0; i < _tune_num_chans; ++i)
    tune_stopnote(i);
  tune_playing = false;
}

//-----------------------------------------------
// Start playing a score
//-----------------------------------------------

void tune_playscore (const byte *score) {
  if (tune_playing) tune_stopscore();
  tune_playnote (0, 60);  /* start and stop channel 0 on middle C so wait works */
  tune_stopnote (0);
  score_start = score;
  volume_present = ASSUME_VOLUME;

  // look for the optional file header
  memcpy_P(&file_header, score, sizeof(file_hdr_t)); // copy possible header from PROGMEM to RAM
  if (file_header.id1 == 'P' && file_header.id2 == 't') { // validate it
    volume_present = file_header.f1 & HDR_F1_VOLUME_PRESENT;
#if DBUG
    Serial.print("header: volume_present="); Serial.println(volume_present);
#endif
    score_start += file_header.hdr_length; // skip the whole header
  }
  score_cursor = score_start;
  tune_playing = true;
  tune_stepscore();  /* execute initial commands */
}

void tune_speed (unsigned speed) {
  if (speed > 500) speed = 500; // pin between 1/5 and 5x speed
  if (speed < 20) speed = 20;
  _tune_speed = speed;
}

void tune_stepscore (void) {
  byte cmd, opcode, chan, note;
  unsigned duration;
  /* Do score commands until a "wait" is found, or the score is stopped.
     This is called initially from tune_playscore, but then is called
     from the interrupt routine when waits expire.
   */
#define CMD_PLAYNOTE  0x90  /* play a note: low nibble is generator #, note is next byte */
#define CMD_STOPNOTE  0x80  /* stop a note: low nibble is generator # */
#define CMD_INSTRUMENT  0xc0 /* change instrument; low nibble is generator #, instrument is next byte */
#define CMD_RESTART 0xe0  /* restart the score from the beginning */
#define CMD_STOP  0xf0  /* stop playing */
  /* if CMD < 0x80, then the other 7 bits and the next byte are a 15-bit big-endian number of msec to wait */
  while (1) {
    cmd = pgm_read_byte(score_cursor++);
    if (cmd < 0x80) { /* wait count in msec. */
      duration = ((unsigned)cmd << 8) | (pgm_read_byte(score_cursor++));
      if (_tune_speed != 100)
        duration = (unsigned) (((unsigned long)duration * 100UL) / _tune_speed);
#if TESLA_COIL // count for true frequency
      wait_toggle_count = ((unsigned long) wait_timer_frequency8 * duration  + 8 * 500) / (8 * 1000);
#else // count for double frequency
      wait_toggle_count = ((unsigned long) wait_timer_frequency8 * duration  + 4 * 500) / (4 * 1000);
#endif
      if (wait_toggle_count == 0) wait_toggle_count = 1;
#if DBUG
      Serial.print("wait "); Serial.print(duration);
      Serial.print("ms, cnt ");
      Serial.print(wait_toggle_count); Serial.print(" freq8 "); Serial.println(wait_timer_frequency8);
#endif
      break;
    }
    opcode = cmd & 0xf0;
    chan = cmd & 0x0f;
    if (opcode == CMD_STOPNOTE) { /* stop note */
      tune_stopnote (chan);
    }
    else if (opcode == CMD_PLAYNOTE) { /* play note */
      note = pgm_read_byte(score_cursor++); // argument evaluation order is undefined in C!
      if (volume_present) {
#if TESLA_COIL
        byte volume = pgm_read_byte(score_cursor);
#if DBUG
        Serial.print("chan "); Serial.print(chan); Serial.print(" volume "); Serial.println(volume);
#endif
        if (chan < _tune_num_chans && _tune_volume[chan] != volume) { // if volume is changing
          _tune_volume[chan] = volume;
          teslacoil_change_volume(chan, volume); }
#endif
        ++score_cursor; // ignore volume if present
      }
      tune_playnote (chan, note);
      if (callback_func != NULL && tune_playing)
        callback_func();
    }
    else if (opcode == CMD_INSTRUMENT) { /* change a channel's instrument */
#if TESLA_COIL
      byte instrument = pgm_read_byte(score_cursor);
#if DBUG
      Serial.print("chan "); Serial.print(chan); Serial.print(" instrument "); Serial.println(instrument);
#endif
      if (chan < _tune_num_chans) teslacoil_change_instrument(chan, instrument);
#endif
      ++score_cursor; // skip over it
    }
    else if (opcode == CMD_RESTART) { /* restart score */
      score_cursor = score_start; }
    else if (opcode == CMD_STOP) { /* stop score */
      tune_stopscore();
      break;
    }
  }
}

//-----------------------------------------------
// Delay a specified number of milliseconds
//-----------------------------------------------

void tune_delay (unsigned duration) {
  // This is for compatibility with other Playtune versions
  delay(duration);
}

//-----------------------------------------------
// Stop all channels
//-----------------------------------------------

void tune_stopchans(void) {
  tune_stopscore();
}

//-----------------------------------------------
//  Interval Timer Interrupt Service Routines
//-----------------------------------------------

void ISRtimer0(void) {
  // We keep this running always and use it to time score waits, whether or not it is playing a note.
  if (wait_timer_playing) { // toggle the pin if we're sounding a note
#if TESLA_COIL
    teslacoil_rising_edge(0); // do a Tesla coil pulse
#else
    byte pin = _tune_pins[0];
    digitalWrite(pin, !digitalRead(pin)); // toggle the pin
#endif
  }
  if (tune_playing && wait_toggle_count && --wait_toggle_count == 0) {
    // end of a score wait, so execute more score commands
    tune_stepscore ();  // execute commands
  }
}

void ISRtimer1(void) {
#if TESLA_COIL
  teslacoil_rising_edge(1); // do a Tesla coil pulse
#else
  byte pin = _tune_pins[1];
  digitalWrite(pin, !digitalRead(pin)); // toggle the pin
#endif
}

void ISRtimer2(void) {
#if TESLA_COIL
  teslacoil_rising_edge(2); // do a Tesla coil pulse
#else
  byte pin = _tune_pins[2];
  digitalWrite(pin, !digitalRead(pin)); // toggle the pin
#endif
}

void ISRtimer3(void) {
#if TESLA_COIL
  teslacoil_rising_edge(3); // do a Tesla coil pulse
#else
  byte pin = _tune_pins[3];
  digitalWrite(pin, !digitalRead(pin)); // toggle the pin
#endif
}

//*
