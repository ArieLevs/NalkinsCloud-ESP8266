
#include "Arduino.h"
#include <ArduinoJson.h>
#include "sensors.h"
#include "global_configs.h"
#include "functions.h"
#include "mqtt_client.h"
#include "oled_display.h"
#include "device.h"
#include "led_rgb.h"

#include "HX711.h"

float calibration_factor = -19510; //-7050 worked for my 440lb max scale setup

// HX711 circuit pins set
#define LOADCELL_DOUT_PIN D7
#define LOADCELL_SCK_PIN D8
#define STATUS_LED_PIN D2
#define RESET_BTN_PIN D3

boolean calibrationMode = false;

#define deepSleepDuration 300 // Seconds

StaticJsonDocument<256> jsonDoc;
String sensorPayload;

HX711 scale;

Oled64x48Display *oledDisplay = nullptr;
LedRgb *statusLed = nullptr;

boolean checkResetButton(uint8_t pinNum) {
    if (digitalRead(pinNum) == HIGH) { // If button is pressed then
        scale.tare();
        return true;
    }
    return false;
}

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
    float scale_read_value;

    checkResetButton(RESET_BTN_PIN);

    if (calibrationMode) {
        scale.set_scale(calibration_factor); //Adjust to this calibration factor
        if (DEBUG)
            Serial.println("calibration_factor: " + String(calibration_factor));

        if(Serial.available())
        {
            char temp = Serial.read();
            if(temp == '+' || temp == 'a')
                calibration_factor += 10;
            else if(temp == '-' || temp == 'z')
                calibration_factor -= 10;
        }
    }

    if (scale.wait_ready_retry(10, 5))
        scale_read_value = scale.get_units(3); // Get value from HX711 sensor
    else
        if (DEBUG)
            Serial.println("HX711 not found.");
    if (DEBUG) {
        Serial.println("HX711 reading: " + String(scale_read_value) + " kg");
    }

    jsonDoc["weight_kg"] = scale_read_value;

    serializeJson(jsonDoc, sensorPayload);

    // convert scale_read_value from KG to grams
    statusLed->showColorRange(200, static_cast<int>(scale_read_value*1000), 500);

    char message[sizeof(float)];
    String text = dtostrf(scale_read_value, 4, 2, message); // Arduino based function converting float to string
//    oledDisplay->displaySensorData(text);
    delay(5);
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
    if (DEBUG) {
        Serial.println("##################################");
        Serial.println("# initializing cats litter scale #");
        Serial.println("##################################");
    }

	oledDisplay = new Oled64x48Display(1, WHITE);
	oledDisplay->initDisplay();
	oledDisplay->displayLogo();

    statusLed = new LedRgb(STATUS_LED_PIN, 1);
    statusLed->initLed();

	deviceType = "HX711"; // The devices type definition
	deviceId = "test_hx711_device_id"; // The devices unique id
	chipType = "ESP8266"; // The devices chip type

    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN); // init weight sensor

    if (calibrationMode)
        scale.set_scale();
    else
        scale.set_scale(calibration_factor);
    long zero_factor = 0;
    if (DEBUG)
        Serial.println("Trying to find HX711");
    if (not scale.wait_ready_retry(20, 50)) {
        if (DEBUG)
            Serial.println("HX711 not found.");
    } else {
        if (DEBUG)
            Serial.println("HX711 found, getting zero factor values");
        zero_factor = scale.read_average(); //Get a baseline reading
        scale.tare();
    }

    if (DEBUG) {
        Serial.println(">>> Zero factor: " + String(zero_factor) + " <<<");
    }
	pinMode(CONFIGURATION_MODE_BUTTON, INPUT); // Setup Configuration mode button
}
