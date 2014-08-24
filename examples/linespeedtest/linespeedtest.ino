
#define cs   20
#define dc   2
#define rst  14  // you can also connect this to the Arduino reset
#include <Adafruit_GFX.h>    // Core graphics library
#include <TFT_S6D02A1.h> // Hardware-specific library
#include <SPI.h>

TFT_S6D02A1 tft = TFT_S6D02A1(cs, dc, rst);  // Invoke custom library


void setup(void) {
  tft.init();
  uint16_t time = millis();
  tft.setRotation(1);	// 0 - Portrait, 1 - Lanscape
  tft.fillScreen(S6D02A1_BLACK);
  tft.setTextWrap(true);
  tft.setTextSize(1);
}

void loop() {
  testSpeed();
}


void testSpeed(){
  tft.fillScreen(S6D02A1_BLACK);
  unsigned long stime, etime;
  stime = micros();

  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, 0, x, tft.height()-1, S6D02A1_WHITE);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, 0, tft.width()-1, y, S6D02A1_WHITE);
  }

  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, 0, x, tft.height()-1, S6D02A1_GREEN);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, 0, 0, y, S6D02A1_GREEN);
  }

  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, tft.height()-1, x, 0, S6D02A1_BLUE);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, tft.height()-1, tft.width()-1, y, S6D02A1_BLUE);
  }

  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, x, 0, S6D02A1_MAGENTA);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, 0, y, S6D02A1_MAGENTA);
  }

  etime = micros();
  delay(1000);
  tft.fillScreen(S6D02A1_BLACK);

  tft.setTextColor (S6D02A1_WHITE,S6D02A1_BLACK);
  tft.setCursor (10, 10);
  tft.print((etime-stime),DEC);
  tft.println(" microsec");
  delay(1000);
}


