#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Which pin your LED strip is connected to
#ifdef Attiny85
#  define PIN 3
#else
#  define PIN 6
#endif

// Which pin to pull to go full white. Comment to disable this feature.
#define WHITE_PIN 4

// Order of the lights you got

// How many LEDs you have. It's okay if this is too big.
#ifdef Attiny85
#  define NUM_LEDS 80
#else
#  define NUM_LEDS 200
#endif

// What percentage chance a chosen light has of being on
#define ACTIVITY 50

// Debug LED
#define LED_PIN 1


Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_RGB | NEO_KHZ800);

const uint32_t colors[] = {
  0x004400, // Green
  0xdd4400, // Yellow
  0xdd4400, // Yellow
  0xdd2200, // Amber
  0xdd2200, // Amber
  0x0000ff, // Red
  0x880044, // Purple
  0x000088, // Blue
};

const int ncolors = sizeof(colors) / sizeof(*colors);

void setup() {
  strip.begin();
  strip.show();
#ifdef WHITE_PIN
  pinMode(WHITE_PIN, INPUT_PULLUP);
#endif

#ifdef LED_PIN
  pinMode(LED_PIN, OUTPUT);
#endif
}

void loop_color() {
  for (int i = 0; i < NUM_LEDS/12; i += 1) {
    int pos = random(NUM_LEDS);
    if (random(100) < ACTIVITY) {
     int color = random(ncolors);
     strip.setPixelColor(pos, colors[color]);
   } else {
     strip.setPixelColor(pos, 0);
   }
  }
  strip.show();
  delay(240);
}

void loop_white() {
  for (int i = 0; i < NUM_LEDS/12; i += 1) {
    int pos = random(NUM_LEDS);
    if (random(100) < 5) {
      strip.setPixelColor(pos, 0x0);
    } else {
      strip.setPixelColor(pos, 0x99ffaa);
    }
  }
  strip.show();
  delay(24);
}

void loop() {
#ifdef WHITE_PIN
  if (! digitalRead(WHITE_PIN)) {
    loop_white();
  } else {
#else
  {
#endif
    loop_color();
  }

#ifdef LED_PIN
  static bool led = true;
  digitalWrite(LED_PIN, led);
  led = !led;
#endif
}
