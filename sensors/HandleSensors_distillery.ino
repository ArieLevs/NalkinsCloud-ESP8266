
/**
 * Execute incoming message to sensor,
 * The message will be converted to real actions that should apply on sensors
 * This function is being forwarded from the callback() function
 * 
 * @param topic incoming message topic
 * @param payload incoming message payload
 */
void sendDataToSensor(const char *topic, byte *payload) {
	if (DEBUG)
		Serial.println("Running 'sendDataToSensor' function");

	// topic should be of type: "device_id/device_type/data"
	// Char array that will store the topic in parts
	char *topicArray[4];

	int i = 0;
	// Get the first section from topic
	topicArray[i] = strtok((char *) topic, "/");

	// Brake the topic string into parts
	// Each array cell contains part of the topic
	while (topicArray[i] != NULL) {
		topicArray[++i] = strtok(NULL, "/");
	}

	// If received message is 'update_now' then
	if (areEqual(topicArray[2], "update_now")) {
		// DO update data from sensor
	}

	// If received message is 'automation' then
	if (areEqual(topicArray[2], "automation")) {

		// If received 1 then
		if ((char) payload[0] == '1') {
			/**
			*  automation payload is build like: ("1,w,x,y,z")
			*  v - temperature when main ssr stop working
			*  w - temperature to start water cooler ssr
			*  x - temperature to start air cooler ssr
			*  y - temperature to start to barrel disposal ssr
			*/
			// Char array that will store the message in parts
			char *messageArray[5];

			i = 0;
			// Get the first section from topic
			messageArray[i] = strtok((char *) payload, ",");
			Serial.println(messageArray[i]);

			// Brake the message string into parts
			// Each array cell contains part of the topic
			if (DEBUG)
				Serial.println("Message is: ");
			while (messageArray[i] != '\0') {
				messageArray[++i] = strtok(NULL, ",");
				if (DEBUG) {
					Serial.println(messageArray[i]);
				}
			}

			// Set variables from message to temperature vars
			mainHeaterStartTemperature = atoi(messageArray[1]);
			waterCoolerStartTemperature = atoi(messageArray[2]);
			airCoolerStartTemperature = atoi(messageArray[3]);
			toBarrelDisposalStartTemperature = atoi(messageArray[4]);
			if (DEBUG) {
				Serial.println("Temperature configurations set to:");
				Serial.print("MAIN_HEATER: ");
				Serial.println(mainHeaterStartTemperature);
				Serial.print("WATER_COOLER: ");
				Serial.println(waterCoolerStartTemperature);
				Serial.print("AIR_COOLER: ");
				Serial.println(airCoolerStartTemperature);
				Serial.print("BARREL_DISPOSAL: ");
				Serial.println(toBarrelDisposalStartTemperature);
			}

			isAutomatedJob = true;

			digitalWrite(MAIN_HEATER, LOW);
			digitalWrite(WATER_COOLER, LOW);
			digitalWrite(AIR_COOLER, LOW);
			digitalWrite(GARBAGE_DISPOSAL, LOW);
			digitalWrite(BARREL_DISPOSAL, LOW);
		} else if ((char) payload[0] == '0') { // If received 0 then
			isAutomatedJob = false;
			isErrorOccurred = false;

			digitalWrite(MAIN_HEATER, LOW);
			digitalWrite(WATER_COOLER, LOW);
			digitalWrite(AIR_COOLER, LOW);
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
		  digitalWrite(LED_COOLER, LOW); // Send 'LOW' to sensor
		  led4Status = false; // And set state to false
		} else {
		  digitalWrite(LED_COOLER, HIGH);
		  led4Status = true;
		}
		if (DEBUG)
		  Serial.println("LED_COOLER power changed");
	  }
	}*/
}


// Function will publish a message for each sensor configured here
void getDataFromSensor() {

	if (isErrorOccurred) {
		doAlarmBuzzer();
	}

	// Check if automation job requested for distillery job
	if (isAutomatedJob) {
		doDistilleryJob();
	}

	//For each sensor, check its last publish time
	unsigned long currentMillis = millis();
	if (currentMillis - previousPublish >= publishInterval) {
		previousPublish = currentMillis;

		String topic;
		char message[sizeof(float)];
		float temperature_c;

		temperature_c = getTempFromMAX31855(); // Get temperature from MAX chip
		//If there was an error reading data from sensor then
		if (temperature_c == -9999) { // In case there was an error with the chip MAX returns -9999
			if (DEBUG)
				Serial.println("Failed to read temperature from MAX31855k sensor.");
			return;
		}
		if (DEBUG) {
			Serial.print("Temperature is (C): ");
			Serial.println(temperature_c);
		}

		topic = generalTopic + "temperature"; //Set a topic string that will change depending on the relevant sensor
		dtostrf(temperature_c, 4, 2, message); // Arduino based function converting float to string
		publishMessageToMQTTBroker((char *) topic.c_str(), message, notRetained); //Send the data
	}
}


