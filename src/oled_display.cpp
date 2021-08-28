//
// Created by Arie Lev on 2019-07-13.
//

//#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#include "oled_display.h"
#include "global_configs.h"
#include "wifi_handler.h"
#include "mqtt_client.h"

// time durations to display information
unsigned long dataInterval = 60000;
unsigned long serverInterval = 10000;
unsigned long ssidInterval = 5000;
unsigned long batteryInterval = 5000;
/**
 * cycleInterval is the time frame of when we "reset" shown info,
 * and its value is the sum of all displayed components.
 * display cycle is needed for calculating how much display time each component will get
 * for example:
 * 	0 seconds					  	   60s	   	   	   70s		   75s	     80 seconds
 * 		|-------------------------------|---------------|-----------|-----------|
 * 		|								|				|			|			|
 * 		|								|				|-----------|-----------|
 * 		|-------------------------------|				|  display  	display
 * 			 display data		        |				|  	WIFI		battery
 *										|				|	SSID		states
 *										|---------------|
 * 									 	    display
 * 									 	  server data
 *
 * NOTE!!! components must be in that orders, as a calculation in displaySensorData function takes this into account
 */
unsigned long cycleInterval = dataInterval + serverInterval + ssidInterval + batteryInterval;
unsigned long previousCycleInterval = 0;


/**
 * print 'nalkinsBitmap' logo to display
 */
void Oled64x48Display::displayLogo() {
	display.clearDisplay();
	// Display nalkisncloud on start, aligned with 64x48 display
	display.setCursor(10,0);
	display.println("Nalkins");
	display.setCursor(16,8);
	display.println("CLoud");
	display.display();
	delay(3000);

	// Display nalkinscloud bitmap image on start, aligned with 64x48 display
	display.clearDisplay();
	display.drawBitmap(8, 0, nalkinsBitmap, 48, 48, WHITE);
	display.display();
}

/**
 * print content starting at display location (0, 0).
 * @param content
 */
void Oled64x48Display::displaySensorData(String& content) {
    unsigned long currentMillis = millis();
    /**
     * case when we need to reset cycle
     * 													    previousCycleInterval
     * 																 |
     * 																 |
     *  	0s						  	  100s	   	   	  115s		 |
     * 		|-------------------------------|---------------|--------*--|--* <- currentMillis
     * 		|								|				|			|
     * 		|								|				|-----------*
     * 		|-------------------------------|				|			|
     * 			display content 			|				|			|
     *										|				|	  cycleInterval
     *										|---------------|
     * 									 	    display
     * 									 	  server data
     */
    if (currentMillis - previousCycleInterval >= cycleInterval) {
        // previousCycleInterval will be used as the "head" of the display cycle (point 0s on graph)
        previousCycleInterval = currentMillis;
    } else { // current time frame is somewhere in between 0 ("head") to cycleInterval

        // calculate time passed since 0 point (head)
        unsigned long currentCycleTime = currentMillis - previousCycleInterval;
        if (currentCycleTime < dataInterval) {
            /**
             * current time is somewhere in the dataInterval, for example
             *
             * previousCycleInterval
             *			  |
             * 			  |	currentMillis	        dataInterval
             * 			  |		  |						|
             *	   		  |  	  | 			  	  100s	   	   	  115s		 120 seconds
             * head 0s->|-*-------*---------------------|---------------|-----------|
             * 			|		  						|				|			|
             *	 		|								|				|-----------|
             * 			|-------------------------------|				|
             * 				    display content 		|				|
             *											|				|
             *											|---------------|
             */
            displayText(content);
        } else if (currentCycleTime < (dataInterval + serverInterval)) {
            String broker = "MQTT connected: " + String(isClientConnectedToMQTTServer()) + "\nHost:\n" + String(configs.mqttServer) + ":" + String(configs.mqttPort);
            displayText(broker);
        } else if (currentCycleTime < (dataInterval + serverInterval + ssidInterval)) {
            String wifi =  "WiFi\nConnected: " + String(isWifiConnected()) + "\nssid:\n" +  String(configs.wifiSsid);
            displayText(wifi);
        } else { // display last component in order
            // TODO implement real battery voltage calculation
            String batt =  "Batt left:\n" +  String("85% (FAKE)");
            displayText(batt);
        }
    }
}

/**
 * print content starting at display location (0, 0).
 * @param content
 */
void Oled64x48Display::displayText(String& content) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(content);
    display.display();
}

/**
 * init ith the I2C addr 0x3C (for 64x48),
 * set relevant text size and color
 */
void Oled64x48Display::initDisplay() {
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize w
	display.display();
	delay(1000);
	display.setTextSize(textSize);
	display.setTextColor(textColor);
}

/**
 * init new Oled64x48Display object, full example can be found here https://github.com/adafruit/Adafruit_SSD1306
 *
 * @param _textSize text side to be used for current object
 * @param _textColor text color to be used for current object
 */
Oled64x48Display::Oled64x48Display(uint8_t _textSize, uint8_t _textColor) {
	textSize = _textSize;
	textColor = _textColor;
}