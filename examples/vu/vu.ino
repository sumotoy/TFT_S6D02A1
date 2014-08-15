
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TFT_S6D02A1.h>


#define TFT_CS_PIN        20
#define TFT_DC_PIN        2
#define TFT_RST_PIN       14

boolean redrawTable = true;
int16_t     adc_Offst =   512;
volatile     int32_t     ppm_Level =     0;
float       rms_Level =   0.0;

TFT_S6D02A1 tft = TFT_S6D02A1(TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);


void setup(void) {
  Serial.begin(38400);
  tft.init();  
  tft.setRotation(1);
  tft.fillScreen(S6D02A1_BLACK);
  tft.setTextWrap(true);
  tft.setTextColor(S6D02A1_WHITE);
  tft.setCursor(0,0);
  adc_Offst = analogRead(A5);

}


void loop(){
  if (redrawTable){
    Draw_Table();
    redrawTable = false;
  }
  char incomingByte;
  int32_t temp;
  audioIn();
  temp = ppm_Level;                   // Take a copy, so Real Value not affected by calculation.
  temp = sqrt(temp);
  rms_Level = 20.0 * log10(temp +1);  // Calculated, available over Serial
  Draw_Arrow(temp);
  delay(5);
}


void Draw_Table(){
  tft.setCursor(3,3);
  tft.setTextColor(S6D02A1_GREEN);
  tft.print("-30 -20 10 5 3 1 ");
  tft.setTextColor(S6D02A1_RED);
  tft.print("0 1 2 3 4");
  tft.setTextColor(S6D02A1_WHITE);
  //tft.setCursor(5,52);
  //tft.print("    VU");
  //tft.setCursor(75,52);
  //tft.print("   dB");
  tft.drawRect(0,0,160,64,S6D02A1_WHITE);  
  //tft.drawLine(80,63,5,10,ST7735_WHITE) ;
  //tft.drawLine(63,62,4,10,ST7735_WHITE) ;
}

void Draw_Arrow(int32_t scale){
  const uint8_t min_ = 5;
  const uint8_t max_ = 155;
  const uint8_t endArrow = 20;
  static int st1 = min_;
  static int st2 = min_;
  st2 = map(scale,20,300,min_,max_);     //  23.5 dB  
  if (st2 > max_) st2 = max_;
  if (st2 < min_) st2 = min_;
  if (abs(st1 - st2) > 3)               // 1/3 dB
  { 
    tft.drawLine(80,63,st1,endArrow,0x0000);
    if (st1 == min_) tft.drawLine(79,63,st1 -1,endArrow,S6D02A1_BLACK);
    tft.drawLine(80,63,st2,endArrow,S6D02A1_CYAN);
    st1 = st2;
  }
  //tft.drawCircle(80,63,10,S6D02A1_WHITE);
}


void audioIn()
{ 
  int32_t temp = random(0,1024) - adc_Offst; 
  temp = temp * temp;
  if (temp > ppm_Level) {
    ppm_Level = ((ppm_Level * 255) + temp) >> 8;
  }
  else{
    ppm_Level = (ppm_Level * 16383) >> 14; 
  }  
}


