
#include "Arduino.h"

#include "sensors.h"
#include "configs.h"
#include "functions.h"
#include "mqtt_client.h"
#include "oled_display.h"

#include "HX711.h"

// HX711 circuit pins set
const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 2;

#define deepSleepDuration 300 // Seconds

long scale_read_value;

HX711 scale;

Oled64x48Display *oledDisplay = nullptr;

//Set time to "delay" a publish message
unsigned long previousPublish = 0;
const long publishInterval = 120000; // interval at which to send message (milliseconds)

unsigned long previousDataDisplay = 999999; // set to 999999 so sensor data print will occur on first iteration
const long dataDisplayInterval = 100000; // interval at which to display sensor data (milliseconds)

unsigned long previousServerDataDisplay = 14999;
const long serverDataDisplayInterval = 15000;

unsigned long previousSsidDDisplay = 4999;
const long ssidDisplayInterval = 5000;

const long batteryDisplayInterval = 5000;

/**
 * displayCycle is the time frame of when we "reset" shown things,
 * and its value is the sum of all displayed components.
 * display cycle is needed for calculating how much display time each component will get
 * for example:
 * 	0 seconds					  	  100s	   	   	  115s		  120s	    125seconds
 * 		|-------------------------------|---------------|-----------|-----------|
 * 		|								|				|			|			|
 * 		|								|				|-----------|-----------|
 * 		|-------------------------------|				|  display  	display
 * 			 display sensor data		|				|  	WIFI		battery
 *										|				|	SSID		states
 *										|---------------|
 * 									 	    display
 * 									 	  server data
 *
 * NOTE!!! components must be in that orders, as a calculation in displayData function takes this into account
 */
const long displayCycleInterval = dataDisplayInterval + serverDataDisplayInterval + ssidDisplayInterval + batteryDisplayInterval;
unsigned long previousDisplayCycleInterval = 0;

/**
 * Publish all data to the mqtt broker
 */
void publishDataToServer() {
	//For each sensor, check its last publish time
	unsigned long currentMillis = millis();
	if (currentMillis - previousPublish >= publishInterval) {
		previousPublish = currentMillis;
		String topic;
		char message[sizeof(float)];
		topic = generalTopic + "weight"; // Set a topic string that will change depending on the relevant sensor

		if (isnan(scale_read_value)) { // If there was an error reading data from sensor then
			if (DEBUG)
				Serial.println("Failed to read data from HX711 sensor!");
			//message = "DHT Error";
			return;
		} else
			dtostrf(scale_read_value, 4, 2, message); // Arduino based function converting float to string
		publishMessageToMQTTBroker((char *) topic.c_str(), message, false); //Send the data
	}
}

/**
 * Execute incoming message to sensor,
 * The message will be converted to real actions that void handleClient() should apply on sensors
 * This function is being forwarded from the callback() function
 * 
 * @param topic incoming message topic
 * @param payload incoming message payload
 */
void sendDataToSensor(const char *topic, byte *payload) {
	// topic should be of type: "device_id/device_type/data"
	// Char array that will store the topic in parts
	char *topicArray[4];

	int i = 0;
	// Get the first section from topic
	topicArray[i] = strtok((char *) topic, "/");

	// Brake the topic string into parts
	// Each array cell contains part of the topic
	while (topicArray[i] != nullptr) {
		topicArray[++i] = strtok(nullptr, "/");
	}

	// If received message is 'update_now' then
	if (areCharArraysEqual(topicArray[2], "update_now")) {
		publishDataToServer();
	}
}

/**
 * Collect data from sensors
 */
void getDataFromSensor() {
	// Get value from HX711 sensor
    if (scale.wait_ready_retry(10)) {
        scale_read_value = scale.read();
        Serial.print("HX711 reading: ");
        Serial.println(scale_read_value);
    } else {
        Serial.println("HX711 not found.");
    }

	if (isnan(scale_read_value)) //If there was an error reading data from sensor then
		if (DEBUG)
			Serial.println("Failed to read data from HX711 sensor!");

	if (DEBUG) {
		Serial.print("Current weight: ");
		Serial.println(scale_read_value);
	}

	unsigned long currentMillis = millis();
	/**
	 * case when we need to reset cycle
	 * 													previousDisplayCycleInterval
	 * 																 |
	 * 																 |
	 *  	0s						  	  100s	   	   	  115s		 |
	 * 		|-------------------------------|---------------|--------*--|--* <- currentMillis
	 * 		|								|				|			|
	 * 		|								|				|-----------*
	 * 		|-------------------------------|				|			|
	 * 			display sensor data			|				|			|
	 *										|				|	 displayCycleInterval
	 *										|---------------|
	 * 									 	    display
	 * 									 	  server data
	 */
	if (currentMillis - previousDisplayCycleInterval >= displayCycleInterval) {
		// previousDisplayCycleInterval will be used as the "head" of the display cycle (point 0s on graph)
		previousDisplayCycleInterval = currentMillis;
	} else { // current time frame is somewhere in between 0 ("head") to displayCycleInterval

		// calculate time passed since 0 point (head)
		unsigned long currentCycleTime = currentMillis - previousDisplayCycleInterval;
		if (currentCycleTime < dataDisplayInterval) {
			/**
			 * current time is some where in the tempDisplayInterval, for example
			 *
			 * previousDisplayCycleInterval
			 *			  |
			 * 			  |	currentMillis	   tempDisplayInterval
			 * 			  |		  |						|
			 *	   		  |  	  | 			  	  100s	   	   	  115s		 120 seconds
			 * head 0s->|-*-------*---------------------|---------------|-----------|
			 * 			|		  						|				|			|
			 *	 		|								|				|-----------|
			 * 			|-------------------------------|				|
			 * 				display sensor data			|				|
			 *											|				|
			 *											|---------------|
			 */
			oledDisplay->displayWeight(scale_read_value);
		} else if (currentCycleTime < (dataDisplayInterval + previousServerDataDisplay)) {
			String portString = String(configs.mqttPort);
			oledDisplay->displayServerData(configs.mqttServer, portString);
		} else if (currentCycleTime < (dataDisplayInterval + previousServerDataDisplay + previousSsidDDisplay)) {
			oledDisplay->displayWifiSSID(configs.wifiSsid);
		} else { // display last component in order
			// TODO implement real battery voltage calculation
			String a = "85% (FAKE)";
			oledDisplay->displayBatteryData(a);
		}
	}
}


/**
 * Subscribe to all relevant sensors connected
 */
void getSensorsInformation() {
	String topic = generalTopic + "update_now";
	subscribeToMQTTBroker((char *) topic.c_str());
}


/**
 * Initialize all sensors present in the system
 */
void initSensor() {
	oledDisplay = new Oled64x48Display(1, WHITE);
	oledDisplay->initDisplay();
	oledDisplay->displayLogo();

	deviceType = "HX711"; // The devices type definition
	deviceId = "test_hx711_device_id"; // The devices unique id
	chipType = "ESP8266"; // The devices chip type

    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN); // init weight sensor

	pinMode(CONFIGURATION_MODE_BUTTON, INPUT); // Setup Configuration mode button
}
