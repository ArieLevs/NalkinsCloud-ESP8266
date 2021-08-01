
#include <Arduino.h>

#include "eeprom_memory.h"
#include "mqtt_client.h"
#include "wifi_handler.h"
#include "functions.h"
#include "led_blinks.h"
#include "http_server_handler.h"

#include "sensors.h" // Load sensors global variables // THIS SHOULD BE CHANGED ON DEVICE CHANGE

strConfigs configs;
LedBlinks *ledBlink = nullptr;

void setup(void) {
	delay(500);
	initEEPROM();
	initConfigs();

    if (DEBUG) {
        Serial.begin(115200);
        Serial.println("Starting NalkinsCloud Sensor");
    }

	initSensor(); // Setup device GPIOs

	ledBlink = new LedBlinks(LED_PIN); // Init led blinker object

	//Get current network and other info configurations from flash memory
	readNetworkConfigs(); // This will also return the wifi SSID and password
	configs.devicePassword = readStringFromEEPROM(DEVICE_PASS_LENGTH_START_ADDR, DEVICE_PASS_START_ADDR);
	configs.mqttServer = readStringFromEEPROM(MQTT_SERVER_LENGTH_START_ADDR, MQTT_SERVER_START_ADDR);
	configs.mqttPort = readIntFromEEPROM(MQTT_PORT_START_ADDR);

	if (DEBUG) {
		Serial.println("Server configs: ");
		Serial.print("Password: ");
		Serial.println(configs.devicePassword.c_str());
		Serial.print("MQTT Host: ");
		Serial.println(configs.mqttServer.c_str());
		Serial.print("MQTT Port: ");
		Serial.println(configs.mqttPort);
		Serial.print("MQTT Fingerprint: ");
		Serial.println(configs.mqttFingerprint);
	}

	initMqttClient();

	setConfigurationMode(getConfigurationStatusFlag()); // Get current device work mode (Normal or configuration)
	setConfigurationStatusFlag(0); // Set the configuration mode flag back to false (Set to normal work mode)

	if (!getConfigurationMode()) { // If The device in NOT on configuration mode
		setNormalOperationMode(); // Set wifi to WIFI_STA
		connectToWifi();
		if (isWifiConnected()) {
			if (connectToMQTTBroker()) {
                disconnectFromMQTTBroker();
                if (DEBUG)
                    Serial.println("Connection tests passed successfully");
                //WiFi.setAutoReconnect(false);
                initializeWifiHandlers();
                return; // All tests successful start main loop when NOT on configuration mode (isConfigurationMode = FALSE)
			} else if (DEBUG)
				Serial.println("MQTT Server Connection failed");
		} else if (DEBUG) {
			Serial.println("Wifi failed connecting to: ");
			Serial.print(configs.wifiSsid.c_str());
		}
		setConfigurationMode(true); // If one of the 3 checks failed, start on configuration mode
	}
	if (DEBUG)
		Serial.println("Starting configuration mode");

	//Start Wifi in configuration mode
	setConfigurationMode(); // Set wifi to WIFI_AP_STA
	startHTTPServer(); // Start the web server with AP content
}


void loop(void) {
	delay(50);
	getDataFromSensor(); // Execute main work here
	if (getConfigurationMode()) { // If device on configuration mode then handle http server
		ledBlink->intervalBlink(500);
		handleClient();
	} else { // If device is on normal work mode
		//checkConfigurationButton(CONFIGURATION_MODE_BUTTON); // Check if conf button pressed for more than 5 seconds
		if (isWifiConnected()) {
			if (mqttClient.loop()) { // If MQTT client is connected to MQTT broker
                sendWifiSignalStrength();
				publishDataToServer(false); // Publish the all sensor messages
				ledBlink->intervalBlink(2500);
			} else {
                if (checkMQTTConnection()) // Try to connect/reconnect
                    getSensorsInformation(); // Get all relevant sensor and start MQTT subscription
                else
                    ledBlink->rapidIntervalBlink(2000);
			}
		} else { // Wifi handler is taking place in this point
			if (isClientConnectedToMQTTServer())
				disconnectFromMQTTBroker();
			ledBlink->rapidIntervalBlink(2000);
		}
	}
} 
