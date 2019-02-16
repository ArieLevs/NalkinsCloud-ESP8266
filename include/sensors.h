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

DHT dht(DHTPIN, DHTTYPE, 11); // 11 works fine for ESP8266

//String deviceType = "dht"; // The devices type definition
//String deviceId = "test_dht_deviceid"; // The devices unique id
//const String chipType = "ESP8266"; // The devices chip type

void sendDataToSensor(const char* topic, byte* payload);
void collectAndPublishData();



// Define sensors GPIOs
#define CONFIGURATIONMODEBUTTON 13 // Pin which define the button that sets the device to configuration mode
// #define BUZZER 9
#define BUZZERFREQUENCY 1000
#define LED_WORK_STATUS 2 // Use GPIO02 only when DEBUG is false

#define MAIN_HEATER 1 // Main SSR IO pin
#define WATER_COLLER 3 // Watter Coller SSR IO pin
#define AIR_COLLER 5 // Air Coller SSR IO pin
#define GARBAGE_DISPOSAL 4 // To Garbage SSR IO pin
#define BARREL_DISPOSAL 0 // To Barrel SSR IO pin

String deviceType = "distillery"; // The devices type definition
String deviceId = "test_distillery_deviceid"; // The devices unique id
const String chipType = "ESP8266"; // The devices chip type

boolean isAutomatedJob = false; // Flag that store if device is on automation mode

// Set garbage / barrel logic variables
unsigned long garbageDisposalTimeSnapshot = 0; // Store current timestamp for garbage
unsigned long barrelDisposalTimeSnapshot = 0; // Store current timestamp for barrel
const long barrelAndGarbageDisposalStopExecute = 20000; // Stop garbage / barrel dispostal after (milliseconds)

/*
boolean wasAboveCoolerStartTemprature = false; // Idnicate if temp was above 60 C
boolean wasAboveDisposalEndTemprature = false; // Idnicate if temp was above 80 C
boolean wasAboveSecondarySSRStartTemprature = false; // Idnicate if temp was above 84 C
*/

// Declare and set default values
int mainHeaterStartTemprature = 94;
int waterCoolerStartTemprature = 60;
int airCoolerStartTemprature = 80;
int toBarrelSisposalStartTemprature = 80;

boolean wasGarbageDisposalExecuted = false;
boolean garbageDisposalMadeTimeSnapshot = false;
boolean barrelDisposalMadeTimeSnapshot = false;
boolean wasBarrelDisposalExecuted = false;
boolean isErrorOccured = false;

void setupSensorsGPIOs();
void getDataFromSensor();
void getSensorsInformation();

#endif /* SENSORS_H_ */
