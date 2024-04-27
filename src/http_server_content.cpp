
#include <Arduino.h>
#include "mqtt_client.h"
#include "functions.h"
#include "http_server_handler.h"

ESP8266WebServer server;


// Called when 'PAGE_GeneralSettings' is built (/)
// Will send all relevant info to be displayed on page
void webServerGetGeneralConfigurationValues() {
	configs.mqttServer = readStringFromEEPROM(MQTT_SERVER_LENGTH_START_ADDR, MQTT_SERVER_START_ADDR); // Read device mqttServer from EEPROM and store to 'configs' struct
    configs.mqttPort = readIntFromEEPROM(MQTT_PORT_START_ADDR); // Read device mqttPort from EEPROM and store to 'configs' struct
    configs.devicePassword = readStringFromEEPROM(DEVICE_PASS_LENGTH_START_ADDR, DEVICE_PASS_START_ADDR); // Read device password from EEPROM and store to 'configs' struct
	String values;
	values += "version|" + configs.deviceVersion + "|input\n";
	values += "model|" + configs.chipType + "|input\n";
	values += "deviceId|" + configs.deviceId + "|input\n";
	values += "deviceType|" + configs.deviceType + "|input\n";
	values += "devicePassword|" + configs.devicePassword + "|input\n";
    values += "mqttServer|" + configs.mqttServer + "|input\n";
    values += "mqttPort|" + String(configs.mqttPort) + "|input\n";
	server.send(200, "text/plain", values.c_str());
	Serial.println(__FUNCTION__);
}


// Called when 'PAGE_Information' is built
// Will send all relevant info to be displayed on page
void webServerGetNetworkInfo() {
	connectToWifi(); // Try connecting to wifi
	String state = "N/A";
	if (WiFi.status() == 0) state = "Idle";
	else if (WiFi.status() == 1) state = "NO SSID AVAILBLE";
	else if (WiFi.status() == 2) state = "SCAN COMPLETED";
	else if (WiFi.status() == 3) state = "CONNECTED";
	else if (WiFi.status() == 4) state = "CONNECT FAILED";
	else if (WiFi.status() == 5) state = "CONNECTION LOST";
	else if (WiFi.status() == 6) state = "DISCONNECTED";

	String values;

	values += "x_ssid|" + WiFi.SSID() + "|div\n";
	values += "x_ip|" + (String) WiFi.localIP()[0] + "." + WiFi.localIP()[1] + "." + WiFi.localIP()[2] + "." +
			  WiFi.localIP()[3] + "|div\n";
	values += "x_gateway|" + (String) WiFi.gatewayIP()[0] + "." + (String) WiFi.gatewayIP()[1] + "." +
			  (String) WiFi.gatewayIP()[2] + "." + (String) WiFi.gatewayIP()[3] + "|div\n";
	values += "x_netmask|" + (String) WiFi.subnetMask()[0] + "." + (String) WiFi.subnetMask()[1] + "." +
			  (String) WiFi.subnetMask()[2] + "." + (String) WiFi.subnetMask()[3] + "|div\n";
	values += "x_mac|" + macToStr(getMacAddress()) + "|div\n";
	values += "wifi_connection_state|" + state + "|div\n";
	server.send(200, "text/plain", values.c_str());
	if (DEBUG)
		Serial.println(__FUNCTION__);
}

void webServerReconnectToWifi() {
	reconnectToWifi();
	webServerGetNetworkInfo();
}

// Called when 'PAGE_Information' is built
// Will send all relevant info to be displayed on page
void webServerGetMqttInfo() {
	String mqttState = "N/A";
	String sslState = "N/A";
	if (connectToMQTTBroker()) { //Try to connect to MQTT server
		mqttState = "CONNECTED";
		// TODO add TLS verification
		//if (checkMQTTSSL()) //Try to verify MQTT server
		//	sslState = "VERIFIED";
		//else
		//	sslState = "NOT SECURED!!!";
	} else
		mqttState = "DISCONNECTED";
	mqttClient.disconnect(); //Disconnect from the MQTT server connected before

	String values;
	values += "mqttServer|" + configs.mqttServer + "|div\n";
	values += "mqttport|" + (String) configs.mqttPort + "|div\n";
	values += "mqttdeviceid|" + configs.deviceId + "|div\n";
	values += "mqttpassword|" + configs.devicePassword + "|div\n";
	values += "mqttconnectionstate|" + mqttState + "|div\n";
	if (isSslEncrypted) {
		values += "fingerprint|" + (String)fingerprint + "|div\n";
		values += "encryptedconnection|True|div\n";
	}
	else {
		values += "fingerprint|Not Used|div\n";
		values += "encryptedconnection|False|div\n";
	}
	values += "sslconnection|" + sslState + "|div\n";
	server.send(200, "text/plain", values.c_str());
	if (DEBUG)
		Serial.println(__FUNCTION__);
}


/**
 * Update network states
 */
void webServerGetAvailableNetworks() {
	String state = "N/A";
	String Networks;
	if (WiFi.status() == 0) state = "Idle";
	else if (WiFi.status() == 1) state = "NO SSID AVAILBLE";
	else if (WiFi.status() == 2) state = "SCAN COMPLETED";
	else if (WiFi.status() == 3) state = "CONNECTED";
	else if (WiFi.status() == 4) state = "CONNECT FAILED";
	else if (WiFi.status() == 5) state = "CONNECTION LOST";
	else if (WiFi.status() == 6) state = "DISCONNECTED";
	Serial.println("Scanning networks");
	int n = WiFi.scanNetworks();

	if (n == 0) {
		Networks = "<font color='#FF0000'>No networks found!</font>";
	} else {
		Networks = "Found " + (String) n + " Networks<br>";
		Networks += "<table border='0' cellspacing='0' cellpadding='3'>";
		Networks += "<tr bgcolor='#DDDDDD' ><td><strong>Name</strong></td><td><strong>Quality</strong></td><td><strong>Enc</strong></td><tr>";
		for (int i = 0; i < n; ++i) {
			if (DEBUG)
				Serial.println("discovered network: " + String(WiFi.SSID(i)));
			int quality = dbmToQuality(WiFi.RSSI(i));

			Networks +=
					"<tr><td><a href='javascript:selssid(\"" + String(WiFi.SSID(i)) + "\")'>" + String(WiFi.SSID(i)) +
					"</a></td><td>" + String(quality) + "%</td><td>" +
					String((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*") + "</td></tr>";
		}
		Networks += "</table>";
	}

	String values;
	values += "wifi_connection_state|" + (String) state + "|div\n";
	values += "networks|" + Networks + "|div\n";
	server.send(200, "text/plain", values.c_str());
	if (DEBUG)
		Serial.println(__FUNCTION__);
}
