/***********************************************************************
A fast driver for Teesy3.x/Due and Arduinos for drive 
Samsung S6D02A1 TFT displays as those:
http://www.ebay.com/itm/370987651121
http://www.ebay.com/itm/141341180758

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

#include "TFT_S6D02A1.h"
#include <limits.h>
#include "pins_arduino.h"
#include "wiring_private.h"
#include <SPI.h>

inline uint16_t swapcolor(uint16_t x) { 
  return (x << 11) | (x & 0x07E0) | (x >> 11);
}


// Constructor when using software SPI.  All output pins are configurable.
TFT_S6D02A1::TFT_S6D02A1(uint8_t cs, uint8_t rs, uint8_t sid,
 uint8_t sclk, uint8_t rst, uint8_t bl) : Adafruit_GFX(S6D02A1_TFTWIDTH, S6D02A1_TFTHEIGHT)
{
  _cs   = cs;
  _rs   = rs;
  _sid  = sid;
  _sclk = sclk;
  _rst  = rst;
  _bl   = bl;
  hwSPI = false;
}

TFT_S6D02A1::TFT_S6D02A1(uint8_t cs, uint8_t rs, uint8_t sid,
 uint8_t sclk, uint8_t rst) : Adafruit_GFX(S6D02A1_TFTWIDTH, S6D02A1_TFTHEIGHT)
{
  _cs   = cs;
  _rs   = rs;
  _sid  = sid;
  _sclk = sclk;
  _rst  = rst;
  _bl   = 0;
  hwSPI = false;
}

// Constructor when using hardware SPI.  Faster, but must use SPI pins
// specific to each board type (e.g. 11,13 for Uno, 51,52 for Mega, etc.)
TFT_S6D02A1::TFT_S6D02A1(uint8_t cs, uint8_t rs, uint8_t rst) :
    Adafruit_GFX(S6D02A1_TFTWIDTH, S6D02A1_TFTHEIGHT) {
  _cs   = cs;
  _rs   = rs;
  _rst  = rst;
  hwSPI = true;
  _sid  = _sclk = _bl = 0;
}

TFT_S6D02A1::TFT_S6D02A1(uint8_t cs, uint8_t rs) :
    Adafruit_GFX(S6D02A1_TFTWIDTH, S6D02A1_TFTHEIGHT) {
  _cs   = cs;
  _rs   = rs;
  _rst  = 0;
  hwSPI = true;
  _sid  = _sclk = _bl = 0;
}
/***************************************************************/
/*     Arduino Uno, Leonardo, Mega, Teensy 2.0, etc            */
/***************************************************************/
#ifdef __AVR__
inline void TFT_S6D02A1::writebegin()
{
}

inline void TFT_S6D02A1::spiwrite(uint8_t c)
{
  //Serial.println(c, HEX);
  if (hwSPI) {
    SPDR = c;
    while(!(SPSR & _BV(SPIF)));
  } else {
    // Fast SPI bitbang swiped from LPD8806 library
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      if(c & bit) *dataport |=  datapinmask;
      else        *dataport &= ~datapinmask;
      *clkport |=  clkpinmask;
      *clkport &= ~clkpinmask;
    }
  }
}

void TFT_S6D02A1::writecommand(uint8_t c)
{
  *rsport &= ~rspinmask;
  *csport &= ~cspinmask;//low
  //Serial.print("C ");
  spiwrite(c);
  *csport |= cspinmask;//high
}

void TFT_S6D02A1::writedata(uint8_t c)
{
  *rsport |=  rspinmask;
  *csport &= ~cspinmask;
  //Serial.print("D ");
  spiwrite(c);
  *csport |= cspinmask;
} 

void TFT_S6D02A1::writedata16(uint16_t d)
{
  *rsport |=  rspinmask;
  *csport &= ~cspinmask;
  //Serial.print("D ");
  spiwrite(d >> 8);
  spiwrite(d);
  *csport |= cspinmask;
} 

void TFT_S6D02A1::setBitrate(uint32_t n)
{
  if (n >= 8000000) {
    SPI.setClockDivider(SPI_CLOCK_DIV2);
  } else if (n >= 4000000) {
    SPI.setClockDivider(SPI_CLOCK_DIV4);
  } else if (n >= 2000000) {
    SPI.setClockDivider(SPI_CLOCK_DIV8);
  } else {
    SPI.setClockDivider(SPI_CLOCK_DIV16);
  }
}

