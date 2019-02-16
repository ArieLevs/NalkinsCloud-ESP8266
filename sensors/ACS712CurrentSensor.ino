
/*
Measuring Current Using ACS712

The voltage returned by the sensor depends on the sensor’s rating. 
The ±5A sensor will output 185mV for each amp (mV/A), the ±20A 100mV/A and the ±30A 66mV/A.
*/




/* Add below row to config.h file */
const int analogACS712InPin = 3; // Setup analog input pic for ACS712



int mVoltsPerAmp = 66; // use 185 for 5A, 100 for 20A Module and 66 for 30A Module
int sensorValue = 0;
int ACSoffset = 2500; 
double voltage = 0;
double amps = 0;

int sensorCalibrationToZero = 0;

/*  FIRST CALIBRATE THE SENSOR
 *  Read the analog input a few times to makes sure that we get
 *  a good zero adjustment from nominal center value. There should be no
 *  current flowing while this is happening. 
 */
void calibrateACS712Sensor () {
  for (int i = 0; i < 10; i++)
  {
   int error = 512 - analogRead(analogACS712InPin);
   sensorCalibrationToZero = ((sensorCalibrationToZero * (i-1)) + error)/i;
  }
}


/*
const uint8_t SENS = 100; // sensor value from datasheet in mV/A

int32_t senseCurrent(void)
{
  int32_t sensorValue = analogRead(analogACS712InPin) + sensorCalibrationToZero;
  int32_t Vcc = readVcc();
 
  int32_t convertedmA = (1000L * (((Vcc*sensorValue)/1024L)-(Vcc/2)))/SENS;

  return(convertedmA);
}*/




// This function is correct for reading consumption of DC current
int getDCValuesFromACS() {
  sensorValue = analogRead(analogACS712InPin); // Get data from Sensor
  voltage = (sensorValue / 1024.0) * 5000; // Returns mili-volts
  amps = ((voltage - ACSoffset) / mVoltsPerAmp);

  // Print voltage \ amps measures
  Serial.print("Sensor Value = " );
  Serial.println(sensorValue); 
  Serial.print("mili-volts = ");
  Serial.println(voltage, 4);
  Serial.print("Amps = ");
  Serial.println(amps, 4);
}



const int AC_FREQUENCY = 50; // in Hz

// This function is correct for reading consumption of DC current
void getACValuesFromACS() {
  const int timeSampling = 2 * (1000 / AC_FREQUENCY);
 
  int convertedmA;
  int sampleMin = 1024, sampleMax = 0;
  sensorValue = analogRead(analogACS712InPin) + sensorCalibrationToZero;
  int Vpp, Vcc = (sensorValue / 1024.0) * 5000; // Returns mili-volts
  

  // Collect samples over the time sampling period and 
  // work out the min/max readings for the waveform
  for (int timeStart = millis(); millis() - timeStart < timeSampling; ) {
    int sensorValue = analogRead(analogACS712InPin) + sensorCalibrationToZero; // Get data from Sensor   
    if (sensorValue > sampleMax) 
      sampleMax = sensorValue;
    if (sensorValue < sampleMin) 
      sampleMin = sensorValue;
  }

  // now calculate the current
  Vpp = (((sampleMax-sampleMin)/2)*Vcc)/1024L;
  convertedmA = (707L * Vpp)/mVoltsPerAmp; // 1/srt(2) = 0.7071 = 707.1/1000

  // Print voltage \ amps measures
  Serial.print("Sensor Value = " );
  Serial.println(sensorValue); 
  Serial.print("mili-volts = ");
  Serial.println(Vpp, 4);
  Serial.print("Amps = ");
  Serial.println(convertedmA, 4);
}




