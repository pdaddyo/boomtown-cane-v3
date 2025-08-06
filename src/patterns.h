#include "ui/ui.h"

// led and lcd displays
lv_disp_t *display_lcd;
lv_disp_t *display_virtual_leds;

// virtual leds display (NOW UNUSED)
#define DISPLAY_VIRTUAL_LEDS_WIDTH 200
#define DISPLAY_VIRTUAL_LEDS_HEIGHT 72
#define DISPLAY_VIRTUAL_LEDS_PIXEL_COUNT (DISPLAY_VIRTUAL_LEDS_WIDTH * DISPLAY_VIRTUAL_LEDS_HEIGHT)
#define DISPLAY_VIRTUAL_LEDS_BYTE_PER_PIXEL 2 /* be 2 for RGB565 */

static lv_color_t virtual_leds_buf[DISPLAY_VIRTUAL_LEDS_PIXEL_COUNT];
static lv_disp_draw_buf_t virtual_leds_draw_buf;
static lv_disp_drv_t virtual_leds_drv;
static lv_obj_t *virtual_leds_ui_screen;

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0;                  // rotating "base color" used by many of the patterns
uint8_t glitter = 0;
float gyro_z = 0;
uint8_t led_brightness = INITIAL_BRIGHTNESS;
uint8_t hue_delay = 1;
bool always_swipe = false;
lv_color_hsv_t selected_hsv = {0, 255, 255};

lv_obj_t *image_on_virtual_screen = NULL;
float gyro_trigger_threshold = 100;
float gyro_swipe_end_threshold = 30;

// -1 = right to left
// 0 = not triggered
// 1 = left to right
bool swipe_in_progress = false;
int8_t gyro_last_direction = 0;
float gyro_last_trigger_force = 0;
unsigned long swipe_trigger_time = 0;
unsigned long last_swipe_duration = 0;

unsigned int image_index_to_swipe = 0;
unsigned int swipe_count_on_current_image = 0;
lv_img_dsc_t *image_to_swipe = NULL;

uint8_t swipes_per_image = 2;
bool randomise_after_swipe = false;
bool randomise_colour_after_swipe = false;

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

uint8_t selected_palette_index = 0;
static const char *const palette_names[] = {
    "Heat",
    "Cloud",
    "Lava",
    "Ocean",
    "Forest",
    "Rainbow",
    "Stripe",
    "Party",
};

static const TProgmemRGBPalette16 *const palette_pointers[] = {
    &HeatColors_p,
    &CloudColors_p,
    &LavaColors_p,
    &OceanColors_p,
    &ForestColors_p,
    &RainbowColors_p,
    &RainbowStripeColors_p,
    &PartyColors_p,
};

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
   uint8_t BeatsPerMinute = 120;
   CRGBPalette16 palette = RainbowStripeColors_p;
   uint8_t beat = beatsin8(BeatsPerMinute, 60, 255);
   for (int i = NUM_LEDS_EACH_SIDE - 1; i >= 0; i--)
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

uint8_t fire_sparking = 90;
uint8_t fire_cooling = 80;

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
      // CRGB color = HeatColor(heat[j]);
      CRGB color = ColorFromPalette(*palette_pointers[selected_palette_index], heat[j], 255, LINEARBLEND);
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
   const uint8_t wobbleSpeed = 5;     // Speed of wobble (lower = faster)

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

   // addGlitter(200);

   // Animate the wobble
   EVERY_N_MILLISECONDS(wobbleSpeed) { sOffset += 4; }
}

void prideFlag()
{
   // Pride flag colors: red, orange, yellow, green, blue, purple
   // with animated wobble effect
   static uint16_t sOffset = 0;

   // Configurable parameters
   const uint8_t wobbleAmplitude = 4; // How many LEDs to wobble up/down
   const uint8_t wobbleSpeed = 5;     // Speed of wobble (lower = faster)

   // Define the pride flag colors
   CRGB red = CRGB(255, 0, 0);      // Red
   CRGB orange = CRGB(255, 128, 0); // Orange
   CRGB yellow = CRGB(255, 255, 0); // Yellow
   CRGB green = CRGB(0, 255, 0);    // Green
   CRGB blue = CRGB(0, 0, 255);     // Blue
   CRGB purple = CRGB(128, 0, 255); // Purple

   // Calculate stripe width based on total LEDs (6 stripes)
   uint16_t stripeWidth = NUM_LEDS_EACH_SIDE / 6;

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
      if (stripeIndex > 5)
         stripeIndex = 5;

      CRGB color;
      switch (stripeIndex)
      {
      case 0:
         color = red;
         break;
      case 1:
         color = orange;
         break;
      case 2:
         color = yellow;
         break;
      case 3:
         color = green;
         break;
      case 4:
         color = blue;
         break;
      case 5:
      default:
         color = purple;
         break;
      }

      leds[i] = color;
   }

   // Add subtle sparkle effect
   // addGlitter(120);

   // Animate the wobble
   EVERY_N_MILLISECONDS(wobbleSpeed) { sOffset += 4; }
}

