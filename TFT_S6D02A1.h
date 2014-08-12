

#ifndef TFT_S6D02A1_h
#define TFT_S6D02A1_h

#if ARDUINO >= 100
 #include "Arduino.h"
 #include "Print.h"
#else
 #include "WProgram.h"
#endif

#include <Adafruit_GFX.h>

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
		   backlight(uint8_t val=1);

  // Pass 8-bit (each) R,G,B, get back 16-bit packed color
  inline uint16_t Color565(uint8_t r, uint8_t g, uint8_t b) {
           return ((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r >> 3);
  }
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
