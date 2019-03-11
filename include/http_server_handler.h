//
// Created by Arie Lev on 15/02/2019.
//

#ifndef NALKINSCLOUD_ESP8266_HTTP_SERVER_HANDLER_H
#define NALKINSCLOUD_ESP8266_HTTP_SERVER_HANDLER_H

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

extern ESP8266WebServer server;

void handleClient();

void startHTTPServer();

#endif //NALKINSCLOUD_ESP8266_HTTP_SERVER_HANDLER_H
