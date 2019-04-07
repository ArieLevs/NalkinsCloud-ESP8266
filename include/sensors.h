/*
* configfile.h
*
*  Created on: 1 May 2017
*      Author: ArieL
*/

#ifndef SENSORS_H_
#define SENSORS_H_

#include "buzzer.h"

// Define sensors GPIOs
#define CONFIGURATION_MODE_BUTTON 13 // GPIO13 -> D7, Pin which define the button that sets the device to configuration mode
#define BUZZER 14					 // GPIO14 -> D5
#define BUZZER_FREQUENCY 1000
#define LED_WORK_STATUS 2			 // GPIO2 -> D4

extern String deviceType; // The devices type definition
extern String deviceId; // The devices unique id
extern String chipType; // The devices chip type

void sendDataToSensor(const char *topic, byte *payload);

void initSensor();

void getDataFromSensor();

void getSensorsInformation();

void publishDataToServer();

#endif /* SENSORS_H_ */
