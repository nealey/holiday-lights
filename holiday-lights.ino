#if defined(__AVR_ATtiny85__)
#define TINY
#endif

#include <FastLED.h>
#include "bounce2.h"
#include "morse.h"

// Do you want it to run forever, or cycle every 24 hours?
#ifdef TINY
#define FOREVER true
#else
#define FOREVER false
#endif

// Which pin your LED strip is connected to
#ifdef TINY
#define NEOPIXEL_PIN 3
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

// What percentage chance a chosen light has of being on
#define ACTIVITY 40

// Which pixel to flash messages in morse code: -1 = disable
#define MORSE_PIXEL 8
#define MORSE_COLOR 0x880000

// How long a dit lasts
#define DIT_DURATION_MS 100

// Color for all-white mode
#define WHITE 0x886655

// Some units of time
#define MILLISECOND 1L
#define SECOND (1000 * MILLISECOND)
#define MINUTE (60 * SECOND)
#define HOUR (60 * MINUTE)
#define DAY (24 * HOUR)

// The Neopixel library masks interrupts while writing.
// This means you lose time.
// How much time do you lose?
// I'm guessing 10 minutes a day.

#define SNOSSLOSS_DAY (DAY - (10 * MINUTE))
#define ON_FOR (5 * HOUR)

const char *messages[] = {
    "seasons greetings",
    "happy holiday",
    "merry xmas",
    "happy new year",
    "CQ CQ OF9X",
};
const int nmessages = sizeof(messages) / sizeof(*messages);

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

bool strandUpdate(bool white) {
  static unsigned long nextEventMillis = 0;
  unsigned long now = millis();
  static int group = 0;

  if (now < nextEventMillis) {
    return false;
  }

  int pos = (group * LEDS_PER_GROUP) + random(LEDS_PER_GROUP);
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

  nextEventMillis = now + (DELAY_MS / NUM_GROUPS);
  return true;
}

bool morseUpdate() {
  static MorseEncoder enc;
  static unsigned long nextEventMillis = 0;
  static bool ARK = true;
  unsigned long now = millis();
  bool ret = false;

  if (now >= nextEventMillis) {
    nextEventMillis = now + DIT_DURATION_MS;
    if (!enc.Tick()) {
      ARK = !ARK;
      if (ARK) {
        enc.SetText("ARK");
        enc.Quiet(MORSE_PAUSE_WORD);
      } else {
#ifdef TINY
        // I have tried twenty different ways to make this work on the ATTINY,
        // including a big switch statement. It always freezes the program.
        // I give up. Maybe it's a compiler bug having to do with a modulo.
        enc.SetText(messages[0]);
#else        
        enc.SetText(messages[now % nmessages]);
#endif
        enc.Quiet(200);
      }
    }
    ret = true;
  }
  leds[MORSE_PIXEL] = enc.Transmitting ? MORSE_COLOR : 0;
  return ret;
}

bool black() {
  static unsigned long nextEventMillis = 0;
  unsigned long now = millis();

  if (now < nextEventMillis) {
    return false;
  }

  FastLED.clear();

  nextEventMillis = now + (DELAY_MS / NUM_GROUPS);  // Keep timing consistent
  return true;
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
