
#include "Arduino.h"
#include <ArduinoJson.h>
#include "sensors.h"
#include "functions.h"
#include "mqtt_client.h"
#include "oled_display.h"

#include "DHT.h"

//Set DHT type and input pin
#define DHT_TYPE 	DHT22	// Set type of used DHT type
#define DHT_PIN 	0 		// GPIO0 -> D3

StaticJsonDocument<256> jsonDoc;
String sensorPayload;

DHT dht(DHT_PIN, DHT_TYPE, 11); // 11 works fine for ESP8266

Oled64x48Display *oledDisplay = nullptr;

//Set time to "delay" a publish message
unsigned long previousPublish = 0;
const long publishInterval = 120000; // interval at which to send message (milliseconds)

/**
 * Publish all data to the mqtt broker
 * in case 'force' is true, publish immediately, else wait for publish cycles (publishInterval)
 *
 * @param force boolean
 */
void publishDataToServer(bool force) {
    if (force) {
        publishMessageToMQTTBroker( "v1/devices/me/telemetry", sensorPayload, false); //Send the data
    } else {
        //For each sensor, check its last publish time
        unsigned long currentMillis = millis();
        if (currentMillis - previousPublish >= publishInterval) {
            previousPublish = currentMillis;
            publishMessageToMQTTBroker("v1/devices/me/telemetry", sensorPayload, false);
        }
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
void sendDataToSensor(const char *topic, byte *payload, unsigned int length) {
    char json[length + 1];
    strncpy (json, (char*)payload, length);
    json[length] = '\0';

    // Decode JSON request, 200 is the capacity of the memory pool in bytes.
    StaticJsonDocument<256> document;
    DeserializationError error = deserializeJson(document, json);

    if (error) {
        Serial.print(F("deserializeJson() failed"));
        return;
    }

    String methodName = String((const char*)document["method"]);

    if (methodName.equals("updateNow")) {
        String responseTopic = String(topic);
        responseTopic.replace("request", "response");
        publishDataToServer(true);
    }
}

/**
 * Collect data from sensors
 */
void getDataFromSensor() {
	float temperature_c = dht.readTemperature(); // Get temperature value from DHT sensor
	if (isnan(temperature_c)) //If there was an error reading data from sensor then
		if (DEBUG)
			Serial.println("Failed to read temperature from DHT sensor!");

    float humidity = dht.readHumidity();
	if (isnan(humidity)) // If there was an error reading data from sensor then
		if (DEBUG)
			Serial.println("Failed to read humidity from DHT sensor!");

    jsonDoc["temperature"] = temperature_c;
    jsonDoc["humidity"] = humidity;

    serializeJson(jsonDoc, sensorPayload);

	if (DEBUG) {
	    Serial.print("Sensor values: ");
		Serial.println(sensorPayload);
	}

    String text = "Temp:\n" + String(temperature_c) + " (C)\n\nHumidity:\n" + String(humidity) + " %";
	oledDisplay->displaySensorData(text);

    delay(500);
    //ESP.deepSleep(deepSleepDuration * 1000000);
}


/**
 * Subscribe to all relevant sensors connected
 */
void getSensorsInformation() {
    subscribeToMQTTBroker("v1/devices/me/rpc/request/+");
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
