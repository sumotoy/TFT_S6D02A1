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
  tft.setRotation(3);
  tft.fillScreen(S6D02A1_BLACK);
  tft.setTextWrap(true);
  tft.setTextColor(S6D02A1_WHITE);
  tft.setCursor(0,0);
  //tft.drawCircle(80,64,51,S6D02A1_WHITE);
}


volatile long ttt = 0;
void loop(){
  if (ttt > 18000L){
    ttt = 0;
  } 
  else {
    ttt++;
  }
  //analog_clock(80,64,62,ttt-1,(ttt-1),S6D02A1_BLACK);
  //delayMicroseconds(10);
  analog_clock(80,64,63,ttt,(ttt),(uint16_t)ttt);
  //delay(5);
}


void analog_clock(int x, int y, int radius, long angle_start, long angle_end, uint16_t color)
{
  long	x_kreis, y_kreis;
  double winkel;
#define DEGREE 2*PI/18000L	
  for (winkel = (double)angle_start; winkel <= angle_end; winkel +=1) // Calculate clock hands
  {
    x_kreis = (long) (sin (winkel * DEGREE) * radius);
    y_kreis = (long) (cos (winkel * DEGREE) * radius);
    tft.drawLine(x,y, x + x_kreis, y - y_kreis, color); //draw clock hands
    //tft.drawPixel(x + x_kreis, y - y_kreis, random(0x001F,0xFFFF)); //draw clock hands
    
  }
}
