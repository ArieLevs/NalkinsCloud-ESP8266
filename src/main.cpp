
#include <string>
#include <Arduino.h>

#include "eeprom_memory.h"
#include "mqtt_client.h"
#include "wifi_handler.h"
#include "functions.h"
#include "led_blinks.h"
#include "http_server_handler.h"

#include "sensors.h" // Load sensors global variables // THIS SHOULD BE CHANGED ON DEVICE CHANGE

strConfigs configs;

void setup(void) {
  delay(500);
  if (DEBUG) {
    Serial.begin(115200);
    Serial.println("Starting NalkinsCloud Sensor");
  }

  initConfigs();
  initSensor(); // Setup device GPIOs
    
  initEEPROM();
  //clearEEPROM();  // Unmark to remove all data from EEPROM
    
  //Get current network and other info configurations from flash memory
  readNetworkConfigs(); // This will also return the wifi SSID and password
  configs.clientUsername = readStringFromEEPROM(USERNAMESTARTADDR);
  configs.devicePassword = readStringFromEEPROM(DEVICEPASSSTARTADDR);
  configs.mqttServer = readStringFromEEPROM(MQTTSERVERSTARTADDR);
  configs.mqttPort = (uint16_t)atoi(readStringFromEEPROM(MQTTPORTSTARTADDR).c_str());
  //configs.mqttFingerprint = readFingerprintFromEEPROM(MQTTFINGERPRINTSTARTADDR);

  /**
   * TEMPORATY
   */
  configs.mqttServer = "10.0.1.1";
  configs.mqttPort = 8883;
  configs.clientUsername = "test_dht_device";
  configs.devicePassword = "12345678";
  if (DEBUG) {
    Serial.println("Server configs: ");
    Serial.print("Username: ");
    Serial.println(configs.clientUsername.c_str());
    Serial.print("Password: ");
    Serial.println(configs.devicePassword.c_str());
    Serial.print("MQTT Host: ");
    Serial.println(configs.mqttServer.c_str());
    Serial.print("MQTT Port: ");
    Serial.println(configs.mqttPort);
    Serial.print("MQTT Fingerprint: ");
    Serial.println(configs.mqttFingerprint);
  }

  // Define general topic for this device
  generalTopic = deviceId + "/" + deviceType + "/";
  if (DEBUG) { 
    Serial.print("General topic is: ");
    Serial.println(generalTopic.c_str());
  }

  initMqttClient();

  setConfigurationMode(getConfigurationStatusFlag()); // Get current device work mode (Normal or configuraion)
  setConfigurationStatusFlag(0); // Set the configuration mode flag back to false (Set to normal work mode)

  if(!getConfigurationMode()) { // If The device in NOT on configuration mode
    setNormalOperationMode(); // Set wifi to WIFI_STA
    connectToWifi();
    if(isWifiConnected()) {
      if(connectToMQTTBroker()) {
        if(checkMQTTSSL()) { // Verify MQTT server certificate fingerprint
          disconnectFromMQTTBroker();
          if (DEBUG)
            Serial.println("Connection tests passed successfully");
          //WiFi.setAutoReconnect(false);
          initializeWifiHandlers();
          return; // All tests successfull start main loop when NOT on configuration mode (isConfigurationMode = FALSE)
        } else
            if (DEBUG)
              Serial.println("MQTT SSL Verification failed");
      } else
          if (DEBUG)
            Serial.println("MQTT Server Connection failed");
    } else
        if (DEBUG) {
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
  if (getConfigurationMode()) { // If device on configuration mode then handle http server
    blinkConfigurationMode(LED_WORK_STATUS);
    handleClient();
  }
  else { // If device is on normal work mode
    checkConfigurationButton(CONFIGURATIONMODEBUTTON); // Check if user is pressing the configuration button for more than 5 seconds
    if (isWifiConnected()) {
      if (mqttClient.loop()) { // If MQTT client is connected to MQTT broker
          //sendWifiSignalStrength();
          getDataFromSensor(); // Get data from sensor(s) and Publish the message
          blinkWorkMode(LED_WORK_STATUS);
      }else {
        if (checkMQTTConnection()) { // Try to connect/reconnect
            getSensorsInformation(); // Get all relevant sensor and start MQTT subscription
        }
      }
    } else {
      // TODO add doOfflineWork here
      if (DEBUG)
        Serial.println("Wifi handler is taking place");
      if (isClientConnectedToMQTTServer())
        disconnectFromMQTTBroker();
      blinkWifiDisconnected(LED_WORK_STATUS);
    }
  }
} 

