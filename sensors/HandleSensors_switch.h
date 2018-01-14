
#ifndef HANDLESENSORS_SWITCH_H_
#define HANDLESENSORS_SWITCH_H_

// Define sensors GPIOs
#define CONFIGURATIONMODEBUTTON 13 // Pin which define the button that sets the device to configuration mode
#define LED_WORK_STATUS 12
#define LED_IO_ 14 //LED_BUILTIN for onboard

#define SWITCH_IN 4 
#define SWITCH_OUT 5

String deviceType = "switch"; // The devices type definition
String deviceId = "test_switch_deviceid"; // The devices unique id
const String chipType = "ESP8266"; // The devices chip type

#endif /* HANDLESENSORS_SWITCH_H_ */
