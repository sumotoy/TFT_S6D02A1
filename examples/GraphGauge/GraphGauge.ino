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

volatile int data_[159-20];

void sineTest(uint8_t x1,uint8_t y1,uint8_t w,uint8_t h,uint16_t gcolor){
  boolean grid = true;

  const uint8_t divisions = 26;
  const int minVal = 0;
  const int maxVal = 1023;
  
  tft.setTextColor(gcolor,S6D02A1_BLACK);
  if (graphRefresh){
    tft.fillScreen(S6D02A1_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(gcolor,S6D02A1_BLACK);
    if ((w + x1) > S6D02A1_TFTHEIGHT) w = S6D02A1_TFTHEIGHT - x1;
    if ((h + y1) > S6D02A1_TFTWIDTH) h = S6D02A1_TFTWIDTH - y1;
    tft.drawRect(x1, y1, w, h,S6D02A1_WHITE);
    int div = h/divisions;

    for (int t=0;t<=divisions;t++){
      if (t <= divisions) {
        tft.drawFastHLine(x1-3,y1+(t*div),6,S6D02A1_WHITE);
        if (grid && t > 0) tft.drawFastHLine(x1+3,y1+(t*div),w-4,S6D02A1_BLUE);
      }
    }

    div = w/divisions;
    for (int t=0;t<=divisions+1;t++){
      tft.drawFastVLine(x1+(t*div),h+y1-3,6,S6D02A1_WHITE);
      if (grid && t > 0) tft.drawFastVLine(x1+(t*div),y1+1,h-4,S6D02A1_BLUE);
    }

    tft.drawLine(x1, (h/2) + y1, w+x1, (h/2) + y1,S6D02A1_WHITE);
    graphRefresh = false;
  }
  // data loop------------------------------------------
  for (int i=0;i<w-x1;i++){
    data_[i] = random(minVal,maxVal);
    int dataY = map(data_[i],minVal,maxVal,-(h/2),(h/2));
    if (dataY < 0) {
      tft.drawRect(x1+i+2,dataY+(h)+y1-1,2,2,gcolor);
    } 
    else if (dataY == 0) {
      tft.drawRect(x1+i+2,h/2+y1-1,2,2,gcolor);
    } 
    else {
      tft.drawRect(x1+i+2,dataY+y1-1,2,2,gcolor);
    }
    if (i > 0){
      tft.setCursor(20,105);
      tft.setTextColor(S6D02A1_BLACK,S6D02A1_BLACK);
      tft.print(map(data_[i-1],minVal,maxVal,-512,512),DEC);
    }
    tft.setTextColor(gcolor,S6D02A1_BLACK);
    tft.setCursor(20,105);
    tft.print(map(data_[i],minVal,maxVal,-512,512),DEC);
    delay(100);
  }
}
