//
// Created by Arie Lev on 2019-04-27.
//

#ifndef NALKINSCLOUD_ESP8266_PASSIVE_INFRARED_SENSOR_H
#define NALKINSCLOUD_ESP8266_PASSIVE_INFRARED_SENSOR_H

#include <Arduino.h>

class PIR {
public:
	explicit PIR(uint8_t _pirPin);

	bool isMotionDetected();

private:
	uint8_t pirPin;
	int pirState;
};

#endif //NALKINSCLOUD_ESP8266_PASSIVE_INFRARED_SENSOR_H
