
#include "radio.h"

void radioSetup(){

  // initialize Radio
	if (manager.init()){
		driver.setFrequency(434);
	}
	else {
		Serial.println F("init failed");
		// Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
	}

}

recv_t radioLoop(){

  // check radio reception and process if data is available
	if (timerLink > millis()) timerLink = millis();
	if (manager.available()){ //If some packet was received by the radio, wait for all its contents to come trough

		warningLevel = setflag(warningLevel, WRN_LINK, FLAGRESET); // clear Link flag

		uint8_t len = sizeof(Data);
		uint8_t from;

		if (manager.recvfromAck((uint8_t *)&Data, &len, &from)){
			timerLink = millis(); //Set a counter for data link loss timeout calculation
			rssi = driver.lastRssi();	//RSSI;
		}
    return dataRecv;
	}
	else  // if no data received
    return dataNotRecv;



}
