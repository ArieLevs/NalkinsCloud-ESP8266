//
// Created by Arie Lev on 15/02/2019.
//

#include "http_server_handler.h"
#include "configs.h"
#include "eeprom_memory.h"
#include "http_server_content.h"
#include "wifi_handler.h"
#include "mqtt_client.h"
#include "sensors.h"


void handleClient() {
	server.handleClient();
}


void handleGeneralSaved() {
	server.on("/generalSaved", []() { // Small redirect page that indicates that general configs been saved
	    String mqttServer = server.arg("mqttServer");
        String devicePassword = server.arg("devicePassword");

        if (mqttServer.isEmpty() || devicePassword.isEmpty()) {
            if (DEBUG) {
                Serial.println("values cannot be empty");
            }
            server.send_P(200, "text/html", PAGE_GeneralSettingsSaveFailed);
            return;
        }
		if (DEBUG) {
            Serial.println("Running generalSaved.html page");
            Serial.print("Store port ");
            Serial.println(server.arg("mqttPort").toInt());
        }

        writeStringToEEPROM(DEVICE_PASS_LENGTH_START_ADDR, DEVICE_PASS_START_ADDR,devicePassword); //Save the password to EEPROM flash memory
        writeStringToEEPROM(MQTT_SERVER_LENGTH_START_ADDR, MQTT_SERVER_START_ADDR,mqttServer);
        writeIntToEEPROM(MQTT_PORT_START_ADDR, server.arg("mqttPort").toInt());

        server.send_P(200, "text/html", PAGE_GeneralSettingsSaved); //And display small success redirect page
	});
}

void handleAdminPage() {
	/*
	* Once http://DEVICEIP/ is accessed it will return HTML code, with main menu
	*/
	server.on("/", []() { //Main menu page
		if (DEBUG)
			Serial.println("Return admin.html page");
		server.send_P(200, "text/html", PAGE_AdminMainPage);
	});
}


void handleReturnId() {
	/*
   * Once http://DEVICEIP/return_id is accessed it will return devices ID and TYPE
   */
	server.on("/return_id", HTTP_POST, []() {
		if (DEBUG)
			Serial.println("JSON return_id initiated");
		StaticJsonDocument<256> newBuffer;
		// Get the request JSON body

		DeserializationError error = deserializeJson(newBuffer, server.arg("plain"));

//		JsonObject &jsonmsg = newBuffer.parseObject(server.arg("plain"));
//		jsonmsg.prettyPrintTo(wifiClientSecure);
//		serializeJsonPretty(server.arg("plain"), wifiClientSecure);
		if (DEBUG) {
			Serial.println("Message received:");
			Serial.println(server.arg("plain"));
		}
		// If message parse failed
		if (error) {
			if (DEBUG)
				Serial.println("Parsing failed");
			server.send(200, "text/plain",
						R"({"status":"failed","message":"message contains error(s), check json syntax"})");
			if (DEBUG)
				Serial.println("/return_id returned error");
		} else {
			server.send(200, "text/plain",
						(R"({"status":"success", "device_id":")" + deviceId + R"(", "device_type":")" +
						 deviceType + "\"}").c_str());
			if (DEBUG)
				Serial.println("/return_id returned success");
		}
		delay(500);
	});
}