void palestineFlag()
{
   // Palestine flag colors: black, white, green with red triangle
   // with animated wobble effect
   static uint16_t sOffset = 0;

   // Configurable parameters
   const uint8_t wobbleAmplitude = 3; // How many LEDs to wobble up/down
   const uint8_t wobbleSpeed = 20;    // Speed of wobble (lower = faster)

   // Define the flag colors
   CRGB black = CRGB(0, 0, 0);       // Black
   CRGB white = CRGB(255, 255, 255); // White
   CRGB green = CRGB(0, 128, 0);     // Green
   CRGB red = CRGB(255, 0, 0);       // Red

   // Calculate stripe width based on total LEDs (3 stripes)
   uint16_t stripeWidth = NUM_LEDS_EACH_SIDE / 3;
   // Red triangle takes up about 1/3 of the flag width
   uint16_t triangleWidth = NUM_LEDS_EACH_SIDE / 3;

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
      if (stripeIndex > 2)
         stripeIndex = 2;

      CRGB color;
      switch (stripeIndex)
      {
      case 0:
         color = black;
         break;
      case 1:
         color = white;
         break;
      case 2:
      default:
         color = green;
         break;
      }

      // Add red triangle on the left side
      // Triangle height decreases linearly from left to right
      float triangleRatio = (float)(triangleWidth - sourcePosition) / (float)triangleWidth;
      if (sourcePosition < triangleWidth && triangleRatio > 0)
      {
         // Calculate if this LED should be red based on triangle shape
         float normalizedStripePosition = (float)(sourcePosition % stripeWidth) / (float)stripeWidth;
         float triangleThreshold = 0.5f - (triangleRatio * 0.5f);
         float triangleThresholdUpper = 0.5f + (triangleRatio * 0.5f);

         if (normalizedStripePosition >= triangleThreshold && normalizedStripePosition <= triangleThresholdUpper)
         {
            color = red;
         }
      }

      leds[i] = color;
   }

   // Animate the wobble
   EVERY_N_MILLISECONDS(wobbleSpeed) { sOffset += 4; }
}

