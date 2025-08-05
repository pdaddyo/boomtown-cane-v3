#include "ui/ui.h"

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0;                  // rotating "base color" used by many of the patterns

uint8_t fire_sparking = 90;
uint8_t fire_cooling = 80;

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void rainbow()
{
   // FastLED's built-in rainbow generator
   fill_rainbow(leds, NUM_LEDS_EACH_SIDE, gHue, 7);
}

void addGlitter(fract8 chanceOfGlitter)
{
   if (random8() < chanceOfGlitter)
   {
      leds[random16(NUM_LEDS_EACH_SIDE)] += CRGB::White;
   }
}

void rainbowWithGlitter()
{
   // built-in FastLED rainbow, plus some random sparkly glitter
   rainbow();
   addGlitter(80);
}

void confetti()
{
   // random colored speckles that blink in and fade smoothly
   fadeToBlackBy(leds, NUM_LEDS_EACH_SIDE, 10);
   int pos = random16(NUM_LEDS_EACH_SIDE);
   leds[pos] += CHSV(gHue + random8(64), 200, 255);
}

void sinelon()
{
   // a colored dot sweeping back and forth, with fading trails
   fadeToBlackBy(leds, NUM_LEDS_EACH_SIDE, 20);
   int pos = beatsin16(53, 0, NUM_LEDS_EACH_SIDE - 1);
   leds[pos] += CHSV(gHue, 255, 192);
}

void bpm()
{
   // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
   uint8_t BeatsPerMinute = 174;
   CRGBPalette16 palette = PartyColors_p;
   uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
   for (int i = 0; i < NUM_LEDS_EACH_SIDE; i++)
   { // 9948
      leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
   }
}

void juggle()
{
   // eight colored dots, weaving in and out of sync with each other
   fadeToBlackBy(leds, NUM_LEDS_EACH_SIDE, 20);
   uint8_t dothue = 0;
   for (int i = 0; i < 8; i++)
   {
      leds[beatsin16(i + 17, 0, NUM_LEDS_EACH_SIDE - 1)] |= CHSV(dothue, 200, 255);
      dothue += 32;
   }
}

void rainbowSlide()
{
   static uint16_t sPseudotime = 0;
   static uint16_t sLastMillis = 0;
   static uint16_t sHue16 = 0;

   uint8_t sat8 = beatsin88(87, 220, 250);
   uint8_t brightdepth = beatsin88(341, 96, 224);
   uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
   uint8_t msmultiplier = beatsin88(147, 23, 60);

   uint16_t hue16 = sHue16; // gHue * 256;
   uint16_t hueinc16 = beatsin88(113, 1, 3000);

   uint16_t ms = millis();
   uint16_t deltams = ms - sLastMillis;
   sLastMillis = ms;
   sPseudotime += deltams * msmultiplier;
   sHue16 += deltams * beatsin88(400, 5, 9);
   uint16_t brightnesstheta16 = sPseudotime;

   for (uint16_t i = 0; i < NUM_LEDS_EACH_SIDE; i++)
   {
      hue16 += hueinc16;
      uint8_t hue8 = hue16 / 256;

      brightnesstheta16 += brightnessthetainc16;
      uint16_t b16 = sin16(brightnesstheta16) + 32768;

      uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
      uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
      bri8 += (255 - brightdepth);

      CRGB newcolor = CHSV(hue8, sat8, bri8);

      uint16_t pixelnumber = i;
      pixelnumber = (NUM_LEDS_EACH_SIDE - 1) - pixelnumber;

      nblend(leds[pixelnumber], newcolor, 64);
   }
}

// This basic one-dimensional 'fire' simulation works roughly as follows:
// There's a underlying array of 'heat' cells, that model the temperature
// at each point along the line.  Every cycle through the simulation,
// four steps are performed:
//  1) All cells cool down a little bit, losing heat to the air
//  2) The heat from each cell drifts 'up' and diffuses a little
//  3) Sometimes randomly new 'sparks' of heat are added at the bottom
//  4) The heat from each cell is rendered as a color into the leds array
//     The heat-to-color mapping uses a black-body radiation approximation.
//
// Temperature is in arbitrary units from 0 (cold black) to 255 (white hot).
//
// This simulation scales it self a bit depending on NUM_LEDS; it should look
// "OK" on anywhere from 20 to 100 LEDs without too much tweaking.
//
// I recommend running this simulation at anywhere from 30-100 frames per second,
// meaning an interframe delay of about 10-35 milliseconds.
//
// Looks best on a high-density LED setup (60+ pixels/meter).
//
//
// There are two main parameters you can play with to control the look and
// feel of your fire: COOLING (used in step 1 above), and SPARKING (used
// in step 3 above).
//
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100
#define COOLING fire_cooling

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING fire_sparking