void handleAutoConfig() {
	/*
   * Once http://DEVICEIP/autoconfig is accessed it will try to parse a json syntax request
   * The request should contain the wifi SSID and password, device password for connecting to MQTT broker, brokers IP/Domain address and port
   * If all conditions are met, store received data to EEPROM storage
   */
	server.on("/autoconfig", HTTP_POST, []() {
		if (DEBUG)
			Serial.println("/autoconfig was accessed");
		StaticJsonDocument<1024> newBuffer;
		// Get the request JSON body
		DeserializationError error = deserializeJson(newBuffer, server.arg("plain"));
//		JsonObject &jsonmsg = newBuffer.parseObject(server.arg("plain"));
//		jsonmsg.prettyPrintTo(wifiClientSecure);
//		serializeJsonPretty(server.arg("plain"), wifiClientSecure);
		if (DEBUG) {
			Serial.println("Message received:");
			Serial.println(server.arg("plain"));
		}
		// If message parse failed
		if (error) {
			if (DEBUG)
				Serial.println("Parsing failed");
			server.send(200, "text/plain",
						R"({"status":"failed","message":"message contains error(s), check json syntax"})");
		} else {
			// Get JSON to global variables
			configs.wifiSsid = (const char *) newBuffer["ssid"]; // Get SSID from JSON
			configs.wifiPassword = (const char *) newBuffer["wifi_pass"];
			configs.devicePassword = (const char *) newBuffer["device_pass"];
			configs.mqttServer = (const char *) newBuffer["mqtt_server"];
			configs.mqttPort = (uint8_t) newBuffer["mqtt_port"];
			//configs.mqttFingerprint = (const char*)jsonmsg["server_fingerprint"];

			if (DEBUG) {
				Serial.print("wifiSsid set: ");
				Serial.println(configs.wifiSsid.c_str());
				Serial.print("wifiPassword set: ");
				Serial.println(configs.wifiPassword.c_str());
				Serial.print("MQTT Server set: ");
				Serial.println(configs.mqttServer.c_str());
				Serial.print("MQTT Port set: ");
				Serial.println(configs.mqttPort);
				//Serial.print("MQTT Fingerprint set: ");
				//Serial.println(configs.mqttFingerprint);
				Serial.print("devicePassword set: ");
				Serial.println(configs.devicePassword.c_str());
				Serial.println("Running connection check from autoconfig");
			}
			// Connect to received wifi SSID and password
			if (connectToWifi()) {
				if (DEBUG)
					Serial.println("Wifi connection successful, Writing wifi configs to EEPROM");
				// If successful store wifi credentials
				writeStringToEEPROM(SSID_LENGTH_START_ADDR, SSID_START_ADDR, configs.wifiSsid);
				writeStringToEEPROM(WIFI_PASS_LENGTH_START_ADDR, WIFI_PASS_START_ADDR, configs.wifiPassword);

				if (DEBUG)
					Serial.println("Writing MQTT config ro EEPROM");
				// Store MQTT server info and credentials
				writeStringToEEPROM(MQTT_SERVER_LENGTH_START_ADDR, MQTT_SERVER_START_ADDR, configs.mqttServer);
				writeIntToEEPROM(MQTT_PORT_START_ADDR, configs.mqttPort);
				writeStringToEEPROM(DEVICE_PASS_LENGTH_START_ADDR, DEVICE_PASS_START_ADDR, configs.devicePassword); //Save the password to flash memory

				server.send(200, "text/plain",
							R"({"status":"success","message":"successfully connected to wifi"})");
				if (DEBUG)
					Serial.println("Autoconfig connection was successful, Restarting device");
				ESP.restart();
			}
			server.send(200, "text/plain", R"({"status":"failed","message":"could not connect to wifi"})");
			if (DEBUG)
				Serial.println("Autoconfig connection failed on WiFi connection");
		}
	});
}


void handleConfigSaved() {
	server.on("/configSaved", []() {
		if (DEBUG)
			Serial.println("Running configSaved.html page");
		for (int i = 0; i < server.args(); i++) { // For each value from network configs page, run on each one and save to 'configs' struct
			if (server.argName(i) == "ssid") configs.wifiSsid = server.arg(i).c_str();
			if (server.argName(i) == "password") configs.wifiPassword = server.arg(i).c_str();
			if (server.argName(i) == "ip_0")
				if (checkRange(server.arg(i)))
					configs.IP[0] = (uint8_t) (server.arg(i).toInt());
			if (server.argName(i) == "ip_1")
				if (checkRange(server.arg(i)))
					configs.IP[1] = (uint8_t) server.arg(i).toInt();
			if (server.argName(i) == "ip_2")
				if (checkRange(server.arg(i)))
					configs.IP[2] = (uint8_t) server.arg(i).toInt();
			if (server.argName(i) == "ip_3")
				if (checkRange(server.arg(i)))
					configs.IP[3] = (uint8_t) server.arg(i).toInt();
			if (server.argName(i) == "nm_0")
				if (checkRange(server.arg(i)))
					configs.Netmask[0] = (uint8_t) server.arg(i).toInt();
			if (server.argName(i) == "nm_1")
				if (checkRange(server.arg(i)))
					configs.Netmask[1] = (uint8_t) server.arg(i).toInt();
			if (server.argName(i) == "nm_2")
				if (checkRange(server.arg(i)))
					configs.Netmask[2] = (uint8_t) server.arg(i).toInt();
			if (server.argName(i) == "nm_3")
				if (checkRange(server.arg(i)))
					configs.Netmask[3] = (uint8_t) server.arg(i).toInt();
			if (server.argName(i) == "gw_0")
				if (checkRange(server.arg(i)))
					configs.Gateway[0] = (uint8_t) server.arg(i).toInt();
			if (server.argName(i) == "gw_1")
				if (checkRange(server.arg(i)))
					configs.Gateway[1] = (uint8_t) server.arg(i).toInt();
			if (server.argName(i) == "gw_2")
				if (checkRange(server.arg(i)))
					configs.Gateway[2] = (uint8_t) server.arg(i).toInt();
			if (server.argName(i) == "gw_3")
				if (checkRange(server.arg(i)))
					configs.Gateway[3] = (uint8_t) server.arg(i).toInt();

			if (server.argName(i) == "dhcp") {
				if (server.arg(i))
					configs.dhcp = true;
				else
					configs.dhcp = false;
			}
		}
		writeNetworkConfigs(); // Write network configurations to EEPROM flash memory
		server.send_P(200, "text/html", PAGE_NetworkConfigurationSaved); // Display small success redirect page
		WiFi.disconnect();
		if (connectToWifi()) // While still inside configuration menu, try to reconnect to wifi with new parameters
			if (DEBUG)
				Serial.println("configSaved page connected to wifi");
	});
}


