#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TFT_S6D02A1.h>

#define TFT_CS_PIN        20
#define TFT_DC_PIN        2
#define TFT_RST_PIN       14


// Color definitions
#define   BLACK           0x0000
#define   BLUE            0x001F
#define   RED             0xF800
#define   GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0 
#define WHITE           0xFFFF
// added more colors to original example
#define DKBLUE          0x0018
#define DKGREEN         0x0600
#define DKRED           0x9802
#define ORANGE          0xF400
#define CREAM           0xFF96
#define BEIGE           0xFF50



TFT_S6D02A1 tft = TFT_S6D02A1(TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);


int sensorValueNew = 0;
int sensorValue7 = 0;      // Backlight Control
int sensor = 0;
int xFill = 0;

// Setting the DialRadius determines the size of the dial.
// int x, y sets the position of the center of the dial.
// All parameters associated with the dial will attempt to
// locate around the dial.  Making the DialRadius too large
// will push parts of text and the dial off the screen.
// Design differnt dials for compass, speed, wind direction, etc.
int DialRadius = 20;
int x = 116;
int y = 87;

// these are used to draw lines and the dial needle
double y_to = 0;
double x_to = 0;
int degree = 0;
double calcSin = 0;
double myPI = 3.14;

// variables to help draw the bars for the accell data
int aXmin = -2048;
int aXmax = 2047;
int aYmin = -2048;
int aYmax = 2047;
int aZmin = -2048;
int aZmax = 2047;

// Array used to draw the tick marks around the dial
double tickMarks[12] = {22.5, 45, 67.5, 112.5, 135, 157.5, 202.5, 225, 247.5, 292.5, 315, 337.5};

String sensorString = "0000";


void setup() {
  Serial.begin(38400);
  tft.init();   // initialize a ST7735S chip, black tab
  tft.setRotation(1);
  tft.fillScreen(S6D02A1_BLACK);
  tft.setTextWrap(true);
  tft.setTextColor(S6D02A1_WHITE);
  tft.setCursor(0,0);
  mainScreen();
}

void loop() {
  getCompass();
  drawNeedle(degree);
  updateAcc();
  updateBacklight();
  delay(100);
}

void mainScreen (void) {

  tft.setTextSize(1);
  tft.fillScreen(BEIGE);
  tft.fillCircle(x, y, DialRadius+7, CREAM);
  tft.drawCircle(x, y, DialRadius+6, BLACK);
  tft.drawCircle(x, y, DialRadius+3, BLACK);
  tft.fillCircle(x, y, DialRadius+1, CREAM);
  // Current N, S, E, W tick marks.  Will update to triangles later
  tft.drawLine(x, y-DialRadius-10, x, y-DialRadius-1, RED);
  tft.fillCircle(x, y-DialRadius-5, 2, RED);
  tft.fillTriangle(x-3, y+DialRadius+10, x+3, y+DialRadius+10, x, y+DialRadius+1, RED);
  tft.fillTriangle(x-DialRadius-10, y-3, x-DialRadius-10, y+3, x-DialRadius-1, y, RED);
  tft.fillTriangle(x+DialRadius+10, y-3, x+DialRadius+10, y+3, x+DialRadius+1, y, RED);
  // the center dot in the middle of the dial
  tft.fillCircle(x, y, 2, BLUE);
  // Text around the dial attempts to adjust to dial size
  tft.setCursor(x-2, y-DialRadius-17);
  tft.setTextColor(RED);
  tft.print("N");
  tft.setCursor(x-2, y+DialRadius+12);
  tft.setTextColor(RED);
  tft.print("S");
  tft.setCursor(x-DialRadius -17, y-3);
  tft.setTextColor(RED);
  tft.print("W");
  tft.setCursor(x+DialRadius+12, y-3);
  tft.setTextColor(RED);
  tft.print("E");
  tft.setCursor((x+((DialRadius+36)/2)), (y-((DialRadius+36)/2)));
  tft.setTextColor(RED);
  tft.print("NE");
  tft.setCursor((x+((DialRadius+30)/2)), (y+((DialRadius+30)/2)));
  tft.setTextColor(RED);
  tft.print("SE");
  tft.setCursor((x-((DialRadius+55)/2)), (y+((DialRadius+30)/2)));
  tft.setTextColor(RED);
  tft.print("SW");
  tft.setCursor((x-((DialRadius+55)/2)), (y-((DialRadius+35)/2)));
  tft.setTextColor(RED);
  tft.print("NW");
  // uses the tick marks array to draw the tick marks
  dispTicks();
  tft.drawRect(57, 1, 102, 10, BLACK);
  tft.drawRect(57, 12, 102, 10, BLACK);
  tft.drawRect(57, 23, 102, 10, BLACK);
  tft.drawFastHLine(58, 36, 102, BLACK);
  tft.drawFastVLine(58, 34, 5, BLACK);  // -2.0
  tft.drawFastVLine(71, 34, 5, BLACK);  // -1.5
  tft.drawFastVLine(84, 34, 5, BLACK);  // -1.0
  tft.drawFastVLine(96, 34, 5, BLACK);  // -0.5
  tft.drawFastVLine(109, 34, 5, BLACK); //  0.0
  tft.drawFastVLine(122, 34, 5, BLACK); //  0.5
  tft.drawFastVLine(134, 34, 5, BLACK); //  1.0
  tft.drawFastVLine(145, 34, 5, BLACK); //  1.5
  tft.drawFastVLine(158, 34, 5, BLACK); //  2.0
  tft.setTextColor(BLACK);
  tft.setCursor(50,39);
  tft.print("-2");
  tft.setCursor(76,39);
  tft.print("-1");
  tft.setCursor(107,39);
  tft.print("0");
  tft.setCursor(132,39);
  tft.print("1");
  tft.setCursor(155,39);
  tft.print("2");
  compassSetup();
}

