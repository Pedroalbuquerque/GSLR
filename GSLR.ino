// **********************************************************************************************************
// Moteino GPS Receiver & Telemetry
// 2015-01-09 (C) luisr320@gmail.com
// **********************************************************************************************************
// It receives data from remote Moteino sensors and display data on an Nokia 5110 LCD or TFT
// **********************************************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this code/library but please abide with the CCSA license:
// http://creativecommons.org/licenses/by-sa/3.0/
// **********************************************************************************************************

/*
version V1.3.0
Changes:
date: 2016.07.16 Edited by Luis Rodrigues
- Adafruit 2.4" TFT display with touchscreen added
date: 2016.07.13 Edited by Luis Rodrigues:
- changed the radio handling to LORA with Reliable Datagram
- remote station battery is now displayed
- committed to always using the interrupt for the GPS
date: 2016.07.04 Edited by Luis Rodrigues:
- send to Google function reactivated on menu 2
- removed all bounce2 library references
- added some source references to the libraries include list
- removed all references to the bounce2 library
- cleaned old EEPROM references
date: 2016.07.01 Edited by Pedro Albuquerque
date: 2016.06.29 Edited by Pedro Albuquerque
- GPSMath.h included to allow GPS calculations
date: 2016.06.26 Edited by Luis Rodrigues
date: 2016.06.14 Created by Pedro Albuquerque
-	button speed reaction improvement
To Do:
- include a GPS module on GS to assist on plane rescue
- include a function send a command to activate a buzzer on RS to improve plane detection
*/


#include <RHReliableDatagram.h> //http://www.airspayce.com/mikem/arduino/RadioHead/RadioHead-1.61.zip
//#include <arduino.h> // uncomment if using ATOM editor
#include <SPIFlash.h> //Arduino/Moteino library for read/write access to SPI flash memory chips. https://github.com/LowPowerLab/SPIFlash
#include <SPI.h> //Arduino native SPI library
#include <RH_RF95.h> //Required for the LORA Radios http://www.airspayce.com/mikem/arduino/RadioHead/
#include <Adafruit_GFX.h> //Required for the Nokia 5110 LCD and ST7735 TFT display https://github.com/adafruit/Adafruit-GFX-Library
#include <FlashLogM.h> //To handle the Flash log https://github.com/Pedroalbuquerque/FlashLogM
#include <math.h> //Arduino native math library
#include <GPSMath.h> //To handle all GPS calculations https://github.com/Pedroalbuquerque/GPSMath
#include <Adafruit_GPS.h> //https://github.com/adafruit/Adafruit_GPS/archive/master.zip

#include "./config/all.h"

#include "menus.h"

// Define the data packet struct that will be received from the GPS transmitter
struct Payload
{
	char HD[3] = "/*";
	uint8_t hour;
	uint8_t minute;
	uint8_t seconds;
	uint16_t miliseconds;
	uint8_t day;
	uint8_t month;
	uint8_t year;
	float groundspeed; //In knots
	float track; //Course over ground in degrees
	float latitude; //ddmm.mmmm
	char lat; //N/S
	float longitude; //dddmm.mmmm
	char lon; //E/W
	float altitude; //MSL Altitude
	uint8_t fixquality; //Same as 3D FIX
	uint8_t satellites; //Range 0 to 14
	float HDOP; //Horizontal Dilution of Precision <2.0 is good - https://en.wikipedia.org/wiki/Dilution_of_precision_(GPS)
	float geoidheight;
	float latitudedeg;
	float longitudedeg;
	bool fix; // FIX 1/0
	float batteryVolts;
};
Payload Data;

Payload GS_GPS; // struct to hold last good position from GS GPS
Payload RS_GPS; // struct to hold last good position from RS GPS, this cold be the last save position on LOG

//Define GPS information LOG received from RS using SPI Flash Memory
FlashLogM mylog;

//Initialize the generic radio driver instance
RH_RF95 driver;

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, GR_STATION_ID);

// cursor object instance
clsCursor menuCursor(menu); //define a cursor for the menu array


// object declaration to use GPS

