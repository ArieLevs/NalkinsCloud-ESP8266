/*
* configfile.h
*
*  Created on: 1 May 2017
*      Author: ArieL
*/

#ifndef HANDLE_SENSORS_SPECIAL_H_
#define HANDLE_SENSORS_SPECIAL_H_

// Define sensors GPIOs
#define CONFIGURATION_MODE_BUTTON 13 // Pin which define the button that sets the device to configuration mode
// #define BUZZER 9
#define BUZZER_FREQUENCY 1000
#define LED_WORK_STATUS 2 // Use GPIO02 only when DEBUG is false

#define MAIN_HEATER 1 // Main SSR IO pin 
#define WATER_COOLER 3 // Watter Cooler SSR IO pin
#define AIR_COOLER 5 // Air Cooler SSR IO pin
#define GARBAGE_DISPOSAL 4 // To Garbage SSR IO pin 
#define BARREL_DISPOSAL 0 // To Barrel SSR IO pin

String deviceType = "distillery"; // The devices type definition
String deviceId = "test_distillery_device_id"; // The devices unique id
const String chipType = "ESP8266"; // The devices chip type

boolean isAutomatedJob = false; // Flag that store if device is on automation mode

//Set time to "delay" a publish message
unsigned long previousPublish = 0;
const long publishInterval = 10000; // interval at which to send message (milliseconds)

// Set garbage / barrel logic variables
unsigned long garbageDisposalTimeSnapshot = 0; // Store current timestamp for garbage
unsigned long barrelDisposalTimeSnapshot = 0; // Store current timestamp for barrel
const long barrelAndGarbageDisposalStopExecute = 20000; // Stop garbage / barrel disposal after (milliseconds)

/*
boolean wasAboveCoolerStartTemperature = false; // Indicate if temp was above 60 C
boolean wasAboveDisposalEndTemperature = false; // Indicate if temp was above 80 C
boolean wasAboveSecondarySSRStartTemperature = false; // Indicate if temp was above 84 C
*/

// Declare and set default values
int mainHeaterStartTemperature = 94;
int waterCoolerStartTemperature = 60;
int airCoolerStartTemperature = 80;
int toBarrelDisposalStartTemperature = 80;

boolean wasGarbageDisposalExecuted = false;
boolean garbageDisposalMadeTimeSnapshot = false;
boolean barrelDisposalMadeTimeSnapshot = false;
boolean wasBarrelDisposalExecuted = false;
boolean isErrorOccurred = false;

void setupSensorsGPIOs();
void getDataFromSensor();
void getSensorsInformation();

#endif /* HANDLE_SENSORS_SPECIAL_H_ */