/***************************************************************/
/*     Arduino Due                                             */
/***************************************************************/
#elif defined(__SAM3X8E__)
inline void TFT_S6D02A1::writebegin()
{
}

inline void TFT_S6D02A1::spiwrite(uint8_t c)
{
  //Serial.println(c, HEX);
  if (hwSPI) {
    SPI.transfer(c);
  } else {
    // Fast SPI bitbang swiped from LPD8806 library
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      if(c & bit) dataport->PIO_SODR |= datapinmask;
      else        dataport->PIO_CODR |= datapinmask;
      clkport->PIO_SODR |= clkpinmask;
      clkport->PIO_CODR |= clkpinmask;
    }
  }
}

void TFT_S6D02A1::writecommand(uint8_t c)
{
  rsport->PIO_CODR |=  rspinmask;
  csport->PIO_CODR  |=  cspinmask;
  //Serial.print("C ");
  spiwrite(c);
  csport->PIO_SODR  |=  cspinmask;
}

void TFT_S6D02A1::writedata(uint8_t c)
{
  rsport->PIO_SODR |=  rspinmask;
  csport->PIO_CODR  |=  cspinmask;
  //Serial.print("D ");
  spiwrite(c);
  csport->PIO_SODR  |=  cspinmask;
} 

void TFT_S6D02A1::writedata16(uint16_t d)
{
  rsport->PIO_SODR |=  rspinmask;
  csport->PIO_CODR  |=  cspinmask;
  //Serial.print("D ");
  spiwrite(d >> 8);
  spiwrite(d);
  csport->PIO_SODR  |=  cspinmask;
}

void TFT_S6D02A1::setBitrate(uint32_t n)
{
  uint32_t divider=1;
  while (divider < 255) {
    if (n >= 84000000 / divider) break;
    divider = divider - 1;
  }
  SPI.setClockDivider(divider);
}

/***************************************************************/
/*     Teensy 3.0 & 3.1                                        */
/***************************************************************/
#elif defined(__MK20DX128__) || defined(__MK20DX256__)

inline void TFT_S6D02A1::writebegin()
{
}

inline void TFT_S6D02A1::spiwrite(uint8_t c)
{
	for (uint8_t bit = 0x80; bit; bit >>= 1) {
		*datapin = ((c & bit) ? 1 : 0);
		*clkpin = 1;
		*clkpin = 0;
	}
}

void TFT_S6D02A1::writecommand(uint8_t c)
{
	if (hwSPI) {
		SPI0.PUSHR = c | (pcs_command << 16) | SPI_PUSHR_CTAS(0);
		while (((SPI0.SR) & (15 << 12)) > (3 << 12)) ; // wait if FIFO full
	} else {
		*rspin = 0;
		*cspin = 0;
		spiwrite(c);
		*cspin = 1;
	}
}

void TFT_S6D02A1::writedata(uint8_t c)
{
	if (hwSPI) {
		SPI0.PUSHR = c | (pcs_data << 16) | SPI_PUSHR_CTAS(0);
		while (((SPI0.SR) & (15 << 12)) > (3 << 12)) ; // wait if FIFO full
	} else {
		*rspin = 1;
		*cspin = 0;
		spiwrite(c);
		*cspin = 1;
	}
}

void TFT_S6D02A1::writedata16(uint16_t d)
{
	if (hwSPI) {
		SPI0.PUSHR = d | (pcs_data << 16) | SPI_PUSHR_CTAS(1);
		while (((SPI0.SR) & (15 << 12)) > (3 << 12)) ; // wait if FIFO full
	} else {
		*rspin = 1;
		*cspin = 0;
		spiwrite(d >> 8);
		spiwrite(d);
		*cspin = 1;
	}
}

static bool spi_pin_is_cs(uint8_t pin)
{
	if (pin == 2 || pin == 6 || pin == 9) return true;
	if (pin == 10 || pin == 15) return true;
	if (pin >= 20 && pin <= 23) return true;
	return false;
}

