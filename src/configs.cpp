//
// Created by Arie Lev on 2019-02-16.
//

#include "Arduino.h"

#include "configs.h"

bool isConfigurationMode; // Runtime var to store work mode status

// MQTT server configurations
String fingerprint; // "AA 33 9C 6F 71 BE D3 C9 47 5B D2 92 AA 13 AC D6 DC 21 DB D2"; Example SHA1 - 60 Bytes

String generalTopic;
int QOS;
bool retained; //Set retained messages to true
bool notRetained; //Set retained messages to false

// DNS name to access device on STA mode
String DNSName;
String versionNum;

void initConfigs() {
	isConfigurationMode = true;

	// MQTT server configurations
	fingerprint = ""; // "AA 33 9C 6F 71 BE D3 C9 47 5B D2 92 AA 13 AC D6 DC 21 DB D2"; Example SHA1 - 60 Bytes

	QOS = 1;

	retained = true; //Set retained messages to true
	notRetained = false; //Set retained messages to false

	// DNS name to access device on STA mode
	DNSName = "efs8266.local";
	versionNum = "1.0.0.0";

	strConfigs configs = {};
};

bool getConfigurationMode() {
	return isConfigurationMode;
}

void setConfigurationMode(bool value) {
	isConfigurationMode = value;
}