Adafruit_GPS GPS(&Serial1); // connect GPS to serial 1 GPS_TX on pin10 GPS_RX on pin 11
#define GPS_BAUD 9600

// Function declaration if using Visual studio IDE
#define VISUALSTD
#ifdef VISUALSTD
	int len(char* str);
	//byte checksum(char *str);
	void changeMenu();
	void displayReset();
	void displaySetCursor(int line, int column);
	void fixposition();
	//void logposition(float loglat, float loglong);
	void displaymenu(byte menuPage, bool forceRepaint);
	void displaywarning(int warningcode);
	int len(char *str);
	char* fill(char* str, int length, char charcode, bool initialize);
	uint8_t setflag(uint8_t flagContainer, uint8_t flag, bool set);
	//void sendToGoogle(Payload stcData);
#endif


void setup()
{
	// initialize Serial port for debugging
	Serial.begin(SERIAL_BAUD); //Initialize the Serial port at the specified baud rate
	Serial.println F("GPS AND TELEMETRY MODULE");
	Serial.println(VERSION);
	Serial.println F("Initializing...");

	if (manager.init())
	{
		driver.setFrequency(434);
	}
	else
	{
		Serial.println F("init failed");
		// Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
	}

	// initialize GPS
	GPS.begin(GPS_BAUD);
	// Enable interrupts on Timer0 for GPS
	// Timer0 is already used for millis() - we'll just interrupt somewhere
	// in the middle and call the "Compare A" function
	OCR0A = 0xAF;
	TIMSK0 |= _BV(OCIE0A);

	// ### Initialize push-buttons
	pinMode(BUTPIN1, INPUT_PULLUP); // Setup the first button with an internal pull-up
	pinMode(BUTPIN2, INPUT_PULLUP); // Setup the second button with an internal pull-up

		#ifdef LCD
		pinMode(PIN_LCD_LIGHT, OUTPUT); //LCD backlight, LOW = backlight ON
	#endif

	#ifdef BUZZER
		oldbuzztimer = 0; //Initialize buzzer timer variable
		pinMode(BUZZ, OUTPUT); //Make BUZZ pin an output
	#endif

	// SPI initialization
	SPI.usingInterrupt(digitalPinToInterrupt(2));

	// ### Initialize  display

	#ifdef TFT_TOUCH_9341
		display.reset();
		uint16_t identifier = display.readID();
		if (identifier == 0x9341)
		{
			display.begin(identifier);
			display.fillScreen(BLACK);
			display.setTextSize(CHARSCALE);
			display.setRotation(SCRROTATION);
			display.setTextColor(WHITE, BLACK);
			display.setTextWrap(true);
			displaySetCursor(2, 0);
			display.println(F("GPS TELEMETRY"));
			display.println(F("Ver:")); display.println(VERSION);
			delay(3000);
			displaySetCursor(8, 0);
			display.setTextColor(WHITE);  display.setTextSize(2);
			display.println("    Please wait...");
			display.println("   Analizing LOG...");

		}
		else
		{
			Serial.print(F("Unknown LCD driver chip: "));
			Serial.println(identifier, HEX);
			return;
		}


	#endif

	#ifdef LCD // display SETUP (Nokia LCD)
		display.begin();
		displayReset(); //Cleanup the LCD
		display.setContrast(60); // you can change the contrast around to adapt the display for the best viewing!
		display.println("Starting LCD display!");
		display.print("Ver. "); display.println(VERSION);
		display.display(); // show splash screen
		delay(1000);
	#endif

	#ifdef TFT_ST7735	// display Setup OLED LCD
		display.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
		// POSITION CURSOR ON TOP LEFT
		displayReset();
		display.println("Starting TFT display!");
		display.print("Ver:"); display.println(VERSION);
		delay(5000);
	#endif

	#ifdef TFT_ILI9340 // display Setup OLED LCD
		display.begin();
		displayReset();
		display.println("Starting TFT display!");
		display.print("Ver:"); display.println(VERSION);
		Serial.println("TFT Display ILI9340 - initialized");
	#endif

	// ### Initialize Log

	// initialize log variables to start write and read
	mylog.initialize(Data);
	Serial.println F("Log data initialized");
	Serial.print F("Log next write addr:"); Serial.println(mylog.nextWrite);
	Serial.print F("Log next read addr:"); Serial.println(mylog.nextRead);
	Serial.print F("Log # records saved:"); Serial.println(mylog.numRecords);

	// load menu screen on LCD/TFT
	displaymenu(menuPage, false); //Start menu display (menu page number, screen refresh requirement)

	Serial.println F("Setup finished");

	timerLink = millis(); //Initialize Data link loss timeout timer variable

	Serial.print("Menu size:");Serial.println( menuCursor.maxMenu());

}