// Function will perform automated job for special distillery work
void doDistilleryJob() {
	// Get temperature from sensor
	char message[sizeof(float)];
	float temperature_c;
	temprature_c = getTempFromMAX31855(); // Get temperature from MAX chip
	dtostrf(temprature_c, 4, 2, message); // Arduino based function converting float to string
	if (DEBUG) {
		Serial.print("temperature_c: ");
		Serial.println(message);
	}

	// This will store main heater temperature at default 94 C,
	// Once temperature is above default temperature, main heater turns off
	// Once temperature is again below default temperature, main heater turns on
	if (temprature_c < mainHeaterStartTemperature) // Default temperature is 94C
		digitalWrite(MAIN_HEATER, HIGH);
	else
		digitalWrite(MAIN_HEATER, LOW);

	if (temprature_c < waterCoolerStartTemperature) // Default temperature is 60C
		digitalWrite(WATER_COOLER, LOW);
	else
		digitalWrite(WATER_COOLER, HIGH);

	if (temprature_c < airCoolerStartTemperature) // Default temperature is 80C
		digitalWrite(AIR_COOLER, LOW);
	else
		digitalWrite(AIR_COOLER, HIGH);

	if (temprature_c < 80) {
		if (!wasGarbageDisposalExecuted) {
			digitalWrite(GARBAGE_DISPOSAL, HIGH);
			if (!garbageDisposalMadeTimeSnapshot) {
				garbageDisposalTimeSnapshot = millis();
				garbageDisposalMadeTimeSnapshot = true;
			}

			if (millis() - garbageDisposalTimeSnapshot >= barrelAndGarbageDisposalStopExecute) {
				digitalWrite(GARBAGE_DISPOSAL, LOW);
				wasGarbageDisposalExecuted = true;
				garbageDisposalMadeTimeSnapshot = false;
			}
		}
	}

	if (temprature_c >= toBarrelDisposalStartTemperature) { // Default temperature is 80C
		if (!wasBarrelDisposalExecuted) {
			if (DEBUG)
				Serial.println("Executing BARREL disposal");
			digitalWrite(BARREL_DISPOSAL, HIGH);
			if (!barrelDisposalMadeTimeSnapshot) {
				barrelDisposalTimeSnapshot = millis();
				barrelDisposalMadeTimeSnapshot = true;
			}

			if (millis() - barrelDisposalTimeSnapshot >= barrelAndGarbageDisposalStopExecute) {
				digitalWrite(BARREL_DISPOSAL, LOW);
				wasBarrelDisposalExecuted = true;
				barrelDisposalMadeTimeSnapshot = false;
			}
		}
	}

	/*
	// The main heater is always on
	digitalWrite(LED_MAIN_SSR, HIGH);

	if(!wasAboveCoolerStartTemp) { // If temperature was NOT yet above 60C then
	  // This is true if temp is C < 60
	  if(temperature_c > coolerStartTemp) // Check if temp above 60
		wasAboveCoolerStartTemp = true; // And set flag 'above 60' to true
	} else { // If temperature was above 60C then
	  // This is true if temp is C > 60
	  digitalWrite(LED_COOLER, HIGH); // Turn cooler on
	  if(!wasAboveDisposalEndTemp) { // If temperature was NOT yet above 80C then
		// This is true if temp is C > 60 AND C < 80
		// If temperature passed the input for starting the disposal ssr then
		if(temperature_c > disposalStartTemp)
		  digitalWrite(LED_DISPOSAL_SSR, HIGH); // Turn disposal SSR on
		if(temperature_c > disposalEndTemp)
		  wasAboveDisposalEndTemp = true;
	  } else { // If temperature was above 80C then
		// This is true if temp is C > 80
		digitalWrite(LED_DISPOSAL_SSR, LOW); // Turn disposal SSR off
		if(!wasAboveSecondarySSRStartTemp) { // If temperature was NOT yet above 84C then
		  // This is true if temp is C > 80 AND C < 84
		  if(temperature_c > secondarySSRStartTemp)
			wasAboveSecondarySSRStartTemp =true;
		}
		else {
		  // This is true if temp is C > 84
		  digitalWrite(LED_SECONDARY_SSR, HIGH);
		  // If temp go back below 84 again
		  if ( temperature_c < secondarySSRStartTemp && temperature_c > 0) {
			// Shutdown all GPIOs
			digitalWrite(LED_MAIN_SSR, LOW);
			digitalWrite(LED_COOLER, LOW);
			digitalWrite(LED_DISPOSAL_SSR, LOW);
			digitalWrite(LED_SECONDARY_SSR, LOW);

			isErrorOccurred = true;
			wasAboveCoolerStartTemp = false;
			wasAboveDisposalEndTemp = false;
			wasAboveSecondarySSRStartTemp = false;

			// Send automation error message
			String topic;
			topic = generalTopic + "automation_error"; // Set a topic string that will change depending on the relevant sensor
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
	if (currentMillis - buzzerPreviousStart >= buzzerInterval) {
		buzzerPreviousStart = currentMillis;
		if (buzzerIsOn) {
			noTone(BUZZER);
			buzzerIsOn = false;
		} else {
			tone(BUZZER, BUZZER_FREQUENCY); // Do sound
			buzzerIsOn = true;
		}
	}
}


/**
 * Subscribe to all relevant sensors connected
 */
void getSensorsInformation() {
	String topic = generalTopic + "ssr";
	subscribeToMQTTBroker((char *) topic.c_str());
	topic = generalTopic + "automation";
	subscribeToMQTTBroker((char *) topic.c_str());
}


/**
 * Initialize all sensors present in the system
 */
void setupSensorsGPIOs() {
	pinMode(MAIN_HEATER, OUTPUT);
	pinMode(WATER_COOLER, OUTPUT);
	pinMode(AIR_COOLER, OUTPUT);
	pinMode(GARBAGE_DISPOSAL, OUTPUT);
	pinMode(BARREL_DISPOSAL, OUTPUT);
	digitalWrite(MAIN_HEATER, LOW);
	digitalWrite(WATER_COOLER, LOW);
	digitalWrite(AIR_COOLER, LOW);
	digitalWrite(GARBAGE_DISPOSAL, LOW);
	digitalWrite(BARREL_DISPOSAL, LOW);
}
