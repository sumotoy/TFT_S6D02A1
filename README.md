TFT_S6D02A1
===========
A hyper-fast driver for <b>Samsung S6D02A1</b> drived TFT (128x160) in SPI for Teensy3,DUE and Arduinos.<br>

Version <b>1.0b1</b><br>

I found some TFT on chinese Ebay that was supposed be compatible with ST7735 and of course NOT! I spent a couple of days to discover that was actually
a Samsung S6D02A1 chip that it's not compatible with ST7735 and actually has one more pin for display blanking.
I modified Adafruit_ST7735 library so now this display works amazing fast with 32bit micros and pretty well with 8 bit ones.
Those displays can work at 5V or 3V3 but if you work with Arduino UNO or 5V level processor you need to put a 1K resistor between
any display/cpu connection!
If you don't need to deal with blanking pin, connect it to VCC.
<br>
You will need the standard Adafruit_GFX library as well.<br>
<br>
Note that many examples uses <b>Arm_Math</b> for Teensy3 so cannot be used with different micros!<br>
On Ebay you can find here (I'm not affiliated in any way)<br>
http://www.ebay.com/itm/370987651121 <br>
http://www.ebay.com/itm/141341180758 <br>


CONNECTION (only TFT, not SD card):<br>
1-RST ---> Reset Pin<br>
2-CE  ---> SPI SS pin (check witch micro pin you can use!)<br>
3-D/C ---> Data/Command (check witch micro pin you can use!)<br>
4-DIN ---> Mosi<br>
5-CLK ---> SCK<br>
6-UCC ---> 5V or 3V3 (check TFT jumper)<br>
7-BL  ---> Blanking (if you will not use, connect to VCC!!!!)<br>
8-GND ---> Ground (of course)<br>

Note for Teensy3.x<br>
CS & DC can use pins 2, 6, 9, 10, 15, 20, 21, 22, 23<br>
but certain pairs must NOT be used: 2+10, 6+9, 20+23, 21+22<br>
