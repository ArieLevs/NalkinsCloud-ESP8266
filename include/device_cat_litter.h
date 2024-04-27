//
// Created by Arie Lev on 21/08/2021.
//

#ifndef NALKINSCLOUD_ESP8266_DEVICE_CAT_LITTER_H
#define NALKINSCLOUD_ESP8266_DEVICE_CAT_LITTER_H

// HX711 circuit pins set
#define LOADCELL_DOUT_PIN D7
#define LOADCELL_SCK_PIN D8
#define RESET_BTN_PIN D3

//#include "global_configs.h"
#include "device.h"
#include "HX711.h"

class CatLitter : public Device {
public:
    explicit CatLitter(const String& _deviceId, boolean _calibrationMode, boolean _initDisplay, boolean _initStatusLed, boolean _initBuzzer, boolean debug);

    //void sendDataToSensor(const char *topic, byte *payload, unsigned int length);
    void getDataFromSensor() override;
    boolean checkResetButton();
private:
    HX711 scale;
    boolean calibrationMode;
    float calibration_factor;
};


#endif //NALKINSCLOUD_ESP8266_DEVICE_CAT_LITTER_H