bool gReverseDirection = false;
void Fire2012()
{
   // Array of temperature readings at each simulation cell
   static uint8_t heat[NUM_LEDS_EACH_SIDE];

   // Step 1.  Cool down every cell a little
   for (int i = 0; i < NUM_LEDS_EACH_SIDE; i++)
   {
      heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / NUM_LEDS_EACH_SIDE) + 2));
   }

   // Step 2.  Heat from each cell drifts 'up' and diffuses a little
   for (int k = NUM_LEDS_EACH_SIDE - 1; k >= 2; k--)
   {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
   }

   // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
   if (random8() < SPARKING)
   {
      int y = random8(7);
      heat[y] = qadd8(heat[y], random8(160, 255));
   }

   // Step 4.  Map from heat cells to LED colors
   for (int j = 0; j < NUM_LEDS_EACH_SIDE; j++)
   {
      CRGB color = HeatColor(heat[j]);
      int pixelnumber;
      if (gReverseDirection)
      {
         pixelnumber = (NUM_LEDS_EACH_SIDE - 1) - j;
      }
      else
      {
         pixelnumber = j;
      }
      leds[pixelnumber] = color;
   }
}

void transgenderFlag()
{
   // Transgender flag colors: light blue, pink, white, pink, light blue
   // with animated wobble effect
   static uint16_t sOffset = 0;

   // Configurable parameters
   const uint8_t wobbleAmplitude = 5; // How many LEDs to wobble up/down
   const uint8_t wobbleSpeed = 10;    // Speed of wobble (lower = faster)

   // Define the flag colors
   CRGB lightBlue = CRGB(91, 206, 250); // Light blue
   CRGB pink = CRGB(255, 105, 180);     // More vibrant pink (hot pink)
   CRGB white = CRGB(255, 255, 255);    // White

   // Calculate stripe width based on total LEDs (5 stripes)
   uint16_t stripeWidth = NUM_LEDS_EACH_SIDE / 5;

   // Calculate wobble offset - simple up/down motion for entire flag
   int8_t wobbleOffset = (sin8(sOffset) - 128) * wobbleAmplitude / 128;

   for (uint16_t i = 0; i < NUM_LEDS_EACH_SIDE; i++)
   {
      // Apply wobble offset to position lookup
      int16_t sourcePosition = i - wobbleOffset;

      // Wrap around for smooth animation
      if (sourcePosition < 0)
         sourcePosition += NUM_LEDS_EACH_SIDE;
      if (sourcePosition >= NUM_LEDS_EACH_SIDE)
         sourcePosition -= NUM_LEDS_EACH_SIDE;

      // Determine which stripe this position belongs to
      uint16_t stripeIndex = sourcePosition / stripeWidth;
      if (stripeIndex > 4)
         stripeIndex = 4;

      CRGB color;
      switch (stripeIndex)
      {
      case 0:
         color = lightBlue;
         break;
      case 1:
         color = pink;
         break;
      case 2:
         color = white;
         break;
      case 3:
         color = pink;
         break;
      case 4:
      default:
         color = lightBlue;
         break;
      }

      leds[i] = color;
   }

   addGlitter(200);

   // Animate the wobble
   EVERY_N_MILLISECONDS(wobbleSpeed) { sOffset += 4; }
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();

SimplePatternList gPatterns = {
    Fire2012,
    bpm,
    juggle,
    rainbow,
    rainbowSlide,
    rainbowWithGlitter,
    sinelon,
    confetti,
    transgenderFlag};

// pattern names
const char *gPatternNames[] = {
    "Fire",
    "BPM",
    "Juggle",
    "Rainbow",
    "Rainbow Slide",
    "Rainbow Glitter",
    "Sinelon",
    "Confetti",
    "Trans Flag",
};

void setCurrentPatternLabel()
{
   // lv_label_set_text(ui_LabelCurrentPattern, gPatternNames[gCurrentPatternNumber]);
}

void setPatternIndex(uint8_t index)
{
   gCurrentPatternNumber = index;
   setCurrentPatternLabel();
}

void nextPattern()
{
   // add one to the current pattern number, and wrap around at the end
   setPatternIndex((gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns));
}
