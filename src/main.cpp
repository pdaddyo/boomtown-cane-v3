
#include <stdint.h>
#include <Arduino.h>
#include <Hardware.h>
#include "app_hal.h"
#include "patterns.h"

#ifdef USE_SDL
// emulator with SDL
int main(void)
{

    hal_setup();

    hal_loop();

    return 0;
};
#else

void leds_setup()
{
    // Serial.begin(115200);
    delay(1000);
    Serial.println("Initializing LEDs");
    setup_leds();
    Serial.println("Initializing I2C bus");
    // setup_i2c();
    // scan_i2c();[]
    // Serial.println("Initializing gyroscope");
    // setup_gyro();
    leds_setup_completed = true;
}

void leds_loop()
{
    if (leds_setup_completed == false)
    {
        sinelon();
        FastLED.show();
        return;
    }

    // Call the current pattern function once, updating the 'leds' array
    gPatterns[gCurrentPatternNumber]();
    mirror_leds();

    // send the 'leds' array out to the actual LED strip
    FastLED.show();
    FastLED.countFPS();
    gHue++;

#ifndef USE_SDL
    EVERY_N_MILLISECONDS(100)
    {
        // read_gyro();
        update_gyro();
        float z = get_gyro_z();

        if (fabs(z) > 200)
        {
            Serial.printf("Gyro:  z:%f\n", z);
        }
    }
#endif

    // do some periodic updates
    EVERY_N_MILLISECONDS(1) { gHue++; } // slowly cycle the "base color" through the rainbow
    // EVERY_N_SECONDS(5) { nextPattern(); } // change patterns periodically
    EVERY_N_SECONDS(1) { Serial.printf("FPS: %d\n", FastLED.getFPS()); }
}

// esp32 hardware
void setup()
{
    hal_setup();
    leds_setup();
}

void loop()
{
    leds_loop();

    EVERY_N_MILLISECONDS(10)
    {
        hal_loop();
    }
}
#endif