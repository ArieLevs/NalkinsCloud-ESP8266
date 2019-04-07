//
// Created by Arie Lev on 2019-04-07.
//

#ifndef NALKINSCLOUD_ESP8266_BUZZER_H
#define NALKINSCLOUD_ESP8266_BUZZER_H

#include "Arduino.h"

class Buzzer {
public:
	Buzzer(uint8_t buzzerPin, int buzzerFrequency, long _buzzerInterval);

	void executeBuzzer();
	void stopBuzzer();

private:
	uint8_t buzzerPin;
	int buzzerFrequency;
	unsigned long buzzerPreviousStart;
	long buzzerInterval;
	bool buzzerIsOn;
};

#endif //NALKINSCLOUD_ESP8266_BUZZER_H
