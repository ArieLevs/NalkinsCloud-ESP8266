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
#define BUZZER 14					 // GPIO14 -> D5
#define BUZZER_FREQUENCY 1000
// Define device GPIOs
#define CONFIGURATION_MODE_BUTTON D6 // Pin which define the button that sets the device to configuration mode

void sendDataToSensor(const char *topic, byte *payload, unsigned int length);

void initSensor();

void getDataFromSensor();

void getSensorsInformation();

void publishDataToServer(bool force);

#endif /* SENSORS_H_ */
