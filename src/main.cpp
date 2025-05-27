
#include <stdint.h>
#include <Arduino.h>
#include <FastLED.h>
#include "app_hal.h"

#ifdef USE_SDL
// emulator with SDL
int main(void)
{

    hal_setup();

    hal_loop();

    return 0;
};
#else

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
