

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
	char *topicArray[4];
	
	int i=0;
	// Get the first section from topic
	topicArray[i] = strtok((char*)topic,"/");

	// Brake the topic string into parts
	// Each array cell contains part of the topic
	while(topicArray[i]!=NULL) {
		topicArray[++i] = strtok(NULL,"/");
	}
	
	// If received message is 'update_now' then
	if(areEqual(topicArray[2], "update_now")) {
		// Do update
	}

	// If topic data equals RECEIVESTATUS then
	if(areEqual(topicArray[2], RECEIVESTATUS)) {
	// If received 1 then system armed
		if((char)payload[0] == '1') {
			isTriggered = true;
		}
		// If received 0 then system released
		else if ((char)payload[0] == '0') {
			isTriggered = false;
		}
	}

	if(areEqual(topicArray[2], RELEASEALARM)) {
	isAlarmOn = false;
	isAlarmSent = false;
	isTriggered = false;
	}
}


// Function will publish a message for each sensor configured here
void getDataFromSensor() {

  if(isAlarmOn) // If alarm flag is set on (alarm triggered) then
    doAlarmBuzzer(); // Do buzzing

  // Check if interval time passed
  unsigned long currentMillis = millis();
  if(currentMillis - previousMagnetCheck >= magnetCheckInterval) {
    previousMagnetCheck = currentMillis;

    String topic = generalTopic + SENDSTATUS; // Setup the publish topic

    // MAGNETINPUT = 1 -> Locked
    // MAGNETINPUT = 0 -> Opened
    int magnetStatus = digitalRead(MAGNETINPUT); // Get sensor state
    
    if(firstStatePublish) { // This will happen only once at application life cycle
      publishMessageToMQTTBroker((char*)topic.c_str(), (char*)String(magnetStatus).c_str(), retained); //Send the data
      firstStatePublish = false;
      lastMagnetState = magnetStatus;
    }

    if(lastMagnetState != magnetStatus) { // If there was a change in the magnet status then 

      publishMessageToMQTTBroker((char*)topic.c_str(), (char*)String(magnetStatus).c_str(), retained); //Send the data
      lastMagnetState = magnetStatus;
      
      if(isTriggered && !magnetStatus) { // If device is triggered, and magnet is in opened state then
        if(!isAlarmSent) { // If the alarm was not sent yet (before release received) then
          isAlarmOn = true; // If Alarm triggered, mark flag for internal chip use
          topic = generalTopic + ALARM; // Setup alarm publish topic
          String alarm = "1";
          publishMessageToMQTTBroker((char*)topic.c_str(), (char*)alarm.c_str(), retained); //Send the data
          isAlarmSent = true;
        }
      }
    }
  }
}


//Function will handle to subscribe all relevant sensors connected
//This funcrion is dynamic and will be changed regarding all connected sensor
void getSensorsInformation() {
  String topic = generalTopic + RECEIVESTATUS;
  subscribeToMQTTBroker((char*)topic.c_str());
  topic = generalTopic + RELEASEALARM;
  subscribeToMQTTBroker((char*)topic.c_str());
}

void setupSensorsGPIOs() {
  // Preparing GPIOs
  pinMode(LED_IO, OUTPUT);
  digitalWrite(LED_IO, LOW);
  
  pinMode(MAGNETINPUT, INPUT);
}


//Set time to "delay" buzzer intervals
unsigned long buzzerPreviousStart = 0;
const long buzzerInterval = 1000; // interval at which to send message (milliseconds)
boolean buzzerIsOn = false;

void doAlarmBuzzer() {
  unsigned long currentMillis = millis();
  if(currentMillis - buzzerPreviousStart >= buzzerInterval) {
    buzzerPreviousStart = currentMillis;
    if(buzzerIsOn) {
      noTone(BUZZER);
      buzzerIsOn = false;
    }  
    else {
      tone(BUZZER, BUZZERFREQUENCY); // Do sound
      buzzerIsOn = true;
    }
  }
}
