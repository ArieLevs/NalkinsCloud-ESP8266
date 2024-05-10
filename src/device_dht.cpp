
// #include "Arduino.h"
// #include <ArduinoJson.h>
#include "device_dht.h"
#include "functions.h"
// #include "mqtt_client.h"
// #include "oled_display.h"

// #include "DHT.h"

/**
 * Execute incoming message to sensor,
 * The message will be converted to real actions that void handleClient() should apply on sensors
 * This function is being forwarded from the callback() function
 * 
 * @param topic incoming message topic
 * @param payload incoming message payload
 */
void DHTDevice::sendDataToSensor(const char *topic, byte *payload, unsigned int length) {
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
        this->publishDataToServer(true);
    }
}

/**
 * Collect data from sensors
 */
void DHTDevice::getDataFromSensor() {
	float temperature_c = dht->readTemperature(); // Get temperature value from DHT sensor
	if (isnan(temperature_c)) //If there was an error reading data from sensor then
		if (DEBUG)
			Serial.println("Failed to read temperature from DHT sensor!");

    float humidity = dht->readHumidity();
	if (isnan(humidity)) // If there was an error reading data from sensor then
		if (DEBUG)
			Serial.println("Failed to read humidity from DHT sensor!");

    StaticJsonDocument<256> jsonDoc;

    jsonDoc["temperature"] = temperature_c;
    jsonDoc["humidity"] = humidity;

    String output;
    serializeJson(jsonDoc, output);

    sensorPayload = output;

	if (DEBUG) {
	    Serial.print("Sensor values: ");
		Serial.println(sensorPayload);
	}


    if (initDisplay) {
        char message[sizeof(float)];
        String text = "Temp:\n" + String(temperature_c) + " (C)\n\nHumidity:\n" + String(humidity) + " %";
        ledDisplay->displaySensorData(text);
    }

    delay(500);
    //ESP.deepSleep(deepSleepDuration * 1000000);
}

DHTDevice::DHTDevice(const String& _deviceId, boolean _calibrationMode, boolean _initDisplay, boolean _initStatusLed, boolean _initBuzzer, boolean debug) {
        if (debug) {
        Serial.println("##################################");
        Serial.println("# initializing device " +  _deviceId);
        Serial.println("##################################");
    }

    deviceType = "dht"; // The devices type definition
    selfDeviceId = _deviceId;
    chipType = "ESP8266"; // The devices chip type
    deviceVersion = "1.0.0";

    initDisplay = _initDisplay;
    initStatusLed = _initStatusLed;
    initBuzzer = _initBuzzer;

    if (initDisplay) {
        ledDisplay = new Oled64x48Display(1, WHITE);
        ledDisplay->initDisplay();
        ledDisplay->displayLogo();
    }

    if (initStatusLed) {
        statusLed = new LedRgb(STATUS_LED_PIN, 1);
        statusLed->initLed();
    }

    if (initBuzzer) {
        alarmBuzzer = new Buzzer(BUZZER_PIN, BUZZER_FREQUENCY, 1000);
        alarmBuzzer->initBuzzer();
        alarmBuzzer->stopBuzzer();
    }
    dht = new DHT(DHT_PIN, DHT_TYPE, 11);
    dht->begin(); // initialize temperature sensor
    pinMode(CONFIGURATION_MODE_BUTTON, INPUT); // Setup Configuration mode button
}