
#include "Arduino.h"

#include "sensors.h"
#include "configs.h"
#include "functions.h"
#include "mqtt_client.h"
#include "passive_infrared_sensor.h"

// Bits	300 reserved for magnet sensor data
#define IS_TRIGGERED_ADDR 300

#define RECEIVE_STATUS "set_lock_status"
#define SEND_STATUS "current_status"
#define ALARM "alarm"
#define MAGNET_INPUT_PIN 	4 // GPIO4 -> D2
#define PIR_INPUT_PIN		5 // GPIO5 -> D1

#define LOCKED 1 // 1 -> Locked
#define OPENED 0 // 0 -> Opened

Buzzer *alarmBuzzer = nullptr;

bool isAlarmOn = false;
bool isAlarmSent = false;
bool isTriggered = false;

int currentMagnetStatus;
int lastMagnetPublish;
int lastMagnetState;

PIR *motionSensor = nullptr;

/**
 * Publish all data to the mqtt broker
 */
void publishDataToServer() {
	String topic = generalTopic + SEND_STATUS; // Setup the publish topic

	if (lastMagnetPublish != currentMagnetStatus) { // If there was a change in the magnet status then
		publishMessageToMQTTBroker((char *) topic.c_str(), (char *) String(currentMagnetStatus).c_str(), retained);
		lastMagnetPublish = currentMagnetStatus;
	}

	if (isAlarmOn && !isAlarmSent) { // If Alarm triggered, and the alarm was not sent yet (before release received) then
		topic = generalTopic + ALARM; // Setup alarm publish topic
		String alarm = "1";
		publishMessageToMQTTBroker((char *) topic.c_str(), (char *) alarm.c_str(), retained); //Send the data
		isAlarmSent = true;
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

	// If topic data equals RECEIVE_STATUS then
	if (areCharArraysEqual(topicArray[2], RECEIVE_STATUS)) {
		if ((char) payload[0] == '1') {// If received 1 then system armed
			writeIntToEEPROM(IS_TRIGGERED_ADDR, (uint8_t) 1);
			isTriggered = true;
		}
		else if ((char) payload[0] == '0') { // If received 0 then system released
			writeIntToEEPROM(IS_TRIGGERED_ADDR, (uint8_t) 0);
			isTriggered = false;
			isAlarmSent = false;
			isAlarmOn = false;
		}
	}
}


/**
 * Collect data from sensors
 */
void getDataFromSensor() {
	currentMagnetStatus = digitalRead(MAGNET_INPUT_PIN); // Get sensor state

	if (lastMagnetState != currentMagnetStatus) { // If there was a change in the magnet status then
		// If device is triggered, And magnet is in opened state then, Set alarm flag
		if (isTriggered && (lastMagnetState == LOCKED) && (currentMagnetStatus == OPENED))
			isAlarmOn = true;
		if (DEBUG) {
			Serial.print("Current magnet state: ");
			Serial.println(currentMagnetStatus);
		}
		lastMagnetState = currentMagnetStatus;
	}

	if (isAlarmOn) // If alarm flag is set on (alarm triggered) then
		alarmBuzzer->executeBuzzer();
	else
		alarmBuzzer->stopBuzzer();
}

/**
 * Subscribe to all relevant sensors connected
 */
void getSensorsInformation() {
	String topic = generalTopic + RECEIVE_STATUS;
	subscribeToMQTTBroker((char *) topic.c_str());
}

/**
 * Initialize all sensors present in the system
 */
void initSensor() {
	motionSensor = new PIR(PIR_INPUT_PIN);

	deviceType = "magnet"; // The devices type definition
	deviceId = "test_magnet_simulator"; // The devices unique id
	chipType = "ESP8266"; // The devices chip type

	// Set some non 0 or 1 value on init, so message will be published on start
	lastMagnetState = 2;
	lastMagnetPublish = 2;

	currentMagnetStatus = digitalRead(MAGNET_INPUT_PIN); // Get sensor state
	alarmBuzzer = new Buzzer(BUZZER, BUZZER_FREQUENCY, 1000);
	alarmBuzzer->initBuzzer();

	// Get triggered state from EEPROM memory on boot
	isTriggered = readIntFromEEPROM(IS_TRIGGERED_ADDR) == 1;
	if (DEBUG) {
		Serial.print("isTrigger status is:");
		Serial.println(isTriggered);
	}

	pinMode(MAGNET_INPUT_PIN, INPUT);
}
