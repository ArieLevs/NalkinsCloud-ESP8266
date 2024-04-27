/*
* configfile.h
*
*  Created on: 24 Feb 2017
*      Author: ArieL
*/

#ifndef NALKINSCLOUD_ESP8266_GLOBAL_CONFIGS_H_
#define NALKINSCLOUD_ESP8266_GLOBAL_CONFIGS_H_

#include "Arduino.h"

// SSID Name and Password, used when device is on Access Point mode
#define ACCESS_POINT_SSID "ESP8266"
#define ACCESS_POINT_PASSWORD "nalkinscloud"

extern bool DEBUG; //Set serial prints

extern bool isSslEncrypted;
extern bool isConfigurationMode; // Runtime var to store work mode status

// MQTT server configurations
const char fingerprint[] PROGMEM = "24 4A 4D E8 D3 5B EE 8A CC C6 26 3C 22 6F 54 2C 59 02 1F F4"; // SHA1 - 60 Bytes

extern int QOS;
extern bool retained; //Set retained messages to true
extern bool notRetained; //Set retained messages to false

// DNS name to access device on STA mode
extern String DNSName;

struct strConfigs {
	String wifiSsid;
	String wifiPassword;
	bool dhcp;
	uint8_t IP[4];
	uint8_t Netmask[4];
	uint8_t Gateway[4];
	String devicePassword;
	String deviceType;
	String chipType;
	String deviceId;
	String deviceVersion;
	String mqttServer;
	uint16_t mqttPort;
	String mqttFingerprint;
};

extern strConfigs configs;

void initConfigs();

bool getConfigurationMode();

void setConfigurationMode(bool value);

#endif /* NALKINSCLOUD_ESP8266_GLOBAL_CONFIGS_H_ */



