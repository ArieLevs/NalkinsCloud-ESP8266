
#include "Arduino.h"

#include "sensors.h"
#include "configs.h"
#include "functions.h"
#include "mqtt_client.h"

//Set time to "delay" a publish message
unsigned long previousPublish = 0;
const long publishInterval = 120000; // interval at which to send message (milliseconds)


void collectAndPublishData() {
  String topic;
  char message[sizeof(float)];
  float temprature_c, humidity;

  // Get temprature value from DHT sensor
  temprature_c = dht.readTemperature();
  topic = generalTopic + "temperature"; //Set a topic string that will change depending on the relavant sensor

  //If there was an error reading data from sensor then
  if (isnan(temprature_c)) {
    if (DEBUG)
      Serial.println("Failed to read temprature from DHT sensor!");
    //message = "DHT Error";
    return;
  } else {
    dtostrf(temprature_c, 4, 2, message); // Arduino based function converting float to string
  }
  publishMessageToMQTTBroker((char*)topic.c_str(), message, false); //Send the data

  humidity = dht.readHumidity();
  topic = generalTopic + "humidity"; //Set a topic string that will change depending on the relavant sensor
  // Send humidity value
  // If there was an error reading data from sensor then
  if (isnan(humidity)) {
    if (DEBUG)
      Serial.println("Failed to read humidity from DHT sensor!");
    //message = "DHT Error";
    return;
  } else {
    dtostrf(humidity, 4, 2, message); // Ardoinu based function converting float to string
  }
  publishMessageToMQTTBroker((char*)topic.c_str(), message, false); //Send the data
}


/**
 * Execute incoming message to sensor,
 * The message will be converted to real actions that should apply on sensors
 * This function is being forwarded from the callback() function
 * 
 * @param topic incoming message topic
 * @param payload incoming message payload
 */
void sendDataToSensor(const char* topic, byte* payload) {
  if(DEBUG)
    Serial.println("Running 'sendDataToSensor' function");
    
  // topic should be of type: "deviceid/devicetype/data"
  // Char array that will store the topic in parts 
  char* topicArray[4];

  int i=0;
  // Get the first section from topic
  topicArray[i] = strtok((char*)topic,"/");

  // Brake the topic string into parts
  // Each array cell contains part of the topic
  while(topicArray[i]!=NULL) {
    topicArray[++i] = strtok(NULL,"/");
  }
  
  // If received message is 'update_now' then
  if(areCharArraysEqual(topicArray[2], "update_now")) {
    collectAndPublishData();
  }
    
}


/**
 * Collect data from sensors
 * Accourdung to each sensor logic, decide if message publish is needed
 */
void getDataFromSensor() {
  //For each sensor, check its last publish time 
  unsigned long currentMillis = millis();
  if(currentMillis - previousPublish >= publishInterval) {
    previousPublish = currentMillis;

    collectAndPublishData();
  }
}




/**
 * Subscribe to all relevant sensors connected
 */
void getSensorsInformation() {
  String topic = generalTopic + "update_now";
  subscribeToMQTTBroker((char*)topic.c_str());
}


/**
 * Initialize all sensonrs present in the system
 */
void setupSensorsGPIOs() {
  dht.begin(); // initialize temperature sensor
  
  pinMode(LED_WORK_STATUS, OUTPUT);
  digitalWrite(LED_WORK_STATUS, LOW);

  pinMode(CONFIGURATIONMODEBUTTON, INPUT); // Setup Configuration mode button
}


