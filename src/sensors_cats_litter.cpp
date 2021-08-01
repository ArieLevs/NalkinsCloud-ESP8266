
#include "Arduino.h"
#include <ArduinoJson.h>
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

StaticJsonDocument<256> jsonDoc;
String sensorPayload;

HX711 scale;

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
        publishDataToServer(true);
    }
}

/**
 * Collect data from sensors
 */
void getDataFromSensor() {
    long scale_read_value;

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

    jsonDoc["weight"] = scale_read_value;

    serializeJson(jsonDoc, sensorPayload);

    char message[sizeof(float)];
    String text = dtostrf(scale_read_value, 4, 2, message); // Arduino based function converting float to string
    oledDisplay->displaySensorData(text);
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

	deviceType = "HX711"; // The devices type definition
	deviceId = "test_hx711_device_id"; // The devices unique id
	chipType = "ESP8266"; // The devices chip type

    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN); // init weight sensor

	pinMode(CONFIGURATION_MODE_BUTTON, INPUT); // Setup Configuration mode button
}
