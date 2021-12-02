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
  * 24 synchronized LEDs with music playback

 ****************************************************/

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>

#include "Adafruit_TLC5947.h"
#include "Playtune.h"
#include "smb.h"
#include "secret.h"

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWD;


WebServer server(80);

#define BUTTON_PIN  4
#define AUDIO_1_PIN 19
#define AUDIO_2_PIN 21
#define AUDIO_3_PIN 22
#define AUDIO_4_PIN 23
#define DATA_PIN    16
#define CLOCK_PIN   17
#define DISABLE_PIN 5
#define LATCH_PIN   18

#define elements(x) (sizeof(x) / sizeof((x)[0]))

Adafruit_TLC5947 tlc = Adafruit_TLC5947(1, CLOCK_PIN, DATA_PIN, LATCH_PIN);

#define TOTAL_LEDS 24
#define FADE_LEDS 18

uint8_t active[FADE_LEDS];  // active leds
int16_t bright[FADE_LEDS];  // current brightness value
int8_t delta[FADE_LEDS];    // current brightness delta
volatile uint8_t stage = -1;         // music stage (0 = no music active)

uint8_t alternate[]  = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
uint8_t shortdot[]   = {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1};
uint8_t longdot[]    = {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1};
uint8_t shortdash[]  = {0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1};
uint8_t widedash[]   = {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1};
uint8_t longdash[]   = {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1};
uint8_t allpoints[]  = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
uint8_t halfpoints[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

void marquee(uint8_t *pattern);
void flash(uint8_t *pattern);

struct demo {
  const unsigned char *music;
  void (*sequence)(uint8_t*);
  uint8_t *pattern;
};

demo levelstart[] = {
  { .music = overworld, .sequence = &marquee, .pattern = widedash},
  { .music = underworld, .sequence = &marquee, .pattern = longdot },
  { .music = underwater, .sequence = &marquee, .pattern = halfpoints },
  { .music = castle, .sequence = &marquee, .pattern = longdot },
}; // randomly pick one

demo invincibility[] = {
  { .music = block, .sequence = &marquee, .pattern = longdot },
  { .music = powerup, .sequence = &flash, .pattern = allpoints },
  { .music = starman, .sequence = &marquee, .pattern = widedash },
}; // randomly pick all or none

demo levelfinish[] = {
  { .music = flagpole, .sequence = &marquee, .pattern = longdash },
  { .music = fanfare, .sequence = &marquee, .pattern = shortdash },
  { .music = ending, .sequence = &marquee, .pattern = widedash },
}; // randomly pick 1 if not cascle, or 2 plus maybe 3 if cascle, or levelfailed

demo levelfailed[] = {
  { .music = death, .sequence = &marquee, .pattern = longdash },
  { .music = gameover, .sequence = &marquee, .pattern = widedash },
}; // randomly pick 1, 2, or 3 instead of levelfinish

demo stardemo[] = {
  { .music = NULL, .sequence = NULL, .pattern = NULL },
  { .music = NULL, .sequence = NULL, .pattern = NULL },
  { .music = NULL, .sequence = NULL, .pattern = NULL },
  { .music = NULL, .sequence = NULL, .pattern = NULL },
  { .music = NULL, .sequence = NULL, .pattern = NULL },
  { .music = NULL, .sequence = NULL, .pattern = NULL },
  { .music = NULL, .sequence = NULL, .pattern = NULL },
  { .music = NULL, .sequence = NULL, .pattern = NULL },
};


void handleRoot() {
  char temp[1024];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf(temp, 1024,

           "<html>\
  <head>\
    <title>Starman Christmas Ornament</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
    <script>\
      function play_music() {\
        const Http = new XMLHttpRequest();\
        const url = '/play';\
        Http.open('GET', url);\
        Http.send();\
      }\
    </script>\
  </head>\
  <body>\
    <h1>Starman! &#127776;</h1>\
    <button onclick='play_music()'>Play Music</button>\
    <p>Tune playing: %d</p>\
    <p>Tune stage: %d</p>\
    <p>Uptime: %02d:%02d:%02d</p>\
  </body>\
</html>",
           tune_playing, stage, hr, min % 60, sec % 60
          );
  server.send(200, "text/html", temp);
}

void handlePlayButton() {
  if (stage == 0) {
    randomLevel();
    stage = 1;
  }

  server.send(200, "text/html", "OK");
}


void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}