/**
 * Starts HTTP server pages with relevant page content
 * Function provides an HTML content menu, and acts as an API to handle requests
 */
void startHTTPServer() {

	IPAddress mdnsIP = WiFi.localIP();

	MDNSResponder mdns;

	//Start mDNS service with the relevant IP
	if (mdns.begin(DNSName.c_str(), mdnsIP)) {
		if (DEBUG) {
			Serial.println("MDNS started successfully");
			Serial.println("Please use this URL to connect:");
			Serial.println("http://" + DNSName + "/");
		}
	} else if (DEBUG)
		Serial.println("MDNS failed to start");

	handleGeneralSaved();
	handleAutoConfig();
	handleReturnId();
	handleAdminPage();

	server.on("/generalSettings",
			  []() {  //General configs, will show device ID, version etc and allow storing new device password
				  if (DEBUG)
					  Serial.println("Return generalSettings.html page");
				  server.send_P(200, "text/html", PAGE_GeneralSettings);
			  });

	server.on("/configNetwork", []() {
		if (DEBUG)
			Serial.println("Running configNetwork.html page");
		server.send_P(200, "text/html", PAGE_NetworkConfiguration);
	});

	handleConfigSaved();

	/**
	 * Once http://DEVICEIP/info is accessed it will return information page containing
	 * Wifi connection status, SSID, IP, Netmask, Gateway and mac address
	 */
	server.on("/info", []() {
		Serial.println("info.html");
		server.send_P(200, "text/html", PAGE_Information);
	});

	/**
	 * Once http://DEVICEIP/restart is accessed it will restart the device
	 */
	server.on("/restart", []() {
		Serial.println("restart.html");
		server.send_P(200, "text/html", PAGE_Restart);
        delay(1000);
		ESP.restart();
	});

    /**
     * Once http://DEVICEIP/factoryReset is accessed it will restart the device
     */
    server.on("/factoryReset", []() {
        Serial.println("factoryReset.html");
        clearEEPROM();
        server.send_P(200, "text/html", PAGE_AdminMainPage);
    });

	server.on("/style.css", []() {
		Serial.println("style.css");
		server.send_P(200, "text/plain", PAGE_Style_css);
	});
	server.on("/microajax.js", []() {
		Serial.println("microajax.js");
		server.send_P(200, "text/plain", PAGE_microajax_js);
	});

	/**
	 * Bellow url calls are made from HTTP pages
	 * When client is to execute a function that should be run from the HTTP server (browser)
	 * The page is being redirected to the below URLs, each url will run the relevant function
	 */
	server.on("/admin/get_networks", webServerGetAvailableNetworks);
	server.on("/admin/info_values", webServerGetNetworkInfo);
	server.on("/admin/wifi_reconnect", webServerReconnectToWifi);
	server.on("/admin/mqtt_info_values", webServerGetMqttInfo);
	server.on("/admin/general_values", webServerGetGeneralConfigurationValues);

	server.onNotFound([]() {
		if (DEBUG)
			Serial.println("Page Not Found");
		server.send(400, "text/html", "Page not Found");
	});

	server.begin(); // Start the HTTP server

	if (DEBUG)
		Serial.println("HTTP Server Started");
}
