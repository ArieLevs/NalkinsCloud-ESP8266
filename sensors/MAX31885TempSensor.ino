
//#include <SPI.h>
#include "Adafruit_MAX31855.h"

// digital IO pins
// This should be places in config.h file
#define MAXDO   4
#define MAXCS   0
#define MAXCLK  2

// initialize the Thermocouple
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

/* Add below row to setup() function
// wait for MAX31855 chip to stabilize
delay(500);
*/

/*  Function will return temperature value as double
 *  In case there is something wrong with the sensor
 *  function will return value of 9999
 */
double getTempFromMAX31855() {
  double celsiusTemp = thermocouple.readCelsius(); // Get data from sensor
   if (isnan(celsiusTemp)) { // If there is some error reading temp value
     Serial.println("Error with thermocouple.");
     return -9999;
   } else { // If all is OK, return the value
     return celsiusTemp;
   }
}
