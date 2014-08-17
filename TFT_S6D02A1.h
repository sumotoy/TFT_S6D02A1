/***********************************************************************
A fast driver for Teesy3.x/Due and Arduinos for drive 
Samsung S6D02A1 TFT displays as those:
http://www.ebay.com/itm/370987651121
http://www.ebay.com/itm/141341180758

Version 1.0b3

those displays are cheaper than ST7735 but not compatible
so they need a different initialization and have a pin called BL
that permit display blanking (not present on ST7735.
They are equipped with SD card as well but circuit it's separate
so you will need a library for that (like SDFat), good thing you
can share MOSI and SCK pin so you only will need a CS pin for that.
Those displays can operate at 5V or 3V3 by an onboard jumper, 
if you want to use at 5V with a 5V Micro (like UNO) you will need
a 1K resistor between every line from micro to TFT!
Library was written originally by Limor Fried of Adafruit for their
ST7735 display product and adapted to high speed Teensy3.x by Paul Stoffregen
and others for DUE. I just modified as well for S6D02A1 initializations and
support BL pin.

You will need the standard Adafruit_GFX library as well.


CONNECTION (only TFT, not SD card):
1-RST ---> Reset Pin
2-CE  ---> SPI SS pin (check witch micro pin you can use!)
3-D/C ---> Data/Command (check witch micro pin you can use!)
4-DIN ---> Mosi
5-CLK ---> SCK
6-UCC ---> 5V or 3V3 (check TFT jumper)
7-BL  ---> Blanking (if you will not use, connect to VCC!!!!)
8-GND ---> Ground (of course)

Note for Teensy3.x
CS & DC can use pins 2, 6, 9, 10, 15, 20, 21, 22, 23
but certain pairs must NOT be used: 2+10, 6+9, 20+23, 21+22

Following, I include the original text header for the Adafruit ST7735 library
as requested by Adafruit for redistributions.
 **********************************************************/
 /***************************************************
  This is a library for the Adafruit 1.8" SPI display.
  This library works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
  as well as Adafruit raw 1.8" TFT display
  ----> http://www.adafruit.com/products/618
 
  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#ifndef TFT_S6D02A1_h
#define TFT_S6D02A1_h

#if ARDUINO >= 100
 #include "Arduino.h"
 #include "Print.h"
#else
 #include "WProgram.h"
#endif

#include <Adafruit_GFX.h>
#include <colorScales.c> // 

#if defined(__SAM3X8E__)
#include <include/pio.h>
  #define PROGMEM
  #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
  #define pgm_read_word(addr) (*(const unsigned short *)(addr))
typedef unsigned char prog_uchar;
#endif
#ifdef __AVR__
  #include <avr/pgmspace.h>
#endif

#define S6D02A1_TFTWIDTH  128
#define S6D02A1_TFTHEIGHT 160

// Some used command definitions kept from original
#define S6D02A1_INVOFF  0x20
#define S6D02A1_INVON   0x21
#define S6D02A1_DISPOFF 0x28
#define S6D02A1_DISPON  0x29
#define S6D02A1_CASET   0x2A
#define S6D02A1_RASET   0x2B
#define S6D02A1_RAMWR   0x2C
#define S6D02A1_RAMRD   0x2E

#define S6D02A1_PTLAR   0x30
#define S6D02A1_COLMOD  0x3A
#define S6D02A1_MADCTL  0x36

// Basic colour definitions
#define	S6D02A1_BLACK   0x0000
#define	S6D02A1_RED     0xF800
#define	S6D02A1_GREEN   0x07E0
#define	S6D02A1_BLUE    0x001F
#define S6D02A1_YELLOW  0xFFE0
#define S6D02A1_MAGENTA 0xF81F
#define S6D02A1_CYAN    0x07FF  
#define S6D02A1_WHITE   0xFFFF
#define S6D02A1_GREY    0x632C


class TFT_S6D02A1 : public Adafruit_GFX {

 public:

  TFT_S6D02A1(uint8_t CS, uint8_t RS, uint8_t SID, uint8_t SCLK, uint8_t RST, uint8_t BL);
  TFT_S6D02A1(uint8_t CS, uint8_t RS, uint8_t SID, uint8_t SCLK, uint8_t RST);
  TFT_S6D02A1(uint8_t CS, uint8_t RS, uint8_t RST, uint8_t BL);
  TFT_S6D02A1(uint8_t CS, uint8_t RS, uint8_t RST);
  TFT_S6D02A1(uint8_t CS, uint8_t RS);

  void     init(),
           setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1),
           pushColor(uint16_t color),
           fillScreen(uint16_t color),
           drawPixel(int16_t x, int16_t y, uint16_t color),
           drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color),
           drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color),
           fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color),
           setRotation(uint8_t r),
           invertDisplay(boolean i),
		   backlight(uint8_t val=1),
		   fillMulticolorRect(int16_t x, int16_t y, int16_t w, int16_t h,uint8_t max_,uint8_t scaleType),
		   barVert(int16_t x, int16_t y, int16_t w, int16_t l, uint16_t color),
		   barHor(int16_t x, int16_t y, int16_t w, int16_t l, uint16_t color);
   uint16_t decodeCScale(uint8_t val,uint8_t divVal,uint8_t scale);

  // Pass 8-bit (each) R,G,B, get back 16-bit packed color
  inline uint16_t Color565(uint8_t r, uint8_t g, uint8_t b) {
           return ((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r >> 3);
  }
  inline uint16_t Color24To565(int32_t color_) { return Color565((color_ & 0xFF),((color_ >> 8) & 0xFF),((color_ >> 16) & 0xFF));}
  void setBitrate(uint32_t n);

 private:
  uint8_t  tabcolor;

  void     writebegin(),
           spiwrite(uint8_t),
           writecommand(uint8_t c),
           writedata(uint8_t d),
           writedata16(uint16_t d),
           commandList(const uint8_t *addr),
           commonInit(const uint8_t *cmdList);
           
  boolean  hwSPI;

#if defined(__AVR__)
volatile uint8_t *dataport, *clkport, *csport, *rsport, *blport;
  uint8_t  _bl, _cs, _rs, _rst, _sid, _sclk,
           datapinmask, clkpinmask, cspinmask, rspinmask, blnmask,
           colstart, rowstart; // some displays need this changed
#endif //  #ifdef __AVR__

#if defined(__SAM3X8E__)
  Pio *dataport, *clkport, *csport, *rsport, *blport;
  uint32_t  _bl, _cs, _rs, _rst, _sid, _sclk,
            datapinmask, clkpinmask, cspinmask, rspinmask, blnmask,
            colstart, rowstart; // some displays need this changed
#endif //  #if defined(__SAM3X8E__)

#if defined(__MK20DX128__) || defined(__MK20DX256__)
  uint8_t  _bl, _cs, _rs, _rst, _sid, _sclk;
  uint8_t colstart, rowstart;
  uint8_t pcs_data, pcs_command;
  uint32_t ctar;
  volatile uint8_t *datapin, *clkpin, *cspin, *rspin, *blpin;
#endif

};
#endif
