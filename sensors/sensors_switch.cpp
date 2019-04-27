
#include "Arduino.h"

#include "sensors.h"
#include "configs.h"
#include "functions.h"
#include "mqtt_client.h"

#define SWITCH_IN 4  // GPIO4 -> D2
#define SWITCH_OUT 0 // GPIO0 -> D3

// By default device is false
int lastSwitchState;
int currentSwitchState;

//Set time to "delay" a consumption publish message
unsigned long previousPublish = 0;
const long consumptionPublishInterval = 120000; // interval at which to send message (milliseconds)

/**
 * Publish all data to the mqtt broker
 */
void publishDataToServer() {
	if (currentSwitchState != lastSwitchState) {
		lastSwitchState = currentSwitchState; // Remember new switch state
		String topic = generalTopic + "from_device_current_status";
		publishMessageToMQTTBroker((char *) topic.c_str(), (char *) String(currentSwitchState).c_str(), retained);
	}

	unsigned long currentMillis = millis();
	if (currentMillis - previousPublish >= consumptionPublishInterval) {
		previousPublish = currentMillis;
		// TODO read voltage/amps from switch and set it to broker
	}
}

/**
 * Execute incoming message to sensor,
 * The message will be converted to real actions that should apply on sensors
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
	while (topicArray[i] != NULL) {
		topicArray[++i] = strtok(NULL, "/");
	}

	// If received message is 'change_switch' then
	if (areCharArraysEqual(topicArray[2], "change_switch")) {
		if ((char) payload[0] == '0') {
			digitalWrite(SWITCH_OUT, LOW);
		} else if ((char) payload[0] == '1') {
			digitalWrite(SWITCH_OUT, HIGH);
		}
	}
}

/**
 * Collect data from sensors
 * According to each sensor logic, decide if message publish is needed
 */
void getDataFromSensor() {
	currentSwitchState = digitalRead(SWITCH_IN);

	if (DEBUG) {
		if (lastSwitchState != currentSwitchState) { // If there was a change in the magnet status then
			Serial.print("Current switch state: ");
			Serial.println(currentSwitchState);
			lastSwitchState = currentSwitchState;
		}
	}
}

/**
 * Subscribe to all relevant sensors connected
 */
void getSensorsInformation() {
	String topic = generalTopic + "change_switch";
	subscribeToMQTTBroker((char *) topic.c_str());
}

/**
 * Initialize all sensors present in the system
 */
void initSensor() {
	deviceType = "switch"; // The devices type definition
	deviceId = "test_switch_device_id"; // The devices unique id
	chipType = "ESP8266"; // The devices chip type

	lastSwitchState = digitalRead(SWITCH_IN);
	currentSwitchState = digitalRead(SWITCH_IN);
	// TODO separate below
	String topic = generalTopic + "from_device_current_status";
	publishMessageToMQTTBroker((char *) topic.c_str(), (char *) String(currentSwitchState).c_str(),
							   retained); //Send the data

	pinMode(SWITCH_OUT, OUTPUT);
	pinMode(SWITCH_IN, INPUT);
}