void loop()
{
	uint8_t button = 0;

	makeHeader();

	// read button to see if there is activity
	button = 0;
	button = buttonPressed();
	if(button !=0) DEBUG_MSG("button: %d\n",button);

	menuAction(button);

	// check radio reception and process if data is available
	if (timerLink > millis()) timerLink = millis();
	if (manager.available()) //If some packet was received by the radio, wait for all its contents to come trough
	{
		warningLevel = setflag(warningLevel, WRN_LINK, FLAGRESET); // clear Link flag

		uint8_t len = sizeof(Data);
		uint8_t from;

		if (manager.recvfromAck((uint8_t *)&Data, &len, &from))

		{
			timerLink = millis(); //Set a counter for data link loss timeout calculation


			rssi = driver.lastRssi();	//RSSI;


			if (Data.fix == 1)	// if GPS fix acquired
			{
				if (fixinMem == 0) fixposition();
					fixinMem = 1;

				warningLevel = setflag(warningLevel, WRN_FIX, FLAGRESET);

				// Run the distance calculating function, passing the memorized position as arguments
				kmflag = GPSDist(homelat, homelon, Data.latitudedeg, Data.longitudedeg, &homedist, &homeazim);


				if (Data.altitude > maxalt + homealt)	// check maximum altitude
					maxalt = Data.altitude - homealt;

				if (kmflag == 0)	// check maximum distance
				{
					kmflagmem = kmflag;
					if (homedist > maxdist)
						maxdist = homedist;
				}
				else
				{
					if (kmflagmem == 0)
					{
						kmflagmem = 1;
						maxdist = homedist;
					}
					else if (homedist > maxdist)
						maxdist = homedist;
				}

				//check maximum speed
				if (Data.groundspeed*1.852 < 1)
					Data.groundspeed = 0;
				if (Data.groundspeed*1.852 > highspeed)
					highspeed = Data.groundspeed*1.852;
			}
			else
				warningLevel = setflag(warningLevel, WRN_FIX, FLAGSET);

			// save data just received to Log memory
			mylog.saveData(Data);
			#ifdef GOOGLEMAPS
				sendToGoogle(Data);
			#endif
			displaymenu(menuPage, false); // update menu info (menu page number, screen refresh)
		}
	}
	else // if no data received
	{
		// check if link timeout, ie, no data received for more than 5 sec
		if (millis() > (timerLink + 5000)) //If LINK lost for more than 5 sec...
		{
			warningLevel = setflag(warningLevel, WRN_LINK, FLAGSET);  // set LINK flag
			#ifdef BUZZER
				newbuzztimer = millis();
				if (newbuzztimer > (oldbuzztimer + 2000))
				{
					oldbuzztimer = newbuzztimer;
					Blink(BUZZ, 5);
					delay(100);
					Blink(BUZZ, 5);
				}
			#endif
		}
	}
	// check Warnings
	if (millis() < timerWarning)
		timerWarning = millis(); // if millis() wrap around reinitialize timer
	if (millis() > timerWarning + 2000)
	{
		displaywarning(warningLevel);
		timerWarning = millis();
	}

	/*
	// check GPS data if on Search menu (menu 9)
	if(menuPage = 9)
	{
		// place here code to process local GPS data, calculate distance and azimuth from last good RS position
		if (GPS.newNMEAreceived())
		{
			if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
			{
				Serial.println("GS NMEA not parsed");
				return;                         // we can fail to parse a sentence in which case we should just wait for another
			}
			else
			{
				// save local GPS data to a var
			}

			// put here code to calculate distance and azimuth from GS to last RS good location
		}

	}
	*/

}

