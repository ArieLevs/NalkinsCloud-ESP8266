//
// Created by Arie Lev on 2019-04-07.
//

#include "buzzer.h"

void Buzzer::executeBuzzer() {
	unsigned long currentMillis = millis();
	if (currentMillis - buzzerPreviousStart >= buzzerInterval) {
		buzzerPreviousStart = currentMillis;
		if (buzzerIsOn) {
			noTone(buzzerPin);
			buzzerIsOn = false;
		} else {
			tone(buzzerPin, buzzerFrequency);
			buzzerIsOn = true;
		}
	}
}

void Buzzer::stopBuzzer() {
	noTone(buzzerPin);
	buzzerIsOn = false;
}

Buzzer::Buzzer(uint8_t _buzzerPin, int _buzzerFrequency, const long _buzzerInterval) {
	buzzerPin = _buzzerPin;
	buzzerFrequency = _buzzerFrequency;

	buzzerPreviousStart = 0;
	buzzerInterval = _buzzerInterval;

	buzzerIsOn = false;
}
