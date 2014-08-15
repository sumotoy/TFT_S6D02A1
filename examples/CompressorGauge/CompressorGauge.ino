#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TFT_S6D02A1.h>

#define TFT_CS_PIN        20
#define TFT_DC_PIN        2
#define TFT_RST_PIN       14

#define  CMP_THRE                   -10      // Compression Threshold  
#define  CMP_RATE                     4      // Compression Ratio 4 : 1
#define  ATT_COEF    (CMP_RATE - 1.0) / CMP_RATE
const        float       rms_Calib = 51.35;  // Calibration Coeficient to adjust "0 dB" to 1.228 V RMS
float       ppm_Arrow =   0.0;
float       rms_Level =   0.0;
float       set_Atten =  10.0;  // Current Attenuation Settings
float       dyn_Range =  40.0;
uint8_t     mod_autom =     0;
int16_t     adc_Offst =   512;
volatile     int32_t     ppm_Level =     0;


TFT_S6D02A1 tft = TFT_S6D02A1(TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);


void setup(void) {
  Serial.begin(38400);
  tft.init();   // initialize a ST7735S chip, black tab
  tft.setRotation(1);
  tft.fillScreen(S6D02A1_BLACK);
  tft.setTextWrap(true);
  tft.setTextColor(S6D02A1_WHITE, S6D02A1_BLACK);
  //tft.setTextColor(S6D02A1_WHITE);
  tft.setCursor(0,0);
  Draw_Table();
}


void loop(){
  dataCapture();
  RMS_calculate();
  if((rms_Level > CMP_THRE) && mod_autom ) {
    set_Atten =  (rms_Level - CMP_THRE) * ATT_COEF;
    //adjust_Att(set_Atten);
  }
  Draw_Arrow();
  delay(100);
}
void   Draw_Table()
{
  tft.setCursor( 3,  8);
  tft.print("-20 10 5 3 1 0 1 2 3");
  tft.drawRoundRect( 0, 0, 126, 63, 5,S6D02A1_WHITE);  

  int16_t temp, mark;

  temp = (pow( 10, (( rms_Calib + CMP_THRE ) / 20.0))) - 1;

  mark = map( temp, 0, 545, 5, 122);            // 1.228 V.
  tft.drawFastVLine(    mark, 3, 5, S6D02A1_MAGENTA ) ;
  tft.drawFastVLine( mark -1, 3, 5, S6D02A1_MAGENTA ) ;

  Draw_Arrow();

  tft.drawRect( 5, 24, 25, 12, S6D02A1_WHITE); 	// RATIO :1
  tft.drawRect(35, 24, 25, 12, S6D02A1_WHITE); 	// Manual:Auto
  tft.drawRect(66, 24, 55, 12, S6D02A1_WHITE); 	// BarGraph
  tft.drawRect( 5, 40, 55, 19, S6D02A1_WHITE); 	// RMS
  tft.drawRect(66, 40, 55, 19, S6D02A1_WHITE); 	// Atten.

  tft.setCursor( 9, 27);
  tft.print("4:1");
  tft.setCursor( 39, 27);
  tft.print("Man");

  tft.setCursor(22, 42);
  tft.print("RMS:");
  tft.setCursor(78, 42);
  tft.print("Atten:");
}

void   Draw_Arrow()
{
  static int st1 = 5;
  static int st2 = 5;

  static float   lpf_arrow = 0.0;
  static float   lpf_level = 0.0;
  static float   lpf_atten = 0.0;


  lpf_arrow = (lpf_arrow * 0.9375) + (ppm_Arrow * 0.0625);  // HMI slowiness

  st2 = map( lpf_arrow, 0, 545, 5, 122);         // 1.228 V RMS at "0" dB

  if ( st2 > 122 ) st2 = 122;
  if ( st2 <   5 ) st2 =   5;

  tft.drawFastVLine(    st1, 15, 5, S6D02A1_BLACK ) ;
  tft.drawFastVLine( st1 -1, 15, 5, S6D02A1_BLACK ) ;
  tft.drawFastVLine(    st2, 15, 5, S6D02A1_CYAN ) ;
  tft.drawFastVLine( st2 -1, 15, 5, S6D02A1_CYAN ) ;

  st1 = st2;

  int16_t pi1, pi2, pi3;

  lpf_level = (lpf_level * 0.9375) + (rms_Level * 0.0625);
  tft.setCursor(17, 51);
  tft.print(lpf_level, 2);
  lpf_atten = (lpf_atten * 0.9375) + (set_Atten * 0.0625); 
  tft.setCursor(78, 51);
  tft.print(lpf_atten, 2);  

  pi1 = lpf_atten;
  pi2 = dyn_Range;

  pi1 = constrain( pi1, 0, pi2);
  pi3 = map( pi1, 0, pi2, 0, 53);        // 53 : 20 

  tft.fillRect( 67, 25, pi3, 10, S6D02A1_GREEN);      // Fill  
  tft.fillRect( 67 + pi3, 25, 53 - pi3, 10, S6D02A1_BLACK); // Empty  
}


void   RMS_calculate()
{
  float temp;

  temp = ppm_Level;                   // Take a copy, so Real Value not affected by calculation.
  temp = sqrt(temp);
  ppm_Arrow = temp;
  rms_Level = 20.0 * log10(temp +1);  // Calculated, available over Serial
  rms_Level -= rms_Calib;
}

/*
void adjust_Att( float att_log )
 {
 union split_float {
 uint16_t tegri[2];
 float    loatf;
 } sf;   
 
 int16_t  vremn;
 float coeff[4];
 
 for( uint8_t i = 0; i < 4; i++)
 {
 sf.tegri[0]  = recall_entry(      2 * i );   // HIGH WORD;
 sf.tegri[1]  = recall_entry( 1 + (2 * i));   // LOW WORD;
 coeff[i] = sf.loatf;
 }
 
 att_log = dyn_Maxim - att_log;
 
 vremn = ((coeff[3] * pow( att_log, 3)) + (coeff[2] * pow( att_log, 2)) +(coeff[1] * att_log) + coeff[0]);
 if (vremn <   0) vremn =   0;
 if (vremn > 200) vremn = 200;
 
 //OCR2B = vremn; 
 }
 */

void dataCapture()
{ 
  int32_t temp = random(0,1024) - adc_Offst; 

  temp = temp * temp;

  if ( temp > ppm_Level ) ppm_Level = ((ppm_Level * 15) +  temp) >> 4;
  else ppm_Level = (ppm_Level * 4095) >> 12;   
}
