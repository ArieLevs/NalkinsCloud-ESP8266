//
// Created by Arie Lev on 14/08/2021.
//

#ifndef NALKINSCLOUD_ESP8266_DEVICE_H
#define NALKINSCLOUD_ESP8266_DEVICE_H

// Define device GPIOs
#define CONFIGURATION_MODE_BUTTON D6 // Pin which define the button that sets the device to configuration mode

#define STATUS_LED_PIN D2
#define BUZZER_PIN D5
#define BUZZER_FREQUENCY 1000

#include "Arduino.h"
#include <ArduinoJson.h>
#include "buzzer.h"
#include "oled_display.h"
#include "mqtt_client.h"
#include "led_rgb.h"

class Device {
public:
    Device() = default;;

    void initSensor() {
        Serial.println("initSensor() function for device '" + selfDeviceId + "' must be overridden! doing nothing");
    };

    /**
     * Subscribe to all relevant sensors connected
     */
    void getSensorsInformation() {
        subscribeToMQTTBroker("v1/devices/me/rpc/request/+");
        Serial.println(
                "getSensorsInformation() function for device '" + selfDeviceId + "' must be overridden! doing nothing");
    };

    virtual /**
     * Collect data from sensors
     */
    void getDataFromSensor() {
        Serial.println("getDataFromSensor() function for device '" + selfDeviceId + "' must be overridden! doing nothing");
    };

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
     * Publish all data to the mqtt broker
     * in case 'force' is true, publish immediately, else wait for publish cycles (publishInterval)
     *
     * @param force boolean
     */
    void publishDataToServer(bool force) {
        if (force) {
            publishMessageToMQTTBroker( "v1/devices/me/telemetry", sensorPayload, false);
        } else {
            //For each sensor, check its last publish time
            unsigned long currentMillis = millis();
            if (currentMillis - previousPublish >= publishInterval) {
                previousPublish = currentMillis;
                publishMessageToMQTTBroker("v1/devices/me/telemetry", sensorPayload, false);
            }
        }
    }

private:
    //Set time to "delay" a publish message
    unsigned long previousPublish = 0;
    const unsigned long publishInterval = 120000; // interval at which to send message (milliseconds)

protected:
    String selfDeviceId;
    String deviceType;
    String chipType;

    StaticJsonDocument<256> jsonDoc;
    String sensorPayload;

    boolean initDisplay = false;
    Oled64x48Display *ledDisplay = nullptr;

    boolean initBuzzer = false;
    Buzzer *alarmBuzzer = nullptr;

    boolean initStatusLed = false;
    LedRgb *statusLed = nullptr;

    boolean debug = false;
};

#endif //NALKINSCLOUD_ESP8266_DEVICE_H
