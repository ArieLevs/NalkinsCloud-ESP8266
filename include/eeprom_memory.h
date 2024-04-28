//
// Created by Arie Lev on 15/02/2019.
//

#ifndef NALKINSCLOUD_ESP8266_EEPROM_MEMORY_H
#define NALKINSCLOUD_ESP8266_EEPROM_MEMORY_H

#include "Arduino.h"
#include <HardwareSerial.h>
#include "EEPROM.h"
#include "global_configs.h"

/*
* EEPROM data storage allocation
*
* WiFi
* Bits 	0-3 	belong to SSID Length info (4 bytes)
* Bits 	4-35  	belong to SSID info (32 bytes)
* Bits 	36-39 	belong to wifi password Length info (4 bytes)
* Bits 	40-71 	belong to wifi password info (32 bytes)
*
* MQTT Broker Info
* Bits 	200 	belong to MQTT server name length (1 bytes)
* Bits 	201-264 belong to MQTT server address (64 bytes)
* Bits 	265     belong to MQTT server port (1 bytes)
*
* Device Info
* Bits 	300     belong to DeviceId length (1 bytes)
* Bits 	301-364 belong to DeviceId (64 bytes)
* Bits 	365     belong to Device password length (1 bytes)
* Bits 	366-397 belong to Device password (32 bytes)
*
* Static Network values
* All addresses between 400 - 411 reserved for static network info - DO NOT USE THIS RANGE
*
# Sensor specific Info
* All addresses between 412 - 499 reserved for sensor info storage - DO NOT USE THIS RANGE
*
* Runtime Info
* Bit   500 	hold data if the device is on configuration mode
* Bit 	501 	sets service mode flag (value 0 = public MQTT server, value 1 = local server)
* Bit 	502 	sets DHCP status
* Bit 	503 	sets version update mode (value 0 = not on update mode, value 1 = device on version update mode)
*
*/
#define SSID_LENGTH_START_ADDR 0
#define SSID_START_ADDR 4
#define WIFI_PASS_LENGTH_START_ADDR 36
#define WIFI_PASS_START_ADDR 40

#define MQTT_SERVER_LENGTH_START_ADDR 200 // Int writable variable (1 bytes)
#define MQTT_SERVER_START_ADDR 201
#define MQTT_SERVER_PORT_START_ADDR 265 // Int writable variable (1 bytes)

#define DEVICE_ID_LENGTH_START_ADDR 300
#define DEVICE_ID_START_ADDR 301
#define DEVICE_PASS_LENGTH_START_ADDR 365
#define DEVICE_PASS_START_ADDR 366

// Int writable variable
#define CONFIGURATION_MODE_ADDR 500
#define SERVICE_MODE_FLAG_ADDR 501
#define DHCP_FLAG_ADDR 502
#define VER_UPDATE_FLAG_ADDR 503

// defines the max allowed storage side for a single key
#define MAX_EEPROM_DATA_SIZE 64

void initEEPROM();

void writeNetworkConfigs();

void readNetworkConfigs();

String readStringFromEEPROM(int lengthStartAddress, int startAddress);

void setConfigurationStatusFlag(uint8_t configurationStatus);

bool getConfigurationStatusFlag();

void writeStringToEEPROM(int lengthStartAddress, int startAddress, String data);

bool checkRange(String value);

void writeIntToEEPROM(int startAddress, int value);

int readIntFromEEPROM(int address);

void clearEEPROM();

#endif //NALKINSCLOUD_ESP8266_EEPROM_MEMORY_H
