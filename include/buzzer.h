//
// Created by Arie Lev on 2019-04-07.
//

#ifndef NALKINSCLOUD_ESP8266_BUZZER_H
#define NALKINSCLOUD_ESP8266_BUZZER_H

#include "Arduino.h"

/**
 * Implements the D1 mini buzzer shield https://www.wemos.cc/en/latest/d1_mini_shield/buzzer.html
 * example: https://github.com/wemos/D1_mini_Examples/blob/master/examples/04.Shields/RGB_LED_Shield/simple/simple.ino
*/
class Buzzer {
public:
	Buzzer(uint8_t buzzerPin, int buzzerFrequency, long _buzzerInterval);

	void executeBuzzer();
	void stopBuzzer();
	void initBuzzer();

private:
	uint8_t buzzerPin;
	int buzzerFrequency;
	unsigned long buzzerPreviousStart;
	unsigned long buzzerInterval;
	bool buzzerIsOn;
};

#endif //NALKINSCLOUD_ESP8266_BUZZER_H
