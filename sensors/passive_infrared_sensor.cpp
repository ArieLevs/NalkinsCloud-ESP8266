//
// Created by Arie Lev on 2019-04-27.
//

#include "passive_infrared_sensor.h"

bool PIR::isMotionDetected() {
	pirState = digitalRead(pirPin); // Read PIR status
	return pirState == HIGH;
}

PIR::PIR(uint8_t _pirPin) {
	pirPin = _pirPin;
	pirState = 0;

	pinMode(pirPin, INPUT);
}