void setup() {
  Serial.begin(115200);
  Serial.println("startup");

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(DISABLE_PIN, OUTPUT);
  digitalWrite(DISABLE_PIN, true);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  ArduinoOTA.onStart([]() {
    Serial.println("Start updating..");
  }).onEnd([]() {
    Serial.println("\n\nDone!");
  }).onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  }).onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();

  if (MDNS.begin("starman")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/play", handlePlayButton);
  server.onNotFound(handleNotFound);
  server.begin();

  randomSeed(analogRead(0));

  tune_initchan(AUDIO_1_PIN);
  tune_initchan(AUDIO_2_PIN);
  tune_initchan(AUDIO_3_PIN);
  tune_initchan(AUDIO_4_PIN);
  tune_callback(&callback);
  Serial.println("Music ready");

  for (uint8_t i = 0; i < FADE_LEDS; i++) {
    active[i] = i;
    bright[i] = (rand() % 4096);
    delta[i] = (rand() & 400) - 200;
  }
  shuffle(active, FADE_LEDS);

  tlc.begin();
  digitalWrite(DISABLE_PIN, false);
  Serial.println("LEDs ready");
  stage = 0;
}


void randomLevel() {
  uint8_t pos = 1;
  long level = random(elements(levelstart));
  long invin = random(2);
  long success = random(2);
  long ending = random(2);

  Serial.print("Play Level: ");
  Serial.println(level);

  Serial.print("Random Invincibility? ");
  Serial.println(invin);

  Serial.print("Random Success? ");
  Serial.println(success);

  Serial.print("Random Ending? ");
  Serial.println(ending);

  memset(stardemo, 0, sizeof(stardemo));
  
  stardemo[pos++] = levelstart[int(level)];

  if (invin) {
    Serial.println("Play Invincibility");
    for (uint8_t i = 0; i < elements(invincibility); i++) {
      stardemo[pos++] = invincibility[i];
    }
  }

  if (success) {
    // lived
    Serial.println("Mario Lived!");
    if (level < 3) {
      stardemo[pos++] = levelfinish[0];
      Serial.println("Play Flagpole");
    }
    else {
      Serial.println("Play Fanfare");
      stardemo[pos++] = levelfinish[1];
      if (ending) {
        Serial.println("Play Ending");
        stardemo[pos++] = levelfinish[2];
      }
    }
  }
  else {
    // died
    Serial.println("Mario Died!");
    stardemo[pos++] = levelfailed[0];
    if (ending) {
      Serial.println("Play GameOver");
      stardemo[pos++] = levelfailed[1];
    }
  }
}



void callback() {
  // Callback function called once per note.  Increment LED pattern on each note.
  void (*sequence)(uint8_t*) = stardemo[stage].sequence;
  uint8_t *pattern  = stardemo[stage].pattern;

  sequence(pattern);
  tlc.write();
}


void loop() {
  server.handleClient();
  ArduinoOTA.handle();
  delay(2);//allow the cpu to switch to other tasks
  
  if (stage == 0) {
    fade();
    tlc.write();
    bool buttonState = digitalRead(BUTTON_PIN);
    if (buttonState == 0) {
      randomLevel();
      stage = 1;
    }
  }
  else if (stage >= 1) {
    Serial.print("playing stage");
    Serial.println(stage);
    tune_playscore(stardemo[stage].music);
    while (tune_playing) {
      // As we block the main thread here, we still want the web server to handle requests.
      server.handleClient(); 
      ArduinoOTA.handle();
      delay(2);
    }
    tune_delay(100); /* wait a moment */
    stage++;
    if (stage == elements(stardemo) ||
        stardemo[stage].music == NULL ) {
      stage = 0;
    }
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

void marquee(uint8_t *pattern) {
  for (uint8_t i = 0; i < TOTAL_LEDS; i++) {
    if (pattern[i])
      tlc.setPWM(i, 4095);
    else
      tlc.setPWM(i, 512);
  }

  uint8_t tmp = pattern[0];
  for (uint8_t i = 0; i < TOTAL_LEDS-1; i++) {
    pattern[i] = pattern[i + 1];
  }
  pattern[TOTAL_LEDS-1] = tmp;
}


void flash(uint8_t *pattern) {
  for (uint8_t i = 0; i < TOTAL_LEDS; i++) {
    if (pattern[i])
      tlc.setPWM(i, 4095);
    else
      tlc.setPWM(i, 256);
  }

  for (uint8_t i = 0; i < TOTAL_LEDS; i++) {
    pattern[i] = !pattern[i];
  }
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
