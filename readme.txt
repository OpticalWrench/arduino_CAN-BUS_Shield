CANDUINO library modified to function with Arduino v1.0 and newer.

The biggest problem with the old library was the SPI MOSI, etc. pin names were duplicate after arduino v1.0.

There were also erroneous hex values in some comments that made the "ff" (full frame) library functions appear to not work.  They work just fine as long as you compare for the correct return values. (see the code and comments for details)

The LCD used in the arduino sketch is parallax 2x16 LCD, parallax part # 27977.  I have a repo for that, too: https://github.com/hubbsy/parallax_LCD_27977.git  It's not a full arduino library, but the friendly names in the header file for the LCD make it pretty easy to use!
