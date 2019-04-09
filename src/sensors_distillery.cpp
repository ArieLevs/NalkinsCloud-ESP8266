
#include "Arduino.h"

#include "sensors.h"
#include "configs.h"
#include "functions.h"
#include "mqtt_client.h"

#include "Adafruit_MAX31855.h"

#define MAIN_HEATER 	 1  // GPIO01 -> TX, Main SSR IO pin
#define WATER_COOLER 	 3  // GPIO03 -> RX, Watter Cooler SSR IO pin
#define AIR_COOLER 		 16 // GPIO16 -> D0, Air Cooler SSR IO pin
#define GARBAGE_DISPOSAL 12 // GPIO12 -> D6, To Garbage SSR IO pin
#define BARREL_DISPOSAL  15 // GPIO15 -> D8, To Barrel SSR IO pin

#define MAXCLK 5 // D1
#define MAXDO  4 // D2
#define MAXCS  0 // D3

// Declare Thermocouple type of MAX31855
Adafruit_MAX31855 *thermocouple = nullptr;
double temperature_c;
Buzzer *alarmBuzzer = nullptr;

// Declare and set default values
int mainHeaterStartTemperature;
int waterCoolerStartTemperature;
int airCoolerStartTemperature;
int toBarrelDisposalStartTemperature;

bool wasGarbageDisposalExecuted;
bool garbageDisposalMadeTimeSnapshot;
bool barrelDisposalMadeTimeSnapshot;
bool wasBarrelDisposalExecuted;
bool isErrorOccurred;

bool isAutomatedJob = false; // Flag that store if device is on automation mode

// Set garbage / barrel logic variables
unsigned long garbageDisposalTimeSnapshot = 0; // Store current timestamp for garbage
unsigned long barrelDisposalTimeSnapshot = 0; // Store current timestamp for barrel
const long barrelAndGarbageDisposalStopExecute = 20000; // Stop garbage / barrel disposal after (milliseconds)

/**
 * Function will return temperature value as double
 * In case there is something wrong with the sensor
 * function will return value of 9999
 *
 * Add below row to setup() function
 * wait for MAX31855 chip to stabilize
 * delay(500);
 */
double getTempFromMAX31855() {
	double celsiusTemp = thermocouple->readCelsius(); // Get data from sensor
	if (isnan(celsiusTemp)) { // If there is some error reading temp value
		Serial.println("Error with thermocouple.");
		return -9999;
	} else
		return celsiusTemp;
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

	// If received message is 'update_now' then
	if (areCharArraysEqual(topicArray[2], "update_now")) {
		// DO update data from sensor
	}

	// If received message is 'automation' then
	if (areCharArraysEqual(topicArray[2], "automation")) {
		if ((char) payload[0] == '1') {
			/**
			*  automation payload is build like: ("1,w,x,y,z")
			*  v - temperature when main ssr stop working
			*  w - temperature to start water cooler ssr
			*  x - temperature to start air cooler ssr
			*  y - temperature to start to barrel disposal ssr
			*/
			// Char array that will store the message in parts
			char *messageArray[5];
			i = 0;
			// Get the first section from topic
			messageArray[i] = strtok((char *) payload, ",");

			// Brake the message string into parts
			// Each array cell contains part of the topic
			if (DEBUG)
				Serial.println("Message is: ");
			while (messageArray[i] != '\0') {
				messageArray[++i] = strtok(NULL, ",");
				if (DEBUG) {
					Serial.println(messageArray[i]);
				}
			}

			// Set variables from message to temperature vars
			mainHeaterStartTemperature = atoi(messageArray[1]);
			waterCoolerStartTemperature = atoi(messageArray[2]);
			airCoolerStartTemperature = atoi(messageArray[3]);
			toBarrelDisposalStartTemperature = atoi(messageArray[4]);
			if (DEBUG) {
				Serial.println("Temperature configurations set to:");
				Serial.print("MAIN_HEATER: ");
				Serial.println(mainHeaterStartTemperature);
				Serial.print("WATER_COOLER: ");
				Serial.println(waterCoolerStartTemperature);
				Serial.print("AIR_COOLER: ");
				Serial.println(airCoolerStartTemperature);
				Serial.print("BARREL_DISPOSAL: ");
				Serial.println(toBarrelDisposalStartTemperature);
			}

			isAutomatedJob = true;

			digitalWrite(MAIN_HEATER, LOW);
			digitalWrite(WATER_COOLER, LOW);
			digitalWrite(AIR_COOLER, LOW);
			digitalWrite(GARBAGE_DISPOSAL, LOW);
			digitalWrite(BARREL_DISPOSAL, LOW);
		} else if ((char) payload[0] == '0') { // If received 0 then
			isAutomatedJob = false;
			isErrorOccurred = false;

			digitalWrite(MAIN_HEATER, LOW);
			digitalWrite(WATER_COOLER, LOW);
			digitalWrite(AIR_COOLER, LOW);
			digitalWrite(GARBAGE_DISPOSAL, LOW);
			digitalWrite(BARREL_DISPOSAL, LOW);

			wasGarbageDisposalExecuted = false;
			garbageDisposalMadeTimeSnapshot = false;
			barrelDisposalMadeTimeSnapshot = false;
			wasBarrelDisposalExecuted = false;
		}
	}
}

