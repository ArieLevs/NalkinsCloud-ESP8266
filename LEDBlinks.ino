
// Set delay for the AP (configuraion) mode LED blink
unsigned long previousConfigurationMode = 0;
const long ConfigurationModeInterval = 500;
int ledState = LOW;

/**
 * Perform led blink to indicate device is on configuration mode
 */
void blinkConfigurationMode() {
  unsigned long currentMillis = millis();
  if(currentMillis - previousConfigurationMode >= ConfigurationModeInterval) { // Blink LED when on configuration mode
    previousConfigurationMode = currentMillis;
    if (ledState == LOW)
      ledState = HIGH;  // Note that this switches the LED *off*
    else
      ledState = LOW;   // Note that this switches the LED *on*
    digitalWrite(LED_WORK_STATUS, ledState);
  }
}

// Set delay for the normal mode LED blink
unsigned long previousWorkMode = 0;
const long workModeInterval = 2500;

/**
 * Perform led blink to indicate device is at normal work mode
 */
void blinkWorkMode() {
  unsigned long currentMillis = millis();
  if(currentMillis - previousWorkMode >= workModeInterval) { // Blink LED when on work mode
    previousWorkMode = currentMillis;
    if (ledState == LOW)
      ledState = HIGH;  // Note that this switches the LED *off*
    else
      ledState = LOW;   // Note that this switches the LED *on*
    digitalWrite(LED_WORK_STATUS, ledState);
  }
}


// Set delay for wifi disconnected mode LED blink
unsigned long previousWifiError = 0;
const long wifiErrorInterval = 2000;

/**
 * Perform led blink to indicate device lost wifi connection
 */
void blinkWifiDisconnected() {
  unsigned long currentMillis = millis();
  if(currentMillis - previousWorkMode >= wifiErrorInterval) { // Blink LED when wifi disconnected
    previousWorkMode = currentMillis;

    digitalWrite(LED_WORK_STATUS, HIGH);
    delay(50);
    digitalWrite(LED_WORK_STATUS, LOW);
    delay(50);
    digitalWrite(LED_WORK_STATUS, HIGH);
    delay(50);
    digitalWrite(LED_WORK_STATUS, LOW);
  }
}

