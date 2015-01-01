/*----------------------------------------------------------------------
  Spark Core Adafruit DotStar 60-LED Strip New Year's Eve Countdown!

  BDub 12-31-2014
  --------------------------------------------------------------------*/

/* ======================= includes ================================= */

#include "application.h"
#include "dotstar.h"
#include "TimeAlarms.h"

/* ======================= prototypes =============================== */

void colorAll(uint32_t c, uint8_t wait);
void colorWipe(uint32_t c, uint8_t wait);
void rainbow(uint8_t wait);
void rainbowCycle(uint8_t wait);
uint32_t Wheel(byte WheelPos);
void midnightCountdown(void);
void letsDoThis(void);
void flashRandom(uint8_t pixel, uint8_t howmany, int wait);
void firework(uint8_t pixel, uint8_t howmany, uint8_t size, int wait);
void nuke(uint8_t size, int cycles, int wait);

/* ======================= globals ================================== */

uint32_t color;
uint32_t newTime;
uint32_t oldTime;
bool countdown = true;

// Here is where you can put in your favorite colors that will appear!
// just add new {nnn, nnn, nnn}, lines. They will be picked out randomly
//                                  R   G   B
// BLUES
uint8_t myFavoriteColors[][3] = {{  0, 200, 200},   // purple
                                 {  0, 100, 100},   // red
                                 {  0,   0, 200},   // white
                                 {  0, 100, 200},   // white
                                 {  0,   0,  50},   // white
                               };
// // REDS, ORANGES and YELLOWS
// uint8_t myFavoriteColors[][3] = {{  200,   0, 0 },
//                                  {  255,  50, 0 },
//                                  {   50,   0, 0 },
//                                  {  100,  50, 0 },
//                                  {  200, 200, 0 },
//                                  {   30,  30, 0 },
//                                };

// don't edit the line below
#define FAVCOLORS sizeof(myFavoriteColors) / 3

/* ======================= defines ================================== */

#define NUMPIXELS 60 // Number of LEDs in strip

// Here's how to control the LEDs from any two pins (Software SPI):
//-------------------------------------------------------------------
#define DATAPIN   D4
#define CLOCKPIN  D5
//Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN);

// Here's how to control the LEDs from any two pins (Hardware SPI):
//-------------------------------------------------------------------
// Hardware SPI is a little faster, but must be wired to specific pins
// (Spark Core = pin A5 for data, A3 for clock)
Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS);

void setup() {

  ////========================////
  ////  I M P O R T A N T !   ////
  ////  SET YOUR TIME ZONE !  ////
  ////========================////
  Time.zone(-6); // -8 = PST, -6 = CST, -5 = EST

  newTime = Time.second();
  oldTime = newTime;
  //Serial.begin(9600);

  // Create the alarms, it will set off the countdown for as many times as you wish!
  Alarm.alarmRepeat(22,59,0, midnightCountdown);  // 10:59pm EST
  Alarm.alarmRepeat(23,59,0, midnightCountdown);  // 11:59pm CST
  Alarm.alarmRepeat( 1,59,0, midnightCountdown);  //  1:59am PST

  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP

  RGB.brightness(5); // turn down the brightness of the RGB LED
}

void loop() {

  // Wait 100ms in a hard loop while processing the set Alarms
  Alarm.delay(100);

  // If it's 1 minute to midnight, let's light the fuse!
  if (countdown) {
    letsDoThis();
  }

}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

// function to be called when it's one minute to midnight:
void midnightCountdown(){
  //Serial.println("Midnight Countdown!");
  //rainbow(20);
  countdown = true;
}

void letsDoThis() {
  uint32_t color = Wheel(random(0,170)); // Omits blues, since our sparks are blue
  newTime = Time.second();
  strip.setPixelColor(59-newTime, color);
  strip.show();
  flashRandom(59-newTime, 5, 3); // Blue sparks!
  if (newTime != oldTime) {
    strip.setPixelColor(59-oldTime, 0UL);
    oldTime = newTime;
    strip.show();

    // Is it the end of the countdown??
    if (newTime == 0) {
      // After the countdown, make sure the first pixel isn't stuck on
      strip.setPixelColor(59-newTime, 0UL);
      strip.show();

      // First one is a doosie!
      nuke(60, 200, 20);

      // Fireworks time for 3 minutes!!
      uint8_t pix;
      uint8_t startMinute = Time.minute();
      while ( Time.minute() - startMinute < 3) {
        // Pick a random pixel to create a firework at
        pix = random(NUMPIXELS);
        strip.setPixelColor(pix, 0xFFFFFF); // WHITE
        strip.show();
        // Blow it up!
        firework(pix, 20, random(10,20), 10);
      }

      // Disable the fireworks and countdown from repeating
      countdown = false;
    }
  }
}

