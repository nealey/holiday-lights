#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6
#define NUM_LEDS 150

#define WHITE_PIN 4

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
  pinMode(WHITE_PIN, INPUT_PULLUP);
}

void loop_color() {
  for (int i = 0; i < NUM_LEDS/12; i += 1) {
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
  if (digitalRead(WHITE_PIN)) {
    loop_white();
  } else {
    loop_color();
  }
}
