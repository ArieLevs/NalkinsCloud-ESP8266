//
// Created by Arie Lev on 2019-07-13.
//

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "oled_display.h"


/**
 * print 'nalkinsBitmap' logo to display
 */
void Oled64x48Display::displayLogo() {
	display.clearDisplay();
	// Display nalkisncloud on start, aligned with 64x48 display
	display.setCursor(10,0);
	display.println("Nalkins");
	display.setCursor(16,8);
	display.println("CLoud");
	display.display();
	delay(3000);

	// Display nalkinscloud bitmap image on start, aligned with 64x48 display
	display.clearDisplay();
	display.drawBitmap(8, 0, nalkinsBitmap, 48, 48, WHITE);
	display.display();
}

/**
 * print temperature and humidity to display in 4 separate lines
 * @param temp temperature to print
 * @param humidity humidity to print
 */
void Oled64x48Display::displayTemp(float temp, float humidity) {
	display.clearDisplay();
	display.setCursor(0,0);
	display.println("Temp (C): ");
	display.println(temp);
	display.setCursor(0,16);
	display.println("Humidity: ");
	display.println(humidity);
	display.display();
}

/**
 * print mqtt broker host and port to display in 2 separate lines
 * @param host host to print
 * @param port port to print
 */
void Oled64x48Display::displayServerData(String& host, String& port) {
	display.clearDisplay();
	display.setCursor(0,0);
	display.println("MQTT Host:");
	display.println(host + ":" + port);
	display.display();
}

void Oled64x48Display::displayBatteryData(String& battPercentage) {
	display.clearDisplay();
	display.setCursor(0,0);
	display.println("Batt left:");
	display.println(battPercentage);
	display.display();
}

/**
 * init ith the I2C addr 0x3C (for 64x48),
 * set relevant text size and color
 */
void Oled64x48Display::initDisplay() {
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize w
	display.display();
	delay(1000);
	display.setTextSize(textSize);
	display.setTextColor(textColor);
}

/**
 * init new Oled64x48Display object, full example can be found here https://github.com/adafruit/Adafruit_SSD1306
 *
 * @param _textSize text side to be used for current object
 * @param _textColor text color to be used for current object
 */
Oled64x48Display::Oled64x48Display(uint8_t _textSize, uint8_t _textColor) {
	textSize = _textSize;
	textColor = _textColor;
}