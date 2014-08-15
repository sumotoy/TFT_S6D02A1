/*
  An example analogue/digital clock using a TFT LCD screen to show the 
 use of some of the drawing commands with the TFT_S6D02A1 library.
 For a more accurate clock, it would be better to use the RTClib library.
 But this is just a demo. 
 
 This examples uses the hardware SPI only. Non-hardware SPI
 is just too slow (~8 times slower!)
 
 Gilchrist 6/2/2014 1.0
 */

#define sclk 13  // Don't change
#define mosi 11  // Don't change
#define cs   20
#define dc   2
#define rst  14  // you can also connect this to the Arduino reset
#include <Adafruit_GFX.h>    // Core graphics library
#include <TFT_S6D02A1.h> // Hardware-specific library
#include <SPI.h>

TFT_S6D02A1 tft = TFT_S6D02A1(cs, dc, rst);       // Invoke custom library

float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0;    // Saved H, M, S x & y multipliers
float sdeg=0, mdeg=0, hdeg=0;
uint16_t osx=64, osy=64, omx=64, omy=64, ohx=64, ohy=64;  // Saved H, M, S x & y coords
uint16_t x0=0;
uint16_t x1=0;
uint16_t yy0=0;
uint16_t yy1=0;
uint32_t targetTime = 0;                    // for next 1 second timeout
uint8_t hh=conv2d(__TIME__), mm=conv2d(__TIME__+3), ss=conv2d(__TIME__+6);  // Get H, M, S from compile time

void setup(void) {
  tft.init();

  tft.setRotation(1);
  tft.fillScreen(S6D02A1_BLACK);
  tft.setTextColor(S6D02A1_WHITE, S6D02A1_BLACK);  // Adding a black background colour erases previous text automatically

  // Draw clock face
  tft.fillCircle(64, 64, 61, S6D02A1_BLUE);
  tft.fillCircle(64, 64, 57, S6D02A1_BLACK);

  // Draw 12 lines
  for(int i = 0; i<360; i+= 30) {
    sx = cos((i-90)*0.0174532925);
    sy = sin((i-90)*0.0174532925);
    x0 = sx*57+64;
    yy0 = sy*57+64;
    x1 = sx*50+64;
    yy1 = sy*50+64;

    tft.drawLine(x0, yy0, x1, yy1, S6D02A1_BLUE);
  }
  tft.fillCircle(65, 65, 3, S6D02A1_RED);
  tft.setCursor (34, 151);
  tft.print(__DATE__);
  targetTime = millis() + 1000; 
}

void loop() {
  if (targetTime < millis()) {
    targetTime = millis()+1000;
    ss++;              // Advance second
    if (ss==60) {
      ss=0;
      mm++;            // Advance minute
      if(mm>59) {
        mm=0;
        hh++;          // Advance hour
        if (hh>23) {
          hh=0;
        }
      }
    }

    // Pre-compute hand degrees, x & y coords for a fast screen update
    sdeg = ss*6;                  // 0-59 -> 0-354
    mdeg = mm*6+sdeg*0.01666667;  // 0-59 -> 0-360 - includes seconds
    hdeg = hh*30+mdeg*0.0833333;  // 0-11 -> 0-360 - includes minutes and seconds
    hx = cos((hdeg-90)*0.0174532925);    
    hy = sin((hdeg-90)*0.0174532925);
    mx = cos((mdeg-90)*0.0174532925);    
    my = sin((mdeg-90)*0.0174532925);
    sx = cos((sdeg-90)*0.0174532925);    
    sy = sin((sdeg-90)*0.0174532925);

    // Erase just old hand positions
    tft.drawLine(ohx, ohy, 65, 65, S6D02A1_BLACK);  
    tft.drawLine(omx, omy, 65, 65, S6D02A1_BLACK);  
    tft.drawLine(osx, osy, 65, 65, S6D02A1_BLACK);
    // Draw new hand positions  
    tft.drawLine(hx*33+65, hy*33+65, 65, 65, S6D02A1_WHITE);
    tft.drawLine(mx*44+65, my*44+65, 65, 65, S6D02A1_WHITE);
    tft.drawLine(sx*47+65, sy*47+65, 65, 65, S6D02A1_RED);
    tft.fillCircle(65, 65, 3, S6D02A1_RED);

    // Update old x&y coords
    osx = sx*47+65;    
    osy = sy*47+65;
    omx = mx*44+65;    
    omy = my*44+65;
    ohx = hx*33+65;    
    ohy = hy*33+65;

    // Update digital time
    tft.setCursor (34, 140);

    if(hh>12) {
      if (hh<22) tft.print('0');      
      tft.print (hh-12);
    } 
    else {
      if (hh<10) tft.print('0');
      tft.print (hh);
    }
    tft.print (':');
    if (mm<10) tft.print('0');
    tft.print (mm);
    tft.print (':');
    if (ss<10) tft.print('0');
    tft.print (ss);
    if (hh>12) tft.print(" pm"); 
    else tft.print (" am");
  }
}

static uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}
