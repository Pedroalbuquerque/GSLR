// **************************************************
// configure your hardware using define directives
//**************************************************

//#define DEBUG //Uncomment to activate Serial Monitor Debug

#define ECHO_PORT Serial
#define DEBUG_PORT Serial



//************************* DEFINITIONS ****************************


#define VERSION "GS LR MEGA V1.3.0"
#define FREQUENCY 434 //Match with the correct radio frequency of the other radio
#define SERIAL_BAUD 115200 //To communicate with serial monitor for debug
#define BUTPIN1 12 //Analog pin assigned to FIX button
#define BUTPIN2 13 //Analog pin assigned to MENUS SCROLL button
#define MPAGES 8 //Number of menu pages
#define GOOGLEMAPS //Uncomment to have Google info sent trough the Serial port on menu 2

// *****  Define  remote and ground stations IDs  ****

#define RM_STATION_ID 2
#define GR_STATION_ID 1


// *****   Define type of LCD to use *****


//#define TFT_TOUCH_9341
//#define TFT_ILI9340 //Uncomment to use Adafruit 2.2" TFT display
//#define LCD // uncomment to use NOKIA LCD display
#define TFT_ST7735 //Uncomment if you use the Seed Studio TFT 1.8"

// **** Button type *****
// if display has touchscreen comment all

#define SERIALBUTTON
//#define PUSHBUTTON

#define btnUP	1
#define btnDOWN 2
#define btnSELECT 3

// **** define if a BUZZER exist to give sound feedback

#define BUZZER // Comment if a buzzer is installed

#ifdef BUZZER
	#define BUZZ 14 //Buzzer output pin
#endif





#ifdef __AVR_ATmega1284P__
	#define LED           15 // Moteino MEGAs have LEDs on D15
#else
	#define LED           9 // Moteinos have LEDs on D9
#endif
