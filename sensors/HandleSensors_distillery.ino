

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
  if(areEqual(topicArray[2], "update_now")) {
    // DO update data from sensor
  }
  
  // If received message is 'automation' then
  if(areEqual(topicArray[2], "automation")) {
    
    // If received 1 then
    if((char)payload[0] == '1') {
       /** 
       *  automation paylod is build like: ("1,w,x,y,z")
       *  v - temperature when main ssr stop working
       *  w - temperature to start water coller ssr
       *  x - temperature to start air coller ssr
       *  y - temperature to start to barrel disposal ssr
       */
      // Char array that will store the message in parts 
      char* messageArray[5];
    
      i=0;
      // Get the first section from topic
      messageArray[i] = strtok((char*)payload,",");
      Serial.println(messageArray[i]);
	  
      // Brake the message string into parts
      // Each array cell contains part of the topic
      if(DEBUG)
        Serial.println("Message is: ");
      while(messageArray[i]!='\0') {
        messageArray[++i] = strtok(NULL,",");
        if(DEBUG) {
          Serial.println(messageArray[i]);
        }
      }
  
      // Set variables from message to temperature vars
      mainHeaterStartTemprature = atoi(messageArray[1]);
      waterCoolerStartTemprature = atoi(messageArray[2]);
      airCoolerStartTemprature = atoi(messageArray[3]);
      toBarrelSisposalStartTemprature = atoi(messageArray[4]);
      if(DEBUG) {
          Serial.println("Temprature configurations set to:");
          Serial.print("MAIN_HEATER: ");
          Serial.println(mainHeaterStartTemprature);
          Serial.print("WATER_COOLER: ");
          Serial.println(waterCoolerStartTemprature);
          Serial.print("AIR_COLLER: ");
          Serial.println(airCoolerStartTemprature);
          Serial.print("BARREL_DISPOSAL: ");
          Serial.println(toBarrelSisposalStartTemprature);
        }
    
      isAutomatedJob = true;

      digitalWrite(MAIN_HEATER, LOW);
      digitalWrite(WATER_COLLER, LOW);
      digitalWrite(AIR_COLLER, LOW);
      digitalWrite(GARBAGE_DISPOSAL, LOW);
      digitalWrite(BARREL_DISPOSAL, LOW);
    } 
	else if ((char)payload[0] == '0') { // If received 0 then
      isAutomatedJob = false;
      isErrorOccured = false;

      digitalWrite(MAIN_HEATER, LOW);
      digitalWrite(WATER_COLLER, LOW);
      digitalWrite(AIR_COLLER, LOW);
      digitalWrite(GARBAGE_DISPOSAL, LOW);
      digitalWrite(BARREL_DISPOSAL, LOW);

      wasGarbageDisposalExecuted = false;
      garbageDisposalMadeTimeSnapshot = false;  
      barrelDisposalMadeTimeSnapshot = false;
      wasBarrelDisposalExecuted = false;
    }
    
  }

  /*
  //Case SSR received on sensor section of topic
  else if (areEqual(topicArray[2], "ssr")) {
    if((char)payload[0] == '1') { // Case SSR 1 received
      if(led1Status) { // If current state is true then
        digitalWrite(LED_MAIN_SSR, LOW);
        led1Status = false;
      } else {
        digitalWrite(LED_MAIN_SSR, HIGH);
        led1Status = true;
      }
      if (DEBUG)
        Serial.println("LED_MAIN_SSR power changed");
    }
    
    else if((char)payload[0] == '2') { // Case SSR 2 received
      if(led2Status) { // If current state is true then
        digitalWrite(LED_DISPOSAL_SSR, LOW);
        led2Status = false;
      } else {
        digitalWrite(LED_DISPOSAL_SSR, HIGH);
        led2Status = true;
      }
      if (DEBUG)
        Serial.println("LED_DISPOSAL_SSR power changed");
    }

    else if((char)payload[0] == '3') { // Case SSR 3 received
      if(led3Status) { // If current state is true then
        digitalWrite(LED_SECONDARY_SSR, LOW);
        led3Status = false;
      } else {
        digitalWrite(LED_SECONDARY_SSR, HIGH);
        led3Status = true;
      }
      if (DEBUG)
        Serial.println("LED_SECONDARY_SSR power changed");
    }

    else if((char)payload[0] == '4') { // Case SSR 4 received
      if(led4Status) { // If current state is true then
        digitalWrite(LED_COLLER, LOW); // Send 'LOW' to sensor
        led4Status = false; // And set state to false
      } else {
        digitalWrite(LED_COLLER, HIGH);
        led4Status = true;
      }
      if (DEBUG)
        Serial.println("LED_COLLER power changed");
    }
  }*/
}


