
#include <ArduinoJson.h>
#include "wifi_handler.h"
#include "global_configs.h"

//WIFI handlers
WiFiEventHandler wifiConnectedHandler;
WiFiEventHandler wifiDisconnectedHandler;

void onWifiConnect(WiFiEventStationModeGotIP eventInfo) {
	if (DEBUG) {
		Serial.println();
		Serial.print("\nConnected to: " + configs.wifiSsid);
		Serial.print("IP address: ");
		Serial.println(WiFi.localIP());
		WiFi.printDiag(Serial);
	}
}

void onWifiDisconnected(WiFiEventStationModeDisconnected eventInfo) {
	//WiFi.disconnect();
	if (DEBUG) {
		Serial.println("WiFi disconnected");
		Serial.print("Will reconnect once '");
		Serial.print(configs.wifiSsid.c_str());
		Serial.println("' is available again");
	}
}

/**
 * Connect to wifi network
 * If connection failed, or not established after 15 seconds return false, else return true
 * @return boolean  true - connected successfully
 *                  false - connection attempt last more then 15 seconds, or connection failed
 */
bool connectToWifi() {
	if (isWifiConnected())
		return true;
	if (DEBUG)
		Serial.println(
				"Starting WiFi connection to: " + configs.wifiSsid + "\nSelf MAC address: " + WiFi.macAddress());

	if (!configs.dhcp) // If DHCP was NOT selected during configuration, set STATIC IP configs
		WiFi.config(IPAddress(configs.IP[0], configs.IP[1], configs.IP[2], configs.IP[3]),
					IPAddress(configs.Gateway[0], configs.Gateway[1], configs.Gateway[2], configs.Gateway[3]),
					IPAddress(configs.Netmask[0], configs.Netmask[1], configs.Netmask[2], configs.Netmask[3]));

	// Set self host name
	WiFi.hostname("ESP8266");
	delay(50);

	WiFi.begin(configs.wifiSsid.c_str(), configs.wifiPassword.c_str());
	int counter = 0;
	while (counter < 30) { // 30 iterations * 500ms = 15 seconds timeout
		//If the connection was successful
		if (isWifiConnected()) {
			if (DEBUG) {
				Serial.println("\nWifi successfully connected");
				Serial.print("IP address: ");
				Serial.println(WiFi.localIP());
			}
			return true;
		}
		delay(500);
		if (DEBUG)
			Serial.print(".");
		counter++;
	}
	if (DEBUG)
		Serial.println("\nConnection timed out");
	return false;
}

/**
 * Reconnect to wifi network
 * Function perform disconnect command and calls connectToWifi() function
 */
void reconnectToWifi() {
	if (DEBUG) {
		Serial.println("Running reconnectToWifi() function");
		Serial.println("Disconnecting from wifi");
	}
	// Disconnect if connected
	WiFi.disconnect();
	connectToWifi();
}

/**
 * Check wifi connection status
 * 
 * @return boolean  true - wifi status is connected
 *                  false - wifi status is disconnected
 */
bool isWifiConnected() {
	return WiFi.status() == WL_CONNECTED;
}

/**
 * Configure device wifi to start up as client mode
 */
void setNormalOperationMode() {
	WiFi.mode(WIFI_STA);
}

/**
 * Configure device wifi to start up as Access Point AND client mode
 * Function will set default gateway address of 10.0.0.1/24
 */
void setConfigurationMode() {
	delay(50);
	WiFi.disconnect();
	delay(50);
	WiFi.mode(WIFI_AP_STA);
	delay(10);

	//Set custom IP\subnet configurations
	IPAddress ip(10, 0, 0, 1);
	IPAddress gateway(10, 0, 0, 1);
	IPAddress subnet(255, 255, 255, 0);
	WiFi.softAPConfig(ip, gateway, subnet);
	delay(10);

	//Start AP mode
	WiFi.softAP(ACCESS_POINT_SSID, ACCESS_POINT_PASSWORD);
	if (DEBUG) {
		Serial.print("\nSoftAP started, address is: ");
		Serial.println(WiFi.softAPIP());
	}
	delay(10);
}

/**
 * Initialize wifi handlers
 * Function onStationModeDisconnected should run when wifi connection lost
 * Function onStationModeGotIP should run when wifi connection established again
 */
void initializeWifiHandlers() {
	wifiConnectedHandler = WiFi.onStationModeGotIP(onWifiConnect);
	wifiDisconnectedHandler = WiFi.onStationModeDisconnected(onWifiDisconnected);
}


int getWifiRSSI() { // RSSI (Received Signal Strength Indicator)
	return WiFi.RSSI();
}

/** Convert RSSI to signal levels
 *  
 *  @return int  1 - Excellent, if RSSI is >= -50 dBm
 *               2 - Good, if RSSI is < -50 dBm and >= -60 dBm
 *               3 - Fair, if RSSI is < -60 dBm and >= -70 dBm
 *               4 - Weak, if RSSI is > -70 dBm
 */
String getWifiSignalStrength() {
	int wifiStrength = getWifiRSSI();
	int wifiQuality = dbmToQuality(wifiStrength);
	if (DEBUG) {
		Serial.print("Wifi signal strength (RSSI) is: ");
		Serial.print(wifiStrength);
		Serial.println(" dBm");
	}

    StaticJsonDocument<256> jsonDoc;
    jsonDoc["rssi"] = wifiStrength;
    jsonDoc["rssi_quality"] = wifiQuality;

	if (wifiStrength >= -50)
        jsonDoc["rssi_description"] = "excellent";
	else if (wifiStrength < -50 && wifiStrength >= -60)
        jsonDoc["rssi_description"] = "good";
	else if (wifiStrength < -60 && wifiStrength >= -70)
        jsonDoc["rssi_description"] = "fair";
	else if (wifiStrength < -70)
        jsonDoc["rssi_description"] = "weak";
	else
        jsonDoc["rssi_description"] = "no data";

    String result;
    serializeJson(jsonDoc, result);
    return result;
}

/**
 * Convert wifi signal strength to percentage between o to 100
 * @param dbm integer representing RSSI strength
 * @return integer
 */
int dbmToQuality(int dbm) {
    int quality;
    if (dbm <= -100) {
        quality = 0;
    } else if (dbm >= -50) {
        quality = 100;
    } else {
        quality = 2 * (dbm + 100);
    }
    return quality;
}

/**
 * Return current wifi SSID name
 * 
 * @return String current wifi SSID
 */
String getWifiSSID() {
	return WiFi.SSID();
}
