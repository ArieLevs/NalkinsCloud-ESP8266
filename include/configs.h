/*
* configfile.h
*
*  Created on: 24 Feb 2017
*      Author: ArieL
*/

#ifndef CONFIGS_H_
#define CONFIGS_H_

#include "Arduino.h"

#define DEBUG 1 //Set serial prints

#define SSID_START_ADDR 0
#define WIFI_PASS_START_ADDR 32
#define MQTT_SERVER_START_ADDR 64
#define USER_NAME_START_ADDR 96
#define DEVICE_PASS_START_ADDR 128
#define MQTT_FINGERPRINT_START_ADDR 256
#define CONFIGURATION_MODE_ADDR 494
#define MQTT_PORT_START_ADDR 495
#define SERVICE_MODE_FLAG_ADDR 500
#define DHCP_FLAG_ADDR 501

// SSID Name and Password, used when device is on Access Point mode
#define ACCESS_POINT_SSID "ESP8266"
#define ACCESS_POINT_PASSWORD "nalkinscloud"

extern bool isConfigurationMode; // Runtime var to store work mode status

// MQTT server configurations
extern String fingerprint; // "AA 33 9C 6F 71 BE D3 C9 47 5B D2 92 AA 13 AC D6 DC 21 DB D2"; Example SHA1 - 60 Bytes

extern String generalTopic;
extern int QOS;
extern bool retained; //Set retained messages to true
extern bool notRetained; //Set retained messages to false

// DNS name to access device on STA mode
extern String DNSName;
extern String versionNum;

struct strConfigs {
	String wifiSsid;
	String wifiPassword;
	bool dhcp;
	uint8_t IP[4];
	uint8_t Netmask[4];
	uint8_t Gateway[4];
	String devicePassword;
	String clientUsername;
	String mqttServer;
	uint16_t mqttPort;
	String mqttFingerprint;
};

extern strConfigs configs;

void initConfigs();

void setConfigurationMode(bool value);

bool getConfigurationMode();

#endif /* CONFIGS_H_ */



