/*
* configfile.h
*
*  Created on: 24 Feb 2017
*      Author: ArieL
*/

#ifndef CONFIGFILE_H_
#define CONFIGFILE_H_

#define DEBUG 1 //Set serial prints

#define SSIDSTARTSTARTADDR 0
#define WIFIPASSSTARTADDR 32
#define MQTTSERVERSTARTADDR 64
#define USERNAMESTARTADDR 96
#define DEVICEPASSSTARTADDR 128
#define MQTTFINGERPRINTSTARTADDR 256
#define MQTTPORTSTARTADDR 495
#define SERVICEMODEFLAG 500
#define DHCPFLAG 501
#define CONFIGURATIONMODEADDR 494

// SSID Name and Password, used when device is on Access Point mode
#define ACCESS_POINT_SSID "ESP8266"
#define ACCESS_POINT_PASSWORD "nalkinscloud"

extern boolean isConfigurationMode; // Runtime var to store work mode status
 
// MQTT server configurations
const char* fingerprint = ""; // "AA 33 9C 6F 71 BE D3 C9 47 5B D2 92 AA 13 AC D6 DC 21 DB D2"; Example SHA1 - 60 Bytes

String generalTopic;
int QOS = 1;
boolean retained = true; //Set retained messages to true
boolean notRetained = false; //Set retained messages to false

// DNS name to access device on STA mode
String DNSName = "efs8266.local";
const String versionNum = "1.0.0.0";

struct strConfigs {
  String wifiSsid;
  String wifiPassword;
  boolean dhcp;
  byte  IP[4];
  byte  Netmask[4];
  byte  Gateway[4];
  String devicePassword;
  //String clientUsername;
  String mqttServer;
  String mqttPort;
  //String mqttFingerprint;
} configs;

#endif /* CONFIGFILE_H_ */



