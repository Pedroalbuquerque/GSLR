#ifndef GLOBALVARS_H
#define GLOBALVARS_H

//  *** Warning Flags

#define WRN_LINK	1	//b 00000001
#define WRN_FIX		2	//b 00000010
#define WRN_FENCE	4	// ...not yet implemented

#define FLAGSET true
#define FLAGRESET false



// variables setup
char input = 0;

#ifdef BUZZER
	unsigned long int newbuzztimer;
	unsigned long int oldbuzztimer;
#endif

int rssi; //variable to hold the Radio Signal Strength Indicator
int level = 0;
bool fixinMem = 0; //variable holding fix in memory condition
bool kmflag = 0; //if distance is > 1000m, display in Km
bool kmflagmem = 0; //hold the previous kmflag status
//byte menuPage = 1; //hold the actual page number on the menu
long int homeazim = 0; //variable to hold azimuth from GPS position to home
long int homealt = 0; //Variable to hold Home altitude (FIX)
long int maxalt = 0; //variable to hold max Altitude
long int homedist = 0; //variable to hold distance to home always in mt
long int maxdist = 0; //variable to hold max distance to home always in mt
float homelat = 0; //variable that will hold the FIX latitude
float homelon = 0; //variable that will hold the FIX longitude
float latmem = 0; //variable that will hold the last latitude memorized
float longmem = 0; //variable that will hold the last longitude memorized
float highspeed = 0; //variable that will hold the highest speed achieved initialized to 0
long major1, minor1;
long major2, minor2;
long longloglat;
long longloglong;
uint8_t oldseconds = 0;
char gpsstr1[80];
char gpsstr2[80];
const byte buff_size = 80; //buffer size must be a constant variable
char buffer[buff_size];
byte index = 0;   //declare all variables that will hold numbers less than '255' as 'byte' data type, because they require only '1-byte' of memory ('int' uses 2-bytes).
byte start_with = 0;
byte end_with = 0;
byte CRC = 0;
boolean data_end = false; //Here we will keep track of EOT (End Of Transmission).




// Menu navigation and Warning messages vars and constants

uint8_t warningLevel = 0;	//warning messages are limited to overlap only some menu screens
																										// while warning level != 0 there should be a warning on screen
                                                    // Timers
unsigned long int timerLink;	//for data loss timeout calculation
unsigned long int timerWarning = 0; //for warning display @ 2000ms intervals

																																				// General purpose auxiliary vars
char strPRT[100]; // to support any print command with sprintf
char strtmp[40];  // to support float to string conversion or other string manipulation


#endif
