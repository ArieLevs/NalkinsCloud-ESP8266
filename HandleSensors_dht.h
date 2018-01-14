/*
* configfile.h
*
*  Created on: 1 May 2017
*      Author: ArieL
*/

#ifndef HANDLESENSORS_DHT_H_
#define HANDLESENSORS_DHT_H_

// Define sensors GPIOs
#define CONFIGURATIONMODEBUTTON 13 // Pin which define the button that sets the device to configuration mode
#define LED_WORK_STATUS 12
#define LED_IO_ 14 //LED_BUILTIN for onboard

//Set DHT type and input pin
#define DHTTYPE DHT22
#define DHTPIN 4 // Set IO pin 4 for DHT sensor

#include <DHT.h> // Load DHT library

DHT dht(DHTPIN, DHTTYPE, 11); // 11 works fine for ESP8266

String deviceType = "dht"; // The devices type definition
String deviceId = "test_dht_deviceid"; // The devices unique id
const String chipType = "ESP8266"; // The devices chip type

#endif /* HANDLESENSORS_DHT_H_ */
