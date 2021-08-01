
#include "Arduino.h"

#include "mqtt_client.h"
#include "configs.h"
#include "sensors.h"
#include "wifi_handler.h"

PubSubClient mqttClient;
WiFiClient wifiClient;

void initMqttClient() {
    if (DEBUG) {
        Serial.print("Initializing mqtt client, isSslEncrypted status is: ");
        Serial.println(isSslEncrypted);
    }
    // TODO figure out the secure client issue
//	wifiClientSecure = WiFiClientSecure();
//	if (isSslEncrypted)
//        wifiClientSecure.setFingerprint(fingerprint);
//	else {
//        if (DEBUG) {
//            Serial.print("[WARNING] setting insecure wificlient");
//        }
//        wifiClientSecure.setInsecure();
//    }

    wifiClient = WiFiClient();

	mqttClient = PubSubClient(configs.mqttServer.c_str(), configs.mqttPort, callback, wifiClient);
	delay(10);
}


/**
 * Subscribe client to topic
 * 
 * @param topic the topic to subscribe tp
 */
void subscribeToMQTTBroker(const char *topic) {
	mqttClient.subscribe(topic, QOS);
	if (DEBUG) {
		Serial.print("Device subscribed to topic: ");
		Serial.println(topic);
	}
	delay(500);
}


/**
 * Publish message to MQTT server
 * 
 * @param topic the topic to publish message to
 * @param message the payload to send to server
 * @param isRetainedMessage indicate if the send message will be retained or not
 * 
 * @return boolean  true - successfully published message to server
 *                  false - failed to publish message
 */
bool publishMessageToMQTTBroker(const String& topic, const String& message, bool isRetainedMessage) {
	// publish function defined here https://pubsubclient.knolleary.net/api#publish
	// 'retained' is a constant, se in ConfigFile.h
	if (DEBUG) {
		Serial.print("Topic: ");
		Serial.print(topic);
		Serial.print(" | Message: ");
		Serial.println(message);
	}
	if (mqttClient.publish(topic.c_str(), message.c_str(), isRetainedMessage)) {
		if (DEBUG)
			Serial.println("Successfully published!");
		return true;
	} else {
		if (DEBUG)
			Serial.println("Failed publish!");
		return false;
	}
}


/**
 * mqtt callback function, called when new messages arrives
 * 
 * @param topic incoming message topic
 * @param payload incoming message payload
 * @param length integer value of payload string length
 */
void callback(const char *topic, byte *payload, unsigned int length) {
	if (DEBUG) {
		Serial.println("Message arrived:");
		Serial.print("Topic: ");
		Serial.println(topic);
		Serial.print("Payload: ");
		//For each char in message body, do
		for (unsigned int i = 0; i < length; i++)
			Serial.print((char) payload[i]);
		Serial.println("");
	}
	delay(10);
	// const char *topic, byte *payload, unsigned int length
	sendDataToSensor(topic, payload, length);
}


/**
 * Perform connection to MQTT broker and verification of SSL fingerprint
 * 
 * @return boolean  true - connection AND verifications passed successfully
 *                  false - connection OR verification failed
 */
bool checkMQTTConnection() {
	if (DEBUG)
		Serial.println("Attempting MQTT connection...");
	if (connectToMQTTBroker()) { // Try to connect to the MQTT server
		if (DEBUG)
			Serial.print("Successfully connected as: " + (String) deviceId.c_str());
        return true;
	} else {
		if (DEBUG)
			Serial.println("Connection failed, Error = " + (String) mqttClient.state());
		delay(2000);
	}
	return false;
}


/**
 * Translate connection return code to String description
 *
 * @param return_code, the return code from mosquitto broker
 * @return String, with description of the return code
 * 		as described here: https://pubsubclient.knolleary.net/api.html#state
 *
 * Available return status will be translated at function 'printMqttConnectionStatus'
 * -4 : MQTT_CONNECTION_TIMEOUT - the server didn't respond within the keepalive time
 * -3 : MQTT_CONNECTION_LOST - the network connection was broken
 * -2 : MQTT_CONNECT_FAILED - the network connection failed
 * -1 : MQTT_DISCONNECTED - the client is disconnected cleanly
 * 0 : MQTT_CONNECTED - the client is connected
 * 1 : MQTT_CONNECT_BAD_PROTOCOL - the server doesn't support the requested version of MQTT
 * 2 : MQTT_CONNECT_BAD_CLIENT_ID - the server rejected the client identifier
 * 3 : MQTT_CONNECT_UNAVAILABLE - the server was unable to accept the connection
 * 4 : MQTT_CONNECT_BAD_CREDENTIALS - the username/password were rejected
 * 5 : MQTT_CONNECT_UNAUTHORIZED - the client was not authorized to connect
 */
