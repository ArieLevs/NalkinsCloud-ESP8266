/*
* HandleSensors_magnet.h
*
*  Created on: 1 May 2017
*      Author: ArieL
*/

#ifndef HANDLE_SENSORS_MAGNET_H_
#define HANDLE_SENSORS_MAGNET_H_

// Define sensors GPIOs
#define MAGNET_INPUT 4
#define BUZZER 5
#define BUZZER_FREQUENCY 1000
#define CONFIGURATION_MODE_BUTTON 13 // Pin which define the button that sets the device to configuration mode
#define LED_WORK_STATUS 12
#define LED_IO 14 //LED_BUILTIN for on_board

#define RECEIVE_STATUS "set_lock_status"
#define SEND_STATUS "current_status"
#define RELEASE_ALARM "release_alarm"
#define ALARM "alarm"

String deviceType = "magnet"; // The devices type definition
String deviceId = "test_device_id2"; // The devices unique id
const String chipType = "ESP8266"; // The devices chip type

//Set time to "delay" a publish message
unsigned long previousMagnetCheck = 0;
const long magnetCheckInterval = 3000; // interval at which to send message (milliseconds)

boolean isTriggered = false;
boolean firstStatePublish = true;
int lastMagnetState = 0;
boolean isAlarmSent = false;
boolean isAlarmOn = false;

#endif /* HANDLE_SENSORS_MAGNET_H_ */