/**
 * Perform distillery work logic here
 *
 * This will keep main heater temperature at default 94 C,
 * Once temperature is above default temperature, main heater turns off
 * Once temperature is again below default temperature, main heater turns on
 */
void doDistilleryJob() {
	if (temperature_c < mainHeaterStartTemperature) // Default temperature is 94C
		digitalWrite(MAIN_HEATER, HIGH);
	else
		digitalWrite(MAIN_HEATER, LOW);

	if (temperature_c < waterCoolerStartTemperature) // Default temperature is 60C
		digitalWrite(WATER_COOLER, LOW);
	else
		digitalWrite(WATER_COOLER, HIGH);

	if (temperature_c < airCoolerStartTemperature) // Default temperature is 80C
		digitalWrite(AIR_COOLER, LOW);
	else
		digitalWrite(AIR_COOLER, HIGH);

	if (temperature_c < 80) {
		if (!wasGarbageDisposalExecuted) {
			digitalWrite(GARBAGE_DISPOSAL, HIGH);
			if (!garbageDisposalMadeTimeSnapshot) {
				garbageDisposalTimeSnapshot = millis();
				garbageDisposalMadeTimeSnapshot = true;
			}

			if (millis() - garbageDisposalTimeSnapshot >= barrelAndGarbageDisposalStopExecute) {
				digitalWrite(GARBAGE_DISPOSAL, LOW);
				wasGarbageDisposalExecuted = true;
				garbageDisposalMadeTimeSnapshot = false;
			}
		}
	}

	if (temperature_c >= toBarrelDisposalStartTemperature) { // Default temperature is 80C
		if (!wasBarrelDisposalExecuted) {
			if (DEBUG)
				Serial.println("Executing BARREL disposal");
			digitalWrite(BARREL_DISPOSAL, HIGH);
			if (!barrelDisposalMadeTimeSnapshot) {
				barrelDisposalTimeSnapshot = millis();
				barrelDisposalMadeTimeSnapshot = true;
			}

			if (millis() - barrelDisposalTimeSnapshot >= barrelAndGarbageDisposalStopExecute) {
				digitalWrite(BARREL_DISPOSAL, LOW);
				wasBarrelDisposalExecuted = true;
				barrelDisposalMadeTimeSnapshot = false;
			}
		}
	}

	/*
	// The main heater is always on
	digitalWrite(LED_MAIN_SSR, HIGH);

	if(!wasAboveCoolerStartTemp) { // If temperature was NOT yet above 60C then
	  // This is true if temp is C < 60
	  if(temperature_c > coolerStartTemp) // Check if temp above 60
		wasAboveCoolerStartTemp = true; // And set flag 'above 60' to true
	} else { // If temperature was above 60C then
	  // This is true if temp is C > 60
	  digitalWrite(LED_COOLER, HIGH); // Turn cooler on
	  if(!wasAboveDisposalEndTemp) { // If temperature was NOT yet above 80C then
		// This is true if temp is C > 60 AND C < 80
		// If temperature passed the input for starting the disposal ssr then
		if(temperature_c > disposalStartTemp)
		  digitalWrite(LED_DISPOSAL_SSR, HIGH); // Turn disposal SSR on
		if(temperature_c > disposalEndTemp)
		  wasAboveDisposalEndTemp = true;
	  } else { // If temperature was above 80C then
		// This is true if temp is C > 80
		digitalWrite(LED_DISPOSAL_SSR, LOW); // Turn disposal SSR off
		if(!wasAboveSecondarySSRStartTemp) { // If temperature was NOT yet above 84C then
		  // This is true if temp is C > 80 AND C < 84
		  if(temperature_c > secondarySSRStartTemp)
			wasAboveSecondarySSRStartTemp =true;
		}
		else {
		  // This is true if temp is C > 84
		  digitalWrite(LED_SECONDARY_SSR, HIGH);
		  // If temp go back below 84 again
		  if ( temperature_c < secondarySSRStartTemp && temperature_c > 0) {
			// Shutdown all GPIOs
			digitalWrite(LED_MAIN_SSR, LOW);
			digitalWrite(LED_COOLER, LOW);
			digitalWrite(LED_DISPOSAL_SSR, LOW);
			digitalWrite(LED_SECONDARY_SSR, LOW);

			isErrorOccurred = true;
			wasAboveCoolerStartTemp = false;
			wasAboveDisposalEndTemp = false;
			wasAboveSecondarySSRStartTemp = false;

			// Send automation error message
			String topic;
			topic = generalTopic + "automation_error"; // Set a topic string that will change depending on the relevant sensor
			startMQTTPublisher((char*)topic.c_str(), "1", retained); // Send the data

			// Set automation flag to false, so job will stop executing
			isAutomatedJob = false;
		  }
		}
	  }
	}*/
}

