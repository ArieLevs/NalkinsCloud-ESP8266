//
// Created by Arie Lev on 2019-07-13.
//

#ifndef NALKINSCLOUD_ESP8266_OLED_DISPLAY_H
#define NALKINSCLOUD_ESP8266_OLED_DISPLAY_H

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// SCL GPIO5
// SDA GPIO4
#define OLED_RESET 0  // GPIO0
//#define NUMFLAKES 10
//#define XPOS 0
//#define YPOS 1
//#define DELTAY 2
//
//#define LOGO16_GLCD_HEIGHT 16
//#define LOGO16_GLCD_WIDTH  16

// 'nala', 48x48px
const unsigned char nalkinsBitmap [] PROGMEM = {
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x7f, 0xff,
		0xff, 0xff, 0xff, 0xfb, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xf7,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xef, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc,
		0xff, 0xc7, 0xe3, 0x7f, 0xfb, 0xfd, 0xfc, 0x4f, 0x80, 0x0f, 0xf8, 0x49, 0x98, 0x3f, 0x80, 0x0f,
		0xf8, 0x3b, 0x3f, 0x7f, 0x80, 0x0f, 0xf8, 0x3a, 0x7f, 0xff, 0x80, 0x0f, 0xf8, 0x38, 0xff, 0xff,
		0xc0, 0x0f, 0xf8, 0x38, 0xff, 0xff, 0xc0, 0x0f, 0xf8, 0x2d, 0xff, 0xff, 0xc0, 0x0f, 0xf8, 0x23,
		0xff, 0xff, 0xc0, 0x0f, 0xf8, 0x3b, 0xff, 0xff, 0xe0, 0x0f, 0xf8, 0x03, 0xff, 0xff, 0xe0, 0x0f,
		0xf8, 0x07, 0xff, 0xff, 0xe0, 0x0f, 0xf8, 0x07, 0xff, 0x7f, 0x80, 0x0f, 0xf8, 0x0f, 0xff, 0xbf,
		0x80, 0x0f, 0xf8, 0x0f, 0x78, 0x3f, 0x90, 0x0f, 0xf8, 0x00, 0x00, 0x1f, 0xd0, 0x0f, 0xf8, 0x41,
		0x88, 0x0f, 0xf0, 0x0f, 0xf8, 0xe1, 0xee, 0x0e, 0xf0, 0x0f, 0xf8, 0xf1, 0xe7, 0xf2, 0x20, 0x0f,
		0xf8, 0xf1, 0xf1, 0xe2, 0x00, 0x0f, 0xf8, 0xfe, 0x7d, 0xe3, 0x00, 0x0f, 0xf8, 0xff, 0xff, 0x60,
		0x00, 0x0f, 0xf9, 0xff, 0xff, 0xe0, 0x00, 0x0f, 0xf9, 0xff, 0xff, 0xc0, 0x04, 0x0f, 0xf9, 0xff,
		0xf1, 0xc0, 0x00, 0x0f, 0xf8, 0xff, 0xe1, 0xc0, 0x00, 0x0f, 0xf8, 0xff, 0xe0, 0x00, 0x00, 0x0f,
		0xf8, 0xff, 0xf0, 0x00, 0x00, 0x0f, 0xf8, 0x7f, 0xf0, 0x00, 0x00, 0x0f, 0xf8, 0x4f, 0xe0, 0x00,
		0x00, 0x0f, 0xf8, 0x03, 0xf0, 0x00, 0x80, 0x0f, 0xf8, 0x00, 0x30, 0x00, 0x60, 0x0f, 0xf8, 0x00,
		0x00, 0x00, 0xc0, 0x0f, 0xf8, 0x80, 0x00, 0x7e, 0x00, 0x0f, 0xf9, 0xc0, 0x00, 0x7e, 0x00, 0x0f
};

class Oled64x48Display {
public:
	Oled64x48Display(uint8_t textSize, uint8_t textColor);

	void displayLogo();
	void displayTemp(float temp, float humidity);
    void displayWeight(long weight);
	void displayServerData(String& host, String& port);
	void displayBatteryData(String& battPercentage);
	void displayWifiSSID(String& ssid);
	void initDisplay();

private:
	Adafruit_SSD1306 display = Adafruit_SSD1306(OLED_RESET);
	uint8_t textSize;
	uint8_t textColor;
};

#endif //NALKINSCLOUD_ESP8266_OLED_DISPLAY_H
