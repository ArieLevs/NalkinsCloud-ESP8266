/*
* configfile.h
*
*  Created on: 1 May 2017
*      Author: ArieL
*/

#ifndef SENSORS_H_
#define SENSORS_H_

// Define sensors GPIOs
#define CONFIGURATIONMODEBUTTON 13 // Pin which define the button that sets the device to configuration mode
#define LED_WORK_STATUS 12

//Set DHT type and input pin
#define DHTTYPE DHT22
#define DHTPIN 4 // Set IO pin 4 for DHT sensor

#include "DHT.h" // Load DHT library

extern DHT dht; // 11 works fine for ESP8266

extern String deviceType; // The devices type definition
extern String deviceId; // The devices unique id
extern String chipType; // The devices chip type

void sendDataToSensor(const char* topic, byte* payload);
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
