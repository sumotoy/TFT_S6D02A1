#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TFT_S6D02A1.h>

#define TFT_CS_PIN        20
#define TFT_DC_PIN        2
#define TFT_RST_PIN       14

//colors (uses 24 bit color! will be trasformed later
#define  COMPASS_BODY_COLOR        0x555555
#define  COMPASS_CIRCLES_COLOR     0xFFFF26
#define  COMPASS_NEEDLE_COLOR      0xFFFFFF  
#define  COMPASS_TICKS_COLOR       0x6BFB68
#define  COMPASS_TEXT_COLOR        0xFFFFFF
#define  COMPASS_TRIMARKS_COLOR    0xFF0000

TFT_S6D02A1 tft = TFT_S6D02A1(TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);

const double tickMarks[12] = {
  22.5, 45, 67.5, 112.5, 135, 157.5, 202.5, 225, 247.5, 292.5, 315, 337.5};
const double myPI = 3.14;

//vars
double y_to = 0;//vecchia Posizione
double x_to = 0;//vecchia Posizione
double calcSin = 0;

boolean redrawCompassBody  = true;

//here I transform 24 bit color to 18 bit color
const uint16_t comp_body_col = tft.Color24To565(COMPASS_BODY_COLOR);
const uint16_t comp_bcircles_col = tft.Color24To565(COMPASS_CIRCLES_COLOR);
const uint16_t comp_needle_col = tft.Color24To565(COMPASS_NEEDLE_COLOR);
const uint16_t comp_ticks_col = tft.Color24To565(COMPASS_TICKS_COLOR);
const uint16_t comp_text_col = tft.Color24To565(COMPASS_TEXT_COLOR);
const uint16_t comp_tri_col = tft.Color24To565(COMPASS_TRIMARKS_COLOR);

void setup() {
  Serial.begin(38400);
  tft.init();   // initialize a ST7735S chip, black tab
  tft.setRotation(1);
  tft.fillScreen(S6D02A1_BLACK);
  tft.setTextWrap(true);
  tft.setTextColor(S6D02A1_WHITE);
  tft.setCursor(0,0);
  tft.setTextSize(1);
  tft.fillScreen(S6D02A1_BLACK);
  
}

void loop() {
  if (redrawCompassBody){
      compassBody(80,64,40);//draw the compass body just one time
      redrawCompassBody = false;
  }
  
  compassUpdate(random(0.0,360.0),80,64,40);//update the compass needle by using degrees
  delay(100);
}


