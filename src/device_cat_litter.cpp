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

    int maxVal = 500; // indicates 500 grams in wheight
    if (initStatusLed) {
        statusLed->showColorRange(200, static_cast<int>(scale_read_value * 1000), maxVal);
    }

    if (initDisplay) {
        char message[sizeof(float)];
        String dtostrfResult = dtostrf(scale_read_value, 4, 2, message); // Arduino based function converting float to string
        String text = "wheight:\n" + dtostrfResult + " kg";
        ledDisplay->displaySensorData(text);
    }

    if (initBuzzer) {
        if (static_cast<int>(scale_read_value * 1000) > maxVal)
            alarmBuzzer->executeBuzzer();
        else
            alarmBuzzer->stopBuzzer();
    }

    checkResetButton();
    delay(500);
}

/**
 * Based on the D1 mini 1-button shield https://www.wemos.cc/en/latest/d1_mini_shield/1_button.html
 * examples: https://github.com/wemos/D1_mini_Examples/tree/master/examples/04.Shields/1_Button_Shield
 * 
 * @return true if a tare (scale rest to 0) has occured
*/
boolean CatLitter::checkResetButton() {
    if (digitalRead(RESET_BTN_PIN) == LOW) { // If button is pressed then
        if (DEBUG) {
            Serial.print("detected a LOW (pressed) value for pin: ");
            Serial.print(RESET_BTN_PIN);
            Serial.println(" issue a scale tare (reset to 0)");
        }
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
    deviceVersion = "1.0.0";

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