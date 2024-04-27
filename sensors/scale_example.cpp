//
// Created by Arie Lev on 02/08/2021.
//

// IoT Scale based on Instructables Internet of Things Class sample code and SparkFun HX711 example
// Circuit saves weight online
// Weight valies are saved in an Adafruit.io feed
// Multiple NeoPixels to visualize actions
//
// Modified by Igor Fonseca 2017
// based on Instructables Internet of Things Class sample code by Becky Stern 2017
// based on Example using the SparkFun HX711 breakout board with a scale by Nathan Seidle 2017
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************ Adafruit IO Configuration *******************************/

// visit io.adafruit.com if you need to create an account,
// or if you need your Adafruit IO key.
#define IO_USERNAME    "xxxxx"
#define IO_KEY         "yyyyy"

/******************************* WIFI Configuration **************************************/

#define WIFI_SSID       "zzzzz"
#define WIFI_PASS       "wwwww"

#include <AdafruitIO_WiFi.h>
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

/************************ Main Program Starts Here *******************************/

/******************************** Libraries **************************************/
#include <ESP8266WiFi.h> // ESP8266 library
#include <AdafruitIO.h>  // Adafruit IO library
#include <Adafruit_MQTT.h> // Adafruit MQTT library
#include <ArduinoHttpClient.h>  // http Client
#include <HX711.h>  // HX711 library for the scla
#include "DFRobot_HT1632C.h" // Library for DFRobot LED matrix display

#define calibration_factor -21700.0 //-7050.0 //This value is obtained using the SparkFun_HX711_Calibration sketch

#define DOUT 0  // Pin connected to HX711 data output pin
#define CLK  12  // Pin connected to HX711 clk pin

#define NUM_MEASUREMENTS 20 // Number of measurements
#define THRESHOLD 2			// Measures only if the weight is greater than 2 kg. Convert this value to pounds if you're not using SI units.
#define THRESHOLD1 0.5  // Restart averaging if the weight changes more than 0.5 kg.

#define DATA D6 // Pin for DFRobot LED matrix display
#define CS D2   // Pin for DFRobot LED matrix display
#define WR D7   // Pin for DFRobot LED matrix display

DFRobot_HT1632C ht1632c = DFRobot_HT1632C(DATA, WR,CS); // set up LED matrix display

AdafruitIO_Feed *myWeight = io.feed("my-weight"); // set up the 'iot scale' feed

HX711 scale(DOUT, CLK);

float weight = 0.0;
float prev_weight = 0.0;


void setup() {

    // start the serial connection
    Serial.begin(115200);

    // setup LED matrix display
    ht1632c.begin();
    ht1632c.isLedOn(true);
    ht1632c.clearScreen();
    delay(500);

    // connect to io.adafruit.com
    Serial.print("Connecting to Adafruit IO");
    io.connect();

    // set up a message handler for the 'my weight' feed.
    // the handleMessage function (defined below)
    // will be called whenever a message is
    // received from adafruit io.
    myWeight->onMessage(handleMessage);

    //ESP.wdtDisable();

    ht1632c.print("connecting...",50);
    // wait for a connection
    while(io.status() < AIO_CONNECTED) {
        Serial.print(".");
        ht1632c.print("...",50);
        ESP.wdtFeed();
        delay(500);
    }

    // we are connected
    Serial.println(io.statusText());
    ESP.wdtFeed();
    ht1632c.print("connected...",50);

    scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
    scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0
}

void loop() {

    // io.run(); is required for all sketches.
    // it should always be present at the top of your loop
    // function. it keeps the client connected to
    // io.adafruit.com, and processes any incoming data.
    io.run();
    ht1632c.clearScreen();
    ESP.wdtFeed();
    weight = scale.get_units();
    Serial.println(weight);

    float avgweight = 0;

    if (weight > THRESHOLD) { // Takes measures if the weight is greater than the threshold
        float weight0 = scale.get_units();
        for (int i = 0; i < NUM_MEASUREMENTS; i++) {  // Takes several measurements
            delay(100);
            weight = scale.get_units();
            avgweight = avgweight + weight;
            if (abs(weight - weight0) > THRESHOLD1) {
                avgweight = 0;
                i = 0;
            }
            weight0 = weight;
        }
        avgweight = avgweight / NUM_MEASUREMENTS; // Calculate average weight

        Serial.print("Measured weight: ");
        Serial.print(avgweight, 1);
        Serial.println(" kg");

        char result[8]; // Buffer big enough for 7-character float
        dtostrf(avgweight, 6, 1, result);
        ht1632c.print(result);  // Display on LED matrix

        //save myWeight to Adafruit.io
        myWeight->save(avgweight);


        // wait while there's someone on the scake
        while (scale.get_units() > THRESHOLD) {
            ESP.wdtFeed();
        }

        //keep LEDs on for two seconds then restart
        delay(2000);

    }
}

void handleMessage(AdafruitIO_Data *data) {
}