/**
 * Collect data from sensors
 */
void getDataFromSensor() {
	temperature_c = getTempFromMAX31855(); // Get temperature from MAX chip
	if (temperature_c == -9999) { // In case there was an error with the chip MAX returns -9999
		if (DEBUG)
			Serial.println("Failed to read temperature from MAX31855k sensor.");
		return;
	}
	if (DEBUG) {
		Serial.print("Temperature is (C): ");
		Serial.println(temperature_c);
	}

	if (isErrorOccurred)
		alarmBuzzer->executeBuzzer();

	// Check if automation job requested for distillery job
	if (isAutomatedJob)
		doDistilleryJob();
}

//Set time to "delay" a publish message
unsigned long previousPublish = 0;
const long publishInterval = 10000; // interval at which to send message (milliseconds)

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

		topic = generalTopic + "temperature"; // Set a topic string that will change depending on the relevant sensor
		dtostrf(temperature_c, 4, 2, message); // Arduino based function converting float to string
		publishMessageToMQTTBroker((char *) topic.c_str(), message, notRetained); // Send the data
	}
}

/**
 * Subscribe to all relevant sensors connected
 */
void getSensorsInformation() {
	String topic = generalTopic + "ssr";
	subscribeToMQTTBroker((char *) topic.c_str());
	topic = generalTopic + "automation";
	subscribeToMQTTBroker((char *) topic.c_str());
}

/**
 * Initialize all sensors present in the system
 */
void initSensor() {
	thermocouple = new Adafruit_MAX31855(MAXCLK, MAXCS, MAXDO);
	alarmBuzzer = new Buzzer(BUZZER, BUZZER_FREQUENCY, 1000);
	alarmBuzzer->initBuzzer();
	alarmBuzzer->stopBuzzer();

	//pinMode(MAIN_HEATER, OUTPUT);
	pinMode(WATER_COOLER, OUTPUT);
	pinMode(AIR_COOLER, OUTPUT);
	pinMode(GARBAGE_DISPOSAL, OUTPUT);
	pinMode(BARREL_DISPOSAL, OUTPUT);
	//digitalWrite(MAIN_HEATER, LOW);
	digitalWrite(WATER_COOLER, LOW);
	digitalWrite(AIR_COOLER, LOW);
	digitalWrite(GARBAGE_DISPOSAL, LOW);
	digitalWrite(BARREL_DISPOSAL, LOW);
}