String printMqttConnectionStatus(int return_code) {

	switch (return_code) {
		case -4 :
			return "MQTT_CONNECTION_TIMEOUT (-4) - the server didn't respond within the keepalive time";
		case -3 :
			return "MQTT_CONNECTION_LOST (-3) - the network connection was broken";
		case -2 :
			return "MQTT_CONNECT_FAILED (-2) - the network connection failed";
		case -1 :
			return "MQTT_DISCONNECTED (-1) - the client is disconnected cleanly";
		case 0 :
			return "MQTT_CONNECTED (0) - the client is connected";
		case 1 :
			return "MQTT_CONNECT_BAD_PROTOCOL (1) - the server doesn't support the requested version of MQTT";
		case 2 :
			return "MQTT_CONNECT_BAD_CLIENT_ID (2) - the server rejected the client identifier";
		case 3 :
			return "MQTT_CONNECT_UNAVAILABLE (3) - the server was unable to accept the connection";
		case 4 :
			return "MQTT_CONNECT_BAD_CREDENTIALS (4) - the username/password were rejected";
		case 5 :
			return "MQTT_CONNECT_UNAUTHORIZED (5) - the client was not authorized to connect";
		default:
			return "ERROR - return code was not found";
	}
}

/**
 * Connect to MQTT server
 * 
 * @return boolean  true - connected successfully 
 *                  false - connection failed
 */
bool connectToMQTTBroker() {
	// This is a LWT message, that will be sent if device loss connection to the server
	const char *willMessage = "offline";
	if (DEBUG) {
		Serial.println("Trying connection to MQTT broker");
		Serial.println("connecting to: '" + configs.mqttServer + "'");
		Serial.println("port: '" +String(configs.mqttPort)+ "'");
		Serial.print("deviceId: '" + deviceId + "'");
		Serial.print("Password: ");
		Serial.println(configs.devicePassword.c_str());
	}
	// boolean connect (clientID, username, password, willTopic, willQoS, willRetain, willMessage)
    // function api https://pubsubclient.knolleary.net/api#connect
	if (mqttClient.connect(deviceId.c_str(), configs.devicePassword.c_str(), nullptr,
						   "v1/devices/me/attributes", QOS, retained, willMessage)) {
		if (DEBUG) {
			Serial.print("Connection successful, status: ");
			Serial.println(printMqttConnectionStatus(mqttClient.state()));
		}
		// If connected, send message with status 'online'
        /* online status is no longer needed as server attributes contain 'active' status attribute
		StaticJsonDocument<256> jsonDoc;
        jsonDoc["online"] = true;
        String status;
        serializeJson(jsonDoc, status);
        publishMessageToMQTTBroker("v1/devices/me/attributes", status, retained);
        */
		return true;
	}
	if (DEBUG) {
		Serial.print("Connection failed, status: ");
		Serial.println(printMqttConnectionStatus(mqttClient.state()));
	}
	return false;
}


/**
 * Disconnect from MQTT server
 * Also publish a message of 'offline' for the status topic
 */
void disconnectFromMQTTBroker() {
	//Before disconnecting, send message with status 'offline'
    /* offline status is no longer needed as server attributes contain 'active' status attribute
    StaticJsonDocument<256> jsonDoc;
    jsonDoc["online"] = false;
    String status;
    serializeJson(jsonDoc, status);
    publishMessageToMQTTBroker("v1/devices/me/attributes", status, retained);
    */
	mqttClient.disconnect();
}


/**
 * Check if client is connected to MQTT server
 * 
 * @return boolean  true - client is connected
 *                  false - client is not connected
 */
bool isClientConnectedToMQTTServer() {
	return mqttClient.connected();
}


//Set time to "delay" a wifi signal strength publish message
unsigned long wifiSignalPreviousPublish = 0;
const long wifiSignalPublishInterval = 300000; // interval at which to send message (milliseconds)


/**
 * Send wifi signal strength to broker
 * Function will publish RSSI level to MQTT broker
 *
 * @param topic, topic to send rssi data to
 */
void sendWifiSignalStrength() {
	unsigned long currentMillis = millis();
	if (currentMillis - wifiSignalPreviousPublish >= wifiSignalPublishInterval) {
		wifiSignalPreviousPublish = currentMillis;
		publishMessageToMQTTBroker("v1/devices/me/attributes", getWifiSignalStrength(), notRetained);
	}
}
