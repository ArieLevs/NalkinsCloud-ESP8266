#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#include "ConfigFile.h" //Configuration file

#include "HandleSensors_dht.h" // Load sensors global variables // THIS SHOULD BE CHANGED ON DEVICE CHANGE

ESP8266WebServer server(80); 
WiFiClientSecure wifiClientSecure;
PubSubClient mqttClient;

MDNSResponder mdns;

boolean isConfigurationMode;

void setup(void) {
  delay(500);
  if (DEBUG)
    Serial.begin(115200);

  setupSensorsGPIOs(); // Setup device GPIOs
    
  EEPROM.begin(512); // Initiate flash rom with 512 bytes
  //clearEEPROM();  // Unmark to remove all data from EEPROM
    
  //Get current network and other info configurations from flash memory
  readNetworkConfigs(); // This will also return the wifi SSID and password
  //configs.clientUsername = readStringFromEEPROM(USERNAMESTARTADDR);
  configs.devicePassword = readStringFromEEPROM(DEVICEPASSSTARTADDR);
  configs.mqttServer = readStringFromEEPROM(MQTTSERVERSTARTADDR);
  configs.mqttPort = readStringFromEEPROM(MQTTPORTSTARTADDR);
  //configs.mqttFingerprint = readFingerprintFromEEPROM(MQTTFINGERPRINTSTARTADDR);
  
  if (DEBUG) {
    Serial.println("Server configs: ");
    //Serial.print("Username: ");
    //Serial.println(configs.clientUsername);
    Serial.print("Password: ");
    Serial.println(configs.devicePassword);
    Serial.print("MQTT Host: ");
    Serial.println(configs.mqttServer);
    Serial.print("MQTT Port: ");
    Serial.println(configs.mqttPort);
    //Serial.print("MQTT Fingerprint: ");
    //Serial.println(configs.mqttFingerprint);
  }

  // Define general topic for this device
  generalTopic = deviceId + "/" + deviceType + "/";
  if (DEBUG) { 
    Serial.print("General topic is: ");
    Serial.println(generalTopic);
  }
    
  mqttClient = PubSubClient((char*)configs.mqttServer.c_str(), configs.mqttPort.toInt(), callback, wifiClientSecure); //Setup the MQTT client
  delay(10);

  isConfigurationMode = getConfigurationStatusFlag(); // Get current device work mode (Normal or configuraion)
  setConfigurationStatusFlag(0); // Set the configuration mode flag back to false (Set to normal work mode)

  if(!isConfigurationMode) { // If The device in NOT on configuration mode
    setNormalOperetionMode(); // Set wifi to WIFI_STA
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
        if (DEBUG)
          Serial.println("Wifi failed connecting to: " + configs.wifiSsid);
    isConfigurationMode = true; // If one of the 3 checks failed, start on configuration mode 
  }
  if (DEBUG)
    Serial.println("Starting configuration mode");
    
  //Start Wifi in configuration mode
  setConfigurationMode(); // Set wifi to WIFI_AP_STA
  startHTTPServer(); // Start the web server with AP content
}


void loop(void){
  delay(50);
  if (isConfigurationMode) { // If device on configuration mode then handle http server
    blinkConfigurationMode();
    server.handleClient();
  }
  else { // If device is on normal work mode
    checkConfigurationButton(); // Check if user is pressing the configuration button for more than 5 seconds
    if (isWifiConnected()) {
      if (mqttClient.loop()) { // If MQTT client is connected to MQTT broker
          //sendWifiSignalStrength();
          getDataFromSensor(); // Get data from sensor(s) and Publish the message
          blinkWorkMode();
      }else {
        if (checkMQTTconnection()) { // Try to connect/reconnect
            getSensorsInformation(); // Get all relevant sensor and start MQTT subscription
        }
      }
    } else {
      if (DEBUG)
        Serial.println("Wifi handler is taking place");
      if (isClientConectedToMQTTServer())
        disconnectFromMQTTBroker();
      blinkWifiDisconnected();
    }
  }
} 

