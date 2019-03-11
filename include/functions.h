//
// Created by Arie Lev on 15/02/2019.
//

#ifndef NALKINSCLOUD_ESP8266_FUNCTIONS_H
#define NALKINSCLOUD_ESP8266_FUNCTIONS_H

#include "configs.h"
#include "wifi_handler.h"
#include "eeprom_memory.h"

void checkConfigurationButton(uint8_t pinNum);

uint8_t *getMacAddress();

String macToStr(uint8_t *mac);

bool areCharArraysEqual(const char *s1, const char *s2);

#endif //NALKINSCLOUD_ESP8266_FUNCTIONS_H
