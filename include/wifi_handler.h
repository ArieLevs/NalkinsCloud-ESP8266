//
// Created by Arie Lev on 15/02/2019.
//

#ifndef NALKINSCLOUD_ESP8266_WIFI_HANDLER_H
#define NALKINSCLOUD_ESP8266_WIFI_HANDLER_H

#include <ESP8266WiFi.h>

extern WiFiClientSecure wifiClientSecure;

bool isWifiConnected();
bool connectToWifi();
void setNormalOperetionMode();
void initializeWifiHandlers();
void setConfigurationMode();
void reconnectToWifi();
int getWifiSignalStrength();

#endif //NALKINSCLOUD_ESP8266_WIFI_HANDLER_H
