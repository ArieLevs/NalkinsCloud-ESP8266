//
// Created by Arie Lev on 2019-07-13.
//

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "oled_display.h"

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

void Oled64x48Display::initDisplay() {
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for 64x48)
	display.display();
	delay(1000);
	display.setTextSize(textSize);
	display.setTextColor(textColor);
}


Oled64x48Display::Oled64x48Display(uint8_t _textSize, uint8_t _textColor) {
	textSize = _textSize;
	textColor = _textColor;
}