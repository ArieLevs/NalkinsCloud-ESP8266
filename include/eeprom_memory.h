//
// Created by Arie Lev on 15/02/2019.
//

#ifndef NALKINSCLOUD_ESP8266_EEEPROM_MEMORY_H
#define NALKINSCLOUD_ESP8266_EEEPROM_MEMORY_H

#include "Arduino.h"


void initEEPROM();
void writeNetworkConfigs();
void readNetworkConfigs();
String readStringFromEEPROM(int beginAddress);
void setConfigurationStatusFlag(const uint8_t configurationStatus);
bool getConfigurationStatusFlag();
void writeStringToEEPROM(int startAddress, String value);
bool checkRange(String value);

#endif //NALKINSCLOUD_ESP8266_EEEPROM_MEMORY_H