void updateBacklight (void) {

}

void compassSetup(void){

}

void getCompass(void){

    degree = random(0.0,360.0);
}


void drawNeedle(double _degree){
  //  draw or erase needle
 
  // Erase the old needle
  tft.drawLine(x + x_to, y + y_to, x, y, CREAM);
 
  x_to = 0;
  y_to = 0;
 
  // I use our old friend from geometry, the pythagorean theorem, to draw the needle
  double x_t = 0;
  double calcRad = _degree * myPI / 180;
  // I set this up to use in place of multiple sin(calcRad) calls, but I
  // left the sin(calcRad) in for now
  calcSin = sin(calcRad);
 
  // Each quadrant around the circle has + or - values in relation to the center
  // point (x, y).  The sin() function returns +/- values for x_to, but y_to has
  // to be manipulated.
  if (degree > -1 && degree < 91)
  {
    x_to = sin(calcRad)*DialRadius;
    // The x_t value was inserted for trouble shooting.  May be removed later.
    // Shouldn't matter because (neg * neg) = positive number.
    x_t = -1 * x_to;
    y_to = -1 * sqrt((DialRadius*DialRadius)-(x_t * x_t));
  }
  if (degree > 90 && degree < 181)
  {
    x_to = sin(calcRad)*DialRadius;
    y_to = (sqrt((DialRadius*DialRadius)-(x_to * x_to)));
  }
  if (degree > 180 && degree < 271)
  {
    x_to = sin(calcRad)*DialRadius;
    y_to = (sqrt((DialRadius*DialRadius)-(x_to * x_to)));
  }
  if (degree > 270 && degree < 361)
  {
    x_to = sin(calcRad)*DialRadius;
    x_t = -1 * x_to;
    y_to = -1*(sqrt((DialRadius*DialRadius)-(x_t * x_t)));
  }
 
  // update the screen data
  tft.drawLine(x + x_to, y + y_to, x, y, BLUE);
  tft.fillCircle(x, y, 2, BLUE);
  tft.setCursor(2, 2);
  tft.setTextColor(BLUE);
  tft.print("AX:");
  tft.fillRect(2, 12, 50, 8, BEIGE);
  tft.setCursor(2, 12);
  //tft.print(compass.a.x);
 
  tft.setCursor(2, 22);
  tft.setTextColor(BLUE);
  tft.print("AY:");
  tft.fillRect(2, 32, 50, 8, BEIGE);
  tft.setCursor(2, 32);
  //tft.print(compass.a.y);
 
  tft.setCursor(2, 42);
  tft.setTextColor(BLUE);
  tft.print("AZ:");
  tft.fillRect(2, 52, 50, 8, BEIGE);
  tft.setCursor(2, 52);
  //tft.print(compass.a.z);
 
  tft.setCursor(2, 87);
  tft.setTextColor(RED);
  tft.print("DEGREE:");
  tft.fillRect(2, 98, 60, 25, BEIGE);
  tft.setTextSize(3);
  tft.setTextColor(BLUE);
  tft.setCursor(4, 100);
  tft.print(degree);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
}

