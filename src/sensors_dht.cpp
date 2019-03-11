
#include "Arduino.h"

#include "sensors.h"
#include "configs.h"
#include "functions.h"
#include "mqtt_client.h"

DHT dht(DHT_PIN, DHT_TYPE, 11); // 11 works fine for ESP8266

//Set time to "delay" a publish message
unsigned long previousPublish = 0;
const long publishInterval = 120000; // interval at which to send message (milliseconds)


void collectAndPublishData() {
	String topic;
	char message[sizeof(float)];
	float temperature_c, humidity;

	// Get temperature value from DHT sensor
	temperature_c = dht.readTemperature();
	topic = generalTopic + "temperature"; //Set a topic string that will change depending on the relevant sensor

	//If there was an error reading data from sensor then
	if (isnan(temperature_c)) {
		if (DEBUG)
			Serial.println("Failed to read temperature from DHT sensor!");
		//message = "DHT Error";
		return;
	} else {
		dtostrf(temperature_c, 4, 2, message); // Arduino based function converting float to string
	}
	publishMessageToMQTTBroker((char *) topic.c_str(), message, false); //Send the data

	humidity = dht.readHumidity();
	topic = generalTopic + "humidity"; //Set a topic string that will change depending on the relevant sensor
	// Send humidity value
	// If there was an error reading data from sensor then
	if (isnan(humidity)) {
		if (DEBUG)
			Serial.println("Failed to read humidity from DHT sensor!");
		//message = "DHT Error";
		return;
	} else {
		dtostrf(humidity, 4, 2, message); // Arduino based function converting float to string
	}
	publishMessageToMQTTBroker((char *) topic.c_str(), message, false); //Send the data
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
	if (DEBUG)
		Serial.println("Running 'sendDataToSensor' function");

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
		collectAndPublishData();
	}
}


/**
 * Collect data from sensors
 * According to each sensor logic, decide if message publish is needed
 */
void getDataFromSensor() {
	//For each sensor, check its last publish time
	unsigned long currentMillis = millis();
	if (currentMillis - previousPublish >= publishInterval) {
		previousPublish = currentMillis;

		collectAndPublishData();
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
	deviceType = "dht"; // The devices type definition
	deviceId = "test_dht_device_id"; // The devices unique id
	chipType = "ESP8266"; // The devices chip type

	dht.begin(); // initialize temperature sensor

	pinMode(LED_WORK_STATUS, OUTPUT);
	digitalWrite(LED_WORK_STATUS, LOW);

	pinMode(CONFIGURATION_MODE_BUTTON, INPUT); // Setup Configuration mode button
}
