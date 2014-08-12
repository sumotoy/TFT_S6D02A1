TFT_S6D02A1
===========

I found some TFT on chinese Ebay that was supposed be compatible with ST7735 and of course NOT! I spent a couple of days to discover that was actually
a Samsung S6D02A1 chip that it's not compatible with ST7735 and actually has one more pin for display blanking.
I modified Adafruit_ST7735 library so now this display works amazing fast with 32bit micros and pretty well with 8 bit ones.
Those displays can work at 5V or 3V3 but if you work with Arduino UNO or 5V level processor you need to put a 1K resistor between
any display/cpu connection!
If you don't need to deal with blanking pin, connect it to VCC.
