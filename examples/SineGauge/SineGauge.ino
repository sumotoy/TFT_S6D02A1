#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TFT_S6D02A1.h>

#include <arm_math.h>

#define TFT_CS_PIN        20
#define TFT_DC_PIN        2
#define TFT_RST_PIN       14

boolean graphRefresh = true;

TFT_S6D02A1 tft = TFT_S6D02A1(TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);


void setup()
{
  Serial.begin(38400);
  tft.init();   
  tft.setRotation(1);
  tft.fillScreen(S6D02A1_BLACK);
  tft.setTextWrap(true);
  tft.setTextColor(S6D02A1_WHITE);
  tft.setCursor(0,0);

}

volatile uint8_t countScreen = 0;
const uint16_t clrs[5] = {
  0xF800,
  0x07E0,
  0x07FF,
  0xF81F,
  0xFFE0
};

void loop(){

  if (countScreen > 5){
    countScreen = 0;
    graphRefresh = true;
  } 
  else {
    countScreen++;
  }
  sineTest(20,10,159,80,clrs[countScreen]);
}



void sineTest(uint8_t x1,uint8_t y1,uint8_t w,uint8_t h,uint16_t gcolor){
  boolean grid = true;
  uint8_t freq = random(2,10);
  if (graphRefresh){
    tft.fillScreen(S6D02A1_BLACK);
    if ((w + x1) > S6D02A1_TFTHEIGHT) w = S6D02A1_TFTHEIGHT - x1;
    if ((h + y1) > S6D02A1_TFTWIDTH) h = S6D02A1_TFTWIDTH - y1;
    tft.drawRect(x1, y1, w, h,S6D02A1_WHITE);
    uint8_t div = h/10;

    for (int t=0;t<=10;t++){
      if (t < 10) {
        tft.drawFastHLine(x1-3,y1+(t*div),6,S6D02A1_WHITE);
        if (grid && t > 0) tft.drawFastHLine(x1+3,y1+(t*div),w-4,S6D02A1_BLUE);
      }
    }

    div = w/10;
    for (int t=0;t<=10;t++){
      tft.drawFastVLine(x1+(t*div),h+y1-3,6,S6D02A1_WHITE);
      if (grid && t > 0) tft.drawFastVLine(x1+(t*div),y1+1,h-4,S6D02A1_BLUE);
    }

    tft.drawLine(x1, (h/2) + y1, w+x1, (h/2) + y1,S6D02A1_WHITE);
    graphRefresh = false;
  }
  for (int i=x1; i<=w+x1; i++)
  {
    double y = i*0.017453292519943295769236907684886;
    tft.drawPixel(i,(arm_sin_f32(y*freq)*32)+(h/2)+y1,gcolor);
    delay(10);
  }

}