void flashRandom(uint8_t pixel, uint8_t howmany, int wait) {

  for(uint8_t i=0; i<howmany; i++) {

    // Pick a random favorite color!
    int c = random(FAVCOLORS);
    int red = myFavoriteColors[c][0];
    int green = myFavoriteColors[c][1];
    int blue = myFavoriteColors[c][2];

    // Get a random pixel around the current pixel,
    // but don't overwrite the current pixel!
    int j;
    do {
      j = random(pixel-3,pixel+4);
    } while (j == pixel);

    // now we will 'fade' it in 2 steps
    for (int x=0; x < 2; x++) {
      uint8_t r = red * (x+1); r /= 2;
      uint8_t g = green * (x+1); g /= 2;
      uint8_t b = blue * (x+1); b /= 2;

      strip.setPixelColor(j, r, g, b);
      strip.show();
      delay(wait);
    }

    // & fade out in 5 steps
    for (int x=5; x >= 0; x--) {
      uint8_t r = red * x; r /= 5;
      uint8_t g = green * x; g /= 5;
      uint8_t b = blue * x; b /= 5;

      strip.setPixelColor(j, r, g, b);
      strip.show();
      delay(wait);
    }
  }
  // LEDs will be off when done (they are faded to 0)
}

void firework(uint8_t pixel, uint8_t howmany, uint8_t size, int wait) {

  // Fade out the WHITE center LED
  uint8_t center_r = 200;
  uint8_t center_g = 200;
  uint8_t center_b = 200;

  for(uint8_t i=0; i<howmany; i++) {

    if (i > howmany/2) {
      // Fade out the center LED
      center_r /= 2;
      center_g /= 2;
      center_b /= 2;
      strip.setPixelColor(pixel, center_r, center_g, center_b);
      // no strip.show() to help speed up this routine
      // the next strip.show() will write out these values
    }

    // Pick a random color of the rainbow!
    uint32_t c = Wheel(random(256));
    uint8_t green = (uint8_t)(c >>  8); // G
    uint8_t blue  = (uint8_t)c;         // B
    uint8_t red   = (uint8_t)(c >> 16); // R

    // Get a random pixel around the current pixel,
    // but don't overwrite the current pixel!
    int j;
    do {
      j = random(pixel-(size/2),pixel+(size/2)+1);
    } while (j == pixel);

    // now we will 'fade' it in 2 steps
    for (int x=0; x < 2; x++) {
      uint8_t r = red * (x+1); r /= 2;
      uint8_t g = green * (x+1); g /= 2;
      uint8_t b = blue * (x+1); b /= 2;

      strip.setPixelColor(j, r, g, b);
      strip.show();
      delay(wait);
    }

    // & fade out in 5 steps
    for (int x=5; x >= 0; x--) {
      uint8_t r = red * x; r /= 5;
      uint8_t g = green * x; g /= 5;
      uint8_t b = blue * x; b /= 5;

      strip.setPixelColor(j, r, g, b);
      strip.show();
      delay(wait);
    }

  }
  // LEDs will be off when done (they are faded to 0)
}

void nuke(uint8_t size, int cycles, int wait) {

  for (uint8_t x=0; x<cycles; x++) {
    for (uint8_t i=0; i<size; i++) {

      // Pick a random color of the rainbow!
      uint32_t c = Wheel(random(256));
      strip.setPixelColor(i, c);

    }
    strip.show();
    delay(wait);
  }

  // Fade out the last array of colors
  for (uint8_t x=0; x<8; x++) {
    for (uint8_t i=0; i<size; i++) {
      uint32_t c = strip.getPixelColor(i);
      uint8_t green = (uint8_t)(c >>  8); // G
      uint8_t blue  = (uint8_t)c;         // B
      uint8_t red   = (uint8_t)(c >> 16); // R
      green /= 2;
      blue /= 2;
      red /= 2;
      strip.setPixelColor(i, red, green, blue);
    }
    strip.show();
    delay(wait*2);
  }
}
