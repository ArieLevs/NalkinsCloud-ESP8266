
#include "Arduino.h"

#include "sensors.h"
#include "configs.h"
#include "functions.h"
#include "mqtt_client.h"
#include "oled_display.h"

#include "DHT.h"

//Set DHT type and input pin
#define DHT_TYPE 	DHT22	// Set type of used DHT type
#define DHT_PIN 	0 		// GPIO0 -> D3

#define deepSleepDuration 300 // Seconds

float temperature_c, humidity;

DHT dht(DHT_PIN, DHT_TYPE, 11); // 11 works fine for ESP8266

Oled64x48Display *oledDisplay = nullptr;

//Set time to "delay" a publish message
unsigned long previousPublish = 0;
const long publishInterval = 120000; // interval at which to send message (milliseconds)

unsigned long previousTempDisplay = 999999; // set to 999999 so temp print will occur on first iteration
const long tempDisplayInterval = 100000; // interval at which to display temperature (milliseconds)

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
 * 			 display temperature		|				|  	WIFI		battery
 *										|				|	SSID		states
 *										|---------------|
 * 									 	    display
 * 									 	  server data
 *
 * NOTE!!! components must be in that orders, as a calculation in displayData function takes this into account
 */
const long displayCycleInterval = tempDisplayInterval + serverDataDisplayInterval + ssidDisplayInterval + batteryDisplayInterval;
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
		topic = generalTopic + "temperature"; //Set a topic string that will change depending on the relevant sensor

		if (isnan(temperature_c)) { //If there was an error reading data from sensor then
			if (DEBUG)
				Serial.println("Failed to read temperature from DHT sensor!");
			//message = "DHT Error";
			return;
		} else
			dtostrf(temperature_c, 4, 2, message); // Arduino based function converting float to string
		publishMessageToMQTTBroker((char *) topic.c_str(), message, false); //Send the data

		topic = generalTopic + "humidity"; //Set a topic string that will change depending on the relevant sensor
		if (isnan(humidity)) { // If there was an error reading data from sensor then
			if (DEBUG)
				Serial.println("Failed to read humidity from DHT sensor!");
			//message = "DHT Error";
			return;
		} else
			dtostrf(humidity, 4, 2, message); // Arduino based function converting float to string
		publishMessageToMQTTBroker((char *) topic.c_str(), message, false); //Send the data

		ESP.deepSleep(deepSleepDuration * 1000000);
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

void getLevel() {
	int nVoltageRaw = analogRead(A0);
	float fVoltage = (float)nVoltageRaw * 0.00486;
	Serial.print("nVoltageRaw: "); Serial.println(nVoltageRaw);
	Serial.print("fVoltage: "); Serial.println(fVoltage);

	float fVoltageMatrix[22][2] = {
			{4.2,  100},
			{4.15, 95},
			{4.11, 90},
			{4.08, 85},
			{4.02, 80},
			{3.98, 75},
			{3.95, 70},
			{3.91, 65},
			{3.87, 60},
			{3.85, 55},
			{3.84, 50},
			{3.82, 45},
			{3.80, 40},
			{3.79, 35},
			{3.77, 30},
			{3.75, 25},
			{3.73, 20},
			{3.71, 15},
			{3.69, 10},
			{3.61, 5},
			{3.27, 0},
			{0, 0}
	};

	int i, perc;

	perc = 100;

	for(i=20; i>=0; i--) {
		if(fVoltageMatrix[i][0] >= fVoltage) {
			perc = fVoltageMatrix[i + 1][1];
			break;
		}
	}



	Serial.print("Level: "); Serial.println(perc);


	oledDisplay->displayTemp(fVoltage, (float)perc);
}

/**
 * Collect data from sensors
 */
void getDataFromSensor() {
	getLevel();
	return;

	// Get temperature value from DHT sensor
	temperature_c = dht.readTemperature();
	if (isnan(temperature_c)) //If there was an error reading data from sensor then
		if (DEBUG)
			Serial.println("Failed to read temperature from DHT sensor!");

	humidity = dht.readHumidity();
	if (isnan(humidity)) // If there was an error reading data from sensor then
		if (DEBUG)
			Serial.println("Failed to read humidity from DHT sensor!");
	if (DEBUG) {
		Serial.print("Current temperature C: ");
		Serial.println(temperature_c);
		Serial.print("Current humidity: ");
		Serial.println(humidity);
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
	 * 			display temperature			|				|			|
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
		if (currentCycleTime < tempDisplayInterval) {
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
			 * 				display temperature			|				|
			 *											|				|
			 *											|---------------|
			 */
			oledDisplay->displayTemp(temperature_c, humidity);
		} else if (currentCycleTime < (tempDisplayInterval + previousServerDataDisplay)) {
			String portString = String(configs.mqttPort);
			oledDisplay->displayServerData(configs.mqttServer, portString);
		} else if (currentCycleTime < (tempDisplayInterval + previousServerDataDisplay + previousSsidDDisplay)) {
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

	deviceType = "dht"; // The devices type definition
	deviceId = "test_dht_device_id"; // The devices unique id
	chipType = "ESP8266"; // The devices chip type

	dht.begin(); // initialize temperature sensor

	pinMode(CONFIGURATION_MODE_BUTTON, INPUT); // Setup Configuration mode button
}
