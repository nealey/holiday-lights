#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6
#define NUM_LEDS 150

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB | NEO_KHZ800);

const uint32_t colors[] = {
  0x440000, // Green
  0x44dd00, // Yellow
  0x44dd00, // Yellow
  0x22dd00, // Amber
  0x22dd00, // Amber
  0x00ff00, // Red
  0x008844, // Purple
  0x000088, // Blue
};

const int ncolors = sizeof(colors) / sizeof(*colors);

void setup() {
  strip.begin();
  strip.show();
}

void loop() {
  for (int i = 0; i < 12; i += 1) {
    int pos = random(NUM_LEDS);
    if (random(100) < 20) {
      int color = random(ncolors);
      strip.setPixelColor(pos, colors[color]);
    } else {
      strip.setPixelColor(pos, 0);
    }
  }
  strip.show();
  delay(240);
}
