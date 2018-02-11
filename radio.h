#ifndef RADIO_H
#define RADIO_H

#include <RHReliableDatagram.h> //http://www.airspayce.com/mikem/arduino/RadioHead/RadioHead-1.61.zip
#include <RH_RF95.h> //Required for the LORA Radios http://www.airspayce.com/mikem/arduino/RadioHead/

enum recv_t { dataNotRecv, dataRecv, };

//Initialize the generic radio driver instance
RH_RF95 driver;

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, GR_STATION_ID);


#endif
