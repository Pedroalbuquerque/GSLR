

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


//#include <arduino.h> // uncomment if using ATOM editor
#include <SPIFlash.h> //Arduino/Moteino library for read/write access to SPI flash memory chips. https://github.com/LowPowerLab/SPIFlash
#include <SPI.h> //Arduino native SPI library
#include <Adafruit_GFX.h> //Required for the Nokia 5110 LCD and ST7735 TFT display https://github.com/adafruit/Adafruit-GFX-Library
#include <FlashLogM.h> //To handle the Flash log https://github.com/Pedroalbuquerque/FlashLogM
#include <math.h> //Arduino native math library
#include <GPSMath.h> //To handle all GPS calculations https://github.com/Pedroalbuquerque/GPSMath
#include <Adafruit_GPS.h> //https://github.com/adafruit/Adafruit_GPS/archive/master.zip

#include "./config/all.h"

#include "menus.h"
#include "radio.h"

#include <avr/wdt.h>
#define Reset_AVR() wdt_enable(WDTO_1S); while(1) {} // Soft reset Arduino using watchdog


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



void debug_menu(){
  DEBUG_MSG("Navigate :%d\n",menuCursor.navigate);
  DEBUG_MSG("Menu:%d\t",menuCursor.menuIdx);
  DEBUG_MSG("Sub:%d\n",menuCursor.subIdx);
}

void dot(){ //uint32_t i, uint8_t value){
	display.print(".");
}

void setup(){

	// initialize Serial port for debugging
	Serial.begin(SERIAL_BAUD); //Initialize the Serial port at the specified baud rate
	Serial.println F("GPS AND TELEMETRY MODULE");
	Serial.println(VERSION);
	Serial.println F("Initializing...");

	Serial.print("Flash DeviceID:"); Serial.print(flash.readDeviceId());
	if(flash.readDeviceId() != 0xEF30){
    Serial.print("Soft Reset");
    Reset_AVR();
  }


	// initialize display
	displaySetup();

	// initialize Buzzer
	#ifdef BUZZER
		oldbuzztimer = 0; //Initialize buzzer timer variable
		pinMode(BUZZ, OUTPUT); //Make BUZZ pin an output
	#endif


	// ### Initialize Log

	// initialize log variables to start write and read
	delay(1000);
	display.print("Start - ");
	//mylog.initialize(Data,display);
	mylog.setCallback(dot);
	mylog.initialize(Data);
	Serial.println F("Log data initialized");
	Serial.print F("Log next write addr:"); Serial.println(mylog.nextWrite);
	Serial.print F("Log next read addr:"); Serial.println(mylog.nextRead);
	Serial.print F("Log # records saved:"); Serial.println(mylog.numRecords);
	display.print("End\n");

	// SPI initialization
	SPI.usingInterrupt(digitalPinToInterrupt(2));

	// initialize radio
	radioSetup();

	// initialize GPS
	GPS.begin(GPS_BAUD);
	// Enable interrupts on Timer0 for GPS
	// Timer0 is already used for millis() - we'll just interrupt somewhere
	// in the middle and call the "Compare A" function
	OCR0A = 0xAF;
	TIMSK0 |= _BV(OCIE0A);


	// load menu screen on LCD/TFT
	menuCursor.menuIdx = 0;
	displaymenu(menuCursor.menuIdx, true); //Start menu display (menu page number, screen refresh requirement)
	displayButtons();

	Serial.println F("Setup finished");

	timerLink = millis(); //Initialize Data link loss timeout timer variable



}

void loop()
{
	uint8_t button ;
	makeHeader();

	// read button to see if there is activity
	button = buttonPressed();
	if(button!=0){
		menuCursor.update(button);
		debug_menu();
		displaymenu(menuCursor.menuIdx, false);

	}

	//menuAction(button);

	if(radioLoop() == dataRecv){

		telemetryUpdate();

		// save data just received to Log memory
	  mylog.saveData(Data);
	  #ifdef GOOGLEMAPS
	    sendToGoogle(Data);
	  #endif

		displaymenu(menuCursor.menuIdx, false); // update menu info (menu page number, screen refresh)


	}
	else{ // if no data received

		// check if link timeout, ie, no data received for more than 5 sec
		if (millis() > (timerLink + 5000)){ //If LINK lost for more than 5 sec...

			warningLevel = setflag(warningLevel, WRN_LINK, FLAGSET);  // set LINK flag
			#ifdef BUZZER
				newbuzztimer = millis();
				if (newbuzztimer > (oldbuzztimer + 2000)){
					oldbuzztimer = newbuzztimer;
					Blink(BUZZ, 5);
					delay(100);
					Blink(BUZZ, 5);
				}
			#endif
		}
	}

	// check Warnings
	if (millis() < timerWarning) timerWarning = millis(); // if millis() wrap around reinitialize timer
	if (millis() > timerWarning + 2000){
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



uint8_t setflag(uint8_t flagContainer, uint8_t flag, bool set)
{
	if (set) return (flagContainer | flag); else return (flagContainer & ~flag);
}


void changeMenu(){
	//  Blink(BUZZ, 50);
	menuPage = menuPage + 1;
	if (menuPage > MPAGES)
	{
		menuPage = 1;;
	}
}

#ifdef BUZZER
void Blink(byte PIN, int DELAY_MS) {//The BUZZ Blinking function

	digitalWrite(PIN, HIGH);
	delay(DELAY_MS);
	digitalWrite(PIN, LOW);
}
#endif


void fixposition(){
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
	str[i] = 0;
	return str;
}

// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect)
{
		GPS.read();
}
