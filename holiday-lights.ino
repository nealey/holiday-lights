#if defined(__AVR_ATtiny85__)
#define TINY
#endif

#include <FastLED.h>
#include "bounce2.h"
#include "durations.h"
#include "morse.h"
#include "pulse.h"

// Do you want it to run forever, or cycle every 24 hours?
#ifdef TINY
#define FOREVER true
#else
#define FOREVER false
#endif

// Which pin your LED strip is connected to
#if defined(TINY)
#define NEOPIXEL_PIN 3
#elif defined(ADAFRUIT_TRINKET_M0)
#define NEOPIXEL_PIN 2
#else
#define NEOPIXEL_PIN 6
#endif

// Which pin has the momentary switch to toggle full white.
#define BUTTON_PIN 4

// How many LEDs you have. It's okay if this is too big.
#define LEDS_PER_GROUP 10
#ifdef TINY
#define NUM_GROUPS 7
#else
#define NUM_GROUPS 20
#endif
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

// Color for all-white mode
#define WHITE 0x886655

// The Neopixel library masks interrupts while writing.
// This means you lose time.
// How much time do you lose?
// I'm guessing 10 minutes a day.

#define SNOSSLOSS_DAY (DURATION_DAY - (10 * DURATION_MINUTE))
#define ON_FOR (5 * DURATION_HOUR)

#define ARK "\x03\x04"
const char *message = (

    "seasons greetings" ARK
    "happy holiday" ARK
    "merry xmas" ARK
    "happy new year" ARK
    "CQ CQ OF9X" ARK

);

// These colors mirror pretty closely some cheapo LED lights we have
const uint32_t colors[] = {
    0xdd4400,  // Yellow
    0xff0000,  // Red
    0xdd2200,  // Amber
    0x004400,  // Green

    0xdd4400,  // Yellow
    0xff0000,  // Red
    0xdd2200,  // Amber
    0x880044,  // Purple

    0xdd4400,  // Yellow
    0xff0000,  // Red
    0xdd2200,  // Amber
    0x000088,  // Blue
};
const int ncolors = sizeof(colors) / sizeof(*colors);

CRGB leds[NUM_LEDS];
Bounce button;

void setup() {
  FastLED.addLeds<WS2812, NEOPIXEL_PIN, RGB>(leds, NUM_LEDS);
  button.attach(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
}

Pulse mainPulse = Pulse(DELAY_MS);

bool strandUpdate(bool white) {
  if (!mainPulse.Ticked()) {
    return false;
  }

  for (int group = 0; group < NUM_GROUPS; ++group) {
    int pos = (group * LEDS_PER_GROUP) + random(LEDS_PER_GROUP);
    if (random(100) < GROUP_UPDATE_PROBABILITY) {
      if (random(100) < ACTIVITY) {
        uint32_t color;

        if (white) {
          color = WHITE;
        } else {
          color = colors[random(ncolors)];
        }
        leds[pos] = color;
      } else {
        leds[pos] = 0;
      }
      group = (group + 1) % NUM_GROUPS;
    }
  }

  return true;
}

bool black() {
  if (!mainPulse.Ticked()) {
    return false;
  }

  FastLED.clear();

  return true;
}

bool morseUpdate() {
  static MorseEncoder enc;
  static Pulse pulse = Pulse(DIT_DURATION_MS);

  leds[MORSE_PIXEL] = enc.Transmitting ? MORSE_COLOR : CRGB::Black;
  if (pulse.Ticked()) {
    if (!enc.Tick()) {
      enc.SetText(message);
    }
    return true;
  }
  return false;
}

bool millisUpdate() {
  unsigned long now = millis();
  for (int i = 0; i < sizeof(unsigned long) * 8; ++i) {
    leds[i] = CHSV(0, 255, bitRead(now, i) ? 32 : 0);
  }
  return false;
}

void loop() {
  static bool white = false;
  bool update = false;

  button.update();
  if (button.fell()) {
    white = !white;
  }

  if (FOREVER || white || (millis() % SNOSSLOSS_DAY < ON_FOR)) {
    update |= strandUpdate(white);
    update |= morseUpdate();
  } else {
    update |= black();
    update |= millisUpdate();
  }

  if (update) {
    FastLED.show();
  }

  // blink the heart for a little bit
  if (millis() < 30 * 1000) {
    bool on = (millis() % 1000) < 500;
    digitalWrite(LED_BUILTIN, on);
  }
}
