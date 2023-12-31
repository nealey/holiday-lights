#include <FastLED.h>
#include "Debounce.h"
#include "durations.h"
#include "morse.h"
#include "pulse.h"

// Which pins your LED strips are connected to
#define NUM_OUTPUTS 2
#define OUTPUT_PIN1 7
#define OUTPUT_PIN2 8

// Which pin has the momentary switch to toggle full white.
#define BUTTON_PIN 4

// How many LEDs you have per output. It's okay if this is too big.
#define LEDS_PER_GROUP 10
#define NUM_GROUPS 10
#define NUM_LEDS (LEDS_PER_GROUP * NUM_GROUPS)

// How many milliseconds between activity in one group
#define DELAY_MS 600

// Each group has this percentage chance of
#define GROUP_UPDATE_PROBABILITY 25

// What percentage chance a chosen light has of being on
#define ACTIVITY 40

// Which pixel to flash messages in morse code: -1 = disable
#define MORSE_PIXEL 8
#define MORSE_COLOR CRGB::Red

// How long a dit lasts
#define DIT_DURATION_MS 150

#define ARK "\x03\x04"
const char *message = (
    "seasons greetings" ARK
    "happy holiday" ARK
    "merry xmas" ARK
    "happy new year" ARK
    "CQ CQ OF9X" ARK
);

CRGB leds[NUM_OUTPUTS][NUM_LEDS];
Debounce button(BUTTON_PIN, false, true);

void setup() {
  FastLED.addLeds<WS2812, OUTPUT_PIN1, RGB>(leds[0], NUM_LEDS);
  FastLED.addLeds<WS2812, OUTPUT_PIN2, RGB>(leds[1], NUM_LEDS);
  FastLED.setTemperature(Tungsten40W);
  FastLED.setBrightness(52);
  pinMode(LED_BUILTIN, OUTPUT);
}

uint8_t RandomHue() {
  switch (random(12)) {
    case 0 ... 2:
      return 52; // reddish yellow
    case 3 ... 5:
      return HUE_RED;
    case 6 ... 8:
      return 28; // reddish orange
    case 9:
      return HUE_BLUE;
    case 10:
      return HUE_GREEN;
    case 11:
      return 204; // reddish purple
  }
}


bool strandUpdate(unsigned long now, bool white) {
  static Pulse pulse = Pulse(DELAY_MS);
  if (!pulse.Ticked(now)) {
    return false;
  }

  for (int output = 0; output < NUM_OUTPUTS; output += 1) {
    for (int group = 0; group < NUM_GROUPS; group += 1) {
      int pos = (group * LEDS_PER_GROUP) + random(LEDS_PER_GROUP);
      if (random(100) < GROUP_UPDATE_PROBABILITY) {
        uint8_t hue = 0;
        uint8_t saturation = white?0:255;
        uint8_t value = 255;
        if (random(100) < ACTIVITY) {
          hue = RandomHue();
        } else {
          value = 0;
        }
        leds[output][pos] = CHSV(hue, saturation, value);
        group = (group + 1) % NUM_GROUPS;
      }
    }
  }

  return true;
}

bool morseUpdate(unsigned long now) {
  static MorseEncoder enc;
  static Pulse pulse = Pulse(DIT_DURATION_MS);
  if (!pulse.Ticked(now)) {
    return false;
  }

  if (!enc.Tick()) {
    enc.SetText(message);
  }
  leds[0][MORSE_PIXEL] = enc.Transmitting ? MORSE_COLOR : CRGB::Black;

  return true;
}

void loop() {
  bool white = false;
  bool update = false;
  unsigned long now = millis(); // Everybody uses the same time so we don't do spurious updates 5ms apart

  button.read();
  white = (button.count() % 2 == 1);

  update |= strandUpdate(now, white);
  //update |= morseUpdate(now);

  if (update) {
    FastLED.show();
  }

  // blink the heart for a little bit
  if (millis() < 30 * 1000) {
    bool on = (millis() % 1000) < 500;
    digitalWrite(LED_BUILTIN, on);
  }
}
