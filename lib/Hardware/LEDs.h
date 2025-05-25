#ifndef CANE_LEDS_H
#define CANE_LEDS_H

#include <FastLED.h>

#define LED_TYPE HD107HD
#define LED_DATA_PIN 21
#define LED_CLOCK_PIN 33
#define LED_MAX_REFRESH_RATE 500
#define NUM_LEDS_EACH_SIDE 72
#define NUM_LEDS (NUM_LEDS_EACH_SIDE * 2)

#define INITIAL_BRIGHTNESS 5
#define COLOR_ORDER BGR

CRGB leds[NUM_LEDS];

void setup_leds()
{
   FastLED.addLeds<LED_TYPE, LED_DATA_PIN, LED_CLOCK_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
   FastLED.setMaxRefreshRate(LED_MAX_REFRESH_RATE);
   FastLED.setBrightness(INITIAL_BRIGHTNESS);
}

void mirror_leds()
{
   for (int i = 0; i < NUM_LEDS_EACH_SIDE; i++)
   {
      leds[NUM_LEDS_EACH_SIDE + i] = leds[NUM_LEDS_EACH_SIDE - i - 1].as_uint32_t();
   }
}

#endif
