//
// Created by Arie Lev on 15/02/2019.
//

#ifndef NALKINSCLOUD_ESP8266_LED_BLINKS_H
#define NALKINSCLOUD_ESP8266_LED_BLINKS_H

#include <Arduino.h>

class LedBlinks {
public:
	explicit LedBlinks(uint8_t _ledPin);

	void intervalBlink(long interval);
	void rapidIntervalBlink(long interval);

private:
	uint8_t ledPin;
	unsigned long blinkPreviousStart;
	uint8_t ledState;
};

#endif //NALKINSCLOUD_ESP8266_LED_BLINKS_H