void solidColour()
{
   for (int i = 0; i < NUM_LEDS; i++)
   {
      // Convert selected_hsv (h: 0-360, s: 0-100, v: 0-100) to FastLED HSV (h: 0-255, s: 0-255, v: 0-255)
      uint8_t h = (selected_hsv.h * 255) / 360;
      uint8_t s = (selected_hsv.s * 255) / 100;
      uint8_t v = (selected_hsv.v * 255) / 100;
      leds[i].setHSV(h, s, v);
      // leds[i].setHSV(selected_hsv.h, selected_hsv.s, selected_hsv.v);
   }
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
void imageSwipe();

SimplePatternList gPatterns = {
    imageSwipe,
    Fire2012,
    bpm,
    juggle,
    rainbow,
    rainbowSlide,
    rainbowWithGlitter,
    sinelon,
    confetti,
    transgenderFlag,
    prideFlag,
    palestineFlag,
    solidColour};
// pattern names
const char *gPatternNames[] = {
    "Image",
    "Fire",
    "BPM",
    "Juggle",
    "Rainbow",
    "Rainbow Slide",
    "Rainbow Glitter",
    "Sinelon",
    "Confetti",
    "Trans Flag",
    "Pride Flag",
    "Palestine Flag",
    "Colour",
};

struct RGBA
{
   union
   {
      struct
      {
         union
         {
            uint8_t r;   ///< Red channel value
            uint8_t red; ///< @copydoc r
         };
         union
         {
            uint8_t g;     ///< Green channel value
            uint8_t green; ///< @copydoc g
         };
         union
         {
            uint8_t b;    ///< Blue channel value
            uint8_t blue; ///< @copydoc b
         };
         union
         {
            uint8_t a;     ///< Alpha channel value
            uint8_t alpha; ///< @copydoc a
         };
      };
      /// Access the red, green, and blue data as an array.
      /// Where:
      /// * `raw[0]` is the red value
      /// * `raw[1]` is the green value
      /// * `raw[2]` is the blue value
      /// * `raw[3]` is the alpha value
      uint8_t raw[4];
   };
};

void setFirstSelectedImageIndex()
{
   image_index_to_swipe = -1;
   for (int i = 0; i < lv_obj_get_child_cnt(ui_ContainerImages); i++)
   {
      lv_obj_t *button = lv_obj_get_child(ui_ContainerImages, i);
      if (lv_obj_has_state(button, LV_STATE_CHECKED))
      {
         if (image_index_to_swipe == -1)
         {
            image_index_to_swipe = i;
         }
      }
   }

   if (image_index_to_swipe == -1)
   {
      image_index_to_swipe = 0;
   }

   swipe_count_on_current_image = 0;
}

uint8_t getRandomSelectedImageIndex()
{
   int selected_image_indices[lv_obj_get_child_cnt(ui_ContainerImages)];
   int selectedCount = 0;
   for (int i = 0; i < lv_obj_get_child_cnt(ui_ContainerImages); i++)
   {
      lv_obj_t *button = lv_obj_get_child(ui_ContainerImages, i);
      if (lv_obj_has_state(button, LV_STATE_CHECKED) && i != image_index_to_swipe)
      {
         selected_image_indices[selectedCount++] = i;
      }
   }
   if (selectedCount == 0)
   {
      return 0;
   }

   return selected_image_indices[random(0, selectedCount - 1)];
}

void setNextSelectedImageIndex()
{
   swipe_count_on_current_image = 0;
   auto current_image_index = image_index_to_swipe;

   if (randomise_after_swipe)
   {
      image_index_to_swipe = getRandomSelectedImageIndex();
      return;
   }

   for (int i = 0; i < lv_obj_get_child_cnt(ui_ContainerImages); i++)
   {
      lv_obj_t *button = lv_obj_get_child(ui_ContainerImages, i);
      if (lv_obj_has_state(button, LV_STATE_CHECKED))
      {
         if (i > current_image_index)
         {
            image_index_to_swipe = i;
            return;
         }
      }
   }

   // if we get here we didn't find a next image, so wrap around
   setFirstSelectedImageIndex();
}

void swipeEnd()
{
   if (!swipe_in_progress)
   {
      return;
   }
   swipe_in_progress = false;
   unsigned long current_time = millis();
   last_swipe_duration = current_time - swipe_trigger_time;
   Timber.i("Swipe end, duration: %dms", last_swipe_duration);
   if (randomise_colour_after_swipe)
   {
      selected_hsv = {(unsigned short)random(0, 360), 100, 100};
      lv_colorwheel_set_hsv(ui_ColorWheel, selected_hsv);
   }
   swipe_count_on_current_image++;
   if (swipe_count_on_current_image >= swipes_per_image)
   {
      setNextSelectedImageIndex();
   }
}

void swipeImage(uint8_t image_index, float duration)
{
   Timber.i("Swipe image: %d", image_index);
   auto start_end_delay = 0;

   auto img_dsc = image_array[image_index];
   auto palette_then_image_data = img_dsc->data;

   // pallete is at start of data
   auto pallete = (RGBA *)palette_then_image_data;
   auto img_data = palette_then_image_data + 1024;
   uint32_t width = img_dsc->header.w;
   uint32_t height = img_dsc->header.h;

   delay(start_end_delay);

   auto time_of_last_gyro_update = millis();
   bool mask_pattern_mode = always_swipe && gCurrentPatternNumber > 0;

   for (uint16_t swipe_x = 0; swipe_x <= width; swipe_x++)
   {

      if (mask_pattern_mode)
      {
         // run the pattern, and mirror the leds
         gPatterns[gCurrentPatternNumber]();
         mirror_leds();
      }

      auto x = swipe_x;
      if (gyro_last_direction < 0)
      {
         x = width - swipe_x;
      }
      for (uint16_t y = 0; y < DISPLAY_VIRTUAL_LEDS_HEIGHT; y++)
      {
         // data structure is indexed pallete (4 bytes per pixel for 256 colors)
         // then pixel data is a list of indexes

         // calculate the x position from the swipe progress

         // BACK OF CANE
         auto index = img_data[y * width + x];
         RGBA color = pallete[index];
         if (mask_pattern_mode)
         {
            if (color.r == 0 && color.g == 0 && color.b == 0)
            {
               leds[NUM_LEDS_EACH_SIDE + y] = CRGB(0, 0, 0);
            }
         }
         else
         {
            leds[NUM_LEDS_EACH_SIDE + y] = CRGB(color.b, color.g, color.r);
         }

         // FRONT OF CANE
         auto front_x = width - x;
         index = img_data[y * width + front_x];
         color = pallete[index];
         if (mask_pattern_mode)
         {
            if (color.r == 0 && color.g == 0 && color.b == 0)
            {
               leds[NUM_LEDS_EACH_SIDE - y - 1] = CRGB(0, 0, 0);
            }
         }
         else
         {
            leds[NUM_LEDS_EACH_SIDE - y - 1] = CRGB(color.b, color.g, color.r);
         }
      }
      FastLED.show();
      // auto delay_time = (unsigned long)(duration / 2.0F / (float)width);
      // Timber.i("delay_time: %d", delay_time);
      if (width < 111)
      {
         delay(1);
      }

      // // update gyro every 100ms during swipe
      // auto current_time = millis();
      // auto time_since_last_gyro_update = current_time - time_of_last_gyro_update;
      // if (time_since_last_gyro_update > 100)
      // {
      //    time_of_last_gyro_update = current_time;
      //    update_gyro();
      // }
   }

   delay(start_end_delay);

   swipeEnd();
}

float linear_map(float x, float x1, float y1, float x2, float y2)
{
   return ((y2 - y1) / (x2 - x1)) * (x - x1) + y1;
}

float map_force_to_estimated_duration(float input)
{
   // Linear mapping from (100,420) and (200,300)
   // Slope: m = (300-420)/(200-100) = -120/100 = -1.2
   // Intercept: b = 420 - (-1.2 * 100) = 540

   return linear_map(input, 100, 420, 200, 300);
}

void swipeStart(float gyroForce)
{
   if (swipe_in_progress)
   {
      swipeEnd();
   }
   gyro_last_direction = gyroForce > 0 ? 1 : -1;
   gyro_last_trigger_force = fabs(gyro_z);
   Timber.i("%s force: %.2f, est duration: %.0fms", gyro_last_direction > 0 ? "LTR -->" : "RTL <--", gyro_last_trigger_force, map_force_to_estimated_duration(gyro_last_trigger_force));
   unsigned long current_time = millis();
   swipe_trigger_time = current_time;
   swipe_in_progress = true;
   uint16_t duration = map_force_to_estimated_duration(gyro_last_trigger_force);
   swipeImage(image_index_to_swipe, duration);
}

void detectSwipe()
{
   // Timber.i("gyro_z: %f", gyro_z);
   if (gyro_last_direction <= 0)
   {
      if (gyro_z > gyro_trigger_threshold)
      {
         swipeStart(gyro_z);
      }
   }

   if (gyro_last_direction >= 0)
   {
      if (gyro_z < -gyro_trigger_threshold)
      {
         swipeStart(gyro_z);
      }
   }

   if (fabs(gyro_z) < gyro_swipe_end_threshold && gyro_last_direction != 0)
   {
      swipeEnd();
   }

   // clear LEDs
   for (int i = 0; i < NUM_LEDS; i++)
   {
      leds[i] = CRGB(0, 0, 0);
   }
   FastLED.show();
}

void imageSwipe()
{
   detectSwipe();
}

void setModeContainer(uint8_t index)
{
   if (index == 0)
   {
      lv_obj_add_flag(ui_ContainerAlwaysSwipe, LV_OBJ_FLAG_HIDDEN);
   }
   else
   {
      lv_obj_clear_flag(ui_ContainerAlwaysSwipe, LV_OBJ_FLAG_HIDDEN);
   }

   lv_obj_t *gModeContainers[] = {
       ui_ContainerSettingsImage,
       ui_ContainerSettingsFire,
       NULL, // bpm
       NULL, // juggle
       NULL, // rainbow
       NULL, // rainbowSlide
       NULL, // rainbowWithGlitter
       NULL, // sinelon
       NULL, // confetti
       NULL, // transgenderFlag
       NULL, // prideFlag
       NULL, // palestineFlag
       ui_ContainerSettingsColour};

   if (index >= ARRAY_SIZE(gModeContainers))
   {
      Timber.e("Invalid index %d", index);
      return;
   }
   // lv_obj_add_flag(ui_ContainerSettingsFire, LV_OBJ_FLAG_HIDDEN);

   // return;
   for (uint8_t i = 0; i < ARRAY_SIZE(gModeContainers); i++)
   {
      lv_obj_t *modeContainer = gModeContainers[i];
      if (modeContainer != NULL)
      {
         if (i != index)
         {
            // Timber.i("Hiding mode container %d", i);
            lv_obj_add_flag(modeContainer, LV_OBJ_FLAG_HIDDEN);
         }
         else
         {
            if (lv_obj_has_flag(modeContainer, LV_OBJ_FLAG_HIDDEN))
            {
               lv_obj_clear_flag(modeContainer, LV_OBJ_FLAG_HIDDEN);
            }
         }
      }
   }
}

void setCurrentPatternLabel()
{
   lv_dropdown_set_selected(ui_DropdownMode, gCurrentPatternNumber);
   // lv_label_set_text(ui_LabelCurrentPattern, gPatternNames[gCurrentPatternNumber]);
}

void setPatternIndex(uint8_t index)
{
   gCurrentPatternNumber = index;
   setCurrentPatternLabel();
   setModeContainer(index);
}

void nextPattern()
{
   // add one to the current pattern number, and wrap around at the end
   setPatternIndex((gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns));
}

void prevPattern()
{
   // subtract one from the current pattern number, and wrap around at the end
   setPatternIndex((gCurrentPatternNumber - 1 + ARRAY_SIZE(gPatterns)) % ARRAY_SIZE(gPatterns));
}