static uint8_t spi_configure_cs_pin(uint8_t pin)
{
        switch (pin) {
                case 10: CORE_PIN10_CONFIG = PORT_PCR_MUX(2); return 0x01; // PTC4
                case 2:  CORE_PIN2_CONFIG  = PORT_PCR_MUX(2); return 0x01; // PTD0
                case 9:  CORE_PIN9_CONFIG  = PORT_PCR_MUX(2); return 0x02; // PTC3
                case 6:  CORE_PIN6_CONFIG  = PORT_PCR_MUX(2); return 0x02; // PTD4
                case 20: CORE_PIN20_CONFIG = PORT_PCR_MUX(2); return 0x04; // PTD5
                case 23: CORE_PIN23_CONFIG = PORT_PCR_MUX(2); return 0x04; // PTC2
                case 21: CORE_PIN21_CONFIG = PORT_PCR_MUX(2); return 0x08; // PTD6
                case 22: CORE_PIN22_CONFIG = PORT_PCR_MUX(2); return 0x08; // PTC1
                case 15: CORE_PIN15_CONFIG = PORT_PCR_MUX(2); return 0x10; // PTC0
        }
        return 0;
}

#define CTAR_24MHz   (SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) | SPI_CTAR_CSSCK(0) | SPI_CTAR_DBR)
#define CTAR_16MHz   (SPI_CTAR_PBR(1) | SPI_CTAR_BR(0) | SPI_CTAR_CSSCK(0) | SPI_CTAR_DBR)
#define CTAR_12MHz   (SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) | SPI_CTAR_CSSCK(0))
#define CTAR_8MHz    (SPI_CTAR_PBR(1) | SPI_CTAR_BR(0) | SPI_CTAR_CSSCK(0))
#define CTAR_6MHz    (SPI_CTAR_PBR(0) | SPI_CTAR_BR(1) | SPI_CTAR_CSSCK(1))
#define CTAR_4MHz    (SPI_CTAR_PBR(1) | SPI_CTAR_BR(1) | SPI_CTAR_CSSCK(1))

void TFT_S6D02A1::setBitrate(uint32_t n)
{
	if (n >= 24000000) {
		ctar = CTAR_24MHz;
	} else if (n >= 16000000) {
		ctar = CTAR_16MHz;
	} else if (n >= 12000000) {
		ctar = CTAR_12MHz;
	} else if (n >= 8000000) {
		ctar = CTAR_8MHz;
	} else if (n >= 6000000) {
		ctar = CTAR_6MHz;
	} else {
		ctar = CTAR_4MHz;
	}
	SIM_SCGC6 |= SIM_SCGC6_SPI0;
	SPI0.MCR = SPI_MCR_MDIS | SPI_MCR_HALT;
	SPI0.CTAR0 = ctar | SPI_CTAR_FMSZ(7);
	SPI0.CTAR1 = ctar | SPI_CTAR_FMSZ(15);
	SPI0.MCR = SPI_MCR_MSTR | SPI_MCR_PCSIS(0x1F) | SPI_MCR_CLR_TXF | SPI_MCR_CLR_RXF;
}

#endif //#if defined(__SAM3X8E__)


// Rather than a bazillion writecommand() and writedata() calls, screen
// initialization commands and arguments are organized in these tables
// stored in PROGMEM.  The table may look bulky, but that's mostly the
// formatting -- storage-wise this is hundreds of bytes more compact
// than the equivalent code.  Companion function follows.
#define DELAY 0x80