//----------------------------------------------------------------------//
//                             FUNCTIONS                                //
//----------------------------------------------------------------------//

void makeHeader()
{
	if (Data.batteryVolts < 3.2)
	{
		display.fillRect(204, 0, 14, 12, RED);
		display.drawRect(204, 0, 14, 12, BLACK);
		display.fillRect(216, 0, 14, 12, BLACK);
		display.drawRect(216, 0, 14, 12, BLACK);
		display.fillRect(228, 0, 14, 12, BLACK);
		display.drawRect(228, 0, 14, 12, BLACK);
	}
	else if (Data.batteryVolts >= 3.2 && Data.batteryVolts < 3.6)
	{
		display.fillRect(204, 0, 14, 12, YELLOW);
		display.drawRect(204, 0, 14, 12, BLACK);
		display.fillRect(216, 0, 14, 12, YELLOW);
		display.drawRect(216, 0, 14, 12, BLACK);
		display.fillRect(228, 0, 14, 12, BLACK);
		display.drawRect(228, 0, 14, 12, BLACK);
	}
	else
	{
		display.fillRect(204, 0, 14, 12, GREEN);
		display.drawRect(204, 0, 14, 12, BLACK);
		display.fillRect(216, 0, 14, 12, GREEN);
		display.drawRect(216, 0, 14, 12, BLACK);
		display.fillRect(228, 0, 14, 12, GREEN);
		display.drawRect(228, 0, 14, 12, BLACK);
	}
	display.setCursor(90, 0);
	if (Data.hour < 10)
		display.print("0");
	display.print(Data.hour);
	display.print(":");
	if (Data.minute < 10)
		display.print("0");
	display.print(Data.minute);
	display.drawLine(0, 20, 240, 20, RED);
}


uint8_t setflag(uint8_t flagContainer, uint8_t flag, bool set)
{
	if (set) return (flagContainer | flag); else return (flagContainer & ~flag);
}


void changeMenu()
{
	//  Blink(BUZZ, 50);
	menuPage = menuPage + 1;
	if (menuPage > MPAGES)
	{
		menuPage = 1;;
	}
}

#ifdef BUZZER
void Blink(byte PIN, int DELAY_MS)//The BUZZ Blinking function
{
	digitalWrite(PIN, HIGH);
	delay(DELAY_MS);
	digitalWrite(PIN, LOW);
}
#endif

	void displayReset()
	{
		display.fillScreen(BLACK);

		display.fillRect(0, 200, BOXSIZE, BOXSIZE, RED);
		display.drawRect(0, 200, BOXSIZE, BOXSIZE, WHITE);

		display.fillRect(BOXSIZE, 200, BOXSIZE, BOXSIZE, YELLOW);
		display.drawRect(BOXSIZE, 200, BOXSIZE, BOXSIZE, WHITE);

		displaySetCursor(16, 4);
		display.setTextColor(WHITE);  display.setTextSize(4);
		display.println("B1");

		displaySetCursor(16, 14);
		display.setTextColor(BLUE);  display.setTextSize(4);
		display.println("B2");


		display.setTextSize(CHARSCALE);
		display.setRotation(SCRROTATION);
		display.setTextColor(WHITE, BLACK);
		display.setTextWrap(true);
		display.setCursor(0, 0);
		return;
	}


void displaySetCursor(int line, int column)
{
	display.setCursor(column * CHARWIDTH * CHARSCALE, line * CHARHEIGHT*CHARSCALE);
	return;
}

void fixposition()
{
	highspeed = 0;//Reset maximum speed
	maxdist = 0; //Reset maximum distance
	maxalt = 0; //Reset maximum altitude
	kmflagmem = 0; //Reset Km flag mem
	homeazim = 0;
	homelat = Data.latitudedeg;//Memorize FIX latitude in DDMM.SS
	homelon = Data.longitudedeg;//Memorize FIX longitude in DDDMM.SS
	displayReset();
	display.println("FIX Position Memorized");
	display.println("LAT:"); display.println(homelat, 8); display.println("LON:"); display.println(homelon, 8);
	homealt = Data.altitude;
	#ifdef LCD
		display.display();
	#endif
	delay(2000);
	displaymenu(menuPage, true);
}


