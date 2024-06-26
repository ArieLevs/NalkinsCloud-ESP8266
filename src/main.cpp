
#include <Arduino.h>

#include "eeprom_memory.h"
#include "mqtt_client.h"
#include "wifi_handler.h"
#include "functions.h"
#include "led_blinks.h"
#include "http_server_handler.h"
#include "global_configs.h"

// #include "device_cat_litter.h"
#include "device_dht.h"

strConfigs configs;
LedBlinks *ledBlink = nullptr;

// CatLitter *device = nullptr;
DHTDevice *device = nullptr;

void setup(void) {
	delay(500);
	initEEPROM();
	initConfigs();

    if (DEBUG) {
        Serial.begin(115200);
		Serial.println("");
        Serial.println("Starting NalkinsCloud Sensor");
    }

    // device = new CatLitter("048018f0-04aa-11ef-af4c-95c2f1842409", false, true, false, false, true);
	device = new DHTDevice("71562250-06f1-11ef-9f91-7d818d7f7044", false, true, false, false, true);
	ledBlink = new LedBlinks(LED_BUILTIN); // Init led blinker object LED_BUILTIN == D4 == GPIO 2

	
	//Get current network and other info configurations from flash memory
	readNetworkConfigs(); // This will also return the wifi SSID and password
	configs.deviceId = device->getDeviceId();
	configs.deviceType = device->getDeviceType();
	configs.chipType = device->getChipType();
	configs.deviceVersion = device->getDeviceVersion();
	configs.devicePassword = readStringFromEEPROM(DEVICE_PASS_LENGTH_START_ADDR, DEVICE_PASS_START_ADDR);
	configs.mqttServer = readStringFromEEPROM(MQTT_SERVER_LENGTH_START_ADDR, MQTT_SERVER_START_ADDR);
	configs.mqttPort = readIntFromEEPROM(MQTT_SERVER_PORT_START_ADDR);

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
	device->getDataFromSensor(); // Execute main work here
	if (getConfigurationMode()) { // If device on configuration mode then handle http server
		ledBlink->intervalBlink(500);
		handleClient();
	} else { // If device is on normal work mode
		//checkConfigurationButton(CONFIGURATION_MODE_BUTTON); // Check if conf button pressed for more than 5 seconds
		if (isWifiConnected()) {
			if (mqttClient.loop()) { // If MQTT client is connected to MQTT broker
                sendWifiSignalStrength();
                device->publishDataToServer(false); // Publish the all sensor messages
				ledBlink->intervalBlink(2500);
			} else {
                if (checkMQTTConnection()) { // Try to connect/reconnect
                    device->getSensorsInformation(); // Get all relevant sensor and start MQTT subscription
				}
                else
                    ledBlink->rapidIntervalBlink(2000);
			}
		} else { // Wifi handler is taking place at this point
			ledBlink->rapidIntervalBlink(2000); // if network is lost then connection to server must be lost
		}
	}
}