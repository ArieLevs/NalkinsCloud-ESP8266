/*
 * Handles MQQT messaging
 */

/**
 * Subscribe client to topic
 * 
 * @param topic the topic to subscribe tp
 */
void subscribeToMQTTBroker(const char* topic) {
  mqttClient.subscribe(topic, QOS);
  if (DEBUG) {
    Serial.print("Device subscribed to topic: ");
    Serial.println(topic);
  }
  delay(500);
}


// 
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
boolean publishMessageToMQTTBroker(const char* topic, char* message, boolean isRetainedMessage) {
  // publish function is: int publish (topic, payload, length, retained)
  // 'retained' is a constant, se in ConfigFile.h
  if (mqttClient.publish(topic, message, isRetainedMessage)) {
    if (DEBUG) {
      Serial.print("Published: ");
      Serial.print(topic);
      Serial.print(", Message: ");
      Serial.println(message);
    }
    return true;
  } else {
    if (DEBUG) {
      Serial.println("Publish failed");
      Serial.print("Topic: ");
      Serial.println(topic);
      Serial.print("Message: ");
      Serial.println(message);
    }
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
void callback(const char* topic, byte* payload, unsigned int length) {
  if (DEBUG) {
    Serial.println("Message arrived - ");
    Serial.print("Topic: ");
    Serial.println(topic);
    Serial.print("Payload: ");
    //For each char in message body, do
    for (unsigned int i=0; i<length ;i++) {
      //Read current char and print
      Serial.print((char)payload[i]);
    }
    Serial.println("");
  }
  delay(10);
  sendDataToSensor(topic, payload);
}


// Perform connection to and verification of MQTT broker
/**
 * Perform connection to MQTT broker and verification of SSL fingerprint
 * 
 * @return boolean  true - connection AND verefications passed successfully
 *                  false - connection OR verefication failed
 */
boolean checkMQTTconnection() {
  if (DEBUG)
    Serial.println("Attempting MQTT connection...");
  if (connectToMQTTBroker()) { // Try to connect to the MQTT server
    if (DEBUG) {
      Serial.print("Successfully connected as: ");
      Serial.println(deviceId);
    }
    if (checkMQTTSSL()) { // If connection was successful, check for servers SSL fingerprint
      if (DEBUG)
        Serial.println("Connection secure.");
      return true;
    } else {
        if (DEBUG)
          Serial.println("Connection insecure! Disconnecting");
        mqttClient.disconnect();
        delay(5000);
      }
  } else {
    if (DEBUG) {
      //Print connection error details
      Serial.println("Connection failed, Error = " + mqttClient.state());
    }
    delay(5000);
  }
  return false;
}


/**
 * Connect to MQTT server
 * 
 * @return boolean  true - connected successfully 
 *                  false - connection failed
 * Available return status
 * -4 : MQTT_CONNECTION_TIMEOUT - the server didn't respond within the keepalive time
 * -3 : MQTT_CONNECTION_LOST - the network connection was broken
 * -2 : MQTT_CONNECT_FAILED - the network connection failed
 * -1 : MQTT_DISCONNECTED - the client is disconnected cleanly
 * 0 : MQTT_CONNECTED - the cient is connected
 * 1 : MQTT_CONNECT_BAD_PROTOCOL - the server doesn't support the requested version of MQTT
 * 2 : MQTT_CONNECT_BAD_CLIENT_ID - the server rejected the client identifier
 * 3 : MQTT_CONNECT_UNAVAILABLE - the server was unable to accept the connection
 * 4 : MQTT_CONNECT_BAD_CREDENTIALS - the username/password were rejected
 * 5 : MQTT_CONNECT_UNAUTHORIZED - the client was not authorized to connect
 */
boolean connectToMQTTBroker() {
  // This is a LWT message, that will be sent if device loss connection to the server
  const char* willMessage = "offline";
  if (DEBUG) {
    Serial.println("Trying connection to MQTT broker");
    Serial.print("connecting to: ");
    Serial.println(configs.mqttServer);
    Serial.print("Using port: ");
    Serial.println(configs.mqttPort);
    Serial.print("Using deviceId: ");
    Serial.println((char*)deviceId.c_str());
    Serial.print("Using Password: ");
    Serial.println(configs.devicePassword.c_str());
    Serial.print("Using topic: ");
    Serial.println(generateTopic("status").c_str());
  }
  // boolean connect (clientID, username, password, willTopic, willQoS, willRetain, willMessage)
  if (mqttClient.connect(deviceId.c_str(), deviceId.c_str(), configs.devicePassword.c_str(), generateTopic("status").c_str(), QOS, retained, willMessage)) {
    if(DEBUG) {
      Serial.print("Connection successful, status = ");
      Serial.println(mqttClient.state());
    }
    // If connected, send message with status 'online'
    publishMessageToMQTTBroker(generateTopic("status").c_str(), "online", retained);
    return true;
  }
  if(DEBUG) {
    Serial.print("Connection failed, status = ");
    Serial.println(mqttClient.state());
  }
  Serial.println("BLABBBBBBBBBBBB");
  return false;
}


/**
 * Checks for servers digital signature fingerprint
 * 
 * @return boolean  true - MQTT server fingerprint verified
 *                  false - MQTT server fingerprint invalid
 */
boolean checkMQTTSSL() {
  if (wifiClientSecure.verify(fingerprint, (char*)configs.mqttServer.c_str()))
    return true;
  return false;
}


/**
 * Disconnect from MQTT server
 * Also publish a message of 'offline' for the status topic
 */
void disconnectFromMQTTBroker() {
  //Before disconnecting, send message with status 'offline'
  publishMessageToMQTTBroker(generateTopic("status").c_str(), "offline", retained);
  mqttClient.disconnect();
}


/**
 * Check if client is connected to MQTT server
 * 
 * @return boolean  true - client is connected
 *                  false - client is not connected
 */
boolean isClientConectedToMQTTServer() {
  return mqttClient.connected();
}


/**
 * Generate topic contaning constat device id and type, with relevent value
 * 
 * @param data last topic section 
 * 
 * @return String full topic (as of DEVICE_ID/DEVICE_TYPE/DATA)
 */
String generateTopic(String data) {
  String result = "";
  result += deviceId;
  result += "/";
  result += deviceType;
  result += "/";
  result += data;
  return result;
}

//Set time to "delay" a wifi signal strength publish message
unsigned long wifiSignalPreviousPublish = 0;
const long wifiSignalPublishInterval = 60000; // interval at which to send message (milliseconds)

/**
 * Send wifi signal strength to broker
 * Function will publish RSSI level to MQTT broker
 */
void sendWifiSignalStrength() {
  unsigned long currentMillis = millis();
  if(currentMillis - wifiSignalPreviousPublish >= wifiSignalPublishInterval) {
    wifiSignalPreviousPublish = currentMillis;

    String topic = generateTopic("wifi_rssi"); //Set a topic string to indicate wifi RSSI
    String wifiStrengthLevel = String(getWifiSignalStrength());

    publishMessageToMQTTBroker((char*)topic.c_str(), (char*)wifiStrengthLevel.c_str(), notRetained); //Send the data
  }
}


