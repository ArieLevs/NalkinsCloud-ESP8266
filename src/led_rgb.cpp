//
// Created by Arie Lev on 24/08/2021.
//

#include "led_rgb.h"

void LedRgb::rainbow(int rainbowLoops) {
    for(long firstPixelHue = 0; firstPixelHue < rainbowLoops*65536; firstPixelHue += 256) {
        for(int i=0; i< led->numPixels(); i++) {
            long pixelHue = firstPixelHue + (i * 65536L / led->numPixels());

            led->setPixelColor(i, Adafruit_NeoPixel::gamma32(Adafruit_NeoPixel::ColorHSV(pixelHue)));
        }
        led->show();
        delay(10);
    }
}

void LedRgb::showColorRange(int minVal, int currentVal, int maxVal) {
    float percentage;
    if (currentVal < minVal) { // set 0 (green)
        percentage = 0;
    } else if (currentVal > maxVal) { // set 100 (red)
        percentage = 100;
    } else { // set 0-100 (all colors between green to red)
        // calculate currentVal as a percentage between maxVal and minVal
        percentage = (static_cast<float>(currentVal - minVal) / static_cast<float>(maxVal - minVal)) * 100;
    }

    // 21845 is hue value for green
    // 437 is the value of 1/100 from (65536 - 21845)
    long pixelHue = static_cast<long>(percentage) * 437 + 21845;

    for(int i=0; i< led->numPixels(); i++) {
        led->setPixelColor(i, Adafruit_NeoPixel::gamma32(Adafruit_NeoPixel::ColorHSV(pixelHue)));
    }
    led->show();
}

void LedRgb::showRgbOOnce() {
    int colors[3][3] = {
        {10, 0, 0},
        {0, 10, 0},
        {0, 0, 10}
    };
    for (int i = 0; i < ledNum; i++) {
        for (auto & color : colors) {
            led->setPixelColor(i, Adafruit_NeoPixel::Color(color[0], color[1], color[2]));
            led->show();
            delay(500);
        }
    }
}

void LedRgb::colorWipe(uint32_t color) {
    for(int i=0; i<led->numPixels(); i++) {
        led->setPixelColor(i, color);
        led->show();
    }
}

void LedRgb::initLed() {
    led->begin();

    // test RGB and stop
    rainbow(1);
    led->clear();

    led->setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
}

LedRgb::LedRgb(uint16_t _ledPin, uint16_t _numOfLeds) {
    ledPin = _ledPin;
    ledNum = _numOfLeds;
    led = new Adafruit_NeoPixel(ledNum, ledPin, NEO_GRB + NEO_KHZ800);
}