static const uint8_t PROGMEM	// Multiple LCD init commands removed
  S6D02A1[] = {                  // S6D02A1 support only now
	29,
	0xf0,	2,	0x5a, 0x5a,				// Excommand2
	0xfc,	2,	0x5a, 0x5a,				// Excommand3
	0x26,	1,	0x01,					// Gamma set
	0xfa,	15,	0x02, 0x1f,	0x00, 0x10,	0x22, 0x30, 0x38, 0x3A, 0x3A, 0x3A,	0x3A, 0x3A,	0x3d, 0x02, 0x01,	// Positive gamma control
	0xfb,	15,	0x21, 0x00,	0x02, 0x04,	0x07, 0x0a, 0x0b, 0x0c, 0x0c, 0x16,	0x1e, 0x30,	0x3f, 0x01, 0x02,	// Negative gamma control
	0xfd,	11,	0x00, 0x00, 0x00, 0x17, 0x10, 0x00, 0x01, 0x01, 0x00, 0x1f, 0x1f,							// Analog parameter control
	0xf4,	15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x3f, 0x07, 0x00, 0x3C, 0x36, 0x00, 0x3C, 0x36, 0x00,	// Power control
	0xf5,	13, 0x00, 0x70, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6d, 0x66, 0x06,				// VCOM control
	0xf6,	11, 0x02, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x02, 0x00, 0x06, 0x01, 0x00,							// Source control
	0xf2,	17, 0x00, 0x01, 0x03, 0x08, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x08, 0x08,	//Display control
	0xf8,	1,	0x11,					// Gate control
	0xf7,	4, 0xc8, 0x20, 0x00, 0x00,	// Interface control
	0xf3,	2, 0x00, 0x00,				// Power sequence control
	0x11,	DELAY, 50,					// Wake
	0xf3,	2+DELAY, 0x00, 0x01, 50,	// Power sequence control
	0xf3,	2+DELAY, 0x00, 0x03, 50,	// Power sequence control
	0xf3,	2+DELAY, 0x00, 0x07, 50,	// Power sequence control
	0xf3,	2+DELAY, 0x00, 0x0f, 50,	// Power sequence control
	0xf4,	15+DELAY, 0x00, 0x04, 0x00, 0x00, 0x00, 0x3f, 0x3f, 0x07, 0x00, 0x3C, 0x36, 0x00, 0x3C, 0x36, 0x00, 50,	// Power control
	0xf3,	2+DELAY, 0x00, 0x1f, 50,	// Power sequence control
	0xf3,	2+DELAY, 0x00, 0x7f, 50,	// Power sequence control
	0xf3,	2+DELAY, 0x00, 0xff, 50,	// Power sequence control
	0xfd,	11, 0x00, 0x00, 0x00, 0x17, 0x10, 0x00, 0x00, 0x01, 0x00, 0x16, 0x16,							// Analog parameter control
	0xf4,	15, 0x00, 0x09, 0x00, 0x00, 0x00, 0x3f, 0x3f, 0x07, 0x00, 0x3C, 0x36, 0x00, 0x3C, 0x36, 0x00,	// Power control
	0x36,	1, 0x08,					// Memory access data control
	0x35,	1, 0x00,					// Tearing effect line on
	0x3a,	1+DELAY, 0x05, 150,			// Interface pixel control
	0x29,	0,							// Display on
	0x2c,	0							// Memory write
  };


// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void TFT_S6D02A1::commandList(const uint8_t *addr) {

  uint8_t  numCommands, numArgs;
  uint16_t ms;

  numCommands = pgm_read_byte(addr++);   // Number of commands to follow
  
  while(numCommands--) {                 // For each command...
	writecommand(pgm_read_byte(addr++)); //   Read, issue command
    numArgs  = pgm_read_byte(addr++);    //   Number of args to follow
    ms       = numArgs & DELAY;          //   If hibit set, delay follows args
    numArgs &= ~DELAY;                   //   Mask out delay bit
    while(numArgs--) {                   //   For each argument...
      writedata(pgm_read_byte(addr++));  //     Read, issue argument
    }
    if(ms) {
      ms = pgm_read_byte(addr++); // Read post-command delay time (ms)
      if(ms == 255) ms = 500;     // If 255, delay for 500 ms
      delay(ms);
    }
  }
}


