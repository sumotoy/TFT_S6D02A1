#include <Adafruit_GFX.h>
#include <TFT_S6D02A1.h> // Hardware-specific library
#include <SPI.h>

#define cs   20
#define dc   2
#define rst  14 
#define BLACK 0
#define WHITE 1

TFT_S6D02A1 lcd = TFT_S6D02A1(cs, dc, rst); 

float sin_d[] = { 
  0,0.17,0.34,0.5,0.64,0.77,0.87,0.94,0.98,1,0.98,0.94,
  0.87,0.77,0.64,0.5,0.34,0.17,0,-0.17,-0.34,-0.5,-0.64,
  -0.77,-0.87,-0.94,-0.98,-1,-0.98,-0.94,-0.87,-0.77,
  -0.64,-0.5,-0.34,-0.17 };
float cos_d[] = { 
  1,0.98,0.94,0.87,0.77,0.64,0.5,0.34,0.17,0,-0.17,-0.34,
  -0.5,-0.64,-0.77,-0.87,-0.94,-0.98,-1,-0.98,-0.94,-0.87,
  -0.77,-0.64,-0.5,-0.34,-0.17,0,0.17,0.34,0.5,0.64,0.77,
  0.87,0.94,0.98};

float d = 10;
float px[] = { 
  -d,  d,  d, -d, -d,  d,  d, -d };
float py[] = { 
  -d, -d,  d,  d, -d, -d,  d,  d };
float pz[] = { 
  -d, -d, -d, -d,  d,  d,  d,  d };

float p2x[] = {
  0,0,0,0,0,0,0,0};
float p2y[] = {
  0,0,0,0,0,0,0,0};

int r[] = {
  0,0,0};

void setup(void) 
{
  lcd.init();
  lcd.setRotation(0);	// 0 - Portrait, 1 - Lanscape
  lcd.fillScreen(S6D02A1_BLACK);
}

void loop(){
  lcd.fillScreen(S6D02A1_BLACK);
  r[0]=r[0]+1;
  r[1]=r[1]+1;
  if (r[0] == 36) r[0] = 0;
  if (r[1] == 36) r[1] = 0;
  if (r[2] == 36) r[2] = 0;
  for (int i=0;i<8;i++)
  {
    float px2 = px[i];
    float py2 = cos_d[r[0]]*py[i] - sin_d[r[0]]*pz[i];
    float pz2 = sin_d[r[0]]*py[i] + cos_d[r[0]]*pz[i];

    float px3 = cos_d[r[1]]*px2 + sin_d[r[1]]*pz2;
    float py3 = py2;
    float pz3 = -sin_d[r[1]]*px2 + cos_d[r[1]]*pz2;

    float ax = cos_d[r[2]]*px3 - sin_d[r[2]]*py3;
    float ay = sin_d[r[2]]*px3 + cos_d[r[2]]*py3;
    float az = pz3-190;

    p2x[i] = ((lcd.width())/2)+ax*500/az;
    p2y[i] = ((lcd.height())/2)+ay*500/az;
  }
  for (int i=0;i<3;i++) {
    lcd.drawLine(p2x[i],p2y[i],p2x[i+1],p2y[i+1],S6D02A1_GREEN);
    lcd.drawLine(p2x[i+4],p2y[i+4],p2x[i+5],p2y[i+5],S6D02A1_GREEN);
    lcd.drawLine(p2x[i],p2y[i],p2x[i+4],p2y[i+4],S6D02A1_GREEN);
  }   
  lcd.drawLine(p2x[3],p2y[3],p2x[0],p2y[0],S6D02A1_GREEN);
  lcd.drawLine(p2x[7],p2y[7],p2x[4],p2y[4],S6D02A1_GREEN);
  lcd.drawLine(p2x[3],p2y[3],p2x[7],p2y[7],S6D02A1_GREEN);
  //lcd.refresh();
  delay(10);
}