//it draws the compass body
void compassBody(uint8_t x,uint8_t y,uint8_t r){
  double xtick_F;
  double ytick_F;
  double xtick_T;
  double ytick_T;
  double x_t;
  double calcRad;
  uint8_t i;
  tft.fillCircle(x, y, r+7, comp_body_col);
  tft.drawCircle(x, y, r+6, comp_bcircles_col);
  tft.drawCircle(x, y, r+3, comp_bcircles_col);
  tft.fillCircle(x, y, r+1, comp_body_col);
  // Current N, S, E, W tick marks.  Will update to triangles later
  tft.drawLine(x, y-r-10, x, y-r-1, comp_tri_col);
  tft.fillCircle(x, y-r-5, 2, comp_tri_col);
  tft.fillTriangle(x-3, y+r+10, x+3, y+r+10, x, y+r+1, comp_tri_col);
  tft.fillTriangle(x-r-10, y-3, x-r-10, y+3, x-r-1, y, comp_tri_col);
  tft.fillTriangle(x+r+10, y-3, x+r+10, y+3, x+r+1, y, comp_tri_col);
  // the center dot in the middle of the dial
  tft.fillCircle(x, y, 2, comp_needle_col);
  // Text around the dial attempts to adjust to dial size
  tft.setCursor(x-2, y-r-17);
  tft.setTextColor(comp_text_col);
  tft.print("N");
  tft.setCursor(x-2, y+r+12);
  tft.setTextColor(comp_text_col);
  tft.print("S");
  tft.setCursor(x-r -17, y-3);
  tft.setTextColor(comp_text_col);
  tft.print("W");
  tft.setCursor(x+r+12, y-3);
  tft.setTextColor(comp_text_col);
  tft.print("E");
  tft.setCursor((x+((r+36)/2)), (y-((r+36)/2)));
  tft.setTextColor(comp_text_col);
  tft.print("NE");
  tft.setCursor((x+((r+30)/2)), (y+((r+30)/2)));
  tft.setTextColor(comp_text_col);
  tft.print("SE");
  tft.setCursor((x-((r+55)/2)), (y+((r+30)/2)));
  tft.setTextColor(comp_text_col);
  tft.print("SW");
  tft.setCursor((x-((r+55)/2)), (y-((r+35)/2)));
  tft.setTextColor(comp_text_col);
  tft.print("NW");
  //draw ticks
  for (i = 0; i < 12; i++){
    xtick_F = 0;
    ytick_F = 0;
    xtick_T = 0;
    ytick_T = 0;
    x_t = 0;
    calcRad = tickMarks[i] * myPI / 180;
    calcSin = sin(calcRad);
    if (tickMarks[i] > -1 && tickMarks[i] < 91)
    {
      xtick_F = sin(calcRad)*(r+1);
      x_t = -1 * xtick_F;
      ytick_F = -1 * sqrt((r+1)*(r+1)-(x_t * x_t));
      xtick_T = sin(calcRad)*(r+7);
      x_t = -1 * xtick_T;
      ytick_T = -1 * sqrt((r+7)*(r+7)-(x_t * x_t));
    }
    if (tickMarks[i] > 90 && tickMarks[i] < 181)
    {
      xtick_F = sin(calcRad)*(r+1);
      x_t = xtick_F;
      ytick_F = sqrt((r+1)*(r+1)-(x_t * x_t));
      xtick_T = sin(calcRad)*(r+7);
      x_t = xtick_T;
      ytick_T = sqrt((r+7)*(r+7)-(x_t * x_t));
    }
    if (tickMarks[i] > 180 && tickMarks[i] < 271)
    {
      xtick_F = sin(calcRad)*(r+1);
      x_t = xtick_F;
      ytick_F = sqrt((r+1)*(r+1)-(x_t * x_t));
      xtick_T = sin(calcRad)*(r+7);
      x_t = xtick_T;
      ytick_T = sqrt((r+7)*(r+7)-(x_t * x_t));
    }
    if (tickMarks[i] > 270 && tickMarks[i] < 361)
    {
      xtick_F = sin(calcRad)*(r+1);
      x_t = -1 * xtick_F;
      ytick_F = -1 * sqrt((r+1)*(r+1)-(x_t * x_t));
      xtick_T = sin(calcRad)*(r+7);
      x_t = -1 * xtick_T;
      ytick_T = -1 * sqrt((r+7)*(r+7)-(x_t * x_t));
    }
    tft.drawLine(x + xtick_T, y + ytick_T, x + xtick_F, y + ytick_F, comp_ticks_col);
  }
  /*
  // draws optional containers for other compass data
  // AX
   tft.setCursor(2, 2);
   tft.setTextColor(BLUE);
   tft.print("AX:");
   tft.fillRect(2, 12, 50, 8, BEIGE);
   // AY
   tft.setCursor(2, 22);
   tft.setTextColor(BLUE);
   tft.print("AY:");
   tft.fillRect(2, 32, 50, 8, BEIGE);
   //AZ
   tft.setCursor(2, 42);
   tft.setTextColor(BLUE);
   tft.print("AZ:");
   tft.fillRect(2, 52, 50, 8, BEIGE);
   */
}

void compassUpdate(double _degree,uint8_t x, uint8_t y,uint8_t r){
  tft.drawLine(x + x_to, y + y_to, x, y, comp_body_col);// Erase the old needle
  x_to = 0;
  y_to = 0;
  // I use our old friend from geometry, the pythagorean theorem, to draw the needle
  double x_t = 0;
  double calcRad = _degree * myPI / 180;
  // I set this up to use in place of multiple sin(calcRad) calls, but I left the sin(calcRad) in for now
  calcSin = sin(calcRad);
  // Each quadrant around the circle has + or - values in relation to the center point (x, y).  The sin() function returns +/- values for x_to, but y_to has to be manipulated.
  if (_degree > -1 && _degree < 91)
  {
    x_to = sin(calcRad)*r;
    x_t = -1 * x_to;
    y_to = -1 * sqrt((r*r)-(x_t * x_t));
  }
  if (_degree > 90 && _degree < 181)
  {
    x_to = sin(calcRad)*r;
    y_to = (sqrt((r*r)-(x_to * x_to)));
  }
  if (_degree > 180 && _degree < 271)
  {
    x_to = sin(calcRad)*r;
    y_to = (sqrt((r*r)-(x_to * x_to)));
  }
  if (_degree > 270 && _degree < 361)
  {
    x_to = sin(calcRad)*r;
    x_t = -1 * x_to;
    y_to = -1*(sqrt((r*r)-(x_t * x_t)));
  }

  // update the screen data
  tft.drawLine(x + x_to, y + y_to, x, y, comp_needle_col);
  tft.fillCircle(x, y, 2, comp_needle_col);
  /*
  //optional data for other compass infos
  // AX
   tft.setCursor(2, 12);
   //tft.print(compass.a.x);
   //AY
   tft.setCursor(2, 32);
   //tft.print(compass.a.y);
   //AZ
   tft.setCursor(2, 52);
   //tft.print(compass.a.z);
   
   //DEGREES - shows compass deg data
   tft.setCursor(2, 87);
   tft.setTextColor(RED);
   tft.print("DEGREE:");
   tft.setTextSize(3);
   tft.setTextColor(BLUE,BEIGE);
   tft.setCursor(4, 100);
   tft.print(_degree,0);
   tft.setTextSize(1);
   tft.setTextColor(WHITE);
   */
}