// Initialization code for  S6D02A1 displays
void TFT_S6D02A1::commonInit(const uint8_t *cmdList) {
  colstart  = rowstart = 0; // May be overridden in init func
  
#ifdef __AVR__
  pinMode(_rs, OUTPUT);
  pinMode(_cs, OUTPUT);
  csport    = portOutputRegister(digitalPinToPort(_cs));
  rsport    = portOutputRegister(digitalPinToPort(_rs));
  cspinmask = digitalPinToBitMask(_cs);
  rspinmask = digitalPinToBitMask(_rs);
  if (_bl > 0){
	pinMode(_bl, OUTPUT);
	blport    = portOutputRegister(digitalPinToPort(_bl));
	blpinmask = digitalPinToBitMask(_bl);
	*blport   &= ~blpinmask;//off screen
  }

  if(hwSPI) { // Using hardware SPI
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV4); // 4 MHz (half speed)
    //Due defaults to 4mHz (clock divider setting of 21)
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
  } else {
    pinMode(_sclk, OUTPUT);
    pinMode(_sid , OUTPUT);
    clkport     = portOutputRegister(digitalPinToPort(_sclk));
    dataport    = portOutputRegister(digitalPinToPort(_sid));
    clkpinmask  = digitalPinToBitMask(_sclk);
    datapinmask = digitalPinToBitMask(_sid);
    *clkport   &= ~clkpinmask;
    *dataport  &= ~datapinmask;
  }
  // toggle RST low to reset; CS low so it'll listen to us
  *csport &= ~cspinmask;
  
#elif defined(__SAM3X8E__)
  pinMode(_rs, OUTPUT);
  pinMode(_cs, OUTPUT);
  csport    = digitalPinToPort(_cs);
  rsport    = digitalPinToPort(_rs);
  cspinmask = digitalPinToBitMask(_cs);
  rspinmask = digitalPinToBitMask(_rs);
  if (_bl > 0){
	pinMode(_bl, OUTPUT);
	blport    = digitalPinToPort(_bl);
	blpinmask = digitalPinToBitMask(_bl);
	blport   ->PIO_CODR  |= blpinmask;//off screen
  }
  if(hwSPI) { // Using hardware SPI
    SPI.begin();
    SPI.setClockDivider(21); // 4 MHz
    //Due defaults to 4mHz (clock divider setting of 21), but we'll set it anyway 
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
  } else {
    pinMode(_sclk, OUTPUT);
    pinMode(_sid , OUTPUT);
    clkport     = digitalPinToPort(_sclk);
    dataport    = digitalPinToPort(_sid);
    clkpinmask  = digitalPinToBitMask(_sclk);
    datapinmask = digitalPinToBitMask(_sid);
    clkport ->PIO_CODR  |=  clkpinmask; // Set control bits to LOW (idle)
    dataport->PIO_CODR  |=  datapinmask; // Signals are ACTIVE HIGH
  }
  // toggle RST low to reset; CS low so it'll listen to us
  csport ->PIO_CODR  |=  cspinmask; // Set control bits to LOW (idle)
  
#elif defined(__MK20DX128__) || defined(__MK20DX256__)
	if (_sid == 0) _sid = 11;
	if (_sclk == 0) _sclk = 13;
	if ( spi_pin_is_cs(_cs) && spi_pin_is_cs(_rs)
	 && (_sid == 7 || _sid == 11)
	 && (_sclk == 13 || _sclk == 14)
	 && !(_cs ==  2 && _rs == 10) && !(_rs ==  2 && _cs == 10)
	 && !(_cs ==  6 && _rs ==  9) && !(_rs ==  6 && _cs ==  9)
	 && !(_cs == 20 && _rs == 23) && !(_rs == 20 && _cs == 23)
	 && !(_cs == 21 && _rs == 22) && !(_rs == 21 && _cs == 22) ) {
		hwSPI = true;
		if (_sclk == 13) {
			CORE_PIN13_CONFIG = PORT_PCR_MUX(2) | PORT_PCR_DSE;
			SPCR.setSCK(13);
		} else {
			CORE_PIN14_CONFIG = PORT_PCR_MUX(2);
			SPCR.setSCK(14);
		}
		if (_sid == 11) {
			CORE_PIN11_CONFIG = PORT_PCR_MUX(2);
			SPCR.setMOSI(11);
		} else {
			CORE_PIN7_CONFIG = PORT_PCR_MUX(2);
			SPCR.setMOSI(7);
		}
		ctar = CTAR_12MHz;
		pcs_data = spi_configure_cs_pin(_cs);
		pcs_command = pcs_data | spi_configure_cs_pin(_rs);
		SIM_SCGC6 |= SIM_SCGC6_SPI0;
		SPI0.MCR = SPI_MCR_MDIS | SPI_MCR_HALT;
		SPI0.CTAR0 = ctar | SPI_CTAR_FMSZ(7);
		SPI0.CTAR1 = ctar | SPI_CTAR_FMSZ(15);
		SPI0.MCR = SPI_MCR_MSTR | SPI_MCR_PCSIS(0x1F) | SPI_MCR_CLR_TXF | SPI_MCR_CLR_RXF;
	} else {
		hwSPI = false;
		cspin = portOutputRegister(digitalPinToPort(_cs));
		rspin = portOutputRegister(digitalPinToPort(_rs));
		clkpin = portOutputRegister(digitalPinToPort(_sclk));
		datapin = portOutputRegister(digitalPinToPort(_sid));
		*cspin = 1;
		*rspin = 0;
		*clkpin = 0;
		*datapin = 0;
		pinMode(_cs, OUTPUT);
		pinMode(_rs, OUTPUT);
		pinMode(_sclk, OUTPUT);
		pinMode(_sid, OUTPUT);
	}

