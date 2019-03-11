/*
* configfile.h
*
*  Created on: 1 May 2017
*      Author: ArieL
*/

#ifndef SENSORS_H_
#define SENSORS_H_

// Define sensors GPIOs
#define CONFIGURATION_MODE_BUTTON 13 // Pin which define the button that sets the device to configuration mode
#define LED_WORK_STATUS 2

//Set DHT type and input pin
#define DHT_TYPE DHT22
#define DHT_PIN 4 //Set IO pin 4 for DHT sensor

#include "DHT.h"

extern DHT dht;

extern String deviceType; // The devices type definition
extern String deviceId; // The devices unique id
extern String chipType; // The devices chip type

void sendDataToSensor(const char *topic, byte *payload);

void collectAndPublishData();

// Declare and set default values
extern int mainHeaterStartTemprature;
extern int waterCoolerStartTemprature;
extern int airCoolerStartTemprature;
extern int toBarrelSisposalStartTemprature;

extern bool wasGarbageDisposalExecuted;
extern bool garbageDisposalMadeTimeSnapshot;
extern bool barrelDisposalMadeTimeSnapshot;
extern bool wasBarrelDisposalExecuted;
extern bool isErrorOccured;

void initSensor();

void getDataFromSensor();

void getSensorsInformation();

#endif /* SENSORS_H_ */
