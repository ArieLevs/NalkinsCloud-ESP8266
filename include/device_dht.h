//
// Created by Arie Lev on 21/08/2021.
//

#ifndef NALKINSCLOUD_ESP8266_DEVICE_DHT_H
#define NALKINSCLOUD_ESP8266_DEVICE_DHT_H

//#include "global_configs.h"
#include "device.h"
#include "DHT.h"

//Set DHT type and input pin
#define DHT_TYPE 	DHT22	// Set type of used DHT type
#define DHT_PIN 	0 		// GPIO0 -> D3

class DHTDevice : public Device {
public:
    explicit DHTDevice(const String& _deviceId, boolean _calibrationMode, boolean _initDisplay, boolean _initStatusLed, boolean _initBuzzer, boolean debug);

    void sendDataToSensor(const char *topic, byte *payload, unsigned int length);
    void getDataFromSensor() override;
private:
    DHT *dht = nullptr;
};


#endif //NALKINSCLOUD_ESP8266_DEVICE_DHT_H