#endif
  if (_bl > 0){
	pinMode(_bl, OUTPUT);
	digitalWrite(_bl, LOW);
  }
  if (_rst) {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(500);
    digitalWrite(_rst, LOW);
    delay(500);
    digitalWrite(_rst, HIGH);
    delay(500);
  }

  if(cmdList) commandList(cmdList);
}


// Initialization for S6D02A1 LCD only
void TFT_S6D02A1::init() {
	commonInit(0);
	commandList(S6D02A1);
	if (_bl > 0){
		backlight(1);
	}
}

void TFT_S6D02A1::backlight(uint8_t val) {
	if (_bl > 0){
		if (val){
			#ifdef __AVR__
			*blport |=  blpinmask;
			#elif defined(__SAM3X8E__)
			blport->PIO_CODR  |=  blpinmask;
			#elif defined(__MK20DX128__) || defined(__MK20DX256__)
			digitalWrite(_bl, HIGH);
			#endif
		} else {
			#ifdef __AVR__
			*blport &= ~blpinmask;
			#elif defined(__SAM3X8E__)
			blport->PIO_SODR  |=  blpinmask;
			#elif defined(__MK20DX128__) || defined(__MK20DX256__)
			digitalWrite(_bl, LOW);
			#endif
		}
	}
}

void TFT_S6D02A1::setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1,
 uint8_t y1) {

  writecommand(S6D02A1_CASET); // Column addr set
  writedata16(x0+colstart);   // XSTART 
  writedata16(x1+colstart);   // XEND
  writecommand(S6D02A1_RASET); // Row addr set
  writedata16(y0+rowstart);   // YSTART
  writedata16(y1+rowstart);   // YEND
  writecommand(S6D02A1_RAMWR); // write to RAM
}


void TFT_S6D02A1::pushColor(uint16_t color) {
  writebegin();
  writedata16(color);
 }

void TFT_S6D02A1::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;
  setAddrWindow(x,y,x+1,y+1);
  writedata16(color);
}

void TFT_S6D02A1::drawFastVLine(int16_t x, int16_t y, int16_t h,
 uint16_t color) {

  // Rudimentary clipping
  if((x >= _width) || (y >= _height)) return;
  if((y+h-1) >= _height) h = _height-y;
  setAddrWindow(x, y, x, y+h-1);
  while (h--) {
    writedata16(color);
  }
}


void TFT_S6D02A1::drawFastHLine(int16_t x, int16_t y, int16_t w,
  uint16_t color) {

  // Rudimentary clipping
  if((x >= _width) || (y >= _height)) return;
  if((x+w-1) >= _width)  w = _width-x;
  setAddrWindow(x, y, x+w-1, y);
  while (w--) {
    writedata16(color);
  }
}


void TFT_S6D02A1::fillScreen(uint16_t color) {
  fillRect(0, 0,  _width, _height, color);
}



// fill a rectangle
void TFT_S6D02A1::fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
  uint16_t color) {

  // rudimentary clipping (drawChar w/big text requires this)
  if((x >= _width) || (y >= _height)) return;
  if((x + w - 1) >= _width)  w = _width  - x;
  if((y + h - 1) >= _height) h = _height - y;
  setAddrWindow(x, y, x+w-1, y+h-1);
  for(y=h; y>0; y--) {
    for(x=w; x>0; x--) {
      writedata16(color);
    }
  }
}



#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04


