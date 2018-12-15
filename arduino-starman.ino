/***************************************************

    Arduino Starman Christmas Ornament
    Copyright (C) 2018  Adam McDaniel

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

  ------

  * 24 randomly fading LEDs
  * TODO: add music button
  * TODO: flash lights in sequence with music

 ****************************************************/

#include "Adafruit_TLC5947.h"
#include "Playtune.h"
#include "Music.h"

#define BUTTON_PIN  2

#define DATA_PIN    4
#define CLOCK_PIN   5
#define LATCH_PIN   6

#define AUDIO_1_PIN  7
#define AUDIO_2_PIN  8
#define AUDIO_3_PIN  9

Adafruit_TLC5947 tlc = Adafruit_TLC5947(1, CLOCK_PIN, DATA_PIN, LATCH_PIN);

#define TOTAL_LEDS 24
#define FADE_LEDS 18

uint8_t active[FADE_LEDS];  // active leds
int16_t bright[FADE_LEDS];  // current brightness value
int8_t delta[FADE_LEDS];   // current brightness delta
uint8_t stage = 0;         // music stage (0 = no music active)

const unsigned char* music[] = { block, powerup, starman, fanfare, death }; 

Playtune pt;


void setup() {
  Serial.begin(9600);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), button, RISING);
  Serial.println("Button ready");

  pt.tune_callback(&callback);
  Serial.println("Music ready");

  for (uint8_t i = 0; i < FADE_LEDS; i++) {
    active[i] = i;
    bright[i] = (rand() % 4096);
    delta[i] = (rand() & 400) - 200;
  }
  shuffle(active, FADE_LEDS);

  tlc.begin();
  Serial.println("LEDs ready");
}


void button() {
  Serial.println("button!");
  if (stage == 0) {
    pt.tune_initchan(3);
    pt.tune_initchan(4);
    pt.tune_initchan(5);

    stage = 1;
    pt.tune_playscore(music[stage]);
  }
}


void callback() {
  Serial.println("note tune callback!");
  // Callback function called once per note.  Increment LED pattern on each note.

  if (stage >= 1)
    marquee();

  tlc.write();
}


void loop() {
  if (stage == 0) {
    fade();
    tlc.write();
  }
  else {
    pt.tune_playscore(music[stage]);
    while (pt.tune_playing) ; /* wait here until playing stops */
    pt.tune_delay(100); /* wait a moment */
    stage++;
    if (stage == 5) {
      stage = 0;
      pt.tune_stopchans();
    }
    // tlc.write() called during callback()
  }

}


void fade() {
  for (uint8_t i = 0; i < FADE_LEDS; i++) {
    int16_t brightness;

    bright[i] += delta[i];
    brightness = bright[i];

    if (bright[i] >= 4095) {
      brightness = 4095;
      delta[i] *= -1;
    }
    if (bright[i] <= 0) {
      brightness = 0;
      delta[i] *= -1;
    }

    tlc.setPWM(active[i], brightness);

    if (bright[i] <= 0) {
      // randomly select a different LED not already in active array;
      if (FADE_LEDS < TOTAL_LEDS) {
        boolean new_number = true;
        while (new_number) {
          uint8_t new_active = (rand() % 24);
          new_number = false;
          for (uint8_t j = 0; j < FADE_LEDS; j++) {
            if (active[j] == new_active) {
              new_number = true;
            }
          }
          if (!new_number)
            active[i] = new_active;
        }
      }
    }
  }
  delay(20);
}


void marquee() {
  static uint8_t count = 1;
  static uint8_t list[24] = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1};

  for (uint8_t i = 0; i < TOTAL_LEDS; i++) {
    if (list[i])
      tlc.setPWM(i, 4095);
    else
      tlc.setPWM(i, 512);
  }

  list[TOTAL_LEDS] = list[0];
  for (uint8_t i = 0; i < TOTAL_LEDS; i++) {
    list[i] = list[i + 1];
  }
  delay(50);
}


void shuffle(uint8_t *array, size_t n)
{
  if (n > 1)
  {
    size_t i;
    for (i = 0; i < n - 1; i++)
    {
      size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
      int t = array[j];
      array[j] = array[i];
      array[i] = t;
    }
  }
}
