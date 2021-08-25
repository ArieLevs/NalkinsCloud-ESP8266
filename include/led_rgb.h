//
// Created by Arie Lev on 24/08/2021.
//

#ifndef NALKINSCLOUD_ESP8266_LED_RGB_H
#define NALKINSCLOUD_ESP8266_LED_RGB_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class LedRgb {
public:
    explicit LedRgb(uint16_t _ledPin, uint16_t _numOfLeds = 1);
    void initLed();
    void rainbow(int rainbowLoops);
    void showRgbOOnce();
    void showColorRange(int minVal, int currentVal, int maxVal);
    void colorWipe(uint32_t color);

private:
    uint16_t ledPin;
    uint16_t ledNum;
    Adafruit_NeoPixel *led;
};

#endif //NALKINSCLOUD_ESP8266_LED_RGB_H