void TFT_S6D02A1::setRotation(uint8_t m) {
// Generally 0 - Portrait 1 - Landscape

  writecommand(S6D02A1_MADCTL);
  rotation = m % 4; // can't be higher than 3
  switch (rotation) {
   case 0:
     writedata(MADCTL_MX | MADCTL_MY | MADCTL_BGR);
     _width  = S6D02A1_TFTWIDTH;
     _height = S6D02A1_TFTHEIGHT;
     break;
   case 1:
     writedata(MADCTL_MY | MADCTL_MV | MADCTL_BGR);
     _width  = S6D02A1_TFTHEIGHT;
     _height = S6D02A1_TFTWIDTH;
     break;
  case 2:
     writedata(MADCTL_BGR);
     _width  = S6D02A1_TFTWIDTH;
     _height = S6D02A1_TFTHEIGHT;
    break;
   case 3:
//     writedata(MADCTL_MX | MADCTL_MV | MADCTL_RGB);
     writedata(MADCTL_MX | MADCTL_MV | MADCTL_BGR);
     _width  = S6D02A1_TFTHEIGHT;
     _height = S6D02A1_TFTWIDTH;
     break;
  }
}


void TFT_S6D02A1::fillMulticolorRect(int16_t x, int16_t y, int16_t w, int16_t h,uint8_t max_,uint8_t scaleType) {
	uint8_t index;
	//uint16_t colour;
	if ((x >= _width) || (y >= _height)) return;
	if ((x + w - 1) >= _width)  w = _width  - x;
	if ((y + h - 1) >= _height) h = _height - y;

	setAddrWindow(x,y,x+w-1,y+h-1);
	for (y=h; y>0; y--) {
		if (scaleType > 0) {
			index = map(h-y+1,0,max_,10,0);//v bar
/* 			Serial.print("--> Y:");
			Serial.print(y,DEC);
			Serial.print(" / h-y:");
			Serial.print(h-y+1,DEC);
			Serial.print(" - max:");
			Serial.print(max_,DEC);
			Serial.print(" - idx:");
			Serial.print(index,DEC);
			Serial.print("\n"); */
			for (x=w; x>0; x--) {
/* 				Serial.print("X:");
				Serial.print(x,DEC);
				Serial.print(" / IDX:");
				Serial.print(index,DEC);
				Serial.print("\n"); */
				writedata16(TFTcolorScale_1[index]);
				//delay(100);
			}
		} else {
			for (x=w; x>0; x--) {
				if (scaleType < 1) index = map(w-x,0,max_,0,10);//h bar
				writedata16(TFTcolorScale_1[index]);
			}
		}
		//delay(500);
	}
}

uint16_t TFT_S6D02A1::decodeCScale(uint8_t val,uint8_t divVal,uint8_t scale){
    for (uint8_t i=0;i<11;i++){
      if (val <= (divVal*i)+i) {
        //color = tft.Color24To565(colorScale_1[10-cl]);//for 24 bit colors
        switch(scale){
           case 0:
           return TFTcolorScale_1[11-i];
           case 1:
           return TFTcolorScale_1[i];
           default:
           return TFTcolorScale_1[11-i];
        }
      }
    }
    return 0xFFFF;
}

void TFT_S6D02A1::barVert(int16_t x, int16_t y, int16_t w, int16_t l, uint16_t color) {
	fillTriangle(x+1, y+2*w, x+w, y+w+1, x+2*w-1, y+2*w, color);
	fillTriangle(x+1, y+2*w+l+1, x+w, y+3*w+l, x+2*w-1, y+2*w+l+1, color);
	fillRect(x, y+2*w+1, 2*w+1, l, color);
}
 
void TFT_S6D02A1::barHor(int16_t x, int16_t y, int16_t w, int16_t l, uint16_t color) {
	fillTriangle(x+2*w, y+2*w-1, x+w+1, y+w, x+2*w, y+1, color);
	fillTriangle(x+2*w+l+1, y+2*w-1, x+3*w+l, y+w, x+2*w+l+1, y+1, color);
	fillRect(x+2*w+1, y, l, 2*w+1, color);
}

void TFT_S6D02A1::invertDisplay(boolean i) {
  writecommand(i ? S6D02A1_INVON : S6D02A1_INVOFF);
}
