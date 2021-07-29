//
// Created by Arie Lev on 15/02/2019.
//

#ifndef NALKINSCLOUD_ESP8266_MQTT_CLIENT_H
#define NALKINSCLOUD_ESP8266_MQTT_CLIENT_H

#include <PubSubClient.h>
#include <WiFiClientSecure.h>

extern PubSubClient mqttClient;
extern BearSSL::WiFiClientSecure wifiClientSecure;
extern WiFiClient wifiClient;

void initMqttClient();

void callback(const char *topic, byte *payload, unsigned int length);

String printMqttConnectionStatus(int return_code);

bool connectToMQTTBroker();

void disconnectFromMQTTBroker();

bool checkMQTTConnection();

bool isClientConnectedToMQTTServer();

void subscribeToMQTTBroker(const char *topic);

bool publishMessageToMQTTBroker(const String& topic, const String& message, bool isRetainedMessage);

void sendWifiSignalStrength(const String& topic);

#endif //NALKINSCLOUD_ESP8266_MQTT_CLIENT_H
