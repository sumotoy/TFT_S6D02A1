#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TFT_S6D02A1.h>

#define TFT_CS_PIN        20
#define TFT_DC_PIN        2
#define TFT_RST_PIN       14

#define  NBINS                10

  
const       uint8_t      bar_Width =     7;
uint32_t     avrg_TmrF =    0;
uint16_t     t_b[NBINS];

uint16_t datax_[NBINS];

TFT_S6D02A1 tft = TFT_S6D02A1(TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);


void setup(void) {
  Serial.begin(38400);
  //while(!Serial);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(S6D02A1_BLACK);
  tft.setTextWrap(true);
  tft.setTextColor(S6D02A1_WHITE, S6D02A1_BLACK);
  tft.setCursor(0,0);
}


void loop(){
  for (int i=0;i<NBINS;i++){
    t_b[i] = random(0,4096);
    datax_[i] = random(0,1024);
  }
  //Print_Data();
  verticalBarGraphs(datax_,12,64,10);
  delay(10);
}


void verticalBarGraphs(uint16_t datax[],uint8_t barWidth,uint8_t barHeight,uint8_t vOrigin){//3,12,64,10
  uint8_t startX;
  uint16_t color;
  uint8_t dataToWidth;
  uint8_t div;
  for (uint8_t i = 1; i <= NBINS-1; i++) {
    startX = (i * 15);
    tft.drawRect((startX-1),vOrigin,barWidth,barHeight,S6D02A1_WHITE);//container
    dataToWidth = map(datax[i],0,1024,(barHeight-2),0);
    div = (barHeight-2)/10;
    color = tft.decodeCScale(dataToWidth,div,0);
    tft.fillRect(startX,(vOrigin+1),(bar_Width+3),dataToWidth,S6D02A1_BLACK);//mask ok
    tft.fillRect(startX,(dataToWidth+vOrigin)+1,(bar_Width+3),((barHeight-2)-dataToWidth),color);//fillRect(X,Y,width,height,color)
  }
}



void printBits(int32_t data,uint8_t howMany){
  for (int i=howMany-1; i>=0; i--){
    if (bitRead(data,i)==1){
      Serial.print("1");
    } 
    else {
      Serial.print("0");
    }
  }
  Serial.print(" -> 0x");
  Serial.print(data,HEX);
  Serial.print("\n");
}