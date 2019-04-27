
/* Add below row to config.h file */
const int motionPin = 2; // Input pin (for PIR sensor)


/* Add next to setup() function 

pinMode(inputPin, INPUT);

*/

int lastPIRState = LOW; // On start program assume no motion detected
int pirValue = 0; // variable for reading the pin status

boolean isMotionDetected() {
	pirValue = digitalRead(motionPin); // Read PIR status

	if (pirValue == HIGH) { // Motion detected
		// If lastPIRState was LOW, do below so statement will run only on PIR output change,
		// If lastPIRState was already High, then just skipp below if statement.
		if (lastPIRState == LOW) {
			Serial.println("Motion detected.");
			lastPIRState = HIGH;
		}
		return true; // Return TRUE for motion detected

	} else { // NO Motion detected
		// If lastPIRState was HIGH, this means there where motion last call,
		// And it is now stopped
		// If there was not motion on this call, and no motion last call just skipp if statement
		if (lastPIRState == HIGH) {
			Serial.println("Motion ended.");
			lastPIRState = LOW;
		}
		return false; // Return FALSE for NO motion detected
	}
}