void displaymenu(byte menuPage, bool forceRepaint)
{

	static byte lastmenu;  //remember last menu for paint or refresh

	switch (menuPage)
	{
		case 1: //NAVIGATION STATUS MENU
		{
			if (lastmenu != menuPage || forceRepaint)
			{
				lastmenu = menuPage;
				displayReset();
				displaySetCursor(2, 0);
				display.print("1 - STATUS");
				displaySetCursor(3, 0); sprintf(strPRT, "SAT:%i", Data.satellites); display.print(strPRT);
				displaySetCursor(4, 0); sprintf(strPRT, "QUAL:%d", Data.fixquality); display.print(strPRT);
				displaySetCursor(5, 0); sprintf(strPRT, "HDOP:%s", dtostrf(Data.HDOP, 5, 2, strtmp)); display.print(strPRT);
				displaySetCursor(6, 0); sprintf(strPRT, "RX_RSSI:%d", rssi); display.print(strPRT);
				displaySetCursor(7, 0); sprintf(strPRT, "RS BAT:%s V", dtostrf(Data.batteryVolts, 4, 2, strtmp)); display.print(strPRT);
			}
			else // if already in menu, print just info to reduce screen flicker
			{
				displaySetCursor(3, 4);
				display.print(Data.satellites);
				displaySetCursor(4, 5);
				display.print(Data.fixquality);
				displaySetCursor(5, 5);  sprintf(strPRT, "%s", dtostrf(Data.HDOP, 5, 2, strtmp)); display.print(strPRT);
				displaySetCursor(6, 8);	display.print(rssi);
				displaySetCursor(7, 7); sprintf(strPRT, "%s", dtostrf(Data.batteryVolts, 4, 2, strtmp));	display.print(strPRT);
			}
			break;
		}

		case 2: //INFO MENU
		{
			if (lastmenu != menuPage || forceRepaint)
			{
				lastmenu = menuPage;
				displayReset();
				displaySetCursor(2, 0); display.print("2 - INFO");
				sprintf(strPRT, "ALT:%s m", dtostrf(Data.altitude - homealt, 4, 0, strtmp)); displaySetCursor(3, 0); display.print(strPRT);
				sprintf(strPRT, "SPD:%s Km/h", dtostrf(Data.groundspeed*1.852, 4, 0, strtmp)); displaySetCursor(4, 0); display.print(strPRT);
				sprintf(strPRT, "AZM:%s Deg", dtostrf(homeazim, 4, 0, strtmp)); displaySetCursor(5, 0); display.print(strPRT);
				displaySetCursor(6, 0); sprintf(strPRT, "DST:%s", dtostrf(homedist, 4, 0, strtmp)); display.print(strPRT);
				if (kmflag == 0) display.print(" m "); else display.print(" Km");
				display.setTextSize(CHARSCALE + 2);
				displaySetCursor(8, 0); sprintf(strPRT, "%s Km/h", dtostrf(highspeed, 1, 0, strtmp)); display.print(strPRT);
				display.setTextSize(CHARSCALE);
			}
			else
			{
				sprintf(strPRT, "%s m      ", dtostrf(Data.altitude - homealt, 4, 0, strtmp)); displaySetCursor(3, 4); display.print(strPRT);
				sprintf(strPRT, "%s Km/h      ", dtostrf(Data.groundspeed*1.852, 4, 0, strtmp)); displaySetCursor(4, 4); display.print(strPRT);
				sprintf(strPRT, "%s Deg      ", dtostrf(homeazim, 4, 0, strtmp)); displaySetCursor(5, 4); display.print(strPRT);
				displaySetCursor(6, 4); sprintf(strPRT, "%s", dtostrf(homedist, 4, 0, strtmp)); display.print(strPRT);
				if (kmflag == 0) display.print(" m "); else display.print(" Km"); //Spaces added to allow "m" or "Km" to be erased after large numbers
				display.setTextSize(CHARSCALE + 2);
				displaySetCursor(8, 0); sprintf(strPRT, "%s Km/h", dtostrf(highspeed, 1, 0, strtmp)); display.print(strPRT);
				display.setTextSize(CHARSCALE);
			}
			break;
		}

		case 3: //MAXIMUM MENU
		{
			if (lastmenu != menuPage || forceRepaint)
			{
				lastmenu = menuPage;
				displayReset();
				displaySetCursor(2, 0); display.print("3 - MAXIMUM");
				displaySetCursor(3, 0); sprintf(strPRT, "MxSpd:%s K/h", dtostrf(highspeed, 4, 0, strtmp)); display.print(strPRT);
				displaySetCursor(4, 0); sprintf(strPRT, "MxAlt:%4ld m", maxalt); display.print(strPRT);
				displaySetCursor(5, 0); sprintf(strPRT, "MxDst:%4ld", maxdist); display.print(strPRT);
				if (kmflag == 0) display.print(" m "); else display.print(" Km");
				displaySetCursor(7, 0); sprintf(strPRT, "GPS Alt:%s m", dtostrf(Data.altitude, 5, 0, strtmp)); display.print(strPRT);
			}
			else
			{
				displaySetCursor(3, 6); sprintf(strPRT, "%s", dtostrf(highspeed, 4, 0, strtmp)); display.print(strPRT);
				displaySetCursor(4, 6); sprintf(strPRT, "%4ld", maxalt); display.print(strPRT);
				displaySetCursor(5, 6); sprintf(strPRT, "%4ld", maxdist); display.print(strPRT);
				if (kmflag == 0) display.print(" m "); else display.print(" Km");
				displaySetCursor(7, 8); sprintf(strPRT, "%s ", dtostrf(Data.altitude, 5, 0, strtmp)); display.print(strPRT);
			}
			break;
		}

		case 4: //COORDINATES MENU
		{
			if (lastmenu != menuPage || forceRepaint)
			{
				lastmenu = menuPage;
				displayReset();
				displaySetCursor(2, 0); display.print("4 - P. POS");
				displaySetCursor(3, 0); sprintf(strPRT, "LAT:%c %s", Data.lat, dtostrf(Data.latitude, 9, 4, strtmp)); display.print(strPRT);
				displaySetCursor(4, 0); sprintf(strPRT, "LON:%c %s", Data.lon, dtostrf(Data.longitude, 9, 4, strtmp)); display.print(strPRT);
				displaySetCursor(5, 0); sprintf(strPRT, "GOOGLE COORD:"); display.print(strPRT);
				displaySetCursor(6, 0); sprintf(strPRT, "%s", dtostrf(Data.latitudedeg, 10, 6, strtmp)); display.print(strPRT);
				displaySetCursor(7, 0); sprintf(strPRT, "%s", dtostrf(Data.longitudedeg, 10, 6, strtmp)); display.print(strPRT);
			}
			else
			{
				displaySetCursor(3, 4); sprintf(strPRT, "%c %s", Data.lat, dtostrf(Data.latitude, 9, 4, strtmp)); display.print(strPRT);
				displaySetCursor(4, 4); sprintf(strPRT, "%c %s", Data.lon, dtostrf(Data.longitude, 9, 4, strtmp)); display.print(strPRT);
				displaySetCursor(6, 0); sprintf(strPRT, "%s", dtostrf(Data.latitudedeg, 10, 6, strtmp)); display.print(strPRT);
				displaySetCursor(7, 0); sprintf(strPRT, "%s", dtostrf(Data.longitudedeg, 10, 6, strtmp)); display.print(strPRT);
			}
			break;
		}

		case 5: //RECOVERY MENU
		{
			if (lastmenu != menuPage || forceRepaint)
			{
				lastmenu = menuPage;
				displayReset();
				displaySetCursor(2, 0); display.print("5 - RECOVERY");
				displaySetCursor(3, 0); display.print("GOOGLE COORD:");
				displaySetCursor(4, 0); sprintf(strPRT, "%s", dtostrf(Data.latitudedeg, 9, 6, strtmp)); display.print(strPRT);
				displaySetCursor(5, 0); sprintf(strPRT, "%s", dtostrf(Data.longitudedeg, 9, 6, strtmp)); display.print(strPRT);
				displaySetCursor(6, 0); sprintf(strPRT, "AZM:%ld", homeazim); display.print(strPRT);
				displaySetCursor(7, 0); sprintf(strPRT, "DIS:%ld", homedist); display.print(strPRT);
				if (kmflag == 0) display.print("m"); else display.print("Km");
			}
			else
			{
				displaySetCursor(4, 0); sprintf(strPRT, "%s", dtostrf(Data.latitudedeg, 9, 6, strtmp)); display.print(strPRT);
				displaySetCursor(5, 0); sprintf(strPRT, "%s", dtostrf(Data.longitudedeg, 9, 6, strtmp)); display.print(strPRT);
				displaySetCursor(6, 4); sprintf(strPRT, "%ld", homeazim); display.print(strPRT);
				displaySetCursor(7, 4); sprintf(strPRT, "%ld", homedist); display.print(strPRT);
				if (kmflag == 0) display.print("m"); else display.print("Km");
			}
			break;
		}

		case 6: //TOOLS MENU
		{
			if (lastmenu != menuPage || forceRepaint)
			{
				lastmenu = menuPage;

				displayReset();
				displaySetCursor(2, 0); display.print("6 - TOOLS");
				displaySetCursor(3, 0); display.print("Curr. Ver.:");
				displaySetCursor(4, 0); sprintf(strPRT, "%s", VERSION); display.print(strPRT);
			}
			break;
		}

		case 7: //LOG MEN
		{
			if (lastmenu != menuPage || forceRepaint)
			{
				lastmenu = menuPage;

				displayReset();
				displaySetCursor(2, 0); display.print("7 - LOGGER");
				displaySetCursor(3, 0); sprintf(strPRT, "#Records:%lu", mylog.numRecords); display.print(strPRT);
				displaySetCursor(4, 0); sprintf(strPRT, "press B2 to erase!"); display.print(strPRT);
			}
			else
			{
				displaySetCursor(3, 9); sprintf(strPRT, "%lu", mylog.numRecords); display.print(strPRT);
			}
			break;
		}
		case 8: //LOG dump
		{
			if (lastmenu != menuPage || forceRepaint)
			{
				lastmenu = menuPage;

				displayReset();
				displaySetCursor(2, 0); display.print("8 - Log Dump");
				displaySetCursor(3, 0); sprintf(strPRT, "connect to PC %ld", mylog.numRecords); display.print(strPRT);
				displaySetCursor(4, 0); sprintf(strPRT, "press B2 to dump!"); display.print(strPRT);
			}
			break;
		}
	}
	#ifdef LCD
		display.display();
	#endif
	return;
}

