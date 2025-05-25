#include <Arduino.h>
#include <Hardware.h>
#include "patterns.h"

bool has_setup_completed = false;

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("Initializing LEDs");
  setup_leds();
  Serial.println("Initializing I2C bus");
  setup_i2c();
  scan_i2c();
  Serial.println("Initializing gyroscope");
  setup_gyro();
  has_setup_completed = true;
}

void wave_direction_change(int estimate_duration_ms)
{
}

void loop()
{
  if (has_setup_completed == false)
  {
    sinelon();
    FastLED.show();
    return;
  }

  EVERY_N_MILLISECONDS(10)
  {
    // detect_cane_wave();
  }

  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();
  mirror_leds();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  FastLED.countFPS();
  gHue++;

  EVERY_N_SECONDS(1) { Serial.printf("FPS: %d\n", FastLED.getFPS()); }
  // do some periodic updates
  // EVERY_N_MILLISECONDS(1) { gHue++; }   // slowly cycle the "base color" through the rainbow
  // EVERY_N_SECONDS(2) { nextPattern(); } // change patterns periodically
}
