
// By default device is false
int lastSwitchState = 0;
boolean firstStatePublish = true;

//Set time to "delay" a consumption publish message
unsigned long previousPublish = 0;
const long consumptionPublishInterval = 120000; // interval at which to send message (milliseconds)

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
    
  // topic should be of type: "device_id/device_type/data"
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
  
  // If received message is 'change_switch' then
  if(areEqual(topicArray[2], "change_switch")) {
    if((char)payload[0] == '0') {
		  digitalWrite(SWITCH_OUT, LOW);
	  } else if ((char)payload[0] == '1') {
		  digitalWrite(SWITCH_OUT, HIGH);
	  }
  }
}


/**
 * Collect data from sensors
 * According to each sensor logic, decide if message publish is needed
 */
void getDataFromSensor() {
	int currentSwitchState = digitalRead(SWITCH_IN);

  if(firstStatePublish) { // This will happen only once at application life cycle
    String tmpTopic = generalTopic + "from_device_current_status";
    publishMessageToMQTTBroker((char*)tmpTopic.c_str(), (char*)String(currentSwitchState).c_str(), retained); //Send the data
    firstStatePublish = false;
    lastSwitchState = currentSwitchState; // Remember new switch state
  }

	if(currentSwitchState != lastSwitchState) {
		lastSwitchState = currentSwitchState; // Remember new switch state
		String topic = generalTopic + "from_device_current_status";
		publishMessageToMQTTBroker((char*)topic.c_str(), (char*)String(currentSwitchState).c_str(), retained); //Send the data
	}

 //For each sensor, check its last publish time 
  unsigned long currentMillis = millis();
  if(currentMillis - previousPublish >= consumptionPublishInterval) {
    previousPublish = currentMillis;

    sendSwitchConsumption();
  }
}


/**
 * Publish device AC / DC consumption
 */
void sendSwitchConsumption() {
  
}


/**
 * Subscribe to all relevant sensors connected
 */
void getSensorsInformation() {
  String topic = generalTopic + "change_switch";
  subscribeToMQTTBroker((char*)topic.c_str());
}


/**
 * Initialize all sensors present in the system
 */
void setupSensorsGPIOs() {
  pinMode(SWITCH_OUT, OUTPUT);
  pinMode(SWITCH_IN, INPUT);
  
  digitalWrite(LED_WORK_STATUS, LOW);

  pinMode(CONFIGURATION_MODE_BUTTON, INPUT); // Setup Configuration mode button
}
