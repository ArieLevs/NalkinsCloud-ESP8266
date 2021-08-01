//
// Created by Arie Lev on 2019-02-16.
//

#include "Arduino.h"

#include "configs.h"

bool DEBUG;

bool isConfigurationMode; // Runtime var to store work mode status

// MQTT server configurations
bool isSslEncrypted;

int QOS;
bool retained; //Set retained messages to true
bool notRetained; //Set retained messages to false

// DNS name to access device on STA mode
String DNSName;
String versionNum;

void initConfigs() {
    DEBUG = true;
	isSslEncrypted = false;
	isConfigurationMode = true;
	// MQTT server configurations
	QOS = 1;
	retained = true; //Set retained messages to true
	notRetained = false; //Set retained messages to false
	// DNS name to access device on STA mode
	DNSName = "esp8266.local";
	versionNum = "1.0.0.0";
	configs = {};
}

bool getConfigurationMode() {
	return isConfigurationMode;
}

void setConfigurationMode(bool value) {
	isConfigurationMode = value;
}
