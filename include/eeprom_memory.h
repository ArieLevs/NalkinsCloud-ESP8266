//
// Created by Arie Lev on 15/02/2019.
//

#ifndef NALKINSCLOUD_ESP8266_EEPROM_MEMORY_H
#define NALKINSCLOUD_ESP8266_EEPROM_MEMORY_H

#include "Arduino.h"

/*
*
* EEPROM Section
* Bits 	0-31 	belong to SSID info
* Bits 	32-63 	belong to wifi password info
* Bits 	64-95 	belong to MQTT server name
* Bits 	96-127 	belong to Customer user name
* Bits 	128-159 belong to Device password
* Bit 	494 	hold data if the device is on configuration mode
* Bits 	495-499 belong to MQTT server port
* Bits 	256-384 belong to MQTT server port // 128bit
* Bit 	500 	sets service mode flag (value 0 = public MQTT server, value 1 = local server)
* Bit 	501 	sets DHCP status
* Bit 	502 	sets version update mode (value 0 = not on update mode, value 1 = device on version update mode)
* Bits 	503-510 sets Device password
*
*/

void initEEPROM();

void writeNetworkConfigs();

void readNetworkConfigs();

String readStringFromEEPROM(int beginAddress);

void setConfigurationStatusFlag(uint8_t configurationStatus);

bool getConfigurationStatusFlag();

void writeStringToEEPROM(int startAddress, String value);

bool checkRange(String value);

#endif //NALKINSCLOUD_ESP8266_EEPROM_MEMORY_H