// Function will publish a message for each sensor configured here
void getDataFromSensor() {

  if(isErrorOccured) {
    doAlarmBuzzer();
  }
  
  // Check if automation job requested for distillery job
  if (isAutomatedJob) {
    doDistilleryJob();
  }
    
  //For each sensor, check its last publish time 
  unsigned long currentMillis = millis();
  if(currentMillis - previousPublish >= publishInterval) {
    previousPublish = currentMillis;

    String topic;
    char message[sizeof(float)];
    float temperature_c;

    temperature_c = getTempFromMAX31855(); // Get temperature from MAX chip
    //If there was an error reading data from sensor then
    if (temperature_c == -9999) { // In case there was an error with the chip MAX returns -9999
      if (DEBUG)
        Serial.println("Failed to read temprature from MAX31855k sensor.");
      return;
    }
    if (DEBUG) {
      Serial.print("Temprature is (C): ");
      Serial.println(temperature_c);
    }
    
    topic = generalTopic + "temperature"; //Set a topic string that will change depending on the relavant sensor
    dtostrf(temperature_c, 4, 2, message); // Arduino based function converting float to string
    publishMessageToMQTTBroker((char*)topic.c_str(), message, notRetained); //Send the data
  }
}






// Function will perform automated job for special distillery work
void doDistilleryJob() {
  // Get temprature from sensor
  char message[sizeof(float)];
  float temprature_c;
  temprature_c = getTempFromMAX31855(); // Get temperature from MAX chip
  dtostrf(temprature_c, 4, 2, message); // Arduino based function converting float to string
  if (DEBUG) {
    Serial.print("temprature_c: ");
    Serial.println(message);
  }

  // This will store main heater temprature at default 94 C,
  // Once temprature is above default temprature, main heater turns off
  // Once temprature is again below default temprature, main heater turns on
  if(temprature_c < mainHeaterStartTemprature) // Default temprature is 94C
    digitalWrite(MAIN_HEATER, HIGH);
  else
    digitalWrite(MAIN_HEATER, LOW);

  if(temprature_c < waterCoolerStartTemprature) // Default temprature is 60C
    digitalWrite(WATER_COLLER, LOW);
  else
    digitalWrite(WATER_COLLER, HIGH);

  if(temprature_c < airCoolerStartTemprature) // Default temprature is 80C
    digitalWrite(AIR_COLLER, LOW);
  else
    digitalWrite(AIR_COLLER, HIGH);

  if(temprature_c < 80) {
    if(!wasGarbageDisposalExecuted) {
      digitalWrite(GARBAGE_DISPOSAL, HIGH);
      if(!garbageDisposalMadeTimeSnapshot) {
        garbageDisposalTimeSnapshot = millis();
        garbageDisposalMadeTimeSnapshot = true;
      }
      
      if(millis() - garbageDisposalTimeSnapshot >= barrelAndGarbageDisposalStopExecute) {
        digitalWrite(GARBAGE_DISPOSAL, LOW);
        wasGarbageDisposalExecuted = true;
        garbageDisposalMadeTimeSnapshot = false;
      }
    }
  }

  if(temprature_c >= toBarrelSisposalStartTemprature) { // Default temprature is 80C
    if(!wasBarrelDisposalExecuted) {
      if (DEBUG)
        Serial.println("Executing BARREL disposal");
      digitalWrite(BARREL_DISPOSAL, HIGH);
      if(!barrelDisposalMadeTimeSnapshot) {
        barrelDisposalTimeSnapshot = millis();
        barrelDisposalMadeTimeSnapshot = true;
      }
      
      if(millis() - barrelDisposalTimeSnapshot >= barrelAndGarbageDisposalStopExecute) {
        digitalWrite(BARREL_DISPOSAL, LOW);
        wasBarrelDisposalExecuted = true;
        barrelDisposalMadeTimeSnapshot = false;
      }
    }
  }


  
  /*
  // The main heater is alway on
  digitalWrite(LED_MAIN_SSR, HIGH);

  if(!wasAboveCoolerStartTemp) { // If temprature was NOT yet above 60C then
    // This is true if temp is C < 60
    if(temprature_c > coolerStartTemp) // Check if temp above 60
      wasAboveCoolerStartTemp = true; // And set flag 'above 60' to true
  } else { // If temprature was above 60C then 
    // This is true if temp is C > 60
    digitalWrite(LED_COLLER, HIGH); // Turn cooler on
    if(!wasAboveDisposalEndTemp) { // If temprature was NOT yet above 80C then
      // This is true if temp is C > 60 AND C < 80
      // If temperature passed the input for starting the disposal ssr then
      if(temprature_c > disposalStartTemp)
        digitalWrite(LED_DISPOSAL_SSR, HIGH); // Turn disposal SSR on
      if(temprature_c > disposalEndTemp)
        wasAboveDisposalEndTemp = true;
    } else { // If temprature was above 80C then
      // This is true if temp is C > 80
      digitalWrite(LED_DISPOSAL_SSR, LOW); // Turn disposal SSR off
      if(!wasAboveSecondarySSRStartTemp) { // If temprature was NOT yet above 84C then
        // This is true if temp is C > 80 AND C < 84
        if(temprature_c > secondarySSRStartTemp)
          wasAboveSecondarySSRStartTemp =true;
      }
      else {
        // This is true if temp is C > 84
        digitalWrite(LED_SECONDARY_SSR, HIGH);
        // If temp go back below 84 again 
        if ( temprature_c < secondarySSRStartTemp && temprature_c > 0) {
          // Shutdown all GPIOs
          digitalWrite(LED_MAIN_SSR, LOW);
          digitalWrite(LED_COLLER, LOW);
          digitalWrite(LED_DISPOSAL_SSR, LOW);
          digitalWrite(LED_SECONDARY_SSR, LOW);

          isErrorOccured = true;
          wasAboveCoolerStartTemp = false;
          wasAboveDisposalEndTemp = false;
          wasAboveSecondarySSRStartTemp = false;

          // Send automation error message
          String topic;
          topic = generalTopic + "automation_error"; // Set a topic string that will change depending on the relavant sensor
          startMQTTPublisher((char*)topic.c_str(), "1", retained); // Send the data

          // Set automation flag to false, so job will stop executing
          isAutomatedJob = false;
        }
      }
    }
  }*/
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



/**
 * Subscribe to all relevant sensors connected
 */
void getSensorsInformation() {
  String topic = generalTopic + "ssr";
  subscribeToMQTTBroker((char*)topic.c_str());
  topic = generalTopic + "automation";
  subscribeToMQTTBroker((char*)topic.c_str());
}


/**
 * Initialize all sensonrs present in the system
 */
void setupSensorsGPIOs() {
  pinMode(MAIN_HEATER, OUTPUT);
  pinMode(WATER_COLLER, OUTPUT);
  pinMode(AIR_COLLER, OUTPUT);
  pinMode(GARBAGE_DISPOSAL, OUTPUT);
  pinMode(BARREL_DISPOSAL, OUTPUT);
  digitalWrite(MAIN_HEATER, LOW);
  digitalWrite(WATER_COLLER, LOW);
  digitalWrite(AIR_COLLER, LOW);
  digitalWrite(GARBAGE_DISPOSAL, LOW);
  digitalWrite(BARREL_DISPOSAL, LOW);
}