void dispTicks(void){
  // uses the tickMark[] array   
  for (int i = 0; i < 12; i++){
   
    double xtick_F = 0;
    double ytick_F = 0;
    double xtick_T = 0;
    double ytick_T = 0;

    double x_t = 0;
    double calcRad = tickMarks[i] * myPI / 180;
    calcSin = sin(calcRad);

    if (tickMarks[i] > -1 && tickMarks[i] < 91)
    {
      xtick_F = sin(calcRad)*(DialRadius+1);
      x_t = -1 * xtick_F;
      ytick_F = -1 * sqrt((DialRadius+1)*(DialRadius+1)-(x_t * x_t));
      xtick_T = sin(calcRad)*(DialRadius+7);
      x_t = -1 * xtick_T;
      ytick_T = -1 * sqrt((DialRadius+7)*(DialRadius+7)-(x_t * x_t));
    }
    if (tickMarks[i] > 90 && tickMarks[i] < 181)
    {
      xtick_F = sin(calcRad)*(DialRadius+1);
      x_t = xtick_F;
      ytick_F = sqrt((DialRadius+1)*(DialRadius+1)-(x_t * x_t));
      xtick_T = sin(calcRad)*(DialRadius+7);
      x_t = xtick_T;
      ytick_T = sqrt((DialRadius+7)*(DialRadius+7)-(x_t * x_t));
    }
    if (tickMarks[i] > 180 && tickMarks[i] < 271)
    {
      xtick_F = sin(calcRad)*(DialRadius+1);
      x_t = xtick_F;
      ytick_F = sqrt((DialRadius+1)*(DialRadius+1)-(x_t * x_t));
      xtick_T = sin(calcRad)*(DialRadius+7);
      x_t = xtick_T;
      ytick_T = sqrt((DialRadius+7)*(DialRadius+7)-(x_t * x_t));
    }
    if (tickMarks[i] > 270 && tickMarks[i] < 361)
    {
      xtick_F = sin(calcRad)*(DialRadius+1);
      x_t = -1 * xtick_F;
      ytick_F = -1 * sqrt((DialRadius+1)*(DialRadius+1)-(x_t * x_t));
      xtick_T = sin(calcRad)*(DialRadius+7);
      x_t = -1 * xtick_T;
      ytick_T = -1 * sqrt((DialRadius+7)*(DialRadius+7)-(x_t * x_t));
    }
    tft.drawLine(x + xtick_T, y + ytick_T, x + xtick_F, y + ytick_F, RED);
  }
}

void updateAcc(void){
  // update Accellerometer rectangles
 
  int myAccX = map(random(0,100), aXmin, aXmax, 0, 100);
  int myAccY = map(random(0,100), aYmin, aYmax, 0, 100);
  int myAccZ = map(random(0,100), aZmin, aZmax, 0, 100);
 
  xFill = myAccX;
  tft.fillRect(58, 2, xFill, 8, DKGREEN);
  tft.fillRect(58+xFill+1, 2, 100-xFill-1, 8, WHITE);
 
  xFill = myAccY;
  tft.fillRect(58, 13, xFill, 8, DKBLUE);
  tft.fillRect(58+xFill+1, 13, 100-xFill-1, 8, WHITE);
 
  xFill = myAccZ;
  tft.fillRect(58, 24, xFill, 8, DKRED);
  tft.fillRect(58+xFill+1, 24, 100-xFill-1, 8, WHITE);
}