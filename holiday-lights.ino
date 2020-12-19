#if defined(__AVR_ATtiny85__)
#define TINY
#endif

#include <FastLED.h>
#include "Debounce.h"
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
#define ON_FOR (6 * DURATION_HOUR)

#define ARK "\x03\x04"
const char *message = (

    "seasons greetings" ARK
    "happy holiday" ARK
    "merry xmas" ARK
    "happy new year" ARK
    "CQ CQ OF9X" ARK

);

CRGB leds[NUM_LEDS];
Debounce button(BUTTON_PIN, false, true);

void setup() {
  FastLED.addLeds<WS2812, NEOPIXEL_PIN, RGB>(leds, NUM_LEDS);
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

Pulse mainPulse = Pulse(DELAY_MS);

bool strandUpdate(unsigned long now, bool white) {
  if (!mainPulse.Ticked(now)) {
    return false;
  }

  for (int group = 0; group < NUM_GROUPS; ++group) {
    int pos = (group * LEDS_PER_GROUP) + random(LEDS_PER_GROUP);
    if (random(100) < GROUP_UPDATE_PROBABILITY) {
      uint8_t hue = 0;
      uint8_t saturation = 255;
      uint8_t value = 255;
      if (random(100) < ACTIVITY) {
        if (white) {
          saturation = 0;
        } else {
          hue = RandomHue();
        }
      } else {
        value = 0;
      }
      leds[pos] = CHSV(hue, saturation, value);
      group = (group + 1) % NUM_GROUPS;
    }
  }

  return true;
}

bool black(unsigned long now) {
  if (!mainPulse.Ticked(now)) {
    return false;
  }

  FastLED.clear();

  return true;
}

bool morseUpdate(unsigned long now) {
  static MorseEncoder enc;
  static Pulse pulse = Pulse(DIT_DURATION_MS);
  bool ret = false;

  if (pulse.Ticked(now)) {
    if (!enc.Tick()) {
      enc.SetText(message);
    }
    ret = true;
  }
  leds[MORSE_PIXEL] = enc.Transmitting ? MORSE_COLOR : CRGB::Black;

  return ret;
}

bool timeUpdate(unsigned long now, unsigned long timeLeft) {
  unsigned int hoursLeft = timeLeft / DURATION_HOUR;
  unsigned int minutesLeft = (timeLeft / DURATION_MINUTE) % 60;

  int i;
  for (i = 0; i < hoursLeft; ++i) {
    leds[i] = CHSV(0, 255, 32);
  }
  if ((timeLeft / DELAY_MS) % minutesLeft == 0) {
    leds[i]= CHSV(0, 255, 32);
  } else {
    leds[i] = CRGB::Black;
  }
  return false;
}

void loop() {
  bool white = false;
  bool update = false;
  unsigned long now = millis(); // Everybody uses the same time so we don't do spurious updates 5ms apart
  unsigned long timeOfDay = now % DURATION_DAY;

  button.read();
  white = (button.count() % 2 == 1);

  if (FOREVER || white || (timeOfDay < ON_FOR)) {
    update |= strandUpdate(now, white);
    update |= morseUpdate(now);
  } else {
    update |= black(now);
    update |= timeUpdate(now, DURATION_DAY - timeOfDay);
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
