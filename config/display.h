
#ifndef DISPLAY_H
#define DISPLAY_H

// **** default pin connection for LCD display  ****

// Adaruit 2.4 TFT with touchscreen

// For the Moteino Mega, use digital Analog pins A0 through A7
//   D0 connects to analog pin A0
//   D1 connects to analog pin A1
//   D2 connects to analog pin A2
//   D3 connects to analog pin A3
//   D4 connects to analog pin A4
//   D5 connects to analog pin A5
//   D6 connects to analog pin A6
//   D7 connects to analog pin A7
#define YP A0
#define XM A1
#define YM A2
#define XP A3


#ifdef TFT_TOUCH_9341

	#include <Adafruit_TFTLCD.h>
	#include <TouchScreen.h>

	#define YP A0  // must be an analog pin, use "An" notation!
	#define XM A1  // must be an analog pin, use "An" notation!
	#define YM A2  // can be a digital pin
	#define XP A3   // can be a digital pin

	#define TS_MINX 150
	#define TS_MINY 120
	#define TS_MAXX 920
	#define TS_MAXY 940

  #define MINPRESSURE 10
  #define MAXPRESSURE 1000


  TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

  // display pins
	#define LCD_CS 18
	#define LCD_CD 19
	#define LCD_WR 20
	#define LCD_RD 21
	#define LCD_RESET 0 	// optional

	// Assign human-readable names to some common 16-bit color values:
	#define	BLACK   0x0000
	#define	BLUE    0x001F
	#define	RED     0xF800
	#define	GREEN   0x07E0
	#define CYAN    0x07FF
	#define MAGENTA 0xF81F
	#define YELLOW  0xFFE0
	#define WHITE   0xFFFF
	#define ORANGE  0xFC00

	// character size from Adafruit_GXF lib
	#define CHARWIDTH 6
	#define CHARHEIGHT 8
	#define SCRROTATION 0 // 90º rotation
	#define CHARSCALE 2
	#define SCRLINES 10   // 20 Lines or 27 characters / CHARSCALE
	#define SCRCHARS 25   // 21 characters or 16 lines /CHARSCALE

	Adafruit_TFTLCD display(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

	#define BOXSIZE 120

#endif

#ifdef LCD

	#include <Adafruit_PCD8544.h> //Required for the Nokia 5110 LCD display

	// Initiate the display instance - Software SPI (slower updates, more flexible pin options):
	// pin 24/A0 - Serial clock out (SCLK)
	// pin 25/A1 - Serial data out (DIN)
	// pin 26/A2 - Data/Command select (D/C)
	// pin 27/A3 - LCD chip select (CS)
	// pin 28/A4 - LCD reset (RST)

	#define RST  0 //A5 //18
	#define RS   1 //A4 //19
	#define SDA  A2 //20
	#define SCL  A0 //21
	#define CS   3 //A3 //22

	Adafruit_PCD8544 display = Adafruit_PCD8544(SCL, SDA, CS, RS, RST);

	// character size from Adafruit_GXF lib
	#define CHARWIDTH 6
	#define CHARHEIGHT 8
	#define SCRROTATION 0 // 0 rotation
	#define CHARSCALE 1
	#define SCRPIXELX 48
	#define SCRPIXELY 84
	#define SCRLINES 6    // 6 lines
	#define SCRCHARS 14   // 14 characters
///  #define BOXSIZE 120

	#define PIN_LCD_LIGHT 9 //Backlight pin for NOKIA LCD
#endif

#ifdef TFT_ST7735
	#include <Adafruit_ST7735.h> //Required for OLED LCD
	// pin definition for ITDB02-1.8 TFT display from ITEAD STUDIO
	// assuming the use of Moteino (several pins are reserved)
	#define RST  1 //A5 //18
	#define RS   0 //A4 //19
	#define SDA  A2 //20
	#define SCL  A0 //21
	#define CS   3 //A3 //22
	// Adafruit_ST7735 display = Adafruit_ST7735(CS, RS, SDA, SCL, RST);
	Adafruit_ST7735 display = Adafruit_ST7735(CS, RS, RST);
	//	Adafruit_ST7735 display = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
	#define SCRPIXELX 160
	#define SCRPIXELY 128
	// character size from Adafruit_GXF lib
	#define CHARWIDTH 6
	#define CHARHEIGHT 8
	#define SCRROTATION 1 // 90 deg rotation
	#define CHARSCALE 1
	#define SCRLINES 14   // 20 Lines or 27 characters / CHARSCALE
	#define SCRCHARS 25   // 21 characters or 16 lines /CHARSCALE
  #define BOXSIZE 120

	#define BLACK ST7735_BLACK
	#define WHITE ST7735_WHITE
	#define RED ST7735_RED
	#define BLUE ST7735_BLUE
	#define YELLOW ST7735_YELLOW
	#define ORANGE 0xFF00
  #define GREEN ST7735_GREEN
#endif

#ifdef TFT_ILI9340

	#include <Adafruit_ILI9340_PA.h> //Library required to handle the TFT I2C with the radio interrupts

	// pin definition for FT_ILI9340 ADAFRUIT TFT display
	#define SCL 7 //Clock
	#define MISO 6 //MISO
	#define MOSI 5 //MOSI
	#define CS A3	// A3 //Chip Select
	#define DC A4	//A4 //Data Chip
	#define RST A5	//A5 //Reset

	//Screen size definitions
	#define SCRPIXELX 320
	#define SCRPIXELY 240

	// character size from Adafruit_GXF lib
	#define CHARWIDTH 6
	#define CHARHEIGHT 8
	#define SCRROTATION 1 // 90º rotation
	#define CHARSCALE 2
	#define SCRLINES 14   // 20 Lines or 27 characters / CHARSCALE
	#define SCRCHARS 25   // 21 characters or 16 lines /CHARSCALE
//  #define BOXSIZE 120

	//Adaruit_ILI9340 display = Adafruit_ILI9340(CS, DC, MOSI, SCL, RST, MISO); //For Software SPI
	Adafruit_ILI9340 display = Adafruit_ILI9340(CS, DC, RST); //For Hardware SPI

	#define BLACK ILI9340_BLACK
	#define WHITE ILI9340_WHITE
	#define RED ILI9340_RED
	#define BLUE ILI9340_BLUE
	#define YELLOW ILI9340_YELLOW
	#define ORANGE 0xFC00
  #define GREEN ILI9340_GREEN
#endif


#endif
