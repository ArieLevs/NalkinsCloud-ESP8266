//
// Created by Arie Lev on 15/02/2019.
//

#ifndef NALKINSCLOUD_ESP8266_MQTT_CLIENT_H
#define NALKINSCLOUD_ESP8266_MQTT_CLIENT_H

#include <PubSubClient.h>

extern PubSubClient mqttClient;

void callback(const char* topic, byte* payload, unsigned int length);
bool connectToMQTTBroker();
bool checkMQTTSSL();
void disconnectFromMQTTBroker();
bool checkMQTTconnection();
bool isClientConectedToMQTTServer();
void subscribeToMQTTBroker(const char* topic);
bool publishMessageToMQTTBroker(const char* topic, char* message, bool isRetainedMessage);

#endif //NALKINSCLOUD_ESP8266_MQTT_CLIENT_H