void displaywarning(int warningcode)
{
	displaySetCursor(SCRLINES, 0);
	display.setTextColor(WHITE, BLACK);
	display.print(fill(strtmp, SCRCHARS, ' ', true));
	displaySetCursor(SCRLINES, 0);
	if (warningcode & WRN_LINK) //link lost
	{
		#ifdef DEBUG
			Serial.print F("link Lost");
		#endif

		display.setTextColor(WHITE, RED);
		display.print(" LNK ");
	}
	if (warningcode & WRN_FIX)  // GPS fix lost
	{
		#ifdef DEBUG
			Serial.println F("GPS fix Lost");
		#endif

		display.setTextColor(BLACK, ORANGE);
		display.print(" GPS ");

	}
	displaySetCursor(0, 0);
	display.setTextColor(WHITE, BLACK);

	#ifdef LCD
		display.display();
	#endif
}


/*
*  utility function for string ( char *) manipulation
*/

int len(char *str)
{
	int i = 0;
	while (str[i++] != 0);
	return i - 1;
}

char* fill(char* str, int length, char charcode, bool initialize)
{
	int i = 0;
	if (!initialize) i = len(str);  // start from were str is already filled
	while (i<length)
	{
		str[i++] = charcode;
	}
	str[i] = NULL;
	return str;
}

// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect)
{
		GPS.read();
}
