//
// Created by Arie Lev on 15/02/2019.
//

#ifndef NALKINSCLOUD_ESP8266_HTTP_SERVER_HANDLER_H
#define NALKINSCLOUD_ESP8266_HTTP_SERVER_HANDLER_H

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include "configs.h"
#include "eeprom_memory.h"
#include "http_server_content.h"
#include "wifi_handler.h"
#include "sensors.h"

extern MDNSResponder mdns;
extern ESP8266WebServer server(80);

void startHTTPServer();

#endif //NALKINSCLOUD_ESP8266_HTTP_SERVER_HANDLER_H
