//
// Created by Arie Lev on 15/02/2019.
//

#ifndef NALKINSCLOUD_ESP8266_EEPROM_MEMORY_H
#define NALKINSCLOUD_ESP8266_EEPROM_MEMORY_H

#include "Arduino.h"

/*
* EEPROM data storage allocation
*
* WiFi
* Bits 	0-3 	belong to SSID Length info (4 bytes)
* Bits 	4-35  	belong to SSID info (32 bytes)
* Bits 	36-39 	belong to wifi password Length info (4 bytes)
* Bits 	40-71 	belong to wifi password info (32 bytes)
*
* MQTT
* Bits 	72-75 	belong to MQTT server name length (4 bytes)
* Bits 	76-107 	belong to MQTT server name (32 bytes)
* Bits 	108-111 belong to DeviceId length (4 bytes)
* Bits 	112-143 belong to DeviceId (32 bytes)
* Bits 	144-147 belong to Device password length (4 bytes)
* Bits 	148-179 belong to Device password (32 bytes)

* Bit 	494 	hold data if the device is on configuration mode
* Bits 	495-499 belong to MQTT server port
* Bits 	256-384 belong to MQTT server port // 128bit

* single bits data
* Bits	300-306 reserved for distillery sensor data
* - bit 301		hold if automation job executed
* - bit 302-305	hold temperature configs
* Bit 	500 	sets service mode flag (value 0 = public MQTT server, value 1 = local server)
* Bit 	501 	sets DHCP status
* Bit 	502 	sets version update mode (value 0 = not on update mode, value 1 = device on version update mode)
* Bits 	503-510 sets Device password
*
*/
#define SSID_LENGTH_START_ADDR 0
#define SSID_START_ADDR 4
#define WIFI_PASS_LENGTH_START_ADDR 36
#define WIFI_PASS_START_ADDR 40

#define MQTT_SERVER_LENGTH_START_ADDR 72
#define MQTT_SERVER_START_ADDR 76
#define DEVICE_ID_LENGTH_START_ADDR 108
#define DEVICE_ID_START_ADDR 112
#define DEVICE_PASS_LENGTH_START_ADDR 144
#define DEVICE_PASS_START_ADDR 148

// Int writable variable (4 bytes)
#define CONFIGURATION_MODE_ADDR 492
#define MQTT_PORT_START_ADDR 496
#define SERVICE_MODE_FLAG_ADDR 500
#define DHCP_FLAG_ADDR 504

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
