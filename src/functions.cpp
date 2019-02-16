

#include "functions.h"

/**
 * Convert mac address uint8_t to String
 * 
 * @param mac MAC address
 * 
 * @return String the string value of the MAC address
 */
String macToStr(uint8_t* mac) {
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}


/**
 * Return devices mac address 
 */
uint8_t* getMacAddress() {
  uint8_t mac[6];
  WiFi.macAddress(mac);
  return mac;
}


/**
 * Compare two strings (char arrays)
 * 
 * @param s1 string
 * @param s2 string
 *  
 * @return boolean  true - both strings are equal
 *                  false - string not equal
 */
bool areCharArraysEqual(const char *s1, const char *s2) {
  if (strlen(s1) != strlen(s2)) // If strings size
    return false; // Strings must be different
  for (unsigned int i = 0; i < sizeof(s1); i++) {
    if (s1[i] != s2[i])
      return false;  // Strings are different
  }
  return true;  // Strings must be the same
}


// Set time to "delay" configuration button press
unsigned long previousConfButton = 0;
const long ConfButtonInterval = 1000; // interval at which to send message (milliseconds)
int confButtonCountDown = 5; // 5 seconds


/**
 * Check if user is pressing the configuration button for more then 5 seconds
 * In case (CONFIGURATIONMODEBUTTON) pressed more then 5 seconds, restart the device
 *
 * @param pinNum num of GPIO of configuration button
 */
void checkConfigurationButton(uint8_t pinNum) {
  // Take current timestamp
  unsigned long currentMillis = millis();
  if(currentMillis - previousConfButton >= ConfButtonInterval) {
    previousConfButton = currentMillis;
    
    int configuration_button = digitalRead(pinNum); // Read button state
    if(configuration_button == HIGH) { // If button is pressed then
      confButtonCountDown--; 
      if (DEBUG) {
        Serial.print("configuration_button counter: ");
        Serial.println(confButtonCountDown);
      }
      if(confButtonCountDown <= 0) { // If button was pressed for 5 seconds in a row
        setConfigurationStatusFlag(1); // Set configuration mode to 1 (true)
        ESP.restart(); // And restart the device
      }
    } else {
      confButtonCountDown = 5; // If button released before 5 seconds passed, reset the counter
    }
  }
}



