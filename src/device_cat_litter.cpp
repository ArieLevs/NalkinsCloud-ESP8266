//
// Created by Arie Lev on 21/08/2021.
//

#include "device_cat_litter.h"

void CatLitter::getDataFromSensor() {
    float scale_read_value;

    if (calibrationMode) {
        scale.set_scale(calibration_factor); //Adjust to this calibration factor
        if (DEBUG)
            Serial.println("calibration_factor: " + String(calibration_factor));

        if (Serial.available()) {
            char temp = Serial.read();
            if (temp == '+' || temp == 'a')
                calibration_factor += 10;
            else if (temp == '-' || temp == 'z')
                calibration_factor -= 10;
        }
    }

    if (scale.wait_ready_retry(10, 5)) {
        scale_read_value = scale.get_units(3); // Get value from HX711 sensor
        if (DEBUG)
            Serial.println("HX711 reading: " + String(scale_read_value) + " kg");
    }
    else if (DEBUG)
        Serial.println("HX711 not found.");

    StaticJsonDocument<256> jsonDoc;
    jsonDoc["weight_kg"] = scale_read_value;

    String output;
    serializeJson(jsonDoc, output);

    sensorPayload = output;

    int maxVal = 500;
    if (initStatusLed) {
        statusLed->showColorRange(200, static_cast<int>(scale_read_value * 1000), maxVal);
    }

    if (initDisplay) {
        char message[sizeof(float)];
        String text = dtostrf(scale_read_value, 4, 2, message); // Arduino based function converting float to string
        ledDisplay->displaySensorData(text);
    }

    if (initBuzzer) {
        if (static_cast<int>(scale_read_value) > maxVal)
            alarmBuzzer->executeBuzzer();
        else
            alarmBuzzer->stopBuzzer();
    }

    delay(500);
}

boolean CatLitter::checkResetButton() {
    Serial.print("checking reset button from pin: ");
    uint8_t pinNum = D3;
    Serial.println(pinNum);
    if (digitalRead(D3) == HIGH) { // If button is pressed then
        scale.tare();
        return true;
    }
    return false;
}

CatLitter::CatLitter(const String& _deviceId, boolean _calibrationMode, boolean _initDisplay, boolean _initStatusLed, boolean _initBuzzer, boolean debug) {
    if (debug) {
        Serial.println("##################################");
        Serial.println("# initializing device " +  _deviceId);
        Serial.println("##################################");
    }

    deviceType = "HX711"; // The devices type definition
    selfDeviceId = _deviceId;
    chipType = "ESP8266"; // The devices chip type

    initDisplay = _initDisplay;
    initStatusLed = _initStatusLed;
    initBuzzer = _initBuzzer;

    if (initDisplay) {
        ledDisplay = new Oled64x48Display(1, WHITE);
        ledDisplay->initDisplay();
        ledDisplay->displayLogo();
    }

    if (initStatusLed) {
        statusLed = new LedRgb(STATUS_LED_PIN, 1);
        statusLed->initLed();
    }

    if (initBuzzer) {
        alarmBuzzer = new Buzzer(BUZZER_PIN, BUZZER_FREQUENCY, 1000);
        alarmBuzzer->initBuzzer();
        alarmBuzzer->stopBuzzer();
    }

    calibrationMode = _calibrationMode;

//    calibration_factor = readIntFromEEPROM(CALIBRATION_ADDR);
    calibration_factor = -19510;

    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN); // init weight sensor

    if (calibrationMode)
        scale.set_scale();
    else
        scale.set_scale(calibration_factor);

    if (debug)
        Serial.println("Trying to find HX711");

    if (not scale.wait_ready_retry(20, 50)) {
        if (debug)
            Serial.println("HX711 not found.");
    } else {
        long zero_factor = scale.read_average(); //Get a baseline reading
        scale.tare();
        if (debug) {
            Serial.println("HX711 found, getting zero factor values");
            Serial.println(">>> Zero factor: " + String(zero_factor) + " <<<");
        }
    }

    pinMode(CONFIGURATION_MODE_BUTTON, INPUT); // Setup Configuration mode button
}