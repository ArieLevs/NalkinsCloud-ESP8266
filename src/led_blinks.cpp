
#include "led_blinks.h"

/**
 * Blink led pin with given interval times
 * NOTE that 'LOW' switches the LED *on* and 'HIGH' to *off*
 *
 * @param interval
 */
void LedBlinks::intervalBlink(long interval) {
	unsigned long currentMillis = millis();
	if (currentMillis - blinkPreviousStart >= interval) {
		blinkPreviousStart = currentMillis;
		if (ledState == LOW)
			ledState = HIGH;
		else
			ledState = LOW;
		digitalWrite(ledPin, ledState);
	}
}

/**
 * Blink led pin twice rapidly with given interval times
 * NOTE that 'LOW' switches the LED *on* and 'HIGH' to *off*
 *
 * @param uint8_t GPIO number of LED pin
 */
void LedBlinks::rapidIntervalBlink(long interval) {
	unsigned long currentMillis = millis();
	if (currentMillis - blinkPreviousStart >= interval) {
		blinkPreviousStart = currentMillis;
		digitalWrite(ledPin, LOW);
		delay(50);
		digitalWrite(ledPin, HIGH);
		delay(50);
		digitalWrite(ledPin, LOW);
		delay(50);
		digitalWrite(ledPin, HIGH);
	}
}

LedBlinks::LedBlinks(uint8_t _ledPin) {
	ledPin = _ledPin;
	blinkPreviousStart = 0;
	ledState = 0;

	pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, LOW);
}
